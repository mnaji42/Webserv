/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/18 19:01:07 by skybt             #+#    #+#             */
/*   Updated: 2020/06/30 09:22:09 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HANDLER_HPP
# define HANDLER_HPP

# include <iostream>
# include <ctime>
# include <sys/types.h>
# include <sys/stat.h>
# include <dirent.h>
# include <string>
# include <vector>
# include "../http/HTTP.hpp"
# include "../encodings/Encoding.hpp"
# include "../config/Config.hpp"

class Context;

typedef struct	s_alias
{
	std::string alias;
	std::string location;
}				t_alias;

typedef struct	s_cgi
{
	std::vector<std::string> ext;
	std::vector<std::string> methods;
	std::string root;
}				t_cgi;

HTTPResponse onHeadersArrived(Context* ctx, HTTPRequest const& req);
// HTTPResponse onHeadersArrived2(Context* ctx, HTTPRequest const& req);

HTTPResponse onError(Context* ctx, HTTPStatus status);

//Response
HTTPResponse& folderResponseGet(HTTPResponse& res, std::string &date, std::string &streamFile, \
	HTTPRequest const& req, std::vector<std::string> &errorPage,\
	std::string &indexOf);
HTTPResponse& fileResponseGet(HTTPResponse& res, std::string &date, std::string &streamFile, int fd, \
	std::string &charset, std::string &source_charset,\
	std::vector<std::string> errorPage);
HTTPResponse& folderResponseHead(HTTPResponse& res, std::string &date, std::string &streamFile, \
	HTTPRequest const& req, std::vector<std::string> &errorPage);
HTTPResponse& fileResponseHead(HTTPResponse& res, std::string &date, std::string &streamFile,\
	std::string &charset, std::vector<std::string> errorPage);
HTTPResponse& fileResponsePut(HTTPResponse& res, std::string &date, std::string &streamFile, \
		HTTPRequest const& req, int fd, std::vector<std::string> &errorPage, Context* ctx,\
		size_t max_body_size);

//Utils
std::string addSlash(std::string &str);
std::string addSlashStr(std::string str);
std::string popSlashStr(std::string str);
bool compareSlash(std::string str1, std::string str2);
int checkLocation(Server_config const &s, std::string path);
bool checkIndex(Location_config const &l, std::string &fileStream, 
	std::vector<std::string> &index);
void checkPath(Server_config const &s, std::string path, 
	std::vector<std::string> &index,
	bool &auto_index, std::vector<std::string> &errorPage,\
	std::string &root, std::vector<std::string> &methods, std::vector<std::string> &reverse_proxy, \
	std::vector<std::string> &int_module, std::string &charset, std::string &source_charset,\
	size_t &max_body_size, std::pair<bool, t_alias> &alias, t_cgi &cgi);
char *popNewLine(char *line);
void checkEncoding(std::string &encoding);
bool checkMethods(std::vector<std::string> &methods, HTTPMethod currentMethod);
void getPath_alias(std::string &streamFile, std::string &root,\
std::string &alias, std::string &location, std::string path);
void create_directory(std::string streamFile);
std::string getNameOfScript(std::string streamFile);
bool checkCgi2(std::vector<std::string> &cgi, std::string &streamFile);
bool checkCgi(t_cgi &cgi, std::string &streamFile, HTTPMethod currentMethod);

//Error Response HTML
HTTPResponse& errorResponse(HTTPResponse& res, bool isHeadReq, HTTPStatus status, std::string const& date, std::vector<std::string> &errorPage);

#endif
