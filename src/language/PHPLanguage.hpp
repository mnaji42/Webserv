/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PHPLanguage.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/17 16:57:48 by skybt             #+#    #+#             */
/*   Updated: 2020/04/18 05:42:05 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHP_LANGUAGE_HPP
# define PHP_LANGUAGE_HPP

# include "Language.hpp"
# include <php_config.h>
# undef HAVE_ASM_GOTO
# include <Zend/zend.h>
# include <main/php.h>
# include <sapi/embed/php_embed.h>

class PHPLanguage: public Language {
  public:
    PHPLanguage();
    virtual ~PHPLanguage();

    virtual void execute(HTTPResponse const& res);
};

#endif
