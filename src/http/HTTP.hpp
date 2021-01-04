/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/18 17:15:18 by skybt             #+#    #+#             */
/*   Updated: 2020/06/30 09:17:20 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_HPP
# define HTTP_HPP

# include "../Util.hpp"
# include "HTTPConstants.hpp"
# include "../encodings/Encoding.hpp"
# include "../language/Language.hpp"

class HTTPHeaders {
  std::map<std::string, std::string> _h;

  public:
    typedef std::map<std::string, std::string> map_type;
    typedef map_type::iterator iterator;
    typedef map_type::const_iterator const_iterator;

    HTTPHeaders(): _h() {}

    size_t size() const {
      return _h.size();
    }

    iterator begin() {
      return _h.begin();
    }

    iterator end() {
      return _h.end();
    }

    const_iterator begin() const {
      return _h.begin();
    }

    const_iterator end() const {
      return _h.end();
    }

    std::string& operator[](std::string const& key) {
      return _h[key];
    }

    std::string operator[](std::string const& key) const {
      for (const_iterator ite = begin(); ite != end(); ++ite) {
        if (!strcasecmp(ite->first.c_str(), key.c_str()))
          return ite->second;
      }
      return "";
    }

    void remove(std::string const& key) {
      _h.erase(key);
    }

    void clear() {
      _h.clear();
    }

    void parse(std::string const& str);
    std::string asString() const;
};

std::ostream& operator<<(std::ostream& o, HTTPHeaders const& h);

// an incoming request
class HTTPRequest {
  HTTPMethod _method;
  std::string _path;
  std::string _queryString;
  std::string _httpVersion;
  HTTPHeaders _headers;
  int _serverPort;

  public:
    HTTPRequest(int serverPort = -1):
      _method(HTTP_METHOD_UNKNOWN), _path(), _queryString(), _httpVersion(), _headers(),
      _serverPort(serverPort) {}

    void setPath(std::string const& path) { _path = path; }
    void setQueryString(std::string const& queryString) { _queryString = queryString; }
    void setHTTPVersion(std::string const& httpVersion) { _httpVersion = httpVersion; }

    int getServerPort() const { return _serverPort; }
    HTTPMethod getMethod() const { return _method; }
    std::string const& getPath() const { return _path; }
    std::string const& getQueryString() const { return _queryString; }
    std::string const& getHTTPVersion() const { return _httpVersion; }
    HTTPHeaders const& getHeaders() const { return _headers; }
    HTTPHeaders& getHeaders() { return _headers; }

    void parse(std::string const& str);
    std::string asString() const;

    void log() const {
      std::cout << "HTTPRequest {" << std::endl;
      std::cout << "  " << getHTTPMethodName(_method) << " "
        << _path << " "
        << _httpVersion << std::endl;
      std::cout << "  Headers: [" << std::endl;
      
      for (HTTPHeaders::const_iterator ite = _headers.begin(); ite != _headers.end(); ++ite) {
        if (ite->second.length() > 0)
          std::cout << "    " << ite->first << ": " << ite->second << std::endl;
      }
      std::cout << "  ]" << std::endl;

      std::cout << "  ServerPort: " << _serverPort << std::endl;

      std::cout << "}" << std::endl;
    }
};

std::ostream& operator<<(std::ostream& o, HTTPRequest const& h);

// an outgoing response

class HTTPResponse {
  std::string _httpVersion;
  HTTPHeaders _headers;
  HTTPStatus _status;
  std::string _status_text;
  std::string _content;
  FD _streamRead;
  FD _streamWrite;
  bool _isOnlyContent;
  bool _isFileDownload;
  bool _isFileUpload;
  bool _isCGI;
  std::string _cgiFilePath;
  HTTPRequest _req;
  std::string _cgiPathInfo;
  std::string _cgiPathTranslated;
  std::string _cgiScriptName;
  std::string _cgiServerName;
  int _cgiServerPort;
  bool _chunked;
  Encoding::Type _encoding;
  bool _reqChunked;
  Encoding::Type _reqEncoding;
  bool _isReverseProxy;
  std::string _reverseProxyIp;
  int _reverseProxyPort;
  Language::Lang _useInternalModule;
  size_t _bodyLimit;
  size_t _contentLength;
  Encoding::Charset _charset;
  bool _noContent;

  public:
    HTTPResponse(HTTPStatus status = STATUS_OK, std::string const& status_text = std::string());

    void log() const {
      std::cout << "HTTPResponse {" << std::endl;
      std::cout << "  " << getHTTPVersion() << " "
        << getStatus() << " "
        << getStatusText() << std::endl;
      std::cout << "  Headers: [" << std::endl;
      
      for (HTTPHeaders::const_iterator ite = _headers.begin(); ite != _headers.end(); ++ite) {
        if (ite->second.length() > 0)
          std::cout << "    " << ite->first << ": " << ite->second << std::endl;
      }
      std::cout << "  ]" << std::endl;

      if (isChunked())
        std::cout << "  Chunked: Yes" << std::endl;
      if (hasContentLength())
        std::cout << "  ContentLength: " << _contentLength << std::endl;
      if (hasBodyLimit())
        std::cout << "  BodyLimit: " << _bodyLimit << std::endl;
      if (_encoding != Encoding::NONE)
        std::cout << "  Encoding: " << Encoding::debugName(_encoding) << std::endl;

      if (isReqChunked())
        std::cout << "  ReqChunked: Yes" << std::endl;
      if (_reqEncoding != Encoding::NONE)
        std::cout << "  ReqEncoding: " << Encoding::debugName(_reqEncoding) << std::endl;

      if (_charset != Encoding::NONE_CHARSET)
        std::cout << "  Charset: " << Encoding::charsetDebugName(_charset) << std::endl;

      if (isCGI())
        std::cout << "  CGI/InternalModule: {"
          << " filePath: " << _cgiFilePath
          << ", pathInfo: " << _cgiPathInfo
          << ", pathTranslated: " << _cgiPathTranslated
          << ", scriptName: " << _cgiScriptName
          << ", serverName: " << _cgiServerName
          << ", serverPort: " << _cgiServerPort
          << ", internalModule: " << Language::debugName(_useInternalModule)
          << " }" << std::endl;
      if (_isOnlyContent)
        std::cout << "  Content: {"
          << " length: " << _content.length()
          << " }" << std::endl;
      if (_isFileDownload)
        std::cout << "  FileDownload: {"
          << " fd: " << _streamRead
          << " }" << std::endl;
      if (_isFileUpload)
        std::cout << "  FileUpload: {"
          << " fd: " << _streamWrite
          << " }" << std::endl;
      if (_isReverseProxy)
        std::cout << "  ReverseProxy: {"
          << " ip: " << _reverseProxyIp
          << ", port: " << _reverseProxyPort
          << " }" << std::endl;

      std::cout << "}" << std::endl;
    }

    std::string const& getHTTPVersion() const { return _httpVersion; }
    HTTPHeaders const& getHeaders() const { return _headers; }
    HTTPStatus getStatus() const { return _status; }
    std::string const& getStatusText() const { return _status_text; }

    std::string const& getContent() const { return _content; }
    bool isOnlyContent() const { return _isOnlyContent; }
    bool isCGI() const { return _isCGI; }
    bool isReverseProxy() const { return _isReverseProxy; }
    bool isFileDownload() const { return _isFileDownload; }
    bool isFileUpload() const { return _isFileUpload; }

    FD getStreamReadFD() const { return _streamRead; }
    FD getStreamWriteFD() const { return _streamWrite; }
    bool hasStreamReadFD() const { return _streamRead != -1; }
    bool hasStreamWriteFD() const { return _streamWrite != -1; }

    std::string const& getCGIFilePath() const { return _cgiFilePath; }
    HTTPRequest const& getCGIReq() const { return _req; }
    std::string const& getCGIPathInfo() const { return _cgiPathInfo; }
    std::string const& getCGIPathTranslated() const { return _cgiPathTranslated; }
    std::string const& getCGIScriptName() const { return _cgiScriptName; }
    std::string const& getCGIServerName() const { return _cgiServerName; }
    int getCGIServerPort() const { return _cgiServerPort; }
    Language::Lang getInternalModule() const { return _useInternalModule; }

    HTTPRequest const& getReverseProxyReq() const { return _req; }
    std::string const& getReverseProxyIp() const { return _reverseProxyIp; }
    int getReverseProxyPort() const { return _reverseProxyPort; }

    bool isChunked() const { return _chunked; }
    Encoding::Type getEncoding() const { return _encoding; }

    Encoding::Charset getCharset() const { return _charset; }

    bool isReqChunked() const { return _reqChunked; }
    Encoding::Type getReqEncoding() const { return _reqEncoding; }

    size_t getBodyLimit() const { return _bodyLimit; }
    size_t hasBodyLimit() const { return _bodyLimit != SIZE_MAX; }
    size_t getContentLength() const { return _contentLength; }
    size_t hasContentLength() const { return _contentLength != SIZE_MAX; }

    void setStreamReadFD(FD fd) { _streamRead = fd; }
    void setStreamWriteFD(FD fd) { _streamWrite = fd; }

    bool hasBody() const { return !_noContent; }
    bool reqHasBody() const { return _reqChunked || hasContentLength(); }

    HTTPResponse& httpVersion(std::string const& httpVersion) {
      _httpVersion = httpVersion;
      return *this;
    }

    HTTPResponse& status(HTTPStatus status) {
      _status = status;
      _status_text = getHTTPTextStatus(status);
      return *this;
    }

    HTTPResponse& header(std::string const& key, std::string const& value) {
      if (!value.length())
        _headers.remove(key);
      _headers[key] = value;
      for (std::string::const_iterator ite = value.begin(); ite != value.end(); ++ite) {
        if (*ite == '\r' || *ite == '\n')
          throw HTTPParseException("cannot have a newline in a header");
      }
      return *this;
    }

    HTTPResponse& status(HTTPStatus status, std::string status_text) {
      _status = status;
      _status_text = status_text;
      return *this;
    }

    HTTPResponse& content(std::string const& content) {
      _isOnlyContent = true;
      _content = content;
      return *this;
    }

    HTTPResponse& fileDownload(FD fd) {
      if (fd < 0)
        throw InternalException("invalid file descriptor passed to HTTPResponse::fileDownload");
      fcntl(fd, F_SETFL, O_NONBLOCK);
      _streamRead = fd;
      _isFileDownload = true;
      return *this;
    }

    HTTPResponse& fileUpload(FD fd) {
      if (fd < 0)
        throw InternalException("invalid file descriptor passed to HTTPResponse::fileUpload");
      fcntl(fd, F_SETFL, O_NONBLOCK);
      _streamWrite = fd;
      _isFileUpload = true;
      return *this;
    }

    HTTPResponse& chunked(bool val = true) {
      _chunked = val;
      return *this;
    }

    HTTPResponse& encoding(Encoding::Type encoding) {
      _encoding = encoding;
      return *this;
    }

    HTTPResponse& charset(Encoding::Charset charset) {
      _charset = charset;
      return *this;
    }

    HTTPResponse& reqChunked(bool val = true) {
      _reqChunked = val;
      return *this;
    }

    HTTPResponse& reqEncoding(Encoding::Type encoding) {
      _reqEncoding = encoding;
      return *this;
    }

    HTTPResponse& cgi(std::string const& path,
      HTTPRequest const& req, std::string const& pathInfo,
      std::string const& pathTranslated, std::string const& scriptName,
      std::string const& serverName, int serverPort);

    HTTPResponse& internalModule(Language::Lang lang,
      std::string const& path,
      HTTPRequest const& req, std::string const& pathInfo,
      std::string const& pathTranslated, std::string const& scriptName,
      std::string const& serverName, int serverPort);

    HTTPResponse& reverseProxy(HTTPRequest const& req, std::string const& ip, int port) {
      _isReverseProxy = true;
      _req = req;
      _reverseProxyIp = ip;
      _reverseProxyPort = port;
      return *this;
    }

    HTTPResponse& bodyLimit(size_t bodyLimit) {
      _bodyLimit = bodyLimit;
      return *this;
    }

    HTTPResponse& noContentLength() {
      _contentLength = SIZE_MAX;
      return *this;
    }

    HTTPResponse& setNoContent() {
      // Reset content headers
      _noContent = true;
      this->header("Transfer-Encoding", "")
        .header("Content-Encoding", "")
        .header("Content-Length", "")
        .encoding(Encoding::NONE)
        .chunked(false);
      return *this;
    }

    HTTPResponse& contentLength(size_t contentLength) {
      _contentLength = contentLength;
      return *this;
    }

    std::string asString() const;
};

std::ostream& operator<<(std::ostream& o, HTTPResponse const& h);

#endif
