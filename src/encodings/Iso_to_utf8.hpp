/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Iso_to_utf8.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: niduches <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/03 21:30:22 by niduches          #+#    #+#             */
/*   Updated: 2020/05/08 01:45:51 by niduches         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ISO_TO_UTF8_HPP
# define ISO_TO_UTF8_HPP

# include <string>
# include "Encoding.hpp"

class Iso_to_utf8: public Encoding
{
	public:
	Iso_to_utf8(void);
	virtual ~Iso_to_utf8(void);

	virtual void init(Encoding::Direction dir);
	virtual void feed(std::string const& bytes);
	virtual bool hasData(void);
	virtual std::string take(void);
	virtual void end(void);

	private:
	static size_t	get_size_to_utf8(unsigned char *data, size_t size);
	std::string		encode_Iso_to_utf8(std::string iso);
	bool			is_good(unsigned char *data, int size);
	int				nb_coded_char(unsigned char *data, unsigned char *end);
	size_t			get_size_to_iso(unsigned char *data, size_t size);
	char			get_value(unsigned char *start, int nb);
	std::string		decode_utf8_to_iso(std::string utf8);
	void			stock_end(unsigned char *data, unsigned char *end);

	Encoding::Direction	_dir;
	std::string			_content;
	char				_tmp[5];
	int					_size_tmp;
};

#endif /* ISO_TO_UTF8_HPP */
