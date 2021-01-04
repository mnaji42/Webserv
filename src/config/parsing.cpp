/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dnicolas </var/spool/mail/dnicolas>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/10 20:15:28 by dnicolas          #+#    #+#             */
/*   Updated: 2020/05/09 14:17:20 by niduches         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parsing.hpp"
#include "Location_config.hpp"

std::string	remove_not_real_char(std::string str)
{
	size_t	i = 0;

	while (i + 1 < str.size())
	{
		if (str[i] == '\\' && str[i + 1] == ' ')
			str.erase(i, 1);
		++i;
	}
	return (str);
}

int		is_space(const char *str, size_t start)
{
	if (*str >= '\t' && *str <= '\r')
		return (1);
	if (*str == ' ' && (!start || *(str - 1) != '\\'))
		return (1);
	return (0);
}

size_t	pass_spaces(std::pair<std::string const&, size_t&> file)
{
	size_t	i = 0;

	while (file.second + i < file.first.size() &&
is_space(file.first.c_str() + file.second + i, file.second + i))
		++i;
	file.second += i;
	return (i);
}

std::string	pass_next_word(std::pair<std::string const&, size_t&> file)
{
	size_t		i = 0;
	std::string	tmp;

	while (file.second + i < file.first.size() &&
!is_space(file.first.c_str() + file.second + i, file.second + i) &&
file.first[file.second + i] != ';')
		++i;
	if (!i)
		return (std::string());
	tmp = remove_not_real_char(file.first.substr(file.second, i));
	file.second += i;
	return (tmp);
}

std::string	get_next_word(std::pair<std::string const&, size_t&> file)
{
	size_t		i = 0;

	while (file.second + i < file.first.size() &&
!is_space(file.first.c_str() + file.second + i, file.second + i) &&
file.first[file.second + i] != ';')
		++i;
	if (!i)
		return (std::string());
	return (remove_not_real_char(file.first.substr(file.second, i)));
}

static size_t	size_next_word(std::string const &file, size_t index)
{
	size_t		i = 0;

	while (index + i < file.size() &&
!is_space(file.c_str() + index + i, index + i) && file[index + i] != ';')
		++i;
	return (i);
}

std::vector<std::string>	pass_arg_value(
std::pair<std::string const&, size_t&> file)
{
	std::vector<std::string>	tmp;
	std::string					word;
	size_t						i = file.second;
	size_t						nb = 0;

	while (i < file.first.size() && file.first[i] != ';')
	{
		if (file.first[i] != ';' && !is_space(file.first.c_str() + i, i))
			++nb;
		i += size_next_word(file.first, i);
		while (i < file.first.size() && is_space(file.first.c_str() + i, i))
			++i;
	}
	if (file.first[i] != ';')
	{
		throw Location_config::SyntaxErrorException(file, "line not end with \';\'");
	}
	tmp.reserve(nb);
	while (file.second < file.first.size() && file.first[file.second] != ';')
	{
		word = pass_next_word(file);
		tmp.push_back(word);
		pass_spaces(file);
	}
	++file.second;
	return (tmp);
}

static bool	is_number(std::string const &str)
{
	std::string::const_iterator	it = str.begin();
	std::string::const_iterator	end = str.end();

	while (it != end)
	{
		if (*it < '0' || *it > '9')
			return (false);
		++it;
	}
	return (true);
}

bool	check_workers_argument(std::vector<std::string> arg)
{
	if (arg.size() != 1 || !is_number(arg[0]))
		return (false);
	return (true);
}

bool	check_max_internal_redir_count_argument(std::vector<std::string> arg)
{
	if (arg.size() != 1 || !is_number(arg[0]))
		return (false);
	return (true);
}

bool	check_error_page_argument(std::vector<std::string> arg)
{
	bool	nb = false;

	if (arg.empty() || arg.size() < 2)
		return (false);
	for (size_t i = 0; i < arg.size(); ++i)
	{
		if (is_number(arg[i]))
			nb = true;
		else if (arg[i] == ";" && nb)
			return (false);
		else if (arg[i] == ";" || (nb && !arg[i].empty() && arg[i][0] == '/'))
			nb = false;
		else
			return (false);
	}
	return (!nb);
}

bool	check_client_max_body_size_argument(std::vector<std::string> arg)
{
	size_t	size;

	if (arg.size() != 1 || arg[0].empty())
		return (false);
	if (arg[0] == "0")
		return (true);
	size = arg[0].size() - 1;
	if ((arg[0][size] < '0' || arg[0][size] > '9') && arg[0][size] != 'k' &&
arg[0][size] != 'm' && arg[0][size] != 'g')
		return (false);
	for (size_t i = 0; i < size; ++i)
	{
		if (arg[0][i] < '0' || arg[0][i] > '9')
			return (false);
	}
	return (true);
}

bool	check_root_argument(std::vector<std::string> arg)
{
	if (arg.empty())
		return (false);
	if (arg[0][0] != '/')
		return (false);
	return (true);
}

bool	check_listen_argument(std::vector<std::string> arg)
{
	if (arg.size() != 1 || arg[0].empty())
		return (false);
	return (true);
}

bool	check_server_names_argument(std::vector<std::string> arg)
{
	if (arg.empty() || arg[0].empty())
		return (false);
	return (true);
}

bool	check_index_argument(std::vector<std::string> arg)
{
	if (arg.empty() || arg[0].empty())
		return (false);
	return (true);
}

bool	check_methods_argument(std::vector<std::string> arg)
{
	std::vector<std::string>::iterator	it = arg.begin();
	std::vector<std::string>::iterator	end = arg.end();

	if (arg.empty())
		return (false);
	while (it != end)
	{
		if (*it != "GET" && *it != "HEAD" && *it != "POST" && *it != "PUT" &&
*it != "DELETE" && *it != "COMMECT" && *it != "OPTIONS" && *it != "TRACE")
			return (false);
		++it;
	}
	return (true);
}

bool	check_auto_index_argument(std::vector<std::string> arg)
{
	if (arg.size() != 1 || arg[0].empty())
		return (false);
	if (arg[0] != "on" && arg[0] != "off")
		return (false);
	return (true);
}

bool	check_cgi_argument(std::vector<std::string> arg)
{
	if (arg.empty())
		return (false);
	return (true);
}

bool	check_encoding_argument(std::vector<std::string> arg)
{
	if (arg.size() == 1 && arg[0].size() == 1)
		return (false);
	return (true);
}


bool	check_proxi_pass_argument(std::vector<std::string> arg)
{
	if (arg.empty())
		return (false);
	return (true);
}
