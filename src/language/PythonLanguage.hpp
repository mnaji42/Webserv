/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PythonLanguage.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/18 06:00:01 by skybt             #+#    #+#             */
/*   Updated: 2020/05/08 04:49:23 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PYTHON_LANGUAGE_HPP
# define PYTHON_LANGUAGE_HPP

# include "Language.hpp"
# include <Python.h>
# define PYTHON_BUFFER_SIZE (4 * 1024)

class PythonLanguage: public Language {
  public:
    PythonLanguage();
    virtual ~PythonLanguage();

    virtual void execute(HTTPResponse const& res);
};

#endif
