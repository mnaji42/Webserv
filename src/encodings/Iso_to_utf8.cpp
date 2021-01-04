/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Iso_to_utf8.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/02 01:23:00 by niduches          #+#    #+#             */
/*   Updated: 2020/06/30 17:21:58 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include "Iso_to_utf8.hpp"

Iso_to_utf8::Iso_to_utf8(void): _dir(Encoding::ENCODE), _size_tmp(0)
{
}

Iso_to_utf8::~Iso_to_utf8(void)
{
}

void	Iso_to_utf8::init(Encoding::Direction dir)
{
	_dir = dir;
}

void	Iso_to_utf8::feed(std::string const& bytes)
{
	if (_size_tmp)
	{
		_content += _tmp;
		_size_tmp = 0;
		*_tmp = '\0';
	}
	_content += bytes;
}

bool	Iso_to_utf8::hasData(void)
{
	return (!_content.empty());
}

std::string	Iso_to_utf8::take(void)
{
	std::string	tmp;

	if (_dir == Encoding::ENCODE)
		tmp = encode_Iso_to_utf8(_content);
	else if (_dir == Encoding::DECODE)
		tmp = decode_utf8_to_iso(_content);
	_content.clear();
	return (tmp);
}

void	Iso_to_utf8::end(void)
{
	_size_tmp = 0;
	_tmp[0] = '\0';
}

size_t	Iso_to_utf8::get_size_to_utf8(unsigned char *data, size_t size)
{
	size_t	nb = 0;
	size_t	i = 0;

	while (i < size)
	{
		if (data[i] < 128)
			++nb;
		else
			nb += 2;
		++i;
	}
	return (nb);
}

std::string		Iso_to_utf8::encode_Iso_to_utf8(std::string iso)
{
	size_t	i = 0;
	std::string	output;

	output.reserve(get_size_to_utf8((unsigned char*)iso.c_str(), iso.size()));
	if (!output.capacity())
		return (output);
	while (i < iso.size())
	{
		if ((unsigned char)iso[i] < 128)
			output += (char)iso[i];
		else
		{
			output += (char)(192 + (((unsigned char)iso[i] & 192) >> 6));
			output += (char)(128 + ((unsigned char)iso[i] & 63));
		}
		++i;
	}
	return (output);
}

bool	Iso_to_utf8::is_good(unsigned char *data, int size)
{
	int		i = 0;

	while (i < size)
	{
		if (!((data[i + 1] & 192U) == 128U))
			return (false);
		++i;
	}
	return (true);
}

void	Iso_to_utf8::stock_end(unsigned char *data, unsigned char *end)
{
	_size_tmp = (int)(end - data);

	for (int i = 0; i < _size_tmp; ++i)
		_tmp[i] = (char)*(data + i);
	_tmp[_size_tmp] = '\0';
}

int		Iso_to_utf8::nb_coded_char(unsigned char *data, unsigned char *end)
{
	if (*data < 128)
		return (1);
	else if (*data < 192)
		return (0);
	else if (*data < 224)
	{
		if (data + 1 >= end)
			stock_end(data, end);
		if (!is_good(data, 1))
			return (0);
		return (2);
	}
	else if (*data < 240)
	{
		if (data + 2 >= end)
			stock_end(data, end);
		if (!is_good(data, 2))
			return (0);
		return (3);
	}
	else if (*data < 248)
	{
		if (data + 3 >= end)
			stock_end(data, end);
		if (!is_good(data, 3))
			return (0);
		return (4);
	}
	return (0);
}

size_t	Iso_to_utf8::get_size_to_iso(unsigned char *data, size_t size)
{
	size_t			nb = 0;
	unsigned char	*end = data + size;
	int				tmp;

	while (data != end)
	{
		tmp = nb_coded_char(data, end);
		if (tmp)
			++nb;
		data += (!tmp) ? 1 : tmp;
	}
	return (nb);
}

char		Iso_to_utf8::get_value(unsigned char *start, int nb)
{
	char	tmp = 0;

	if (nb == 1)
		return (*start);
	if (nb == 2)
	{
		tmp = ((((unsigned int)(start[0] & 0x1F)) << 6) |
(((unsigned int)(start[1] & 0x3F))));
	}
	else if (nb == 3)
	{
		tmp = ((((unsigned int)(start[0] & 0x0FU)) << 12) |
(((unsigned int)(start[1] & 0x3FU)) << 6) |
(((unsigned int)(start[2] & 0x3FU))));
	}
	else
	{
		tmp = ((((unsigned int)(start[0] & 0x07U)) << 18) |
(((unsigned int)(start[1] & 0x3FU)) << 12) |
(((unsigned int)(start[2] & 0x3FU)) << 6) | ((unsigned int)(start[3] & 0x3FU)));
	}
	return (tmp ? tmp : 1);
}

std::string		Iso_to_utf8::decode_utf8_to_iso(std::string utf8)
{
	std::string		output;
	unsigned char	*start = (unsigned char*)utf8.c_str();
	unsigned char	*end = start + utf8.size();
	int				tmp;

	output.reserve(get_size_to_iso(start, utf8.size()));
	if (!output.capacity())
		return (output);
	while (start != end)
	{
		tmp = nb_coded_char(start, end);
		if (tmp)
			output += get_value(start, tmp);
		start += (!tmp) ? 1 : tmp;
	}
	return (output);
}
