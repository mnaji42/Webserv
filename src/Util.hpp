/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Util.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/18 17:18:23 by skybt             #+#    #+#             */
/*   Updated: 2020/06/30 17:19:41 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTIL_HPP
# define UTIL_HPP

# include <unistd.h>
# include <fcntl.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <arpa/inet.h>
# include <signal.h>
# include <stdint.h>
# include <errno.h>
# include <exception>
# include <sstream>
# include <vector>
# include <iostream>
# include <string.h>
# include <algorithm>
# include <map>

//remove
# include <stdio.h>

typedef int FD;

std::vector<std::string> split(std::string const& str, std::string const& sep);

void saveEnv(char const* const* env);
char const* const* getEnv();

bool is_digits(std::string const& str);

bool is_hexa_up(char c);
int hexa_up_to_int(char c);

bool is_hexa_down(char c);
int hexa_down_to_int(char c);

// warn: performs no check, be sure to use is_digits before
unsigned long string_to_unsigned(std::string const& str);

char ascii_to_lower(char in);

std::string		iso_to_utf8(std::string iso);
std::string		utf8_to_iso(std::string utf8);

template <typename T>
std::string to_string(T t) {
    std::stringstream ss;
    ss << t;
    return ss.str();
}

#endif
