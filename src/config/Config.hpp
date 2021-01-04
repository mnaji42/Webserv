/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: najimehdi <najimehdi@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/10 15:35:42 by dnicolas          #+#    #+#             */
/*   Updated: 2020/05/08 01:43:36 by niduches         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <string>
# include "parsing.hpp"
# include "Http_config.hpp"
# include "Event_config.hpp"

class Config
{
	public:
	typedef	class Location_config::SyntaxErrorException	syntax_error;
	typedef	class Location_config::OutOfRangeException	error_exception;
	enum e_keyword
	{
		WORKERS,
		MAX_INTERNAL_REDIR_COUNT,
		NB_KEYWORD
	};
	enum e_flag_keyword
	{
		MIN,
		MAX,
		MULTIPLE,
		NB_FLAG_KEYWORD
	};
	Config(void);
	Config(std::string const &fileName);
	Config(const Config &copy);
	virtual ~Config(void);

	Config const	&operator=(const Config &copy);

	void	reaload(std::string const &fileName);
	void	clear(void);
	void	add_new_event(std::pair<std::string const&, size_t&>file);
	void	add_new_http(std::pair<std::string const&, size_t&>file);
	void	set_argument(unsigned int keyword,
std::vector<std::string> new_arg);

	void	set_recursively_argument(unsigned int keyword,
std::vector<std::string> new_arg);

	void	set_recursively_argument(std::string const &str,
std::vector<std::string> new_arg);

	std::vector<std::string> const	&get_arg(unsigned int nb) const;

	Http_config const	&get_http(void) const;
	Event_config const	&get_event(void) const;
	const char			*get_keyword(unsigned int nb) const;

	private:
	std::string	get_file(std::string const &fileName);
	bool	argument_error(size_t index);
	void	remove_to_eof(std::string &file, size_t idx);
	void	remove_comment(std::string &file);
	void	keyword_get_arg(
std::pair<std::string const&, size_t&> file, std::string const &keyword);

	static const char			*keyword[NB_KEYWORD];
	static const unsigned int	flag_keyword[NB_KEYWORD][NB_FLAG_KEYWORD];

	Http_config					http;
	Event_config				event;
	std::vector<std::string>	arg[NB_KEYWORD];
};

void	display_config(Config const &c);

#endif /* CONFIG_HPP */
