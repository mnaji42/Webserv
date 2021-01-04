/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errorResponse.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/25 01:42:32 by najimehdi         #+#    #+#             */
/*   Updated: 2020/06/30 17:13:07 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Handler.hpp"
#include "../Context.hpp"

HTTPResponse& defaultErrorResponse(HTTPResponse& res, bool isHeadReq, HTTPStatus status, std::string const& date) {
  char const* statusText = getHTTPTextStatus(status);

  std::stringstream ss;
  ss << "\n\
    <html>\n\
      <head><title>" << status << " " << statusText << "</title></head>\n\
      <body>\n\
        <center><h1>" << status << " " << statusText << "</h1></center>\n\
        <hr><center>webserv</center>\n\
      </body>\n\
    </html>";

  std::string content = ss.str();

  if (!isHeadReq)
    res.content(content);
  else
    res.setNoContent();

  if (!res.isChunked()) {
    std::stringstream ss;
    ss << content.length();
    res.header("Content-Length", ss.str());
  }

  return res.status(status)
    .header("Server", "webserv")
    .header("Date", date)
    .header("Content-Type", "text/html");
}

HTTPResponse& errorResponseFile(HTTPResponse& res, bool isHeadReq, HTTPStatus status, std::string const& date, std::string const& f) {
  if (!isHeadReq) {
    int fd = open(f.c_str(), O_RDONLY, 0);
    if (fd < 0)
      return defaultErrorResponse(res, isHeadReq, status, date);
    res.fileDownload(fd);
  } else {
    res.setNoContent();
  }

	return res.status(status)
		.header("Server", "webserv")
		.header("Date", date)
		.header("Content-Type", getMime(&(f.c_str()[f.find_last_of('.') + 1]))); ///////////////
}

HTTPResponse& errorResponse(HTTPResponse& res, bool isHeadReq, HTTPStatus status,
    std::string const& date, std::vector<std::string> &errorPage) {
  // char const* statusText = getHTTPTextStatus(status);

  std::vector<std::vector<std::string> > pages;
  std::vector<std::string> p;
  for (size_t i = 0; i < errorPage.size(); i++) {
    if (errorPage[i] == ";") {
      pages.push_back(p);
      p.clear();
    }
    p.push_back(errorPage[i]);
  }


  for (std::vector<std::vector<std::string> >::iterator ite = pages.begin(); ite != pages.end(); ++ite) {
    if (ite->size() < 2)
      continue;

    for (std::vector<std::string>::iterator ite2 = ite->begin(); ite2 != ite->end() - 1; ++ite2) {
      std::stringstream ss;
      ss << status;
      if (*ite2 == ss.str()) {
        return errorResponseFile(res, isHeadReq, status, date, (*ite)[ite->size() - 1]);
      }
    }
  }

  return defaultErrorResponse(res, isHeadReq, status, date);
}

HTTPResponse onError(Context* ctx, HTTPStatus status) {
  (void)ctx;
  HTTPResponse res;
  time_t tmm = time(0);
  std::string date = ctime(&tmm);
  date.erase(date.end() - 1);

  return defaultErrorResponse(res, false, status, date);
}
