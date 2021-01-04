/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LuaLanguage.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/18 06:00:01 by skybt             #+#    #+#             */
/*   Updated: 2020/04/18 20:12:54 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LUA_LANGUAGE_HPP
# define LUA_LANGUAGE_HPP

# include "Language.hpp"
# define LUA_BUFFER_SIZE (4 * 1024)

extern "C" {
# include <lua.h>
# include <lauxlib.h>
# include <lualib.h>
}

class LuaLanguage: public Language {
  public:
    LuaLanguage();
    virtual ~LuaLanguage();

    virtual void execute(HTTPResponse const& res);
};

#endif
