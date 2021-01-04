/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Constants.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/19 15:11:17 by skybt             #+#    #+#             */
/*   Updated: 2020/04/07 02:45:57 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTP.hpp"

char const* getHTTPMethodName(HTTPMethod method) {
  switch (method) {
    case HTTP_METHOD_OPTIONS: return "OPTIONS";
    case HTTP_METHOD_GET: return "GET";
    case HTTP_METHOD_HEAD: return "HEAD";
    case HTTP_METHOD_POST: return "POST";
    case HTTP_METHOD_PUT: return "PUT";
    case HTTP_METHOD_DELETE: return "DELETE";
    case HTTP_METHOD_TRACE: return "TRACE";
    case HTTP_METHOD_CONNECT: return "CONNECT";
    default: return "UNKNOWN";
  }
}

HTTPMethod getHTTPMethodByName(char const* name) {
  if (!strcmp("OPTIONS", name)) return HTTP_METHOD_OPTIONS;
  if (!strcmp("GET", name)) return HTTP_METHOD_GET;
  if (!strcmp("HEAD", name)) return HTTP_METHOD_HEAD;
  if (!strcmp("POST", name)) return HTTP_METHOD_POST;
  if (!strcmp("PUT", name)) return HTTP_METHOD_PUT;
  if (!strcmp("DELETE", name)) return HTTP_METHOD_DELETE;
  if (!strcmp("TRACE", name)) return HTTP_METHOD_TRACE;
  if (!strcmp("CONNECT", name)) return HTTP_METHOD_CONNECT;
  return HTTP_METHOD_UNKNOWN;
}

char const* getHTTPTextStatus(HTTPStatus status) {
  switch (status) {
    // 1xx Informational response
    case 100: return "Continue";
    case 101: return "Switching Protocols";
    case 102: return "Processing";
    case 103: return "Early Hints";
    // 2xx Success
    case 200: return "OK";
    case 201: return "Created";
    case 202: return "Accepted";
    case 203: return "Non-Authoritative Information";
    case 204: return "No Content";
    case 205: return "Reset Content";
    case 206: return "Partial Content";
    case 207: return "Multi-Status";
    case 208: return "Already Reported";
    case 226: return "IM Used";
    // 3xx Redirection
    case 300: return "Multiple Choices";
    case 301: return "Moved Permanently";
    case 302: return "Found";
    case 303: return "See Other";
    case 304: return "Not Modified";
    case 305: return "Use Proxy";
    case 306: return "Switch Proxy";
    case 307: return "Temporary Redirect";
    case 308: return "Permanent Redirect";
    // 4xx Client errors
    case 400: return "Bad Request";
    case 401: return "Unauthorized";
    case 402: return "Payment Required";
    case 403: return "Forbidden";
    case 404: return "Not Found";
    case 405: return "Method Not Allowed";
    case 406: return "Not Acceptable";
    case 407: return "Proxy Authentication Required";
    case 408: return "Request Timeout";
    case 409: return "Conflict";
    case 410: return "Gone";
    case 411: return "Length Required";
    case 412: return "Precondition Failed";
    case 413: return "Payload Too Large";
    case 414: return "URI Too Long";
    case 415: return "Unsupported Media Type";
    case 416: return "Range Not Satisfiable";
    case 417: return "Expectation Failed";
    case 418: return "I'm a teapot";
    case 421: return "Misdirected Request";
    case 422: return "Unprocessable Entity";
    case 423: return "Locked";
    case 424: return "Failed Dependency";
    case 425: return "Too Early";
    case 426: return "Upgrade Required";
    case 428: return "Precondition Required";
    case 429: return "Too Many Requests";
    case 431: return "Request Header Fields Too Large";
    case 451: return "Unavailable For Legal Reasons";
    // 5xx Server errors
    case 500: return "Internal Server Error";
    case 501: return "Not Implemented";
    case 502: return "Bad Gateway";
    case 503: return "Service Unavailable";
    case 504: return "Gateway Timeout";
    case 505: return "HTTP Version Not Supported";
    case 506: return "Variant Also Negotiates";
    case 507: return "Insufficient Storage";
    case 508: return "Loop Detected";
    case 510: return "Not Extended";
    case 511: return "Network Authentication Required";
    // Unknown
    default: return "Unknown";
  }
}

char const* getMime(char const* str) {
  if (!strcmp("atom", str)) return "application/atom+xml";
  if (!strcmp("json", str)) return "application/json";
  if (!strcmp("map", str)) return "application/json";
  if (!strcmp("topojson", str)) return "application/json";
  if (!strcmp("jsonld", str)) return "application/ld+json";
  if (!strcmp("rss", str)) return "application/rss+xml";
  if (!strcmp("geojson", str)) return "application/geo+json";
  if (!strcmp("xml", str)) return "application/xml";
  if (!strcmp("rdf", str)) return "application/rdf+xml";
  if (!strcmp("js", str)) return "text/javascript";
  if (!strcmp("mjs", str)) return "text/javascript";
  if (!strcmp("wasm", str)) return "application/wasm";
  if (!strcmp("webmanifest", str)) return "application/manifest+json";
  if (!strcmp("webapp", str)) return "application/x-web-app-manifest+json";
  if (!strcmp("appcache", str)) return "text/cache-manifest";
  if (!strcmp("mid", str)) return "audio/midi";
  if (!strcmp("midi", str)) return "audio/midi";
  if (!strcmp("kar", str)) return "audio/midi";
  if (!strcmp("aac", str)) return "audio/mp4";
  if (!strcmp("f4a", str)) return "audio/mp4";
  if (!strcmp("f4b", str)) return "audio/mp4";
  if (!strcmp("m4a", str)) return "audio/mp4";
  if (!strcmp("mp3", str)) return "audio/mpeg";
  if (!strcmp("oga", str)) return "audio/ogg";
  if (!strcmp("ogg", str)) return "audio/ogg";
  if (!strcmp("opus", str)) return "audio/ogg";
  if (!strcmp("ra", str)) return "audio/x-realaudio";
  if (!strcmp("wav", str)) return "audio/x-wav";
  if (!strcmp("mka", str)) return "audio/x-matroska";
  if (!strcmp("bmp", str)) return "image/bmp";
  if (!strcmp("gif", str)) return "image/gif";
  if (!strcmp("jpeg", str)) return "image/jpeg";
  if (!strcmp("jpg", str)) return "image/jpeg";
  if (!strcmp("jxr", str)) return "image/jxr";
  if (!strcmp("hdp", str)) return "image/jxr";
  if (!strcmp("wdp", str)) return "image/jxr";
  if (!strcmp("png", str)) return "image/png";
  if (!strcmp("svg", str)) return "image/svg+xml";
  if (!strcmp("svgz", str)) return "image/svg+xml";
  if (!strcmp("tif", str)) return "image/tiff";
  if (!strcmp("tiff", str)) return "image/tiff";
  if (!strcmp("wbmp", str)) return "image/vnd.wap.wbmp";
  if (!strcmp("webp", str)) return "image/webp";
  if (!strcmp("jng", str)) return "image/x-jng";
  if (!strcmp("3gp", str)) return "video/3gpp";
  if (!strcmp("3gpp", str)) return "video/3gpp";
  if (!strcmp("f4p", str)) return "video/mp4";
  if (!strcmp("f4v", str)) return "video/mp4";
  if (!strcmp("m4v", str)) return "video/mp4";
  if (!strcmp("mp4", str)) return "video/mp4";
  if (!strcmp("mpeg", str)) return "video/mpeg";
  if (!strcmp("mpg", str)) return "video/mpeg";
  if (!strcmp("ogv", str)) return "video/ogg";
  if (!strcmp("mov", str)) return "video/quicktime";
  if (!strcmp("webm", str)) return "video/webm";
  if (!strcmp("flv", str)) return "video/x-flv";
  if (!strcmp("mng", str)) return "video/x-mng";
  if (!strcmp("asf", str)) return "video/x-ms-asf";
  if (!strcmp("asx", str)) return "video/x-ms-asf";
  if (!strcmp("wmv", str)) return "video/x-ms-wmv";
  if (!strcmp("avi", str)) return "video/x-msvideo";
  if (!strcmp("mkv", str)) return "video/x-matroska";
  if (!strcmp("mk3d", str)) return "video/x-matroska";
  if (!strcmp("cur", str)) return "image/x-icon";
  if (!strcmp("ico", str)) return "image/x-icon";
  if (!strcmp("doc", str)) return "application/msword";
  if (!strcmp("xls", str)) return "application/vnd.ms-excel";
  if (!strcmp("ppt", str)) return "application/vnd.ms-powerpoint";
  if (!strcmp("docx", str)) return "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
  if (!strcmp("xlsx", str)) return "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
  if (!strcmp("pptx", str)) return "application/vnd.openxmlformats-officedocument.presentationml.presentation";
  if (!strcmp("woff", str)) return "font/woff";
  if (!strcmp("woff2", str)) return "font/woff2";
  if (!strcmp("eot", str)) return "application/vnd.ms-fontobject";
  if (!strcmp("ttf", str)) return "font/ttf";
  if (!strcmp("ttc", str)) return "font/collection";
  if (!strcmp("otf", str)) return "font/otf";
  if (!strcmp("ear", str)) return "application/java-archive";
  if (!strcmp("jar", str)) return "application/java-archive";
  if (!strcmp("war", str)) return "application/java-archive";
  if (!strcmp("hqx", str)) return "application/mac-binhex40";
  if (!strcmp("bin", str)) return "application/octet-stream";
  if (!strcmp("deb", str)) return "application/octet-stream";
  if (!strcmp("dll", str)) return "application/octet-stream";
  if (!strcmp("dmg", str)) return "application/octet-stream";
  if (!strcmp("exe", str)) return "application/octet-stream";
  if (!strcmp("img", str)) return "application/octet-stream";
  if (!strcmp("iso", str)) return "application/octet-stream";
  if (!strcmp("msi", str)) return "application/octet-stream";
  if (!strcmp("msm", str)) return "application/octet-stream";
  if (!strcmp("msp", str)) return "application/octet-stream";
  if (!strcmp("safariextz", str)) return "application/octet-stream";
  if (!strcmp("pdf", str)) return "application/pdf";
  if (!strcmp("ai", str)) return "application/postscript";
  if (!strcmp("eps", str)) return "application/postscript";
  if (!strcmp("ps", str)) return "application/postscript";
  if (!strcmp("rtf", str)) return "application/rtf";
  if (!strcmp("kml", str)) return "application/vnd.google-earth.kml+xml";
  if (!strcmp("kmz", str)) return "application/vnd.google-earth.kmz";
  if (!strcmp("wmlc", str)) return "application/vnd.wap.wmlc";
  if (!strcmp("7z", str)) return "application/x-7z-compressed";
  if (!strcmp("bbaw", str)) return "application/x-bb-appworld";
  if (!strcmp("torrent", str)) return "application/x-bittorrent";
  if (!strcmp("crx", str)) return "application/x-chrome-extension";
  if (!strcmp("cco", str)) return "application/x-cocoa";
  if (!strcmp("jardiff", str)) return "application/x-java-archive-diff";
  if (!strcmp("jnlp", str)) return "application/x-java-jnlp-file";
  if (!strcmp("run", str)) return "application/x-makeself";
  if (!strcmp("oex", str)) return "application/x-opera-extension";
  if (!strcmp("pl", str)) return "application/x-perl";
  if (!strcmp("pm", str)) return "application/x-perl";
  if (!strcmp("pdb", str)) return "application/x-pilot";
  if (!strcmp("prc", str)) return "application/x-pilot";
  if (!strcmp("rar", str)) return "application/x-rar-compressed";
  if (!strcmp("rpm", str)) return "application/x-redhat-package-manager";
  if (!strcmp("sea", str)) return "application/x-sea";
  if (!strcmp("swf", str)) return "application/x-shockwave-flash";
  if (!strcmp("sit", str)) return "application/x-stuffit";
  if (!strcmp("tcl", str)) return "application/x-tcl";
  if (!strcmp("tk", str)) return "application/x-tcl";
  if (!strcmp("crt", str)) return "application/x-x509-ca-cert";
  if (!strcmp("der", str)) return "application/x-x509-ca-cert";
  if (!strcmp("pem", str)) return "application/x-x509-ca-cert";
  if (!strcmp("xpi", str)) return "application/x-xpinstall";
  if (!strcmp("xhtml", str)) return "application/xhtml+xml";
  if (!strcmp("xsl", str)) return "application/xslt+xml";
  if (!strcmp("zip", str)) return "application/zip";
  if (!strcmp("ics", str)) return "text/calendar";
  if (!strcmp("css", str)) return "text/css";
  if (!strcmp("csv", str)) return "text/csv";
  if (!strcmp("htm", str)) return "text/html";
  if (!strcmp("html", str)) return "text/html";
  if (!strcmp("shtml", str)) return "text/html";
  if (!strcmp("md", str)) return "text/markdown";
  if (!strcmp("markdown", str)) return "text/markdown";
  if (!strcmp("mml", str)) return "text/mathml";
  if (!strcmp("txt", str)) return "text/plain";
  if (!strcmp("vcard", str)) return "text/vcard";
  if (!strcmp("vcf", str)) return "text/vcard";
  if (!strcmp("xloc", str)) return "text/vnd.rim.location.xloc";
  if (!strcmp("jad", str)) return "text/vnd.sun.j2me.app-descriptor";
  if (!strcmp("wml", str)) return "text/vnd.wap.wml";
  if (!strcmp("vtt", str)) return "text/vtt";
  if (!strcmp("htc", str)) return "text/x-component";
  return "text/plain";
}
