/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Language.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/17 16:54:01 by skybt             #+#    #+#             */
/*   Updated: 2020/06/07 07:02:46 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Language.hpp"

#ifdef LANGUAGE_PHP
# include "PHPLanguage.hpp"
#endif
#ifdef LANGUAGE_LUA
# include "LuaLanguage.hpp"
#endif
#ifdef LANGUAGE_PYTHON
# include "PythonLanguage.hpp"
#endif
#ifdef LANGUAGE_PERL
# include "PerlLanguage.hpp"
#endif
#ifdef LANGUAGE_JAVASCRIPT
# include "JavaScriptLanguage.hpp"
#endif

Language* Language::create(Language::Lang lang) {
  switch (lang) {
    case Language::NONE:
      return NULL;
#ifdef LANGUAGE_PHP
    case Language::PHP:
      return new PHPLanguage();
#endif
#ifdef LANGUAGE_LUA
    case Language::LUA:
      return new LuaLanguage();
#endif
#ifdef LANGUAGE_PYTHON
    case Language::PYTHON:
      return new PythonLanguage();
#endif
#ifdef LANGUAGE_PERL
    case Language::PERL:
      return new PerlLanguage();
#endif
#ifdef LANGUAGE_JAVASCRIPT
    case Language::JAVASCRIPT:
      return new JavaScriptLanguage();
#endif
    default:
      throw LanguageException("unsupported language module");
  }
}

bool Language::isSupported(Language::Lang lang) {
  switch (lang) {
#ifdef LANGUAGE_PHP
    case Language::PHP:
      return true;
#endif
#ifdef LANGUAGE_LUA
    case Language::LUA:
      return true;
#endif
#ifdef LANGUAGE_PYTHON
    case Language::PYTHON:
      return true;
#endif
#ifdef LANGUAGE_PERL
    case Language::PERL:
      return true;
#endif
#ifdef LANGUAGE_JAVASCRIPT
    case Language::JAVASCRIPT:
      return true;
#endif
    default:
      return false;
  }
}

char const* Language::debugName(Language::Lang lang) {
  switch (lang) {
    case Language::NONE:
      return "None";
    case Language::PHP:
      return "PHP";
    case Language::LUA:
      return "Lua";
    case Language::PYTHON:
      return "Python";
    case Language::PERL:
      return "Perl";
    case Language::JAVASCRIPT:
      return "JavaScript";
    default:
      return "Unknown";
  }
}
