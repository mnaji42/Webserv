/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/10 16:59:12 by dnicolas          #+#    #+#             */
/*   Updated: 2020/05/09 13:44:32 by niduches         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <unistd.h>
#include <fcntl.h>
#include "parsing.hpp"
#include "Config.hpp"
#include "Http_config.hpp"
#include "Event_config.hpp"

Config::Config(void)
{
}

std::string	Config::get_file(std::string const &fileName)
{
	std::string	content;
	int			fd = open(fileName.c_str(), O_RDONLY);
	char		buff[4096];
	ssize_t		ret = 1;

	if (fd == -1)
		throw error_exception("file failed to open");
	while (ret > 0)
	{
		ret = read(fd, buff, 4095);
		if (ret == -1)
		{
			throw error_exception("file failed to open");
		}
		buff[ret] = '\0';
		content += buff;
	}
	return (content);
}

Config::Config(std::string const &fileName)
{
	size_t		pos = 0;
	std::string	tmp;
	std::string	content = get_file(fileName);

	remove_comment(content);
	std::pair<std::string const&, size_t&>	file(content, pos);

	pass_spaces(file);
	while (pos < file.first.size())
	{
		tmp = pass_next_word(file);
		if (tmp == "event")
			this->add_new_event(file);
		else if (tmp == "http")
			this->add_new_http(file);
		else
			this->keyword_get_arg(file, tmp);
		pass_spaces(file);
	}
}
void	Config::add_new_event(std::pair<std::string const&, size_t&>file)
{
	unsigned int	i = 0;

	this->event = Event_config(file);
	while (i < NB_KEYWORD)
	{
		if (!this->arg[i].empty())
			this->event.set_recursively_argument(
			static_cast<std::string>(this->keyword[i]), this->arg[i]);
		++i;
	}
}

void	Config::add_new_http(std::pair<std::string const&, size_t&>file)
{
	this->http = Http_config(file);
}

Config::Config(const Config &copy)
{
	*this = copy;
}

Config::~Config(void)
{
}

Config const	&Config::operator=(const Config &copy)
{
	this->http = copy.http;
	this->event = copy.event;
	for (size_t i = 0; i < NB_KEYWORD; ++i)
		this->arg[i] = copy.arg[i];
	return (*this);
}

void	Config::remove_to_eof(std::string &file, size_t idx)
{
	size_t	i = 0;

	while (idx + i < file.size() && file[idx + i] != '\n')
		++i;
	file = file.erase(idx, i);
}

void	Config::remove_comment(std::string &file)
{
	size_t	i = 0;

	if (file[0] == '#')
		remove_to_eof(file, i);
	while (i + 1 < file.size())
	{
		if (file[i + 1] == '#' && is_space(file.c_str() + i, i))
			remove_to_eof(file, i + 1);
		else
			++i;
	}
}

void	Config::keyword_get_arg(
std::pair<std::string const&, size_t&> file, std::string const &keyword)
{
	size_t						i = 0;
	std::vector<std::string>	tmp;

	while (i < NB_KEYWORD)
	{
		if (this->keyword[i] == keyword)
			break ;
		++i;
	}
	if (i >= NB_KEYWORD)
		throw syntax_error(file, "not a valid keyword");
	pass_spaces(file);
	tmp = pass_arg_value(file);
	if (flag_keyword[i][MULTIPLE] == 0 || this->arg[i].empty())
		this->arg[i] = tmp;
	else
	{
		this->arg[i].push_back(";");
		for (size_t j = 0; j < tmp.size(); ++j)
			this->arg[i].push_back(tmp[j]);
	}
	if (tmp.size() < flag_keyword[i][MIN] || tmp.size() > flag_keyword[i][MAX])
		throw syntax_error(file, "not good number of arguments");
	 if (!argument_error(i))
	 	throw syntax_error(file, "wrong arguments");
}

void	Config::set_argument(unsigned int keyword,
std::vector<std::string> new_arg)
{
	if (keyword >= NB_KEYWORD)
		return ;
	this->arg[keyword] = new_arg;
}

void	Config::set_recursively_argument(unsigned int keyword,
std::vector<std::string> new_arg)
{
	if (keyword >= NB_KEYWORD || !this->arg[keyword].empty())
		return ;
	this->arg[keyword] = new_arg;
	this->http.set_recursively_argument(
	static_cast<std::string>(this->keyword[keyword]), new_arg);
	this->event.set_recursively_argument(
	static_cast<std::string>(this->keyword[keyword]), new_arg);
}

void	Config::set_recursively_argument(std::string const &str,
std::vector<std::string> new_arg)
{
	unsigned int	i = 0;

	while (i < NB_KEYWORD)
	{
		if (this->keyword[i] == str)
		{
			if (!this->arg[i].empty())
				return ;
			this->arg[i] = new_arg;
			this->http.set_recursively_argument(
			static_cast<std::string>(this->keyword[i]), new_arg);
			this->event.set_recursively_argument(
			static_cast<std::string>(this->keyword[i]), new_arg);
			return ;
		}
		++i;
	}
}

const char	*Config::get_keyword(unsigned int nb) const
{
	if (nb >= NB_KEYWORD)
		return (NULL);
	return (this->keyword[nb]);
}

std::vector<std::string> const &Config::get_arg(unsigned int nb) const
{
	if (nb >= NB_KEYWORD)
		throw error_exception("out of range");
	return (this->arg[nb]);
}

Http_config const	&Config::get_http(void) const
{
	return (this->http);
}

Event_config const	&Config::get_event(void) const
{
	return (this->event);
}

bool	Config::argument_error(size_t index)
{
	if (index == WORKERS)
		return (check_workers_argument(this->arg[index]));
	if (index == MAX_INTERNAL_REDIR_COUNT)
		return (check_max_internal_redir_count_argument(this->arg[index]));
	return (false);
}

const char	*Config::keyword[NB_KEYWORD] = {"workers", "max_internal_redir_count"};

const unsigned int	Config::flag_keyword[NB_KEYWORD][NB_FLAG_KEYWORD] =
{
	{1, 1, 0}, //WORKER
	{1, 1, 0}, //MAX_INTERNAL_REDIR_COUNT
};
