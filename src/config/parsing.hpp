/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dnicolas </var/spool/mail/dnicolas>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/10 20:02:50 by dnicolas          #+#    #+#             */
/*   Updated: 2020/05/09 13:43:56 by niduches         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSING_HPP
# define PARSING_HPP

# include <iostream>
# include <string>
# include <vector>

int		is_space(const char *str, size_t start);
std::string	get_next_word(std::pair<std::string const&, size_t&> file);
std::string	pass_next_word(std::pair<std::string const&, size_t&> file);
size_t		pass_spaces(std::pair<std::string const&, size_t&> file);
std::vector<std::string>	pass_arg_value(
std::pair<std::string const&, size_t&> file);
bool	check_workers_argument(std::vector<std::string> arg);
bool	check_max_internal_redir_count_argument(std::vector<std::string> arg);
bool	check_error_page_argument(std::vector<std::string> arg);
bool	check_client_max_body_size_argument(std::vector<std::string> arg);
bool	check_root_argument(std::vector<std::string> arg);
bool	check_listen_argument(std::vector<std::string> arg);
bool	check_server_names_argument(std::vector<std::string> arg);
bool	check_index_argument(std::vector<std::string> arg);
bool	check_methods_argument(std::vector<std::string> arg);
bool	check_auto_index_argument(std::vector<std::string> arg);
bool	check_cgi_argument(std::vector<std::string> arg);
bool	check_encoding_argument(std::vector<std::string> arg);
bool	check_proxi_pass_argument(std::vector<std::string> arg);

#endif /* PARSING_HPP */
