/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BrotliEncoding.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/27 22:56:34 by niduches          #+#    #+#             */
/*   Updated: 2020/05/08 01:45:41 by niduches         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BROTLIENCODING_HPP
# define BROTLIENCODING_HPP

# define CHUNK 16384
# include <brotli/encode.h>
# include <brotli/decode.h>
# include <brotli/port.h>
# include <brotli/types.h>
# include "Encoding.hpp"

class BrotliEncoding: public Encoding {
	size_t				_avail_in;
	const uint8_t		*_next_in;
	size_t				_avail_out;
	uint8_t				*_next_out;
	size_t				_total_out;
	size_t				_have;
	uint8_t				_buf[CHUNK];
	Encoding::Direction	_dir;
	BrotliEncoderState	*_encode_state;
	BrotliDecoderState	*_decode_state;

	const char	*get_error(void);

	public:
	BrotliEncoding();
	virtual ~BrotliEncoding();

	virtual void init(Encoding::Direction dir);
	virtual void feed(std::string const& bytes);
	virtual bool hasData();
	virtual std::string take();
	virtual void end();
	void	set_quality(int quality = BROTLI_DEFAULT_QUALITY);
};

#endif /* BROTLIENCODING_HPP */
