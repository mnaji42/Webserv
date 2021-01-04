/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PerlLanguage.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: niduches <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/29 17:49:17 by niduches          #+#    #+#             */
/*   Updated: 2020/06/11 14:31:53 by niduches         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PerlLanguage.hpp"

static PerlInterpreter *my_perl;

void	perl_set_global_str(std::string name, std::string content)
{
	std::string	line("$" + name + " = \'" + content + "\';");

	perl_eval_pv(line.c_str(), TRUE);
}

void	perl_set_global_num(std::string name, double content)
{
	std::stringstream	str;

	str << "$" << name << " = " << content << ";";
	perl_eval_pv(str.str().c_str(), TRUE);
}

void	perl_set_global_nil(std::string name)
{
	std::string	line("$" + name + ";");

	perl_eval_pv(line.c_str(), TRUE);
}

void	perl_set_global_bool(std::string name, bool v)
{
	std::string	line("$" + name + " = " + ((v) ? "true": "false") + ";");

	perl_eval_pv(line.c_str(), TRUE);
}

void PerlLanguage::execute(HTTPResponse const& res) {
	char *embedding[] = {(char*)"", (char*)"-e", (char*)"0"};

	my_perl = perl_alloc();

	perl_construct(my_perl);
	perl_parse(my_perl, NULL, 3, embedding, NULL);
	perl_run(my_perl);

	perl_set_global_str("PATH_INFO", res.getCGIPathInfo());
	perl_set_global_str("PATH_TRANSLATED", res.getCGIPathTranslated());
	perl_set_global_str("QUERY_STRING",res.getCGIReq().getQueryString());
	perl_set_global_str("REQUEST_METHOD", getHTTPMethodName(res.getCGIReq().getMethod()));
	perl_set_global_str("SCRIPT_NAME", res.getCGIScriptName());
	perl_set_global_str("SERVER_NAME", res.getCGIServerName());
	perl_set_global_num("SERVER_PORT", res.getCGIServerPort());
	perl_set_global_str("SERVER_PROTOCOL", "HTTP/1.1");
	perl_set_global_str("SERVER_SOFTWARE", "webserv/1.0");

	perl_set_global_nil("CONTENT_LENGTH");
	perl_set_global_nil("CONTENT_TYPE");

	perl_set_global_nil("HEADERS");

	perl_set_global_num("STATUS", 200);
	perl_set_global_bool("BODY_EOF", false);

	perl_eval_pv(res.getCGIFilePath().c_str(), TRUE);

	perl_destruct(my_perl);
	perl_free(my_perl);
}

PerlLanguage::PerlLanguage()
{
}

PerlLanguage::~PerlLanguage()
{
}
