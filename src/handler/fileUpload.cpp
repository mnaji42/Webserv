/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fileUpload.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/04 02:37:49 by najimehdi         #+#    #+#             */
/*   Updated: 2020/06/30 17:18:24 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Handler.hpp"
# include "../Context.hpp"

HTTPResponse& fileResponsePut(HTTPResponse& res, std::string &date, std::string &streamFile, \
		HTTPRequest const& req, int fd, std::vector<std::string> &errorPage, Context* ctx,\
		size_t max_body_size)
{
	(void)req;
	(void)ctx;
	struct stat buff;
	if (stat(streamFile.c_str(), &buff) == -1) {
		close(fd);
		return errorResponse(res, false, STATUS_NOT_FOUND, date, errorPage);
	}

	// Must have a body!
	if (!res.reqHasBody()) { // has no body = no Content-Length header and not Chunked
		close(fd);
		return errorResponse(res, false, STATUS_LENGTH_REQUIRED, date, errorPage);
	}

	return res.status(STATUS_NO_CONTENT)
		.fileUpload(fd)
		.bodyLimit(max_body_size)
		.setNoContent()
		.header("Server", "webserv")
		.header("Date", date);
}
