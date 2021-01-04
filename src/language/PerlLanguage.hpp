/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PerlLanguage.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: niduches <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/29 17:49:22 by niduches          #+#    #+#             */
/*   Updated: 2020/06/11 14:28:15 by niduches         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PERLLANGUAGE_HPP
# define PERLLANGUAGE_HPP

# include "Language.hpp"
# define LUA_BUFFER_SIZE (4 * 1024)

#include <EXTERN.h>
#include <perl.h>

class PerlLanguage: public Language {
  public:
    PerlLanguage();
    virtual ~PerlLanguage();

    virtual void execute(HTTPResponse const& res);
};

#endif /* PERLLANGUAGE_HPP */
