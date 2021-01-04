/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http_config.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: najimehdi <najimehdi@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/10 15:57:28 by dnicolas          #+#    #+#             */
/*   Updated: 2020/05/08 01:44:15 by niduches         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_CONFIG_HPP
# define HTTP_CONFIG_HPP

# include <string>
# include <vector>
# include "Server_config.hpp"

class Http_config
{
	public:
	typedef	class Location_config::SyntaxErrorException	syntax_error;
	typedef	class Location_config::OutOfRangeException	error_exception;
	enum e_keyword
	{
		ERROR_PAGE,
		ROOT,
		NB_KEYWORD
	};
	enum e_flag_keyword
	{
		MIN,
		MAX,
		MULTIPLE,
		NB_FLAG_KEYWORD
	};
	Http_config(void);
	Http_config(std::pair<std::string const&, size_t&> file);
	Http_config(const Http_config &copy);
	virtual ~Http_config(void);

	Http_config const	&operator=(const Http_config &copy);

	void	set_argument(unsigned int keyword,
std::vector<std::string> new_arg);
	void	set_recursively_argument(unsigned int keyword,
std::vector<std::string> new_arg);
	void	set_recursively_argument(std::string const &str,
std::vector<std::string> new_arg);
	std::vector<std::string> const	&get_arg(unsigned int nb) const;

	const char			*get_keyword(unsigned int nb) const;
	Server_config const	&get_server(size_t nb) const;
	size_t				nb_server(void) const;

	private:
	bool	argument_error(size_t index);
	void	pass_http_start(std::pair<std::string const&, size_t&> file);
	void	add_new_server(std::pair<std::string const&, size_t&>file);
	void	keyword_get_arg(
std::pair<std::string const&, size_t&>file, std::string const &keyword);
	void	set_all_recursively_argument(unsigned int keyword,
std::vector<std::string> new_arg);

	static const char			*keyword[NB_KEYWORD];
	static const unsigned int	flag_keyword[NB_KEYWORD][NB_FLAG_KEYWORD];

	std::vector<Server_config>	server;
	std::vector<std::string>	arg[NB_KEYWORD];
};

#endif /* HTTP_CONFIG_HPP */
