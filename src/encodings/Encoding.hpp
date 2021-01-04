/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Encoding.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/31 20:05:18 by skybt             #+#    #+#             */
/*   Updated: 2020/05/11 17:48:04 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ENCODING
# define ENCODING

# include "../Util.hpp"
# include "../exception/Exception.hpp"

class Encoding {
  public:
    Encoding() {}
    virtual ~Encoding() {}

    enum Direction {
      ENCODE,
      DECODE,
    };

    virtual void init(Direction dir) = 0;
    virtual void feed(std::string const& bytes) = 0;
    virtual bool hasData() = 0;
    virtual std::string take() = 0;
    virtual void end() = 0;

    enum Type {
      UNKNOWN = -1,
      NONE = 0,
      DEFLATE,
      GZIP,
      BROTLI,
    };

    enum Charset {
      UNKNOWN_CHARSET = -1,
      NONE_CHARSET = 0,
      UTF8_TO_ISO,
      ISO_TO_UTF8,
    };

    static bool isSupported(Type type);

    // throws when type is not supported
    static Encoding* create(Type type);

    // returns Encoding::UNKNOWN when type is not known
    //  the empty string and NULL maps to Encoding::NONE
    static Type typeByString(char const* str);

    // returns NULL for Encoding::NONE
    //  throws for Encoding::UNKNOWN and invalid values
    static char const* typeToString(Type type);

    static char const* debugName(Type type);
    static char const* charsetDebugName(Charset charset);
};

#endif
