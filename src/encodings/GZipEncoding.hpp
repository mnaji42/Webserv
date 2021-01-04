/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GZipEncoding.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/31 20:05:18 by skybt             #+#    #+#             */
/*   Updated: 2020/04/15 19:00:55 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GZIP_ENCODING
# define GZIP_ENCODING

# include "DeflateEncoding.hpp"

class GZipEncoding: public Encoding {
  DeflateEncoding _enc;

  public:
    GZipEncoding();
    virtual ~GZipEncoding();

    virtual void init(Encoding::Direction dir);
    virtual void feed(std::string const& bytes);
    virtual bool hasData();
    virtual std::string take();
    virtual void end();
};

#endif
