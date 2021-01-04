/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/18 16:28:54 by skybt             #+#    #+#             */
/*   Updated: 2020/06/15 13:07:57 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

# define SERVER_BACKLOG 1024

// the read() buffer size
# define BUFFER_SIZE (1024 * 128)

// maximum number of reprocessing a request can do
# define MAX_CGI_REDIR_COUNT 5

# include "../encodings/Encoding.hpp"
# include "../handler/Handler.hpp"
# include "../encodings/Iso_to_utf8.hpp"

class Context;

class SocketConnection {
	// the file descriptor of the socket
	FD _fd;

	// the port of the socket that accepted the connection
	int _serverPort;

	// the address of the socket
	sockaddr_in _addr;

	// app config & global state
	Context* _ctx;

	std::string _readBufferEncoded;

	// the buffer in which we will store everything that has been read
	//  but not processed yet
	std::string _readBuffer;

	// the buffer in which we will store everything that should be
	//  written in the socket
	std::string _writeBuffer;

	// whether or not the http headers of the request have arrived yet
	bool _headersArrived;

	// the HTTP requests
	HTTPRequest _req;

	// the HTTP response
	//  warning: do not use when _headersArrived is false, because
	//  we haven't decided how to answer yet
	HTTPResponse _res;

	// true when the last read from the socket returned EOF
	bool _socketEOF;

	// true when the CGI headers have arrived
	bool _cgiHeadersArrived;

	// the CGI / reverse proxy read buffer
	//  (used to store headers before they have fully arrived)
	std::string _streamReadBuffer;

	// number of processed CGI redirections
	int _cgiRedirCount;

	// the PID of the child process
	pid_t _cgiChild;

	bool _hasReadAnythingFromStream;

	Encoding* _encOutgoing;

	Encoding* _encIncoming;

	Encoding* _charset;

	size_t _bytesRead;

	char* _rawReadBuf;

	bool _readingBlockedFlag;

	bool _streamEOF;

	bool _streamWriteFDClosed;

	bool _ignoreInput;

	bool _fdChanged;

	private:
		bool handleCGIHeaders(std::string const& str);
		std::vector<char const*> generateCGIEnv() const;
		void resError(HTTPStatus status); // answer with an error
		void headersArrived();
		void reverseProxyHandle();
		void cgiFork();
		void onReadBytes(size_t len);
		void resetState();
		void appendToWriteBuffer(std::string const& str);
		void handleReadOneReq();

	public:
		SocketConnection(Context* ctx, int port):
			_fd(-1), _serverPort(port), _ctx(ctx),
			_readBufferEncoded(), _readBuffer(),
			_writeBuffer(), _headersArrived(false), _socketEOF(false),
			_encOutgoing(NULL), _encIncoming(NULL), _charset(NULL),
			_rawReadBuf(NULL), _ignoreInput(false), _fdChanged(false) {}

		~SocketConnection() {
			delete[] _rawReadBuf;
			delete _encOutgoing;
			delete _encIncoming;
			delete _charset;
		}

		// called when we should kill the CGI child if there is one
		void dropResponse();

		// this will call accept() on the socket server file descriptor
		//  to initialize this object
		void acceptConnection(FD socketServer);

		// true when we should be reading what the client is sending
		//  at this time
		bool elligibleForSocketRead() const;

		// true when there is something to write to the client
		bool elligibleForSocketWrite() const;

		// true when we should be reading from the read stream fd
		bool elligibleForStreamRead() const;

		// true when we should be writing to the write stream fd
		bool elligibleForStreamWrite() const;

		// called when we should read read from the socket
		void handleRead();

		// called when we should write to the socket
		void handleWrite();

		// called when we should read something from the read stream fd
		void handleStreamRead();

		// called when we should write to the stream fd
		void handleStreamWrite();

		// returns true when the connection should be dropped
		bool shouldDrop() const;

		void resetFDChanged();

		bool hasAnyFDChanged() const;

		FD getFD() const { return _fd; }
		int getServerPort() const { return _serverPort; }
		FD getStreamingFDRead() const { return _res.getStreamReadFD(); }
		FD getStreamingFDWrite() const { return _res.getStreamWriteFD(); }
};

bool operator==(SocketConnection const& lhs, SocketConnection const& rhs);
bool operator!=(SocketConnection const& lhs, SocketConnection const& rhs);

class SocketServer {
	std::vector<std::pair<int, FD> > _sockets; // [(port, fd)]
	std::vector<SocketConnection*> _connections;
	int _currentWorkerID;
	Context* _ctx;

	private:
		bool acceptConn(std::pair<int, FD> socket);
		bool connRead(SocketConnection& conn);
		bool connWrite(SocketConnection& conn);
		bool connStreamRead(SocketConnection& conn);
		bool connStreamWrite(SocketConnection& conn);
		void ioLoop();

	public:
		SocketServer(Context* ctx);

		void start();

		int getCurrentWorkerID() const { return _currentWorkerID; }
};

#endif
