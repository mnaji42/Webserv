/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fileResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/25 01:53:19 by najimehdi         #+#    #+#             */
/*   Updated: 2020/06/15 13:08:59 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Handler.hpp"
# include "../Context.hpp"

static Encoding::Charset checkCharset(std::string &charset, std::string source_charset)
{
	if (charset == "iso" && source_charset == "utf8")
		return Encoding::UTF8_TO_ISO;
	else if (charset == "utf8" && source_charset == "iso")
		return Encoding::ISO_TO_UTF8;
	return Encoding::NONE_CHARSET;
}

HTTPResponse& fileResponseGet(HTTPResponse& res, std::string &date, std::string &streamFile, int fd, \
			std::string &charset, std::string &source_charset,\
			std::vector<std::string> errorPage)
{
	struct stat buff;
	if (stat(streamFile.c_str(), &buff) == -1) {
		close(fd);
		return errorResponse(res, false, STATUS_NOT_FOUND, date, errorPage);
	}

	Encoding::Charset Charset;
	std::string content_type = getMime(&(streamFile.c_str()[streamFile.find_last_of('.') + 1]));
	if (charset == "utf8")
		content_type += "; charset=utf-8";
	else if (charset == "iso")
		content_type += "; charset=iso-8859-1";

	Charset = checkCharset(charset, source_charset);

	return res.status(STATUS_OK)
		.fileDownload(fd)
		.charset(Charset)
		.header("Content-Type", content_type.c_str())
		.header("Last-Modified", popNewLine(ctime(&buff.ST_MTIME.tv_sec)));
}

HTTPResponse& fileResponseHead(HTTPResponse& res, std::string &date, std::string &streamFile,\
			std::string &charset, std::vector<std::string> errorPage)
{
	struct stat buff;
	if (stat(streamFile.c_str(), &buff) == -1)
		return errorResponse(res, true, STATUS_NOT_FOUND, date, errorPage);
	std::string content_type = getMime(&(streamFile.c_str()[streamFile.find_last_of('.') + 1]));
	if (charset == "utf8")
		content_type += "; charset=utf-8";
	else if (charset == "iso")
		content_type += "; charset=iso-8859-1";

	return res.status(STATUS_OK)
		.setNoContent()
		.header("Content-Type", content_type.c_str())
		.header("Last-Modified", popNewLine(ctime(&buff.ST_MTIME.tv_sec)));
}
