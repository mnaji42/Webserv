/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Event_config.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dnicolas </var/spool/mail/dnicolas>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/10 21:37:42 by dnicolas          #+#    #+#             */
/*   Updated: 2020/05/03 20:39:10 by niduches         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parsing.hpp"
#include "Location_config.hpp"
#include "Event_config.hpp"

Event_config::Event_config(void)
{
}

Event_config::Event_config(std::pair<std::string const&, size_t&> file)
{
	this->pass_event_start(file);
	while (file.second < file.first.size() && get_next_word(file) != "}")
	{
		this->keyword_get_arg(file, pass_next_word(file));
		pass_spaces(file);
	}
	if (pass_next_word(file) != "}")
		throw syntax_error(file, "block not end");
	pass_spaces(file);
}

Event_config::Event_config(const Event_config &copy)
{
	*this = copy;
}

Event_config::~Event_config(void)
{
}

Event_config const	&Event_config::operator=(const Event_config &copy)
{
	for (size_t i = 0; i < NB_KEYWORD; ++i)
		this->arg[i] = copy.arg[i];
	return (*this);
}

void	Event_config::pass_event_start(std::pair<std::string const&, size_t&> file)
{
	if (pass_spaces(file) == 0)
		throw syntax_error(file, "not a valid syntax to start a block");
	if (pass_next_word(file) != "{")
		throw syntax_error(file, "not a valid syntax to start a block");
	pass_spaces(file);
}

void	Event_config::keyword_get_arg(
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
		throw syntax_error(file, "not a valid keyword for event block");
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
	if (tmp.size() >= flag_keyword[i][MIN] && tmp.size() <= flag_keyword[i][MAX])
		return ;
	throw syntax_error(file, "not good number of arguments");
}

void	Event_config::set_argument(unsigned int keyword,
std::vector<std::string> new_arg)
{
	if (keyword >= NB_KEYWORD)
		return ;
	this->arg[keyword] = new_arg;
}

void	Event_config::set_recursively_argument(unsigned int keyword,
std::vector<std::string> new_arg)
{
	if (keyword >= NB_KEYWORD || !this->arg[keyword].empty())
		return ;
	this->arg[keyword] = new_arg;
}

void	Event_config::set_recursively_argument(std::string const &str,
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
			return ;
		}
		++i;
	}
}

const char	*Event_config::get_keyword(unsigned int nb) const
{
	if (nb >= NB_KEYWORD)
		throw error_exception("out of range");
	return (this->keyword[nb]);
}

std::vector<std::string> const	&Event_config::get_arg(unsigned int nb) const
{
	if (nb >= NB_KEYWORD)
		throw error_exception("out of range");
	return (this->arg[nb]);
}

const char	*Event_config::keyword[NB_KEYWORD] = {};

const unsigned int	Event_config::flag_keyword[NB_KEYWORD][NB_FLAG_KEYWORD] =
{
};
