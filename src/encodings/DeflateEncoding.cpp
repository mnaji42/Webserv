/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeflateEncoding.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/12 21:44:21 by skybt             #+#    #+#             */
/*   Updated: 2020/04/12 21:46:46 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./DeflateEncoding.hpp"

DeflateEncoding::DeflateEncoding(): _have(0) {}

DeflateEncoding::~DeflateEncoding() {
  if (_dir == Encoding::ENCODE)
    (void)deflateEnd(&_strm);
  else if (_dir == Encoding::DECODE)
    (void)inflateEnd(&_strm);
}

void DeflateEncoding::trueInit(Encoding::Direction dir, bool gzip) {
  _dir = dir;

  _strm.zalloc = NULL;
  _strm.zfree = NULL;

  int windowBits = 15 + (gzip ? 16 : 0);

  int ret;
  switch (_dir) {
    case Encoding::ENCODE:
      ret = deflateInit2(&_strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, windowBits, 8, Z_DEFAULT_STRATEGY);
      break;
    case Encoding::DECODE:
      ret = inflateInit2(&_strm, windowBits);
      break;
  }

  if (ret < Z_OK)
    throw EncodingException("error while initializing stream", _strm.msg);
}

void DeflateEncoding::init(Encoding::Direction dir) {
  trueInit(dir, false);
}

void DeflateEncoding::feed(std::string const& bytes) {
  _strm.avail_in = bytes.length();
  _strm.next_in = (uint8_t *)bytes.begin().base();
  _strm.avail_out = CHUNK - _have;
  _strm.next_out = _buf + _have;

  int ret;
  switch (_dir) {
    case Encoding::ENCODE:
      ret = deflate(&_strm, Z_NO_FLUSH);
      break;
    case Encoding::DECODE:
      ret = inflate(&_strm, Z_NO_FLUSH);
      break;
  }

  if (ret < Z_OK)
    throw EncodingException("error while encoding/decoding", _strm.msg);

  _have = CHUNK - _strm.avail_out;
}

bool DeflateEncoding::hasData() {
  return _have > 0;
}

std::string DeflateEncoding::take() {
  std::string bytes(_buf, _buf + _have);
  _have = 0;
  return bytes;
}

void DeflateEncoding::end() {
  _strm.avail_in = 0;
  _strm.avail_out = CHUNK - _have;
  _strm.next_out = _buf + _have;

  int ret;
  switch (_dir) {
    case Encoding::ENCODE:
      ret = deflate(&_strm, Z_STREAM_END);
      break;
    case Encoding::DECODE:
      ret = inflate(&_strm, Z_STREAM_END);
      break;
  }

  if (ret < Z_OK)
    throw EncodingException("error while encoding/decoding", _strm.msg);

  _have = CHUNK - _strm.avail_out;
}

