/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/18 16:29:44 by skybt             #+#    #+#             */
/*   Updated: 2020/06/15 15:38:22 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include "../Context.hpp"

SocketServer::SocketServer(Context* ctx):
   _sockets(), _connections(), _ctx(ctx) {}

void SocketServer::start() {
  _connections.reserve(100);

  // Spawn workers

  int workerCount = atoi(_ctx->config().get_arg(Config::WORKERS)[0].c_str());

  _currentWorkerID = 0;
  int workersSpawned = 1;
  while (workersSpawned < workerCount) {
    pid_t pid = fork();

    if (pid < 0)
      throw SocketException("cannot fork()", strerror(errno));

    if (pid == 0) {
      _currentWorkerID = workersSpawned;
      break;
    }

    workersSpawned++;
  }

  // Create sockets

  Http_config const& conf = _ctx->config().get_http();

  for (size_t i = 0; i < conf.nb_server(); i++) {
    Server_config const& serv = conf.get_server(i);

    std::vector<std::string> const& listenDirective = serv.get_arg(Server_config::LISTEN);
    if (listenDirective.empty())
      continue;

    std::string const& arg = listenDirective[0];
    int port = atoi(arg.c_str());

    bool alreadyBound = false;
    for (std::vector<std::pair<int, FD> >::iterator ite = _sockets.begin();
        ite != _sockets.end(); ++ite) {

      if (ite->first == port) {
        alreadyBound = true;
        break;
      }
    }
    if (alreadyBound)
      continue;

    FD fd;
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
      throw SocketException("cannot create socket", strerror(errno));

    fcntl(fd, F_SETFL, O_NONBLOCK);

    // SO_REUSEADDR

    int opt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
        &opt, sizeof(opt)) < 0)
      throw SocketException("cannot set socket opt", strerror(errno));

    // SO_REUSEPORT

    opt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT,
        &opt, sizeof(opt)) < 0)
      throw SocketException("cannot set socket opt", strerror(errno));

    // Address

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(fd, (sockaddr*)&addr, sizeof(addr)) < 0)
      throw SocketException("cannot bind socket", strerror(errno));

    if (listen(fd, SERVER_BACKLOG) < 0)
      throw SocketException("cannot mark socket as listen", strerror(errno));

    _sockets.push_back(std::make_pair(port, fd));
    std::cout << "[" << _currentWorkerID << "] Listening on port " << port << "!" << std::endl;
  }

  ioLoop();
}

void SocketServer::ioLoop() {
  fd_set rfds;
  fd_set wfds;

  while (1) {
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);

    FD highestFd = -1;
    for (std::vector<std::pair<int, FD> >::iterator ite = _sockets.begin();
        ite != _sockets.end(); ++ite) {

      FD_SET(ite->second, &rfds);
      highestFd = std::max(ite->second, highestFd);
    }

    for (std::vector<SocketConnection*>::iterator ite = _connections.begin();
        ite != _connections.end(); ++ite) {

      SocketConnection& conn = **ite;

      conn.resetFDChanged();

      highestFd = std::max(conn.getFD(), highestFd);
      highestFd = std::max(conn.getStreamingFDRead(), highestFd);
      highestFd = std::max(conn.getStreamingFDWrite(), highestFd);

      if (conn.elligibleForSocketRead())
        FD_SET(conn.getFD(), &rfds);
      if (conn.elligibleForSocketWrite())
        FD_SET(conn.getFD(), &wfds);
      if (conn.elligibleForStreamRead())
        FD_SET(conn.getStreamingFDRead(), &rfds);
      if (conn.elligibleForStreamWrite())
        FD_SET(conn.getStreamingFDWrite(), &wfds);

      // std::cout << conn.getFD() << ": -- (" << _connections.size() << ")" << std::endl
      //   << ">> Socket Read " << conn.elligibleForSocketRead() << std::endl
      //   << ">> Socket Write " << conn.elligibleForSocketWrite() << std::endl
      //   << ">> File Read " << conn.elligibleForStreamRead() << std::endl
      //   << ">> File Write " << conn.elligibleForStreamWrite() << std::endl;
    }

    int ret;
    if ((ret = select(highestFd + 1, &rfds, &wfds, NULL, NULL)) < 0)
      throw SocketException("cannot select()", strerror(errno));

    for (std::vector<SocketConnection*>::iterator ite = _connections.begin();
        ite != _connections.end();) {

      SocketConnection& conn = **ite;

      bool drop = false;

      if (FD_ISSET(conn.getFD(), &rfds))
        drop = connRead(**ite);
      if (!drop && !conn.hasAnyFDChanged() && FD_ISSET(conn.getFD(), &wfds))
        drop = connWrite(**ite);
      if (!drop && !conn.hasAnyFDChanged() && FD_ISSET(conn.getStreamingFDRead(), &rfds))
        drop = connStreamRead(**ite);
      if (!drop && !conn.hasAnyFDChanged() && FD_ISSET(conn.getStreamingFDWrite(), &wfds))
        drop = connStreamWrite(**ite);

      if (drop || conn.shouldDrop()) {
        std::cout << "[" << _currentWorkerID << "] Connection " << conn.getFD() << " dropped!" << std::endl;
        close(conn.getFD());
        conn.dropResponse();
        delete *ite;
        ite = _connections.erase(ite);
      } else {
        ++ite;
      }
    }

    for (std::vector<std::pair<int, FD> >::iterator ite = _sockets.begin();
        ite != _sockets.end(); ++ite) {
      if (FD_ISSET(ite->second, &rfds))
        acceptConn(*ite);
    }
  }
}

// these functions returns true when an unhandled error occurs and the connection should be dropped

bool SocketServer::acceptConn(std::pair<int, FD> socket) {
  try {
    std::cout << "[" << _currentWorkerID << "] Accepting a connection" << std::endl;
    _connections.push_back(new SocketConnection(_ctx, socket.first));
    _connections.back()->acceptConnection(socket.second);
    return false;
  } catch (BaseException& e) {
    e.prettyPrint("accepting connection");
  } catch (std::bad_alloc& e) {
    std::cerr << "ALLOCATION ERROR! Context: while accepting a connection" << std::endl;
  }
  return true;
}

bool SocketServer::connRead(SocketConnection& conn) {
  try {
    conn.handleRead();
    return false;
  } catch (BaseException& e) {
    e.prettyPrint("reading from socket");
  } catch (std::bad_alloc& e) {
    std::cerr << "ALLOCATION ERROR! Context: while reading from socket" << std::endl;
  }
  return true;
}

bool SocketServer::connWrite(SocketConnection& conn) {
  try {
    conn.handleWrite();
    return false;
  } catch (BaseException& e) {
    e.prettyPrint("writing to socket");
  } catch (std::bad_alloc& e) {
    std::cerr << "ALLOCATION ERROR! Context: while writing to socket" << std::endl;
  }
  return true;
}

bool SocketServer::connStreamRead(SocketConnection& conn) {
  try {
    conn.handleStreamRead();
    return false;
  } catch (BaseException& e) {
    e.prettyPrint("reading from file/cgi/remote");
  } catch (std::bad_alloc& e) {
    std::cerr << "ALLOCATION ERROR! Context: while reading from file/cgi/remote" << std::endl;
  }
  return true;
}

bool SocketServer::connStreamWrite(SocketConnection& conn) {
  try {
    conn.handleStreamWrite();
    return false;
  } catch (BaseException& e) {
    e.prettyPrint("writing to file/cgi/remote");
  } catch (std::bad_alloc& e) {
    std::cerr << "ALLOCATION ERROR! Context: while writing to file/cgi/remote" << std::endl;
  }
  return true;
}
