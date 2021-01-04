/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http_config.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/10 21:32:33 by dnicolas          #+#    #+#             */
/*   Updated: 2020/05/08 02:01:37 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parsing.hpp"
#include "Location_config.hpp"
#include "Server_config.hpp"
#include "Http_config.hpp"

Http_config::Http_config(void)
{
}

Http_config::Http_config(std::pair<std::string const&, size_t&> file)
{
	std::string	tmp;

	this->pass_http_start(file);
	while (file.second < file.first.size() && get_next_word(file) != "}")
	{
		tmp = pass_next_word(file);
		if (tmp == "server")
			this->add_new_server(file);
		else
			this->keyword_get_arg(file, tmp);
		pass_spaces(file);
	}
	if (pass_next_word(file) != "}")
		throw syntax_error(file, "block not end");
	pass_spaces(file);
}

Http_config::Http_config(const Http_config &copy)
{
	(void)copy;
}

Http_config::~Http_config(void)
{
}

Http_config const	&Http_config::operator=(const Http_config &copy)
{
	this->server = copy.server;
	for (size_t i = 0; i < NB_KEYWORD; ++i)
		this->arg[i] = copy.arg[i];
	return (*this);
}

void	Http_config::pass_http_start(std::pair<std::string const&, size_t&> file)
{
	if (pass_spaces(file) == 0)
		throw syntax_error(file, "not a valid syntax to start a block");
	if (pass_next_word(file) != "{")
		throw syntax_error(file, "not a valid syntax to start a block");
	pass_spaces(file);
}

void	Http_config::keyword_get_arg(
std::pair<std::string const&, size_t&>file, std::string const &keyword)
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
		throw syntax_error(file, "not a valid keyword for http block");
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

void	Http_config::add_new_server(std::pair<std::string const&, size_t&>file)
{
	this->server.push_back(file);
}

void	Http_config::set_argument(unsigned int keyword,
std::vector<std::string> new_arg)
{
	if (keyword >= NB_KEYWORD)
		return ;
	this->arg[keyword] = new_arg;
}

void	Http_config::set_recursively_argument(unsigned int keyword,
std::vector<std::string> new_arg)
{
	if (keyword >= NB_KEYWORD || !this->arg[keyword].empty())
		return ;
	this->arg[keyword] = new_arg;
	set_all_recursively_argument(keyword, new_arg);
}

void	Http_config::set_recursively_argument(std::string const &str,
std::vector<std::string> new_arg)
{
	unsigned int	i = 0;

	while (i < NB_KEYWORD)
	{
		if (this->keyword[i] == str)
		{
			std::cout << "| " <<this->keyword[i] << " " << str << std::endl;
			if (!this->arg[i].empty())
				return ;
			this->arg[i] = new_arg;
			set_all_recursively_argument(i, new_arg);
			return ;
		}
		++i;
	}
}

void	Http_config::set_all_recursively_argument(unsigned int keyword,
std::vector<std::string> new_arg)
{
	std::vector<Server_config>::iterator	it = this->server.begin();
	std::vector<Server_config>::iterator	end = this->server.end();

	if (keyword >= NB_KEYWORD)
		return ;
	for (; it != end; ++it)
		(*it).set_recursively_argument(
static_cast<std::string>(this->keyword[keyword]), new_arg);
}

const char	*Http_config::get_keyword(unsigned int nb) const
{
	if (nb >= NB_KEYWORD)
		throw error_exception("out of range");
	return (this->keyword[nb]);
}

std::vector<std::string> const	&Http_config::get_arg(unsigned int nb) const
{
	if (nb >= NB_KEYWORD)
		throw error_exception("out of range");
	return (this->arg[nb]);
}

Server_config const	&Http_config::get_server(size_t nb) const
{
	if (nb >= this->server.size())
		throw error_exception("out of range");
	return (this->server[nb]);
}

size_t	Http_config::nb_server(void) const
{
	return (this->server.size());
}

bool	Http_config::argument_error(size_t index)
{
	if (index == ERROR_PAGE)
		return (check_error_page_argument(this->arg[index]));
	if (index == ROOT)
		return (check_root_argument(this->arg[index]));
	return (false);
}

const char	*Http_config::keyword[NB_KEYWORD] = {
"error_page", "root"};

const unsigned int	Http_config::flag_keyword[NB_KEYWORD][NB_FLAG_KEYWORD] =
{
	{2, (unsigned int)-1, 1},	//ERROR_PAGE
	{1, 1, 0},	//ROOT
};
