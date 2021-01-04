/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exception.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/17 19:07:55 by skybt             #+#    #+#             */
/*   Updated: 2020/05/12 04:11:43 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXCEPTION_HPP
# define EXCEPTION_HPP

# include "../Util.hpp"

class BaseException: public std::exception {
  private:
    char const* _expName;
    char const* _what;
    char const* _syserror;

  public:
    BaseException(char const* expName, char const* reason, char const* syserr = NULL):
      _expName(expName), _what(reason), _syserror(syserr) {}

    virtual char const* what() const throw() {
      return _what;
    }

    virtual void prettyPrint(char const* context = NULL) const {
      std::cerr << _expName;
      if (context)
        std::cerr << " (" << context << ")";
      std::cerr << ": " << _what;
      if (_syserror)
        std::cerr << " (" << _syserror << ")";
      std::cerr << std::endl;
    }
};

class InternalException: public BaseException {
  public:
    InternalException(char const* reason, char const* syserr = NULL):
      BaseException("InternalException", reason, syserr) {}
};

class HTTPParseException: public BaseException {
  public:
    HTTPParseException(char const* reason, char const* syserr = NULL):
      BaseException("HTTPParseException", reason, syserr) {}
};

class RedirectionException: public BaseException {
  public:
    RedirectionException(char const* reason, char const* syserr = NULL):
      BaseException("RedirectionException", reason, syserr) {}
};

class CGIException: public BaseException {
  public:
    CGIException(char const* reason, char const* syserr = NULL):
      BaseException("CGIException", reason, syserr) {}
};

class SocketException: public BaseException {
  public:
    SocketException(char const* reason, char const* syserr = NULL):
      BaseException("SocketException", reason, syserr) {}
};

class ReverseProxyException: public BaseException {
  public:
    ReverseProxyException(char const* reason, char const* syserr = NULL):
      BaseException("ReverseProxyException", reason, syserr) {}
};

class LanguageException: public BaseException {
  public:
    LanguageException(char const* reason, char const* syserr = NULL):
      BaseException("LanguageException", reason, syserr) {}
};

class EncodingException: public BaseException {
  public:
    EncodingException(char const* reason, char const* syserr = NULL):
      BaseException("EncodingException", reason, syserr) {}
};

#endif
