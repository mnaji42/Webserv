/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/19 14:37:26 by skybt             #+#    #+#             */
/*   Updated: 2020/05/12 22:31:29 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTP.hpp"

HTTPResponse::HTTPResponse(HTTPStatus status, std::string const& status_text):
  _httpVersion("HTTP/1.1"), _headers(),
  _status(status), _status_text(status_text.length() ? status_text : getHTTPTextStatus(status)),
  _content(),
  _streamRead(-1), _streamWrite(-1), _isOnlyContent(false),
  _isFileDownload(false), _isFileUpload(false),
  _isCGI(false),
  _chunked(false), _encoding(Encoding::NONE),
  _reqChunked(false), _reqEncoding(Encoding::NONE),
  _isReverseProxy(false),
  _useInternalModule(Language::NONE),
  _bodyLimit(-1), _contentLength(-1),
  _charset(Encoding::NONE_CHARSET),
  _noContent(false) {}

HTTPResponse& HTTPResponse::cgi(std::string const& path,
    HTTPRequest const& req, std::string const& pathInfo,
    std::string const& pathTranslated, std::string const& scriptName,
    std::string const& serverName, int serverPort) {

  _isCGI = true;
  _cgiFilePath = path;
  _req = req;
  _cgiPathInfo = pathInfo;
  _cgiPathTranslated = pathTranslated;
  _cgiScriptName = scriptName;
  _cgiServerName = serverName;
  _cgiServerPort = serverPort;
  return *this;
}

HTTPResponse& HTTPResponse::internalModule(Language::Lang lang,
    std::string const& path,
    HTTPRequest const& req, std::string const& pathInfo,
    std::string const& pathTranslated, std::string const& scriptName,
    std::string const& serverName, int serverPort) {

  _isCGI = true;
  _cgiFilePath = path;
  _req = req;
  _cgiPathInfo = pathInfo;
  _cgiPathTranslated = pathTranslated;
  _cgiScriptName = scriptName;
  _cgiServerName = serverName;
  _cgiServerPort = serverPort;
  _useInternalModule = lang;
  return *this;
}

std::ostream& operator<<(std::ostream& o, HTTPResponse const& h) {
  return o << h.getHTTPVersion() << " "
    << h.getStatus() << " "
    << h.getStatusText() << "\r\n"
    << h.getHeaders() << "\r\n";
}

std::string HTTPResponse::asString() const {
  std::stringstream ss;
  ss << *this;
  return ss.str();
}
