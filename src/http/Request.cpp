/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/18 17:40:42 by skybt             #+#    #+#             */
/*   Updated: 2020/04/17 19:31:20 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTP.hpp"

std::ostream& operator<<(std::ostream& o, HTTPRequest const& h) {
  return o << getHTTPMethodName(h.getMethod()) << " "
    << h.getPath() << " "
    << h.getHTTPVersion() << "\r\n"
    << h.getHeaders() << "\r\n";
}

// percent encoding -- see rfc3986
static std::string translatePath(std::string const& path) {
  std::string res;
  res.reserve(path.size());

  for (std::string::const_iterator ite = path.begin(); ite != path.end(); ) {
    if (*ite == '%') {
      size_t index = ite - path.begin();
      if (index + 2 >= path.length())
        throw HTTPParseException("malformated URI");

      char digit1 = *(ite + 1);
      char digit2 = *(ite + 2);

      if (!is_hexa_up(digit1) || !is_hexa_up(digit2))
        throw HTTPParseException("malformated URI");

      char c = (hexa_up_to_int(digit1) << 4) | hexa_up_to_int(digit2);

      if (!isprint(c))
        throw HTTPParseException("malformated URI");

      res.push_back(c);

      ite += 3;
    } else {

      if (!isprint(*ite))
        throw HTTPParseException("malformated URI");

      res.push_back(*ite);
      ++ite;
    }
  }

  return res;
}

void HTTPRequest::parse(std::string const& str) {
  std::string::size_type pos = str.find("\n");

  std::string sub = str.substr(0, pos);
  std::vector<std::string> words = split(sub, " ");
  if (words.size() != 3)
    throw HTTPParseException("malformated request");

  // remove the last CR to support CRLF
  if (words[2].length() > 0 && words[2][words[2].length() - 1] == '\r')
    words[2].erase(words[2].end() - 1);
  
  _method = getHTTPMethodByName(words[0].c_str());

  std::string::size_type queryStrPos = words[1].find('?');
  if (queryStrPos != std::string::npos)
    _queryString = translatePath(words[1].substr(queryStrPos));

  _path = translatePath(words[1].substr(0, queryStrPos));
  _httpVersion = words[2];

  if (_method == HTTP_METHOD_UNKNOWN)
    throw HTTPParseException("unknown http method");
  if (words[1].length() < 1)
    throw HTTPParseException("malformated request");
  if (words[2].length() < 1)
    throw HTTPParseException("malformated request");

  _headers.parse(str.substr(pos + 1));
}

std::string HTTPRequest::asString() const {
  std::stringstream ss;
  ss << *this;
  return ss.str();
}
