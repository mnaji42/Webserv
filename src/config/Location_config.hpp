/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location_config.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/10 15:58:27 by dnicolas          #+#    #+#             */
/*   Updated: 2020/06/30 17:20:30 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_CONFIG_HPP
# define LOCATION_CONFIG_HPP

# include <regex.h>
# include <string>
# include <vector>
# include <list>
# include "../Util.hpp"

class Location_config
{
	public:
	class SyntaxErrorException;
	class OutOfRangeException;

	typedef	SyntaxErrorException	syntax_error;
	typedef	OutOfRangeException		error_exception;

	enum e_keyword
	{
		ROOT,
		INDEX,
		METHODS,
		AUTO_INDEX,
		CGI,
		PROXI_PASS,
		SOURCE_CHARSET,
		CHARSET,
		REVERSE_PROXY,
		ENC_GZIP,
		ENC_DEFLATE,
		ENC_BROTLI,
		INTERNAL_MODULE,
		UPLOAD,
		CLIENT_MAX_BODY_SIZE,
		ALIAS,
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
	Location_config(void);
	Location_config(std::pair<std::string const&, size_t&> file);
	Location_config(const Location_config &copy);
	virtual ~Location_config(void);

	Location_config const	&operator=(const Location_config &copy);
	friend bool	operator<(Location_config const &l, Location_config const &r);

	void	sort(void);

	void	set_argument(unsigned int keyword,
std::vector<std::string> new_arg);

	void	set_recursively_argument(unsigned int keyword,
std::vector<std::string> new_arg);

	void	set_recursively_argument(std::string const &str,
std::vector<std::string> new_arg);

	const char				*get_keyword(unsigned int nb) const;
	Location_config const	&get_location(size_t nb) const;
	size_t					nb_location(void) const;

	std::vector<std::string> const				&get_arg(unsigned int nb) const;
	std::pair<std::string, std::string> const	&get_location_arg(void) const;

	class SyntaxErrorException: public std::exception
	{
		public:
		SyntaxErrorException(void);
		SyntaxErrorException(SyntaxErrorException const &obj);
		SyntaxErrorException(std::pair<std::string const&, size_t&> file,
std::string why = std::string());
		virtual ~SyntaxErrorException(void) throw();
		SyntaxErrorException	&operator=(SyntaxErrorException const &exception);
		virtual const char	*what() const throw();
		private:
		std::pair<std::string, size_t>	file;
		std::string	message;
	};
	class OutOfRangeException: public std::exception
	{
		public:
		OutOfRangeException(void);
		OutOfRangeException(OutOfRangeException const &obj);
		OutOfRangeException(std::string message);
		virtual ~OutOfRangeException(void) throw();
		OutOfRangeException	&operator=(OutOfRangeException const &exception);
		virtual const char	*what() const throw();
		private:
		std::string	message;
	};
	bool	operator==(std::string const &str) const;
	bool	operator==(char const *str) const;
	bool	regex_is_init(void) const;

	private:
	bool	argument_error(size_t index);
	void	set_all_recursively_argument(unsigned int keyword,
std::vector<std::string> new_arg);
	void	get_location_arg(std::pair<std::string const&, size_t&>file);
	void	keyword_get_arg(std::pair<std::string const&, size_t&>file,
std::string const &keyword);
	void	add_new_location(std::pair<std::string const&, size_t&>file);

	static const char			*keyword[NB_KEYWORD];
	static const unsigned int	flag_keyword[NB_KEYWORD][NB_FLAG_KEYWORD];

	std::list<Location_config>			locations;
	std::pair<std::string, std::string>	location_arg;
	std::vector<std::string>			arg[NB_KEYWORD];
	regex_t								regex;
	bool								good_regex;
};

#endif /* LOCATION_CONFIG_HPP */
