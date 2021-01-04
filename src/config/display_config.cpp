/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   display_config.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/10 15:38:35 by dnicolas          #+#    #+#             */
/*   Updated: 2020/05/09 14:18:05 by niduches         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <vector>
#include "Config.hpp"

void	display_event(Event_config const &e)
{
	std::cout << std::endl << "event {" << std::endl;
	for (int i = 0; i < Event_config::NB_KEYWORD; ++i)
	{
		if (!e.get_arg(i).empty())
		{
			std::cout << "    " << e.get_keyword(i);
			for (size_t j = 0; j < e.get_arg(i).size(); ++j)
				std::cout << ' ' << e.get_arg(i)[j];
			std::cout << ";" << std::endl;
		}
	}
	std::cout << "}" << std::endl;
}

void	display_location(Location_config const &e, unsigned int nb)
{
	std::cout << std::endl;
	for (unsigned int i = 1; i < nb; ++i)
		std::cout << "    ";
	std::cout << "location " << e.get_location_arg().first;
	if (!e.get_location_arg().second.empty())
	std::cout << " " << e.get_location_arg().second;
	std::cout << " {" << std::endl;
	for (int i = 0; i < Location_config::NB_KEYWORD; ++i)
	{
		if (!e.get_arg(i).empty())
		{
			for (unsigned int i = 0; i < nb; ++i)
				std::cout << "    ";
			std::cout << e.get_keyword(i);
			for (size_t j = 0; j < e.get_arg(i).size(); ++j)
				std::cout << ' ' << e.get_arg(i)[j];
			std::cout << ";" << std::endl;
		}
	}
	for (size_t i = 0; i < e.nb_location(); ++i)
		display_location(e.get_location(i), nb + 1);
	for (unsigned int i = 1; i < nb; ++i)
		std::cout << "    ";
	std::cout << "}" << std::endl;
}

void	display_server(Server_config const &e)
{
	std::cout << std::endl << "    server {" << std::endl;
	for (int i = 0; i < Server_config::NB_KEYWORD; ++i)
	{
		if (!e.get_arg(i).empty())
		{
			std::cout << "        " << e.get_keyword(i);
			for (size_t j = 0; j < e.get_arg(i).size(); ++j)
				std::cout << ' ' << e.get_arg(i)[j];
			std::cout << ";" << std::endl;
		}
	}
	for (size_t i = 0; i < e.nb_location(); ++i)
		display_location(e.get_location(i), 3);
	std::cout << "    }" << std::endl;
}

void	display_http(Http_config const &e)
{
	std::cout << std::endl << "http {" << std::endl;
	for (int i = 0; i < Http_config::NB_KEYWORD; ++i)
	{
		if (!e.get_arg(i).empty())
		{
			std::cout << e.get_keyword(i);
			for (size_t j = 0; j < e.get_arg(i).size(); ++j)
				std::cout << ' ' << e.get_arg(i)[j];
			std::cout << ";" << std::endl;
		}
	}
	for (size_t i = 0; i < e.nb_server(); ++i)
		display_server(e.get_server(i));
	std::cout << "}" << std::endl;
}

void	display_config(Config const &c)
{
	for (int i = 0; i < Config::NB_KEYWORD; ++i)
	{
		if (!c.get_arg(i).empty())
		{
			std::cout << c.get_keyword(i);
			for (size_t j = 0; j < c.get_arg(i).size(); ++j)
				std::cout << ' ' << c.get_arg(i)[j];
			std::cout << ";" << std::endl;
		}
	}
	// display_event(c.get_event());
	display_http(c.get_http());
}
