/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Language.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/17 16:45:51 by skybt             #+#    #+#             */
/*   Updated: 2020/05/11 17:39:15 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LANGUAGE_HPP
# define LANGUAGE_HPP

# include "../Util.hpp"

class HTTPResponse;

class Language {
  public:
    Language() {}
    virtual ~Language() {}

    enum Lang {
      NONE = -1,
      PHP,
      PERL,
      JAVASCRIPT,
      PYTHON,
      LUA,
    };

    virtual void execute(HTTPResponse const& res) = 0;

    static Language* create(Lang lang);
    static bool isSupported(Lang lang);
    static char const* debugName(Lang lang);
};

# include "../http/HTTP.hpp"

#endif
