/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Context.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: najimehdi <najimehdi@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/30 20:01:10 by skybt             #+#    #+#             */
/*   Updated: 2020/05/11 17:13:12 by najimehdi        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONTEXT_HPP
# define CONTEXT_HPP

# include "socket/Socket.hpp"
# include "config/Config.hpp"

#if __APPLE__
  #define ST_MTIME st_mtimespec
  #define MSG_NOSIGNAL 0
#elif __linux__
  #define ST_MTIME st_mtim
#elif __unix__ // all unices not caught above
  #define ST_MTIME st_mtim
#else
#   error "Unknown compiler"
#endif

class Context {
  Config* _config;
  SocketServer* _server;

  public:
    Context(): _config(NULL), _server(NULL) {}
    ~Context() {
      delete _server;
      delete _config;
    }

    void setServer(SocketServer *const server) { _server = server; }
    void setConfig(Config *const config) { _config = config; }

    SocketServer const& server() const { return *_server; };
    SocketServer& server() { return *_server; };

    Config const& config() const { return *_config; };
    Config& config() { return *_config; };
};

#endif
