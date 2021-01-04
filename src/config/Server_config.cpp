/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_config.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/10 21:40:54 by dnicolas          #+#    #+#             */
/*   Updated: 2020/05/11 19:54:21 by niduches         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parsing.hpp"
#include "Location_config.hpp"
#include "Server_config.hpp"

Server_config::Server_config(void)
{
}

Server_config::Server_config(std::pair<std::string const&, size_t&> file)
{
	std::string	tmp;

	this->pass_server_start(file);
	while (file.second < file.first.size() && get_next_word(file) != "}")
	{
		tmp = pass_next_word(file);
		if (tmp == "location")
			this->add_new_location(file);
		else
			this->keyword_get_arg(file, tmp);
		pass_spaces(file);
	}
	if (pass_next_word(file) != "}")
		throw syntax_error(file, "block not end");
	pass_spaces(file);
	this->sort();
}

Server_config::Server_config(const Server_config &copy)
{
	*this = copy;
}

Server_config::~Server_config(void)
{
}

Server_config const	&Server_config::operator=(const Server_config &copy)
{
	this->locations = copy.locations;
	for (size_t i = 0; i < NB_KEYWORD; ++i)
		this->arg[i] = copy.arg[i];
	return (*this);
}

void	Server_config::pass_server_start(std::pair<std::string const&, size_t&> file)
{
	if (pass_spaces(file) == 0)
		throw syntax_error(file, "not a valid syntax to start a block");
	if (pass_next_word(file) != "{")
		throw syntax_error(file, "not a valid syntax to start a block");
	pass_spaces(file);
}

void	Server_config::add_new_location(std::pair<std::string const&, size_t&>file)
{
	this->locations.push_back(file);
}

void	Server_config::keyword_get_arg(
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
	{
		throw syntax_error(file, "not a valid keyword for server block");
	}
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

void	Server_config::sort(void)
{
	std::list<Location_config>::iterator	it = this->locations.begin();
	std::list<Location_config>::iterator	end = this->locations.end();

	while (it != end)
	{
		(*it).sort();
		++it;
	}
	this->locations.sort();
}

void	Server_config::set_argument(unsigned int keyword,
std::vector<std::string> new_arg)
{
	if (keyword >= NB_KEYWORD)
		return ;
	this->arg[keyword] = new_arg;
}

void	Server_config::set_recursively_argument(unsigned int keyword,
std::vector<std::string> new_arg)
{
	if (keyword >= NB_KEYWORD || !this->arg[keyword].empty())
		return ;
	this->arg[keyword] = new_arg;
	set_all_recursively_argument(keyword, new_arg);
}

void	Server_config::set_recursively_argument(std::string const &str,
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
			set_all_recursively_argument(i, new_arg);
			return ;
		}
		++i;
	}
}

void	Server_config::set_all_recursively_argument(unsigned int keyword,
std::vector<std::string> new_arg)
{
	std::list<Location_config>::iterator	it = this->locations.begin();
	std::list<Location_config>::iterator	end = this->locations.end();

	if (keyword >= NB_KEYWORD)
		return ;
	for (; it != end; ++it)
		(*it).set_recursively_argument(
static_cast<std::string>(this->keyword[keyword]), new_arg);
}

const char	*Server_config::get_keyword(unsigned int nb) const
{
	if (nb >= NB_KEYWORD)
		return (NULL);
	return (this->keyword[nb]);
}

std::vector<std::string> const	&Server_config::get_arg(unsigned int nb) const
{
	if (nb >= NB_KEYWORD)
		throw error_exception("out of range");
	return (this->arg[nb]);
}

Location_config const	&Server_config::get_location(size_t nb) const
{
	if (nb >= this->locations.size())
		throw error_exception("out of range");
	std::list<Location_config>::const_iterator	it = this->locations.begin();
	std::list<Location_config>::const_iterator	end = this->locations.end();
	for (size_t i = 0; it != end; ++it, ++i)
	{
		if (i == nb)
			return (*it);
	}
	return (this->locations.front());
}

size_t	Server_config::nb_location(void) const
{
	return (this->locations.size());
}

bool	Server_config::argument_error(size_t index)
{
	if (index == ERROR_PAGE)
		return (check_error_page_argument(this->arg[index]));
	if (index == CLIENT_MAX_BODY_SIZE)
		return (check_client_max_body_size_argument(this->arg[index]));
	if (index == ROOT)
		return (check_root_argument(this->arg[index]));
	if (index == LISTEN)
		return (check_listen_argument(this->arg[index]));
	if (index == SERVER_NAMES)
		return (check_server_names_argument(this->arg[index]));
	if (index == INDEX)
		return (check_index_argument(this->arg[index]));
	if (index == METHODS)
		return (check_methods_argument(this->arg[index]));
	if (index == AUTO_INDEX)
		return (check_auto_index_argument(this->arg[index]));
	if (index == CGI)
		return (check_cgi_argument(this->arg[index]));
	if (index == ENC_GZIP || index == ENC_DEFLATE || index == ENC_BROTLI)
		return (check_encoding_argument(this->arg[index]));
	return (true);
}

const char	*Server_config::keyword[NB_KEYWORD] = {
"error_page", "client_max_body_size", "root", "listen", "server_names",
"index", "methods", "auto_index", "cgi", "source_charset", "charset",
"reverse_proxy", "encoding_gzip", "encoding_deflate", "encoding_brotli",
"internal_module", "upload", "cgi_methods", "cgi_root"};

const unsigned int	Server_config::flag_keyword[NB_KEYWORD][NB_FLAG_KEYWORD] =
{
	{2, (unsigned int)-1, 1},	//ERROR_PAGE
	{1, 1, 0},	//CLIENT_MAX_BODY_SIZE
	{1, 1, 0},	//ROOT
	{1, 1, 0},	//LISTEN
	{1, (unsigned int)-1, 0},	//SERVER_NAMES
	{1, (unsigned int)-1, 0},	//INDEX
	{1, (unsigned int)-1, 0},	//METHODS
	{1, 1, 0},	//AUTO_INDEX
	{1, (unsigned int)-1, 0},	//CGI
	{1, 1, 0},	//SOURCE_CHARSET
	{1, 1, 0},	//CHARSET
	{1, 2, 0},	//REVERSE_PROXY
	{1, 1, 0},	//ENC_GZIP
	{1, 1, 0},	//ENC_DEFLATE
	{1, 1, 0},	//ENC_BROTLI
	{1, (unsigned int)-1, 0},	//INTERNAL_MODULE,
	{1, 1, 0},	//UPLOAD
	{1, (unsigned int)-1, 0},	//CGI_METHODS
	{1, 1, 0},	//CGI_ROOT
};
