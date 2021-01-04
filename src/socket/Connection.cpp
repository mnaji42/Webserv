/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/18 16:35:49 by skybt             #+#    #+#             */
/*   Updated: 2020/06/30 09:21:13 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include "assert.h"
#include "../Context.hpp"

void SocketConnection::resetState() {

  std::cout << "reset state" << std::endl;

  // flush compression encoding
  if (_headersArrived) {
    std::string buffer;
    if (_res.getEncoding() != Encoding::NONE) {
      _encOutgoing->end();
      buffer.clear();
      if (_encOutgoing->hasData())
        buffer = _encOutgoing->take();
    }
    if (_res.isChunked() && buffer.length() > 0) {
      std::stringstream ss;
      ss << std::hex << buffer.length()
        << "\r\n" << buffer << "\r\n";

      buffer = ss.str();
    }
    _writeBuffer.append(buffer);

    // send terminating chunk
    if (_res.isChunked()) {
      _writeBuffer.append("0\r\n\r\n");
    }
  }

  if (_res.getStreamReadFD() != -1)
    close(_res.getStreamReadFD());
  if (_res.getStreamWriteFD() != -1 &&
      _res.getStreamWriteFD() != _res.getStreamReadFD())
    close(_res.getStreamWriteFD());

  _res.setStreamReadFD(-1);
  _res.setStreamWriteFD(-1);

  _headersArrived = false;
  _req = HTTPRequest(_serverPort);
  _res = HTTPResponse();
  _cgiHeadersArrived = false;
  _streamReadBuffer = std::string();
  _cgiRedirCount = 0;
  _cgiChild = -1;
  _hasReadAnythingFromStream = false;
  _encOutgoing = NULL;
  _encIncoming = NULL;
  _bytesRead = 0;
  _charset = NULL;
  _readingBlockedFlag = false;
  _streamEOF = false;
  _streamWriteFDClosed = false;

  _fdChanged = true;

  _readBuffer.clear();

  dropResponse();
}

void SocketConnection::dropResponse() {
  if (_res.getStreamReadFD() != -1)
    close(_res.getStreamReadFD());
  if (_res.getStreamWriteFD() != -1 &&
      _res.getStreamWriteFD() != _res.getStreamReadFD())
    close(_res.getStreamWriteFD());

  _res.setStreamReadFD(-1);
  _res.setStreamWriteFD(-1);

  if (_res.isCGI()) {
    kill(_cgiChild, SIGKILL);

    int status;
    pid_t ret = waitpid(_cgiChild, &status, 0);
    while (ret && !WIFEXITED(status) && !WIFSIGNALED(status))
      ret = waitpid(_cgiChild, &status, 0);
  }
  _cgiChild = -1;
  _streamReadBuffer.clear();
  _cgiHeadersArrived = false;
}

void SocketConnection::acceptConnection(FD _socketServer) {
  int addrlen = sizeof(sockaddr_in);

  _rawReadBuf = new char[BUFFER_SIZE];

  if ((_fd = accept(_socketServer, (sockaddr*)&_addr, (socklen_t*)&addrlen)) < 0)
    throw SocketException("cannot accept connection"/*, strerror(errno)*/);

  fcntl(_fd, F_SETFL, O_NONBLOCK);

  resetState();
}

bool SocketConnection::elligibleForSocketRead() const {
  return !_readingBlockedFlag && !_socketEOF;
}

bool SocketConnection::elligibleForSocketWrite() const {
  return !_socketEOF && _writeBuffer.length() > 0;
}

bool SocketConnection::elligibleForStreamRead() const {
  return !_streamEOF && _res.hasStreamReadFD() && !_ignoreInput;
}

bool SocketConnection::elligibleForStreamWrite() const {
  return !_streamEOF && _res.hasStreamWriteFD() && _readBuffer.length() > 0 && !_ignoreInput;
}

void SocketConnection::resetFDChanged() {
  _fdChanged = false;
}

bool SocketConnection::hasAnyFDChanged() const {
  return _fdChanged;
}

void SocketConnection::handleRead() {
  ssize_t len;
  if ((len = recv(_fd, _rawReadBuf, BUFFER_SIZE, 0)) < 0)
    throw SocketException("cannot read from socket"/*, strerror(errno)*/);

  if (!len) {
    _socketEOF = true;
    // EOF!

    if (_res.getReqEncoding() != Encoding::NONE) {
      _encIncoming->end();
      if (_encIncoming->hasData()) {
        std::string bytes = _encIncoming->take();
        _readBuffer.append(bytes);
        onReadBytes(bytes.length());
      }
    }

    return;
  }

  // append to end of read buffer
  _readBufferEncoded.insert(_readBufferEncoded.end(), _rawReadBuf, _rawReadBuf + len);

  handleReadOneReq();
}

void SocketConnection::handleReadOneReq() {

  if (_readBufferEncoded.length() > 0 && !_headersArrived) {
    // accept both CRLF and LF
    std::string::size_type posCRLF = _readBufferEncoded.find("\r\n\r\n");
    std::string::size_type posLF = _readBufferEncoded.find("\n\n");

    std::string::size_type pos = std::min(posCRLF, posLF);
    bool isCRLF = posCRLF == pos;
    std::string::size_type headersSepLen = isCRLF ? 4 : 2;

    if (pos != std::string::npos) {
      // we found the end of the headers in the read buffer!
      //  now we only need to parse the headers and call onHeadersArrived

      std::string headstr = _readBufferEncoded.substr(0, pos + headersSepLen / 2);

      // we trim the headers from the read buffer
      _readBufferEncoded.erase(_readBufferEncoded.begin(), _readBufferEncoded.begin() + pos + headersSepLen);
      // read buffer is now only data from the content of the request

      _headersArrived = true;

      try {
        _req.parse(headstr);
      } catch (HTTPParseException e) {
        e.prettyPrint("request parsing");
        resError(STATUS_BAD_REQUEST);
        return;
      }

      try {
        headersArrived();
      } catch (EncodingException e) {
        e.prettyPrint("request handling");
        resError(STATUS_INTERNAL_SERVER_ERROR);
        return;
      } catch (CGIException e) {
        e.prettyPrint("request handling");
        resError(STATUS_INTERNAL_SERVER_ERROR);
        return;
      } catch (ReverseProxyException e) {
        e.prettyPrint("request handling");
        resError(STATUS_INTERNAL_SERVER_ERROR);
        return;
      }

      if (!_res.isCGI() && !_res.isReverseProxy() && !_res.isFileUpload()) {
        _writeBuffer.append(_res.asString());
        if (_res.isOnlyContent())
          appendToWriteBuffer(_res.getContent());
        if ((!_res.hasBody() || _res.isOnlyContent()) && !_res.reqHasBody())
          resetState();
      }
    }
  }

  if (_readBufferEncoded.length() > 0 && _headersArrived) {

    if (!_res.isReqChunked()) {
      // handle Content-Length

      if (_readBufferEncoded.length() + _bytesRead >= _res.getContentLength()) {
        std::string l = _readBufferEncoded.substr(0, _res.getContentLength() - _bytesRead);
        _readBuffer.append(l);
        onReadBytes(l.length());
        _readBufferEncoded.erase(_readBufferEncoded.begin(), _readBufferEncoded.begin() + _res.getContentLength() - _bytesRead);

        // End of request body!

        if (_res.isFileUpload() && !_ignoreInput) {
          _writeBuffer.append(_res.asString());
        }

        if ((_res.hasStreamWriteFD() && !_streamWriteFDClosed) || (!_res.hasStreamWriteFD() && _res.hasStreamReadFD() && !_streamEOF)) {
          _readingBlockedFlag = true;

          if (_res.isCGI() && _readBuffer.length() == 0) {
            // CLOSE write fd!
            if (_res.hasStreamWriteFD())
              close(_res.getStreamWriteFD());
            _res.setStreamWriteFD(-1);
            _streamWriteFDClosed = true;
          }

        } else {
          resetState();
          handleReadOneReq();
        }

        return;

      } else {
        _readBuffer.append(_readBufferEncoded);
        onReadBytes(_readBufferEncoded.length());
        _readBufferEncoded.clear();
      }

    } else {
      // handle Transfer-Encoding: chunked

      std::string unchunked;
      while (1) { // handle chunks until there are no chunk left to handle

        if (_readBufferEncoded.length() == 0)
          break;

        std::string::iterator ite = _readBufferEncoded.begin();
        size_t chunkSize = 0;
        bool chunkHeadArrived = false;
        while (ite != _readBufferEncoded.end()) {

          // skip chunk extensions
          if (*ite == ';') {
            while (ite != _readBufferEncoded.end() && *ite != '\r')
              ++ite;

            if (ite == _readBufferEncoded.end())
              break;
          }

          if (*ite == '\r' && ite == _readBufferEncoded.end() - 1) { // Chunk has not fully arrived!! 
            return;
          }

          if ((ite + 1) != _readBufferEncoded.end() && *ite == '\r' && *(ite + 1) == '\n') {
            ite += 2;
            chunkHeadArrived = true;
            break;
          }

          if (!is_hexa_down(*ite))
            throw HTTPParseException("error while parsing chunked transfer encoding (1)");

          chunkSize = (chunkSize << 4) | hexa_down_to_int(*ite);
          ++ite;
        }

        if (!chunkHeadArrived) {
          return; // chunk hasn't fully arrived yet
        }

        assert(*(ite - 1) == '\n');

        // ite is pointing just after the \r\n part of the length header

        size_t headLength = ite - _readBufferEncoded.begin();
        if (_readBufferEncoded.length() - headLength < chunkSize + 2) {
          return; // chunk hasn't fully arrived yet
        }

        std::string::iterator end = ite + chunkSize;
        if (*end != '\r' || *(end + 1) != '\n')
          throw HTTPParseException("error while parsing chunked transfer encoding (2)");

        unchunked = std::string(ite, ite + chunkSize);

        // remove the chunk from the read buffer
        _readBufferEncoded.erase(_readBufferEncoded.begin(), ite + chunkSize + 2);

        // terminating chunk
        if (chunkSize == 0) {
          if (_res.getReqEncoding() != Encoding::NONE) {
            _encIncoming->end();
            if (_encIncoming->hasData()) {
              std::string bytes = _encIncoming->take();
              _readBuffer.append(bytes);
              onReadBytes(bytes.length());
            }
          }

          if (_res.isFileUpload() && !_ignoreInput) {
            _writeBuffer.append(_res.asString());
          }

          if ((_res.hasStreamWriteFD() && (_res.isCGI() ? !_streamWriteFDClosed : _readBuffer.length() > 0))
              || (!_res.hasStreamWriteFD() && _res.hasStreamReadFD() && !_streamEOF)) {
            _readingBlockedFlag = true;

            if (_res.isCGI() && _readBuffer.length() == 0) {
              // CLOSE write fd!
              if (_res.hasStreamWriteFD())
                close(_res.getStreamWriteFD());
              _res.setStreamWriteFD(-1);
              _streamWriteFDClosed = true;
            }

          } else {
            resetState();
            handleReadOneReq();
          }

          return;
        }

        // handle Content-Encoding
        if (_res.getReqEncoding() != Encoding::NONE) {
          _encIncoming->feed(unchunked);
          if (_encIncoming->hasData()) {
            std::string bytes = _encIncoming->take();
            _readBuffer.append(bytes);
            onReadBytes(bytes.length());
          }
        } else {
          _readBuffer.append(unchunked);
          onReadBytes(unchunked.length());
        }
      }
    }
  }
}

// returns true when the caller should return immediatly 
void SocketConnection::onReadBytes(size_t len) {
  _bytesRead += len;
  if (_bytesRead > _res.getBodyLimit()) {
    dropResponse();
    HTTPResponse err = onError(_ctx, STATUS_PAYLOAD_TOO_LARGE);
    std::cout << "[" << _ctx->server().getCurrentWorkerID() << "] Payload too large for " << _fd << "!" << std::endl;
    _res.log(); // std::cout -- comment this line to disable response logging
    _writeBuffer.append(err.asString());
    appendToWriteBuffer(err.getContent());

    // CLOSE write fd!
    if (_res.hasStreamWriteFD())
      close(_res.getStreamWriteFD());
    _res.setStreamWriteFD(-1);
    _streamWriteFDClosed = true;

    _ignoreInput = true;
    _fdChanged = true;
  }
}

void SocketConnection::handleWrite() {

  // for safety (avoid SIGPIPEs!)
  if (_socketEOF)
    return;

  ssize_t sent;
  if ((sent = send(_fd, _writeBuffer.begin().base(), _writeBuffer.length(), 0)) < 0)
    throw SocketException("cannot write to socket"/*, strerror(errno)*/);

  // remove what we sent from the buffer
  _writeBuffer.erase(_writeBuffer.begin(), _writeBuffer.begin() + sent);
}

void SocketConnection::handleStreamRead() {
  if (!_res.hasStreamReadFD())
    return;

  ssize_t len;
  if ((len = read(getStreamingFDRead(), _rawReadBuf, BUFFER_SIZE)) < 0) {
    if (_res.isReverseProxy() && !_hasReadAnythingFromStream)
      resError(STATUS_BAD_GATEWAY);
    throw SocketException("cannot read from file descriptor"/*, strerror(errno)*/);
  }

  _hasReadAnythingFromStream = true;

  if (!len) {
    std::cout << "Stream EOF!" << std::endl;
    _streamEOF = true;

    if ((_res.isCGI() || _res.isReverseProxy()) && !_cgiHeadersArrived) {
      std::cerr << "Remote did not send any header!" << std::endl;
      resError(STATUS_INTERNAL_SERVER_ERROR);
      resetState();
      return;
    }

    if (!_res.isReverseProxy()) {
      if (_streamWriteFDClosed) { // if body is fully sent
        _readingBlockedFlag = false;
        resetState();
        handleReadOneReq();
      } else if (!_res.hasStreamWriteFD() || _readBuffer.length() == 0) {
        resetState(); // Ready to accept a new request!
      }
    }

    return;
  }

  if (!(_res.isCGI() || _res.isReverseProxy()) || _cgiHeadersArrived) {

    // insert new data at the end
    appendToWriteBuffer(std::string(_rawReadBuf, _rawReadBuf + len));

  } else {

    // insert new data at the end of the cgi buffer
    _streamReadBuffer.insert(_streamReadBuffer.end(), _rawReadBuf, _rawReadBuf + len);

    // accept both CRLF and LF
    std::string::size_type posCRLF = _streamReadBuffer.find("\r\n\r\n");
    std::string::size_type posLF = _streamReadBuffer.find("\n\n");

    std::string::size_type pos = std::min(posCRLF, posLF);
    bool isCRLF = posCRLF == pos;
    std::string::size_type headersSepLen = isCRLF ? 4 : 2;

    if (pos != std::string::npos) {
      // we found the headers!

      _cgiHeadersArrived = true;

      std::string headstr = _streamReadBuffer.substr(0, pos + headersSepLen / 2);
      HTTPResponse res;

      if (_res.isCGI()) {
        try {
          if (handleCGIHeaders(headstr))
            return; // we reprocess the request!
        } catch (HTTPParseException e) {
          e.prettyPrint("CGI headers parsing");
          resError(STATUS_BAD_GATEWAY);
          return;
        } catch (CGIException e) {
          e.prettyPrint("CGI headers parsing");
          resError(STATUS_INTERNAL_SERVER_ERROR);
          return;
        } catch (RedirectionException e) {
          e.prettyPrint("CGI headers parsing");
          resError(STATUS_LOOP_DETECTED);
          return;
        } catch (ReverseProxyException e) {
          e.prettyPrint("CGI headers parsing");
          resError(STATUS_LOOP_DETECTED);
          return;
        } catch (EncodingException e) {
          e.prettyPrint("CGI headers parsing");
          resError(STATUS_INTERNAL_SERVER_ERROR);
          return;
        }
      } else {
        // reverse proxy!
        std::cout << headstr << std::endl;
        _writeBuffer.append(headstr); // use the write buffer directly,
        // bypass appendToWriteBuffer function because the response headers should *not* be chunked or encoded
        _writeBuffer.append("\r\n");
      }

      // copy the excess content to the write buffer
      appendToWriteBuffer(std::string(_streamReadBuffer.begin() + pos + headersSepLen, _streamReadBuffer.end()));
    }
  }
}

void SocketConnection::handleStreamWrite() {

  // Avoid SIGPIPEs!
  if (_streamEOF || !_res.hasStreamWriteFD())
    return;

  ssize_t written;
  if ((written = write(getStreamingFDWrite(), _readBuffer.begin().base(), _readBuffer.length())) < 0)
    throw SocketException("cannot write to file descriptor"/*, strerror(errno)*/);

  // remove what we have written from the read buffer
  _readBuffer.erase(_readBuffer.begin(), _readBuffer.begin() + written);

  if (_readingBlockedFlag && _readBuffer.length() == 0) {
    // CLOSE write fd!
    if (_res.hasStreamWriteFD())
      close(_res.getStreamWriteFD());
    _res.setStreamWriteFD(-1);
    _streamWriteFDClosed = true;

    if (!_res.hasStreamReadFD()) {
      _readingBlockedFlag = false;
      resetState();
      handleReadOneReq();
    }
  }
}

bool SocketConnection::shouldDrop() const {
  return _socketEOF ||
    (_headersArrived && !_res.isChunked() && !_res.hasContentLength() &&
      _streamEOF);
}

void SocketConnection::appendToWriteBuffer(std::string const& str) {
  std::string buf = str;

  // apply charset transformation
  if (_charset) {
    _charset->feed(buf);
    if (_charset->hasData())
      buf = _charset->take();
  }

  // apply encodings
  if (_res.getEncoding() != Encoding::NONE && buf.length() > 0) {
    _encOutgoing->feed(buf);
    buf.clear();
    if (_encOutgoing->hasData())
      buf = _encOutgoing->take();
  }

  // apply the chunked transfer encoding
  if (_res.isChunked() && buf.length() > 0) {
    std::stringstream ss;
    ss << std::hex << buf.length()
      << "\r\n" << buf << "\r\n";

    buf = ss.str();
  }

  _writeBuffer.append(buf);
}

void SocketConnection::resError(HTTPStatus status) {
  HTTPResponse res = onError(_ctx, status);
    std::cout << "[" << _ctx->server().getCurrentWorkerID() << "] Error response for " << _fd << "!" << std::endl;
  _res.log(); // std::cout -- comment this line to disable response logging
  std::string str = res.asString();
  std::string str2(res.getContent().begin(), res.getContent().end());
  _writeBuffer.insert(_writeBuffer.end(), str.begin(), str.end());
  _writeBuffer.insert(_writeBuffer.end(), str2.begin(), str2.end());

  _fdChanged = true;
  _ignoreInput = true;
}

// CGI //

// returns true if we should reprocess the request
bool SocketConnection::handleCGIHeaders(std::string const& str) {
  HTTPResponse res;
  HTTPHeaders headers = _res.getHeaders();
  headers.parse(str); // reuse HTTPHeaders parser

  for (HTTPHeaders::iterator ite = headers.begin(); ite != headers.end(); ++ite) {
    if (!strcasecmp(ite->first.c_str(), "Content-Type"))
      res.header("Content-Type", ite->second);
    else if (!strcasecmp(ite->first.c_str(), "Location")) {
      // Location header means redirection

      if (ite->second.find("://") == std::string::npos) { // TODO is this correct?
        // header value is not an URI, we should reprocess the client request
        //  using this new path

        _cgiRedirCount++;
        if (_cgiRedirCount >= MAX_CGI_REDIR_COUNT)
          throw RedirectionException("too many CGI redirections");

        _req.setPath(ite->second.c_str());

        dropResponse(); // kill the CGI connection
        headersArrived(); // reprocess the request

        return true;

      } else {
        // Regular redirection (header value is an URI)
        res.status(STATUS_FOUND)
          .setNoContent()
          .header("Location", ite->second);
      }

    } else if (!strcasecmp(ite->first.c_str(), "Status")) {
      if ((ite->second.length() != 3 && ite->second.find(' ') != 3) || !isdigit(ite->second[0]) ||
          !isdigit(ite->second[1]) || !isdigit(ite->second[1]))
        throw CGIException("Status header must be a 3-digit number");

      res.status(HTTPStatus(atoi(ite->second.c_str())));

      if (res.getStatusText() == "Unknown")
        throw CGIException("Status header is not valid");
    } else {
      res.header(ite->first, ite->second);
    }
  }

  std::string resStr = res.asString();

  // print the response headers to the write buffer
  _writeBuffer.append(resStr);

  return false;
}

static char* toCStr(std::string const& str) {
  char* newStr = new char[str.length() + 1];

  memcpy(newStr, str.c_str(), str.length() + 1);
  return newStr;
}

static char translateHeaderNameToCGI(char c) {
  if (c == '-')
    return '_';
  return toupper(c);
}

std::vector<char const*> SocketConnection::generateCGIEnv() const {
  std::vector<char const*> ctnr;

  // copy environment variables
  char const* const* env = getEnv();
  while (*env)
    ctnr.push_back(toCStr(std::string(*env++)));

  // AUTH_TYPE unset
  ctnr.push_back(toCStr("GATEWAY_INTERFACE=CGI/1.1"));
  ctnr.push_back(toCStr("PATH_INFO=" + _res.getCGIPathInfo()));
  ctnr.push_back(toCStr("PATH_TRANSLATED=" + _res.getCGIPathTranslated()));
  // ctnr.push_back(toCStr("QUERY_STRING=" + req.getQueryString()));
  // ctnr.push_back(toCStr("REMOTE_ADDR=" + req.getClientAddr()));
  // REMOTE_HOST unset
  // REMOTE_IDENT unset
  // REMOTE_USER unset
  ctnr.push_back(toCStr("REQUEST_METHOD=" + std::string(
    getHTTPMethodName(_res.getCGIReq().getMethod()))));
  ctnr.push_back(toCStr("REQUEST_URI=" + std::string(_req.getPath())));
  ctnr.push_back(toCStr("SCRIPT_NAME=" + _res.getCGIScriptName()));
  ctnr.push_back(toCStr("SERVER_NAME=" + _res.getCGIServerName()));
  std::stringstream ss;
  ss << _res.getCGIServerPort();
  ctnr.push_back(toCStr("SERVER_PORT=" + ss.str()));
  ctnr.push_back(toCStr("SERVER_PROTOCOL=HTTP/1.1"));
  ctnr.push_back(toCStr("SERVER_SOFTWARE=webserv/1.0"));

  for (HTTPHeaders::const_iterator ite = _res.getCGIReq().getHeaders().begin();
      ite != _res.getCGIReq().getHeaders().end(); ++ite) {

    if (!strcasecmp(ite->first.c_str(), "Content-Length")) {
      ctnr.push_back(toCStr("CONTENT_LENGTH=" + ite->second));
    } else if (!strcasecmp(ite->first.c_str(), "Content-Type")) {
      ctnr.push_back(toCStr("CONTENT_TYPE=" + ite->second));
    } else {
      std::string translated = ite->first;
      std::transform(translated.begin(), translated.end(),
        translated.begin(), translateHeaderNameToCGI);
      ctnr.push_back(toCStr("HTTP_" + translated + "=" + ite->second));
    }
  }

  ctnr.push_back(NULL);
  return ctnr;
}

static size_t REQUESTS = 0;

void SocketConnection::headersArrived() {
  std::cout << "[" << _ctx->server().getCurrentWorkerID() << "] Handling request for " << _fd << ": (" << REQUESTS++ << ")" << std::endl;
  _req.log();
  _res = onHeadersArrived(_ctx, _req);
  _res.log(); // std::cout -- comment this line to disable response logging

  if (_res.getEncoding() != Encoding::NONE) {
    _encOutgoing = Encoding::create(_res.getEncoding());
    _encOutgoing->init(Encoding::ENCODE);
  }

  if (_res.getReqEncoding() != Encoding::NONE) {
    _encIncoming = Encoding::create(_res.getReqEncoding());
    _encIncoming->init(Encoding::DECODE);
  }

  if (_res.getCharset() != Encoding::NONE_CHARSET) {
    if (_res.getCharset() == Encoding::ISO_TO_UTF8) {
      _charset = new Iso_to_utf8();
      _charset->init(Encoding::ENCODE);
    } else if (_res.getCharset() == Encoding::UTF8_TO_ISO) {
      _charset = new Iso_to_utf8();
      _charset->init(Encoding::DECODE);
    }
  }

  if (_res.isCGI())
    cgiFork();
  else if (_res.isReverseProxy())
    reverseProxyHandle();

  _fdChanged = true;
}

void SocketConnection::cgiFork() {
  FD pipe1[2];
  FD pipe2[2];

  Language* lang = Language::create(_res.getInternalModule());

  if (pipe(pipe1) < 0)
    throw CGIException("cannot create pipe", strerror(errno));

  if (pipe(pipe2) < 0) {
    close(pipe1[0]);
    close(pipe1[1]);
    throw CGIException("cannot create pipe", strerror(errno));
  }

  _cgiChild = fork();

  if (_cgiChild < 0) {
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);
    throw CGIException("cannot fork()", strerror(errno));
  }

  if (_cgiChild == 0) {
    // current process is the child

    std::string dir = std::string(_res.getCGIFilePath().begin(), _res.getCGIFilePath().begin() + _res.getCGIFilePath().find_last_of('/'));
    chdir(dir.c_str());

    close(pipe1[1]);
    close(pipe2[0]);

    dup2(pipe1[0], 0);
    dup2(pipe2[1], 1);

    if (_res.getInternalModule() == Language::NONE) {

      std::vector<char const*> env = generateCGIEnv();
      char* argv[2];
      argv[0] = const_cast<char *>(_res.getCGIFilePath().c_str());
      argv[1] = NULL;

      execve(_res.getCGIFilePath().c_str(), argv, const_cast<char **>(env.begin().base()));

      std::cerr << "EXECVE error: " << strerror(errno) << std::endl;
      exit(1);

    } else {
      lang->execute(_res);

      exit(0);
    }
  }
  // current process is the parent

  close(pipe1[0]);
  close(pipe2[1]);

  fcntl(pipe2[0], F_SETFL, O_NONBLOCK);
  fcntl(pipe1[1], F_SETFL, O_NONBLOCK);

  _res.setStreamReadFD(pipe2[0]);
  _res.setStreamWriteFD(pipe1[1]);

  delete lang;
}

void SocketConnection::reverseProxyHandle() {
  FD sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    throw ReverseProxyException("error while creating socket", strerror(errno));

  fcntl(sock, F_SETFL, O_NONBLOCK);
  sockaddr_in addr;
  addr.sin_addr.s_addr = inet_addr(_res.getReverseProxyIp().c_str());
  addr.sin_family = AF_INET;
  addr.sin_port = htons(_res.getReverseProxyPort());

  if (connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0 && errno != EINPROGRESS)
    throw ReverseProxyException("error while connecting", strerror(errno));

  _readBuffer = _res.getReverseProxyReq().asString();
  // _res.getReverseProxyReq().log();
  _res.setStreamReadFD(sock);
  _res.setStreamWriteFD(sock);
}

bool operator==(SocketConnection const& lhs, SocketConnection const& rhs) {
  return lhs.getFD() == rhs.getFD();
}

bool operator!=(SocketConnection const& lhs, SocketConnection const& rhs) {
  return lhs.getFD() != rhs.getFD();
}
