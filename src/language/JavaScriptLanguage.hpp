/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JavaScriptLanguage.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/18 06:00:01 by skybt             #+#    #+#             */
/*   Updated: 2020/06/06 09:33:47 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef JAVASCRIPT_LANGUAGE_HPP
# define JAVASCRIPT_LANGUAGE_HPP

# include "Language.hpp"
# define JAVASCRIPT_BUFFER_SIZE (4 * 1024)

class JavaScriptLanguage: public Language {
  public:
    JavaScriptLanguage();
    virtual ~JavaScriptLanguage();

    virtual void execute(HTTPResponse const& res);
};

#endif
