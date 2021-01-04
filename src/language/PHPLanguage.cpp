/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PHPLanguage.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/17 17:02:44 by skybt             #+#    #+#             */
/*   Updated: 2020/04/18 05:55:58 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PHPLanguage.hpp"

// see https://github.com/php/php-src/blob/master/sapi/cgi/cgi_main.c

PHPLanguage::PHPLanguage() {}
PHPLanguage::~PHPLanguage() {}

static void wsrv_php_log_message(char *message, int syslog_type_int) {
  (void)syslog_type_int;
  std::cerr << "[PHP Internal Module] - " << message << std::endl;
}

static size_t wsrv_php_ub_write(char const* str, size_t len) {
  if (!SG(server_context))
    return 0;
  return write(1, str, len);
}

static void wsrv_php_flush(void* serverContext) {
  (void)serverContext;
}

static int wsrv_php_send_headers(sapi_headers_struct *sapi_headers) {
  zend_llist_position pos;
  sapi_header_struct* h =
    (sapi_header_struct*)zend_llist_get_first_ex(&sapi_headers->headers, &pos);

  while (h) {
    std::cout.write(h->header, h->header_len) << "\r\n";
    h = (sapi_header_struct*)zend_llist_get_next_ex(&sapi_headers->headers, &pos);
  }

  std::cout << "\r\n" << std::flush;

  SG(headers_sent) = true;
  return SAPI_HEADER_SENT_SUCCESSFULLY;
}

static size_t wsrv_php_read_post(char *buffer, size_t count_bytes) {
  if (!SG(server_context))
    return 0;
  return read(0, buffer, count_bytes);
}

static char* c_strdup(std::string const& str) {
  char* c_str = new char[str.length() + 1];
  strcpy(c_str, str.c_str());
  return c_str;
}

void PHPLanguage::execute(HTTPResponse const& res) {
  char* c_queryString = c_strdup(res.getCGIReq().getQueryString());
  char* c_pathTranslated = c_strdup(res.getCGIPathTranslated());
  char* c_cookieData = c_strdup(res.getCGIReq().getHeaders()["Cookie"]);
  char* c_requestUri = c_strdup(res.getCGIReq().getPath());
  std::string contentType = res.getCGIReq().getHeaders()["Content-Type"];

  php_embed_module.name = (char*)"webserv";
  php_embed_module.pretty_name = (char*)"Webserv PHP Internal Module";
  php_embed_module.ub_write = &wsrv_php_ub_write;
  php_embed_module.flush = &wsrv_php_flush;
  php_embed_module.send_headers = &wsrv_php_send_headers;
  php_embed_module.read_post = &wsrv_php_read_post;
  php_embed_module.sapi_error = &zend_error;
  php_embed_module.log_message = &wsrv_php_log_message;

  PHP_EMBED_START_BLOCK(0, 0);

  SG(request_info).request_method = getHTTPMethodName(res.getCGIReq().getMethod());
  SG(request_info).query_string = c_queryString;
  SG(request_info).cookie_data = c_cookieData;
  // SG(request_info).content_length;
  
  SG(request_info).path_translated = c_pathTranslated;
  SG(request_info).request_uri = c_requestUri;

  SG(request_info).content_type = contentType.c_str();

  SG(request_info).headers_only = false;
  SG(request_info).no_headers = false;
  SG(request_info).headers_read = false;

  // SG(request_info).auth_user
  // SG(request_info).auth_password
  // SG(request_info).auth_digest

  SG(headers_sent) = false;

  SG(server_context) = (void*)&res;

  zend_file_handle handle = {
    .filename = res.getCGIFilePath().c_str(),
    .opened_path = NULL,
    .type = ZEND_HANDLE_FILENAME,
    .free_filename = false,
  };

  php_execute_script(&handle);

  SG(server_context) = NULL;

  PHP_EMBED_END_BLOCK();

  delete[] c_queryString;
  delete[] c_pathTranslated;
  delete[] c_cookieData;
  delete[] c_requestUri;
}
