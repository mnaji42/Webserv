/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Event_config.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: najimehdi <najimehdi@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/10 15:58:02 by dnicolas          #+#    #+#             */
/*   Updated: 2020/05/08 01:44:00 by niduches         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EVENT_CONFIG_HPP
# define EVENT_CONFIG_HPP

# include <string>
# include <vector>
# include "Location_config.hpp"

class Event_config
{
	public:
	typedef	class Location_config::SyntaxErrorException	syntax_error;
	typedef	class Location_config::OutOfRangeException	error_exception;
	enum e_keyword
	{
		NB_KEYWORD
	};
	enum e_flag_keyword
	{
		MIN,
		MAX,
		MULTIPLE,
		NB_FLAG_KEYWORD
	};
	Event_config(void);
	Event_config(std::pair<std::string const&, size_t&> file);
	Event_config(const Event_config &copy);
	virtual ~Event_config(void);

	Event_config const	&operator=(const Event_config &copy);
	void	pass_event_start(std::pair<std::string const&, size_t&> file);
	void	set_argument(unsigned int keyword,
std::vector<std::string> new_arg);
	void	set_recursively_argument(unsigned int keyword,
std::vector<std::string> new_arg);
	void	set_recursively_argument(std::string const &str,
std::vector<std::string> new_arg);
	std::vector<std::string> const	&get_arg(unsigned int nb) const;

	const char	*get_keyword(unsigned int nb) const;

	private:
	void	keyword_get_arg(
std::pair<std::string const&, size_t&>file, std::string const &keyword);

	static const char			*keyword[NB_KEYWORD];
	static const unsigned int	flag_keyword[NB_KEYWORD][NB_FLAG_KEYWORD];

	std::vector<std::string>	arg[NB_KEYWORD];
};

#endif /* EVENT_CONFIG_HPP */
