/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_config.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/10 15:58:10 by dnicolas          #+#    #+#             */
/*   Updated: 2020/06/30 09:19:26 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_CONFIG_HPP
# define SERVER_CONFIG_HPP

# include <string>
# include <list>

# include "Location_config.hpp"

class Server_config
{
	public:
	typedef	Location_config::SyntaxErrorException	syntax_error;
	typedef	Location_config::OutOfRangeException	error_exception;
	enum e_keyword
	{
		ERROR_PAGE,
		CLIENT_MAX_BODY_SIZE,
		ROOT,
		LISTEN,
		SERVER_NAMES,
		INDEX,
		METHODS,
		AUTO_INDEX,
		CGI,
		SOURCE_CHARSET,
		CHARSET,
		REVERSE_PROXY,
		ENC_GZIP,
		ENC_DEFLATE,
		ENC_BROTLI,
		INTERNAL_MODULE,
		UPLOAD,
		CGI_METHODS,
		CGI_ROOT,
		NB_KEYWORD
	};
	enum e_flag_keyword
	{
		MIN,
		MAX,
		MULTIPLE,
		NB_FLAG_KEYWORD
	};
	Server_config(void);
	Server_config(std::pair<std::string const&, size_t&> file);
	Server_config(const Server_config &copy);
	virtual ~Server_config(void);

	Server_config const	&operator=(const Server_config &copy);
	void	sort(void);
	void	set_argument(unsigned int keyword,
std::vector<std::string> new_arg);
	void	set_recursively_argument(unsigned int keyword,
std::vector<std::string> new_arg);
	void	set_recursively_argument(std::string const &str,
std::vector<std::string> new_arg);
	std::vector<std::string> const	&get_arg(unsigned int nb) const;

	const char				*get_keyword(unsigned int nb) const;
	Location_config const	&get_location(size_t nb) const;
	size_t					nb_location(void) const;

	private:
	bool	argument_error(size_t index);
	void	pass_server_start(std::pair<std::string const&, size_t&> file);
	void	add_new_location(std::pair<std::string const&, size_t&> file);
	void	keyword_get_arg(
std::pair<std::string const&, size_t&> file, std::string const &keyword);

	void	set_all_recursively_argument(unsigned int keyword,
std::vector<std::string> new_arg);

	static const char			*keyword[NB_KEYWORD];
	static const unsigned int	flag_keyword[NB_KEYWORD][NB_FLAG_KEYWORD];

	std::list<Location_config>	locations;
	std::vector<std::string>	arg[NB_KEYWORD];
};

#endif /* SERVER_CONFIG_HPP */
