/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GZipEncoding.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/12 21:44:21 by skybt             #+#    #+#             */
/*   Updated: 2020/04/15 19:00:30 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./GZipEncoding.hpp"

GZipEncoding::GZipEncoding(): _enc() {}
GZipEncoding::~GZipEncoding() {}

void GZipEncoding::init(Encoding::Direction dir) {
  _enc.trueInit(dir, true);
}

void GZipEncoding::feed(std::string const& bytes) {
  _enc.feed(bytes);
}

bool GZipEncoding::hasData() {
  return _enc.hasData();
}

std::string GZipEncoding::take() {
  return _enc.take();
}

void GZipEncoding::end() {
  _enc.end();
}

