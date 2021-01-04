/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Encoding.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/13 02:02:23 by skybt             #+#    #+#             */
/*   Updated: 2020/05/11 17:58:17 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Encoding.hpp"

#ifdef ENCODING_GZIP
# include "GZipEncoding.hpp"
#endif
#ifdef ENCODING_DEFLATE
# include "DeflateEncoding.hpp"
#endif
#ifdef ENCODING_BROTLI
# include "BrotliEncoding.hpp"
#endif

Encoding* Encoding::create(Encoding::Type type) {
  switch (type) {
    case Encoding::NONE:
      return NULL;
#ifdef ENCODING_GZIP
    case Encoding::GZIP:
      return new GZipEncoding();
#endif
#ifdef ENCODING_DEFLATE
    case Encoding::DEFLATE:
      return new DeflateEncoding();
#endif
#ifdef ENCODING_BROTLI
    case Encoding::BROTLI:
      return new BrotliEncoding();
#endif
    default:
      throw EncodingException("unsupported encoding");
  }
}

bool Encoding::isSupported(Encoding::Type type) {
  switch (type) {
    case Encoding::NONE:
      return true;
#ifdef ENCODING_GZIP
    case Encoding::GZIP:
      return true;
#endif
#ifdef ENCODING_DEFLATE
    case Encoding::DEFLATE:
      return true;
#endif
#ifdef ENCODING_BROTLI
    case Encoding::BROTLI:
      return true;
#endif
    default:
      return false;
  }
}

Encoding::Type Encoding::typeByString(char const* str) {
  if (!str || !strcmp(str, ""))
    return Encoding::NONE;
  if (!strcmp(str, "deflate"))
    return Encoding::DEFLATE;
  if (!strcmp(str, "gzip"))
    return Encoding::GZIP;
  if (!strcmp(str, "br"))
    return Encoding::BROTLI;
  if (!strcmp(str, "identity"))
    return Encoding::NONE;
  return Encoding::UNKNOWN;
}

char const* Encoding::typeToString(Encoding::Type type) {
  switch (type) {
    case Encoding::NONE:
      return NULL;
    case Encoding::DEFLATE:
      return "deflate";
    case Encoding::GZIP:
      return "gzip";
    case Encoding::BROTLI:
      return "br";
    default:
      throw EncodingException("unsupported encoding");
  }
}

char const* Encoding::debugName(Encoding::Type type) {
  switch (type) {
    case Encoding::NONE:
      return "None";
    case Encoding::DEFLATE:
      return "Deflate";
    case Encoding::GZIP:
      return "GZip";
    case Encoding::BROTLI:
      return "Brotli";
    default:
      return "Unknown";
  }
}

char const* Encoding::charsetDebugName(Encoding::Charset type) {
  switch (type) {
    case Encoding::NONE_CHARSET:
      return "None";
    case Encoding::ISO_TO_UTF8:
      return "ISO_TO_UTF8";
    case Encoding::UTF8_TO_ISO:
      return "UTF8_TO_ISO";
    default:
      return "Unknown";
  }
}
