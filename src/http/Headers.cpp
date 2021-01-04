/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Headers.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/18 17:25:03 by skybt             #+#    #+#             */
/*   Updated: 2020/05/12 01:34:06 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTP.hpp"

std::ostream& operator<<(std::ostream& o, HTTPHeaders const& h) {
  for (HTTPHeaders::const_iterator ite = h.begin(); ite != h.end(); ++ite) {
    if (ite->second.length())
      o << ite->first << ": " << ite->second << "\r\n";
  }
  return o;
}

void HTTPHeaders::parse(std::string const& str) {
  std::vector<std::string> lines = split(str, "\n");

  for (std::vector<std::string>::iterator ite = lines.begin();
        ite != lines.end() && ite->length() > 0; ++ite) {

      std::string::size_type pos = ite->find(":");

      if (pos == std::string::npos)
        throw HTTPParseException("malformated headers");

      std::string key = ite->substr(0, pos);
      std::string value = ite->substr(pos + 1);

      // remove the last CR to support CRLF
      if (value.length() > 0 && value[value.length() - 1] == '\r')
        value.erase(value.end() - 1);

      // remove the first space which is optional
      if (value.length() > 0 && value[0] == ' ')
        value.erase(value.begin());

      if (_h[key].length() > 0)
        throw HTTPParseException("duplicate header");

      if (key.length() < 1)
        throw HTTPParseException("malformated headers");

      if (value.length() < 1)
        continue; // blank (null) means ignore line

      _h[key] = value;
  }
}

std::string HTTPHeaders::asString() const {
  std::stringstream ss;
  ss << *this;
  return ss.str();
}
