/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location_config.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/10 21:46:22 by dnicolas          #+#    #+#             */
/*   Updated: 2020/06/30 17:19:46 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include "Location_config.hpp"
#include "parsing.hpp"

Location_config::Location_config(void): good_regex(false)
{
}

Location_config::Location_config(std::pair<std::string const&, size_t&> file): good_regex(false)
{
	std::string	tmp;

	this->get_location_arg(file);
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
	if (this->location_arg.second.empty() &&
!regcomp(&this->regex, this->location_arg.first.c_str(), 0))
		this->good_regex = true;
	this->sort();
}

Location_config::Location_config(const Location_config &copy): good_regex(false)
{
	*this = copy;
}

Location_config::~Location_config(void)
{
	if (this->good_regex)
		regfree(&regex);
}

Location_config const	&Location_config::operator=(const Location_config &copy)
{
	this->locations = copy.locations;
	this->location_arg = copy.location_arg;
	for (size_t i = 0; i < NB_KEYWORD; ++i)
		this->arg[i] = copy.arg[i];
	if (this->good_regex == true)
		regfree(&regex);
	this->good_regex = false;
	if (copy.good_regex && this->location_arg.second.empty() &&
!regcomp(&this->regex, this->location_arg.first.c_str(), 0))
		this->good_regex = true;
	return (*this);
}

void	Location_config::get_location_arg(
std::pair<std::string const&, size_t&>file)
{
	pass_spaces(file);
	this->location_arg.first = pass_next_word(file);
	if (this->location_arg.first.empty() || this->location_arg.first == "{")
		throw syntax_error(file, "not a valid syntax to start a block");
	pass_spaces(file);
	if (this->location_arg.first == "=" || this->location_arg.first == "~" ||
this->location_arg.first == "~*" || this->location_arg.first == "^~")
	{
		this->location_arg.second = pass_next_word(file);
		if (this->location_arg.second.empty() || this->location_arg.second == "{")
			throw syntax_error(file, "not a valid syntax to start a block");
		pass_spaces(file);
	}
	if (pass_next_word(file) != "{")
		throw syntax_error(file, "not a valid syntax to start a block");
	pass_spaces(file);
}

void	Location_config::keyword_get_arg(
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
		throw syntax_error(file, "not a valid keyword for location block");
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

void	Location_config::add_new_location(std::pair<std::string const&, size_t&>file)
{
	this->locations.push_back(file);
}

void	Location_config::sort(void)
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

bool	operator<(Location_config const &l, Location_config const &r)
{
	if (l.location_arg.first.empty() && r.location_arg.first.empty())
		return (false);
	if (l.location_arg.first.empty())
		return (true);
	if (r.location_arg.first.empty())
		return (false);
	return (l.location_arg.first.size() > r.location_arg.first.size());
}

void	Location_config::set_argument(unsigned int keyword,
std::vector<std::string> new_arg)
{
	if (keyword >= NB_KEYWORD)
		return ;
	this->arg[keyword] = new_arg;
}

void	Location_config::set_recursively_argument(unsigned int keyword,
std::vector<std::string> new_arg)
{
	if (keyword >= NB_KEYWORD || !this->arg[keyword].empty())
		return ;
	this->arg[keyword] = new_arg;
	set_all_recursively_argument(keyword, new_arg);
}

void	Location_config::set_recursively_argument(std::string const &str,
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

void	Location_config::set_all_recursively_argument(unsigned int keyword,
std::vector<std::string> new_arg)
{
	std::list<Location_config>::iterator	it = this->locations.begin();
	std::list<Location_config>::iterator	end = this->locations.end();

	if (keyword >= NB_KEYWORD)
		return ;
	for (; it != end; ++it)
		(*it).set_recursively_argument(keyword, new_arg);
}

bool	Location_config::argument_error(size_t index)
{
	if (index == ROOT)
		return (check_root_argument(this->arg[index]));
	if (index == INDEX)
		return (check_index_argument(this->arg[index]));
	if (index == METHODS)
		return (check_methods_argument(this->arg[index]));
	if (index == AUTO_INDEX)
		return (check_auto_index_argument(this->arg[index]));
	if (index == CGI)
		return (check_cgi_argument(this->arg[index]));
	if (index == PROXI_PASS)
		return (check_proxi_pass_argument(this->arg[index]));
	if (index == ENC_GZIP || index == ENC_DEFLATE || index == ENC_BROTLI)
		return (check_encoding_argument(this->arg[index]));
	return (true);
}

Location_config::SyntaxErrorException::SyntaxErrorException(void)
{
	this->file.first = "";
	this->file.second = 0;
	this->message = "";
}

Location_config::SyntaxErrorException::SyntaxErrorException(
SyntaxErrorException const &obj)
{
	*this = obj;
}

Location_config::SyntaxErrorException::SyntaxErrorException(
std::pair<std::string const&, size_t&> file, std::string why)
{
	size_t				i = 0;
	size_t				last = 0;
	size_t				end;
	int					nb = 0;

	this->file.first = file.first;
	this->file.second = file.second;
	while (i < this->file.second)
	{
		if (file.first[i] == '\n')
		{
			last = i + 1;
			++nb;
		}
		++i;
	}
	end = 0;
	while (end + last < this->file.first.size())
	{
		if (file.first[end + last] == '\n')
			break ;
		++end;
	}
	this->message = "Syntax error: line " + to_string(nb + 1) +
	" colone " + to_string(this->file.second - last + 1) + ", " + why +
	"\n" + file.first.substr(last, end);
}

Location_config::SyntaxErrorException::~SyntaxErrorException(void) throw() {}

Location_config::SyntaxErrorException	&Location_config::SyntaxErrorException::
operator=(SyntaxErrorException const &exception)
{
	this->file.first = exception.file.first;
	this->file.second = exception.file.second;
	this->message = exception.message;
	return (*this);
}

const char	*Location_config::SyntaxErrorException::what() const throw()
{
	return ((this->message).c_str());
}

Location_config::OutOfRangeException::OutOfRangeException(void)
{
}

Location_config::OutOfRangeException::OutOfRangeException(
OutOfRangeException const &obj)
{
	*this = obj;
}

Location_config::OutOfRangeException::OutOfRangeException(std::string message)
{
	this->message = message;
}

Location_config::OutOfRangeException::~OutOfRangeException(void) throw() {}

Location_config::OutOfRangeException	&Location_config::OutOfRangeException::
operator=(OutOfRangeException const &exception)
{
	this->message = exception.message;
	return (*this);
}

const char	*Location_config::OutOfRangeException::what() const throw()
{
	return (this->message.c_str());
}

const char	*Location_config::get_keyword(unsigned int nb) const
{
	if (nb >= NB_KEYWORD)
		throw error_exception("out of range");
	return (this->keyword[nb]);
}

std::vector<std::string> const
&Location_config::get_arg(unsigned int nb) const
{
	if (nb >= NB_KEYWORD)
		throw error_exception("out of range");
	return (this->arg[nb]);
}

std::pair<std::string, std::string> const
&Location_config::get_location_arg(void) const
{
	return (this->location_arg);
}

Location_config const	&Location_config::get_location(size_t nb) const
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

size_t	Location_config::nb_location(void) const
{
	return (this->locations.size());
}

bool	Location_config::regex_is_init(void) const
{
	std::cout << "iniy " << this->good_regex << std::endl;
	return (this->good_regex);
}

bool	Location_config::operator==(char const *str) const
{
	if (!this->good_regex || !str)
		return (false);
	return (!regexec(&this->regex, str, 0, NULL, 0));
}

bool	Location_config::operator==(std::string const &str) const
{
	return (this->operator==(str.c_str()));
}

const char	*Location_config::keyword[NB_KEYWORD] = {
"root", "index", "methods", "auto_index", "cgi", "proxy_pass", "source_charset",
"charset", "reverse_proxy", "encoding_gzip", "encoding_deflate",
"encoding_brotli", "internal_module", "upload", "client_max_body_size", "alias",
"cgi_methods", "cgi_root"};

const unsigned int	Location_config::flag_keyword[NB_KEYWORD][NB_FLAG_KEYWORD] =
{
	{1, 1, 0},	//ROOT
	{1, (unsigned int)-1, 0},	//INDEX
	{1, (unsigned int)-1, 0},	//METHODS
	{1, 1, 0},	//AUTO_INDEX
	{1, (unsigned int)-1, 0},	//CGI
	{1, 1, 0},	//PROXY_PASS
	{1, 1, 0},	//SOURCE_CHARSET
	{1, 1, 0},	//CHARSET
	{1, 2, 0},	//REVERSE_PROXY
	{1, 1, 0},	//ENC_GZIP
	{1, 1, 0},	//ENC_DEFLATE
	{1, 1, 0},	//ENC_BROTLI
	{1, (unsigned int)-1, 0},	//INTERNAL_MODULE,
	{1, 1, 0},	//UPLOAD
	{1, 1, 0},	//CLIENT_MAX_BODY_SIZE
	{1, 1, 0},	//ALIAS
	{1, (unsigned int)-1, 0},	//CGI_METHODS
	{1, 1, 0},	//CGI_ROOT
};
