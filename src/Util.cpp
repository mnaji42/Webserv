/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Util.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/18 17:27:46 by skybt             #+#    #+#             */
/*   Updated: 2020/06/15 13:06:41 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Util.hpp"

std::vector<std::string> split(std::string const& str, std::string const& sep) {
  std::vector<std::string> rec;

  std::string::size_type sep_len = sep.length();

  std::string::size_type pos = 0;
  std::string::size_type last_pos = 0;
  while ((pos = str.find(sep, last_pos)) != std::string::npos) {
    rec.push_back(str.substr(last_pos, pos - last_pos));
    last_pos = pos + sep_len;
  }

  rec.push_back(str.substr(last_pos));
  return rec;
}

static char const* const* ENV = NULL;

void saveEnv(char const* const* env) {
  ENV = env;
}

char const* const* getEnv() {
  return ENV;
}

bool is_digits(std::string const& str)
{
  return str.find_first_not_of("0123456789") == std::string::npos;
}

bool is_hexa_up(char c) {
  return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F');
}

int hexa_up_to_int(char c) {
  if (c >= '0' && c <= '9')
    return c - '0';
  return 10 + c - 'A';
}

bool is_hexa_down(char c) {
  return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
}

int hexa_down_to_int(char c) {
  if (c >= '0' && c <= '9')
    return c - '0';
  return 10 + c - 'a';
}

unsigned long string_to_unsigned(std::string const& str) {
  unsigned long n = 0;
  for (std::string::const_iterator ite = str.begin(); ite != str.end(); ++ite) {
    n = n * 10 + *ite - '0';
  }
  return n;
}

char ascii_to_lower(char in) {
    if (in <= 'Z' && in >= 'A')
        return in - ('Z' - 'z');
    return in;
}
