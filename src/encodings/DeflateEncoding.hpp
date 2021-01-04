/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeflateEncoding.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/31 20:05:18 by skybt             #+#    #+#             */
/*   Updated: 2020/04/15 18:55:25 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFLATE_ENCODING
# define DEFLATE_ENCODING

# define CHUNK 16384
# include "Encoding.hpp"
# include "DeflateEncoding.hpp"
# include <zlib.h>

class DeflateEncoding: public Encoding {
  z_stream _strm;
  size_t _have;
  uint8_t _buf[CHUNK];
  Encoding::Direction _dir;

  public:
    DeflateEncoding();
    virtual ~DeflateEncoding();

    void trueInit(Encoding::Direction dir, bool gzip); // called to init as a GZip duplex stream instead of Deflate

    virtual void init(Encoding::Direction dir);
    virtual void feed(std::string const& bytes);
    virtual bool hasData();
    virtual std::string take();
    virtual void end();
};

#endif
