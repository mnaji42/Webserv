/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/18 18:44:47 by skybt             #+#    #+#             */
/*   Updated: 2020/06/30 17:09:26 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Handler.hpp"
#include "../Context.hpp"

bool setupEncodings(HTTPResponse& res, HTTPRequest const& req,
		std::string &streamFile, std::string &date, std::vector<std::string> &errorPage,
		size_t max_body_size) {

	(void)streamFile;

	// REQUEST

	bool isChunked = false;
	bool hasContentLength = false;
	bool hasEncoding = false;
	size_t contentLength;

	// chunked request support
	// todo encoding specified in Transfer-Encoding header
	std::string hTransferEncoding = req.getHeaders()["Transfer-Encoding"];
	std::transform(hTransferEncoding.begin(), hTransferEncoding.end(), hTransferEncoding.begin(), ascii_to_lower); // to lowercase
	if (hTransferEncoding.length()) {

		std::vector<std::string> enc = split(hTransferEncoding, ",");

		for (std::vector<std::string>::iterator ite = enc.begin(); ite != enc.end(); ++ite) {
			// remove spaces before and after (trim)
			while (ite->length() > 0 && (*ite)[0] == ' ')
				ite->erase(ite->begin());
			while (ite->length() > 0 && (*ite)[ite->length() - 1] == ' ')
				ite->erase(ite->end() - 1);
		}

		if (enc.size() == 1) {
			if (enc[0] != "chunked") {
				errorResponse(res, req.getMethod() == HTTP_METHOD_HEAD, STATUS_NOT_ACCEPTABLE, date, errorPage);
				return false;
			}

			res.reqChunked(true);
			isChunked = true;
		} else if (enc.size() == 2) {
			if (enc[1] != "chunked") {
				errorResponse(res, req.getMethod() == HTTP_METHOD_HEAD, STATUS_NOT_ACCEPTABLE, date, errorPage);
				return false;
			}

			Encoding::Type type = Encoding::typeByString(enc[0].c_str());
			if (!Encoding::isSupported(type)) {
				errorResponse(res, req.getMethod() == HTTP_METHOD_HEAD, STATUS_NOT_ACCEPTABLE, date, errorPage);
				return false;
			}

			res.reqChunked(true);
			res.reqEncoding(type);
			isChunked = true;
			hasEncoding = true;
		} else {
			errorResponse(res, req.getMethod() == HTTP_METHOD_HEAD, STATUS_NOT_ACCEPTABLE, date, errorPage);
			return false;
		}
	}

	// content length request support
	std::string hContentLength = req.getHeaders()["Content-Length"];
	if (hContentLength.length() > 0) {

		if (!is_digits(hContentLength)) {
			errorResponse(res, req.getMethod() == HTTP_METHOD_HEAD, STATUS_BAD_REQUEST, date, errorPage);
			return false;
		}

		contentLength = string_to_unsigned(req.getHeaders()["Content-Length"]);

		res.contentLength(contentLength);
		hasContentLength = true;
	}

	if (hasContentLength && contentLength > max_body_size) {
		errorResponse(res, req.getMethod() == HTTP_METHOD_HEAD, STATUS_PAYLOAD_TOO_LARGE, date, errorPage);
		return false;
	}

	res.bodyLimit(max_body_size);
		
	if (hasContentLength && isChunked) {
		errorResponse(res, req.getMethod() == HTTP_METHOD_HEAD, STATUS_BAD_REQUEST, date, errorPage);
		return false;
	}

	std::string reqEncoding = req.getHeaders()["Content-Encoding"];

	if (reqEncoding.length()) {
		Encoding::Type type = Encoding::typeByString(reqEncoding.c_str());
		if (hasEncoding || !isChunked || !Encoding::isSupported(type)) {
			errorResponse(res, req.getMethod() == HTTP_METHOD_HEAD, STATUS_NOT_ACCEPTABLE, date, errorPage);
			return false;
		}

		// if transfer encoding is "identity", type here is Encoding::NONE, which means no encoding
		res.reqEncoding(type);
	}

	// RESPONSE

	if (req.getMethod() != HTTP_METHOD_HEAD) {
		// Return everything as chunked!
		res.chunked();

		// Encoding support
		std::string encoding = req.getHeaders()["Accept-Encoding"];
		std::transform(encoding.begin(), encoding.end(), encoding.begin(), ascii_to_lower); // to lowercase
		checkEncoding(encoding);

		if (encoding != "")
			res.encoding(Encoding::typeByString(encoding.c_str()))
				.header("Transfer-Encoding", encoding + ", chunked");
		else
			res.header("Transfer-Encoding", "chunked");
	}

	return true;
}

static Language::Lang checkIntModule(std::vector<std::string> &intModule, std::string &streamFile)
{
	size_t i = streamFile.find_last_of('.');
	for (size_t j = 0; j < intModule.size(); j++)
		if (streamFile.substr(i + 1) == intModule[j])
		{
			if (intModule[j] == "php" && Language::isSupported(Language::PHP))
				return Language::PHP;
			if (intModule[j] == "pl" && Language::isSupported(Language::PERL))
				return Language::PERL;
			if (intModule[j] == "js" && Language::isSupported(Language::JAVASCRIPT))
				return Language::JAVASCRIPT;
			if (intModule[j] == "py" && Language::isSupported(Language::PYTHON))
				return Language::PYTHON;
			if (intModule[j] == "lua" && Language::isSupported(Language::LUA))
				return Language::LUA;
			return Language::NONE;
		}
	return Language::NONE;
}

int initRequest(Context* ctx, HTTPRequest const& req, std::string &date, 
				std::vector<std::string> &index, int &server_id,\
					bool &auto_index, std::vector<std::string> &errorPage,\
					std::string &root, std::vector<std::string> &methods, std::vector<std::string> &reverse_proxy,\
					std::vector<std::string> &int_module, std::string &charset, std::string &source_charset,\
					size_t &max_body_size, t_cgi &cgi)
{
	//stock the curent time
	time_t tmm = time(0);
	date = ctime(&tmm);
	date.erase(date.end() - 1);

	while (!ctx->config().get_http().get_server(server_id).get_arg(Server_config::LISTEN).empty() &&\
	 string_to_unsigned(ctx->config().get_http().get_server(server_id).get_arg(Server_config::LISTEN)[0].c_str()) != (size_t)req.getServerPort())
	{
		server_id++;
		if (server_id == (int)ctx->config().get_http().nb_server())
			return (0);
	}

	if (!ctx->config().get_http().get_server(server_id).get_arg(Server_config::SERVER_NAMES).empty() &&\
		ctx->config().get_http().get_server(server_id).get_arg(Server_config::SERVER_NAMES)[0] != req.getHeaders()["Host"])
			return 0;

	// Stock the root of the server
	if (!ctx->config().get_http().get_server(server_id).get_arg(Server_config::ROOT).empty())
		root = ctx->config().get_http().get_server(server_id).get_arg(Server_config::ROOT)[0];
	// if (req.getPath() == "/")

	//Stock the index general
	index = ctx->config().get_http().get_server(server_id).get_arg(Server_config::INDEX);

	//Check auto_index
	if (!ctx->config().get_http().get_server(server_id).get_arg(Server_config::AUTO_INDEX).empty() &&\
			ctx->config().get_http().get_server(server_id).get_arg(Server_config::AUTO_INDEX)[0] == "on")
			auto_index = true;
	
	//Check cgi
	cgi.ext = ctx->config().get_http().get_server(server_id).get_arg(Server_config::CGI);
	cgi.methods = ctx->config().get_http().get_server(server_id).get_arg(Server_config::CGI_METHODS);
	if (!ctx->config().get_http().get_server(server_id).get_arg(Server_config::CGI_ROOT).empty())
		cgi.root = ctx->config().get_http().get_server(server_id).get_arg(Server_config::CGI_ROOT)[0];

	//Check error pages
	errorPage = ctx->config().get_http().get_server(server_id).get_arg(Server_config::ERROR_PAGE);

	//Check cgi
	methods = ctx->config().get_http().get_server(server_id).get_arg(Server_config::METHODS);

	// Check rever_proxy
	reverse_proxy = ctx->config().get_http().get_server(server_id).get_arg(Server_config::REVERSE_PROXY);

	// Check internal_module
	int_module = ctx->config().get_http().get_server(server_id).get_arg(Server_config::INTERNAL_MODULE);

	// Check internal_module
	if (!ctx->config().get_http().get_server(server_id).get_arg(Server_config::CHARSET).empty())
	{
		charset = ctx->config().get_http().get_server(server_id).get_arg(Server_config::CHARSET)[0];
	}

	//Check source_charset
	if (!ctx->config().get_http().get_server(server_id).get_arg(Server_config::SOURCE_CHARSET).empty())
	{
		source_charset = ctx->config().get_http().get_server(server_id).get_arg(Server_config::SOURCE_CHARSET)[0];
	}

	//Check max_body_size
	if (!ctx->config().get_http().get_server(server_id).get_arg(Server_config::CLIENT_MAX_BODY_SIZE).empty())
	{
		max_body_size = string_to_unsigned(ctx->config().get_http().get_server(server_id).get_arg(Server_config::CLIENT_MAX_BODY_SIZE)[0]);
	}
	return 1;
}

HTTPResponse onHeadersArrived(Context* ctx, HTTPRequest const& req) {
	std::string date;
	int server_id = 0;
	std::string streamFile;
	std::vector<std::string> index;
	bool auto_index = false;
	std::vector<std::string> errorPage;
	std::string root;
	std::vector<std::string> methods;
	std::vector<std::string> reverse_proxy;
	std::vector<std::string> int_module;
	std::string charset;
	std::string source_charset;
	size_t max_body_size = -1;
	std::pair<bool, t_alias> alias;
	t_cgi cgi;

	HTTPResponse res;
	res.header("Server", "webserv");

	if (req.getHeaders()["Host"] == "")
		return errorResponse(res, req.getMethod() == HTTP_METHOD_HEAD, STATUS_BAD_REQUEST, date, errorPage);

	if (!initRequest(ctx, req, date, index, server_id, auto_index, errorPage, root, methods, reverse_proxy, int_module, charset, source_charset, max_body_size, cgi))
		return errorResponse(res, req.getMethod() == HTTP_METHOD_HEAD, STATUS_BAD_REQUEST, date, errorPage);

	res.header("Date", date);

	checkPath(ctx->config().get_http().get_server(server_id), req.getPath(), index, auto_index, errorPage, root, methods, reverse_proxy, int_module, charset, source_charset, max_body_size, alias, cgi);

	// Setup Chunked & Content-Length & Encodings & Body size limits
	if (!setupEncodings(res, req, streamFile, date, errorPage, max_body_size))
		return res;

	if (reverse_proxy.size() == 2) {
		if (res.isReqChunked() || res.getReqEncoding() != Encoding::NONE) // disallow encodings & chunked requests on reverse proxy
			return errorResponse(res, req.getMethod() == HTTP_METHOD_HEAD, STATUS_NOT_ACCEPTABLE, date, errorPage);

		HTTPRequest reqRewrite = req;
		reqRewrite.setHTTPVersion("HTTP/1.0");
		reqRewrite.getHeaders().remove("Accept-Encoding");
		reqRewrite.getHeaders().remove("Transfer-Encoding");
		reqRewrite.getHeaders()["Host"] = reverse_proxy[0] + ":" + reverse_proxy[1];
		res.chunked(false);
		res.noContentLength();
		res.encoding(Encoding::NONE);
		res.charset(Encoding::NONE_CHARSET);
		return res.reverseProxy(reqRewrite, reverse_proxy[0], atoi(reverse_proxy[1].c_str()));
	}

	if (!checkMethods(methods, req.getMethod()))
		return errorResponse(res, req.getMethod() == HTTP_METHOD_HEAD, STATUS_METHOD_NOT_ALLOWED, date, errorPage);


	//add root and path
	if (alias.first == 0)
		streamFile = addSlash(root) + popSlashStr(req.getPath());
	else
	{
		getPath_alias(streamFile, root, alias.second.alias, alias.second.location, req.getPath());
	}

  if (checkCgi(cgi, streamFile, req.getMethod()))
	{
		if (cgi.root.empty())
			return res.cgi(streamFile, req, req.getPath(), \
				streamFile, getNameOfScript(streamFile), req.getHeaders()["Host"], req.getServerPort())
				.bodyLimit(max_body_size);
		return res.cgi(cgi.root, req, req.getPath(), \
			cgi.root, getNameOfScript(cgi.root), req.getHeaders()["Host"], req.getServerPort())
			.bodyLimit(max_body_size);
	}

	//Check if the file or the folder exist
	struct stat buff;
	if (req.getMethod() != HTTP_METHOD_PUT && req.getMethod() != HTTP_METHOD_POST && stat(streamFile.c_str(), &buff) == -1)
		return errorResponse(res, req.getMethod() == HTTP_METHOD_HEAD, STATUS_NOT_FOUND, date, errorPage);

	std::string streamPut = streamFile;
	if (req.getMethod() != HTTP_METHOD_PUT && req.getMethod() != HTTP_METHOD_POST && S_ISDIR(buff.st_mode))
		for (size_t i = 0; i < index.size(); i++)
		{
			if (!stat((addSlash(streamFile) + popSlashStr(index[i])).c_str(), &buff))
			{
				streamFile += index[i];
				break ;
			}
		}

	// if (checkCgi2(cgi.ext, streamFile))
	// {
	// 	return HTTPResponse().cgi(streamFile, req, req.getPath(), \
	// 		streamFile, getNameOfScript(streamFile), req.getHeaders()["Host"], req.getServerPort())
	// 		.bodyLimit(max_body_size)
	//     .reqChunked();
	// }

	Language::Lang lang;
	if((lang = checkIntModule(int_module, streamFile)) != Language::NONE)
		return res.internalModule(lang, streamFile, req, req.getPath(), streamFile, \
			getNameOfScript(streamFile), req.getHeaders()["Host"], req.getServerPort());

	if (req.getMethod() == HTTP_METHOD_PUT || req.getMethod() == HTTP_METHOD_POST)
	{
		if (stat(streamPut.c_str(), &buff))
			create_directory(streamPut);
		int fd = open(streamPut.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
		if (fd == -1)
				return errorResponse(res, req.getMethod() == HTTP_METHOD_HEAD, STATUS_INTERNAL_SERVER_ERROR, date, errorPage);

		struct stat buff;
		if (stat(streamPut.c_str(), &buff)) {
			close(fd);
			return errorResponse(res, req.getMethod() == HTTP_METHOD_HEAD, STATUS_NOT_FOUND, date, errorPage);
		}
		if (S_ISREG(buff.st_mode))
			return fileResponsePut(res, date, streamPut, req, fd, errorPage, ctx, max_body_size); // takes ownership of fd

		close(fd);
		return errorResponse(res, req.getMethod() == HTTP_METHOD_HEAD, STATUS_BAD_REQUEST, date, errorPage);
	}

	if (req.getMethod() == HTTP_METHOD_GET)
	{
		int fd = open(streamFile.c_str(), O_RDONLY);
		if (fd == -1)
			return errorResponse(res, req.getMethod() == HTTP_METHOD_HEAD, STATUS_INTERNAL_SERVER_ERROR, date, errorPage);

		if (stat(streamFile.c_str(), &buff) == -1) {
			close(fd);
			return errorResponse(res, req.getMethod() == HTTP_METHOD_HEAD, STATUS_NOT_FOUND, date, errorPage);
		}

		if (S_ISDIR(buff.st_mode))
		{
			if (auto_index == false) {
				close(fd);
				return errorResponse(res, req.getMethod() == HTTP_METHOD_HEAD, STATUS_NOT_FOUND, date, errorPage);
			}
			
			std::string indexOf = streamFile.substr(root.size() - 1);
			close(fd);

			return folderResponseGet(res, date, streamFile, req, errorPage, indexOf);
		}

		return fileResponseGet(res, date, streamFile, fd, charset, source_charset,\
		errorPage); // takes ownership of fd
	}

	else if (req.getMethod() == HTTP_METHOD_HEAD)
	{
		if (stat(streamFile.c_str(), &buff) == -1)
			return errorResponse(res, req.getMethod() == HTTP_METHOD_HEAD, STATUS_NOT_FOUND, date, errorPage);
		if (S_ISDIR(buff.st_mode))
			return folderResponseHead(res, date, streamFile, req, errorPage);

		return fileResponseHead(res, date, streamFile, charset, errorPage);
	}
	return errorResponse(res, req.getMethod() == HTTP_METHOD_HEAD, STATUS_NOT_IMPLEMENTED, date, errorPage);
}
