/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BrotliEncoding.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/27 23:00:57 by niduches          #+#    #+#             */
/*   Updated: 2020/05/04 19:15:32 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BrotliEncoding.hpp"

BrotliEncoding::BrotliEncoding(void): _have(0), _encode_state(NULL), _decode_state(NULL)
{
}

BrotliEncoding::~BrotliEncoding(void)
{
  if (_dir == Encoding::ENCODE)
	  BrotliEncoderDestroyInstance(_encode_state);
  else if (_dir == Encoding::DECODE)
	  BrotliDecoderDestroyInstance(_decode_state);
}

void	BrotliEncoding::set_quality(int quality)
{
	if (_dir != Encoding::ENCODE || quality < BROTLI_MIN_QUALITY || quality > BROTLI_MAX_QUALITY)
		return ;
	BrotliEncoderSetParameter(_encode_state, BROTLI_PARAM_QUALITY, quality);
}

void	BrotliEncoding::init(Encoding::Direction dir) {
	_dir = dir;

	if (_dir == Encoding::ENCODE)
	{
		if (!(_encode_state = BrotliEncoderCreateInstance(0, 0, NULL)))
			throw EncodingException("error while initializing stream");
	}
	else if (_dir == Encoding::DECODE)
	{
		if (!(_decode_state = BrotliDecoderCreateInstance(0, 0, NULL)))
			throw EncodingException("error while initializing stream");
	}
}

void	BrotliEncoding::feed(std::string const& bytes) {
	_avail_in = bytes.length();
	_next_in = (uint8_t*)bytes.begin().base();
	_avail_out = CHUNK - _have;
	_next_out = _buf + _have;
	_total_out = 0;

	if (_dir == Encoding::ENCODE)
	{
		if ((BrotliEncoderCompressStream(_encode_state, BROTLI_OPERATION_PROCESS,
&_avail_in, &_next_in, &_avail_out, &_next_out, &_total_out)) == BROTLI_FALSE)
			throw EncodingException("error while encoding");
	}
	else if (_dir == Encoding::DECODE)
	{
		if ((BrotliDecoderDecompressStream(_decode_state,
&_avail_in, &_next_in, &_avail_out, &_next_out, &_total_out)) == BROTLI_FALSE)
			throw EncodingException("error while decoding", get_error());
	}
	_have = CHUNK - _avail_out;
}

bool	BrotliEncoding::hasData() {
	return (_have > 0);
}

std::string	BrotliEncoding::take() {
	std::string bytes(_buf, _buf + _have);
	_have = 0;
	return (bytes);
}

void	BrotliEncoding::end() {
	_avail_in = 0;
	_avail_out = CHUNK - _have;
	_next_out = _buf + _have;

	if (_dir == Encoding::ENCODE)
	{
		if ((BrotliEncoderCompressStream(_encode_state, BROTLI_OPERATION_FINISH,
&_avail_in, &_next_in, &_avail_out, &_next_out, &_total_out)) == BROTLI_FALSE)
			throw EncodingException("error while encoding/decoding");
	}
	else if (_dir == Encoding::DECODE)
	{
		if ((BrotliDecoderDecompressStream(_decode_state,
&_avail_in, &_next_in, &_avail_out, &_next_out, &_total_out)) == BROTLI_FALSE)
			throw EncodingException("error while encoding/decoding", get_error());
	}
	_have = CHUNK - _avail_out;
}

const char	*BrotliEncoding::get_error(void)
{
	if (_dir == Encoding::ENCODE)
		return (NULL);
	else if (_dir == Encoding::DECODE)
		return (BrotliDecoderErrorString(BrotliDecoderGetErrorCode(_decode_state)));
	return (NULL);
}
