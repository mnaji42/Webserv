/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilsHandler.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/25 01:49:49 by najimehdi         #+#    #+#             */
/*   Updated: 2020/06/30 17:17:49 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Handler.hpp"
# include "../Context.hpp"

std::string getNameOfScript(std::string streamFile)
{
	size_t i = streamFile.find_last_of('/');
	return streamFile.substr(i + 1);
}

bool checkCgi2(std::vector<std::string> &cgi, std::string &streamFile)
{
	size_t i = streamFile.find_last_of('.');
	for (size_t j = 0; j < cgi.size(); j++)
		if (streamFile.substr(i + 1) == cgi[j])
			return true;
	return false;
}

std::string popSlashStr(std::string str)
{
  if (str[0] == '/')
    str.erase(str.begin());
  return str;
}

std::string addSlash(std::string &str)
{
  if (str[str.size() - 1] != '/')
    str.push_back('/');
  return str;
}

std::string addSlashStr(std::string str)
{
  if (str[str.size() - 1] != '/')
    str.push_back('/');
  return str;
}

bool compareSlash(std::string str1, std::string str2)
{
  if (addSlash(str1) == addSlash(str2))
    return true;
  return false;
}

bool compareSlash(Location_config const &Lc, std::string str2)
{
  if (Lc == addSlash(str2))
    return true;
  str2.erase(str2.end() - 1);
  if (Lc == addSlash(str2))
    return true;
  return false;
}

int checkLocation(Server_config const &s, std::string path, std::string &aliasLocation)
{
  while (path.size() > 0)
  {
    for (size_t i = 0; i < s.nb_location(); i++)
      if (compareSlash(s.get_location(i), path))
      {
        aliasLocation = s.get_location(i).get_location_arg().first;
        return (int)i;
      }
    do
    {
      path.erase(path.end() - 1);
    } while (path.size() > 0 && path[path.size() - 1] != '/');
  }
  return -1;
}

bool checkIndex(Location_config const &l, std::string &fileStream, 
            std::vector<std::string> &index)
{
  struct stat buff;

  for (int i = 0; i < Location_config::NB_KEYWORD; ++i)
    if (compareSlash(l.get_keyword(i), "index"))
      for (size_t j = 0; j < l.get_arg(i).size(); j++)
      {
        if (j == l.get_arg(i).size() - 1 && stat((fileStream + l.get_arg(i)[j]).c_str(), &buff))
          return false;
        if (stat((addSlash(fileStream) + popSlashStr(l.get_arg(i)[j])).c_str(), &buff))
          continue;
        fileStream += l.get_arg(i)[j];
        return true;
      }

  for (size_t j = 0; j < index.size(); j++)
  {
    if (!stat((addSlash(fileStream) + popSlashStr(index[j])).c_str(), &buff))
    {
      fileStream += index[j];
      return true;
    }
  }
  if (!stat((addSlash(fileStream) + "index.html").c_str(), &buff))
    fileStream += "index.html";
  return true;
}

void getPath_alias(std::string &streamFile, std::string &root,\
 std::string &alias, std::string &location, std::string path)
{
  if (location[0] == '/') 
    location.erase(location.begin());
  if (path[0] == '/') 
    path.erase(path.begin());
  size_t i = 0;
  while (location[i] == path[i] && path[i] != '\0')
    i++;
  path = path.substr(i);
  addSlash(alias);
  streamFile = addSlash(root) + popSlashStr(alias) + popSlashStr(path);
}

void checkPath(Server_config const &s, std::string path, 
            std::vector<std::string> &index,\
            bool &auto_index, std::vector<std::string> &errorPage,\
            std::string &root, std::vector<std::string> &methods, std::vector<std::string> &reverse_proxy,\
            std::vector<std::string> &int_module, std::string &charset, std::string &source_charset,\
            size_t &max_body_size, std::pair<bool, t_alias> &alias, t_cgi &cgi)
{
  //checkPath and Location

  alias.first = 0;

  int i = checkLocation(s, path, alias.second.location);
  if (i != -1)
  {
    for (int j = 0; j < Location_config::NB_KEYWORD; ++j)
    {
      if (!strcmp(s.get_location(i).get_keyword(j), "auto_index"))
      {
        if (!s.get_location(i).get_arg(j).empty())
        {
          if (s.get_location(i).get_arg(j)[0] == "on")
            auto_index = true;
          else if (s.get_location(i).get_arg(j)[0] == "off")
            auto_index  = false;
        }
      }
      else if (!strcmp(s.get_location(i).get_keyword(j), "cgi") && \
          !s.get_location(i).get_arg(j).empty())
      {
        cgi.ext = s.get_location(i).get_arg(j);
      }
      else if (!strcmp(s.get_location(i).get_keyword(j), "cgi_methods") && \
          !s.get_location(i).get_arg(j).empty())
      {
        cgi.methods = s.get_location(i).get_arg(j);
      }
      else if (!strcmp(s.get_location(i).get_keyword(j), "cgi_root") && \
          !s.get_location(i).get_arg(j).empty())
      {
        cgi.root = s.get_location(i).get_arg(j)[0];
      }
      else if (!strcmp(s.get_location(i).get_keyword(j), "index") && \
          !s.get_location(i).get_arg(j).empty())
      {
        index = s.get_location(i).get_arg(j);
      }
      else if (!strcmp(s.get_location(i).get_keyword(j), "error_page") && \
          !s.get_location(i).get_arg(j).empty())
      {
        errorPage = s.get_location(i).get_arg(j);
      }
      else if (!strcmp(s.get_location(i).get_keyword(j), "root") && \
          !s.get_location(i).get_arg(j).empty())
      {
        root = s.get_location(i).get_arg(j)[0];
      }
      else if (!strcmp(s.get_location(i).get_keyword(j), "methods") && \
          !s.get_location(i).get_arg(j).empty())
      {
        methods = s.get_location(i).get_arg(j);
      }
      else if (!strcmp(s.get_location(i).get_keyword(j), "reverse_proxy") && \
          !s.get_location(i).get_arg(j).empty())
      {
        reverse_proxy = s.get_location(i).get_arg(j);
      }
      else if (!strcmp(s.get_location(i).get_keyword(j), "internal_module") && \
          !s.get_location(i).get_arg(j).empty())
      {
        int_module = s.get_location(i).get_arg(j);
      }
      else if (!strcmp(s.get_location(i).get_keyword(j), "charset") && \
          !s.get_location(i).get_arg(j).empty())
      {
        charset = s.get_location(i).get_arg(j)[0];
      }
      else if (!strcmp(s.get_location(i).get_keyword(j), "source_charset") && \
          !s.get_location(i).get_arg(j).empty())
      {
        source_charset = s.get_location(i).get_arg(j)[0];
      }
      else if (!strcmp(s.get_location(i).get_keyword(j), "client_max_body_size") && \
          !s.get_location(i).get_arg(j).empty())
      {
        max_body_size = string_to_unsigned(s.get_location(i).get_arg(j)[0]);
      }
      else if (!strcmp(s.get_location(i).get_keyword(j), "alias") && \
          !s.get_location(i).get_arg(j).empty())
      {
        alias.first = 1;
        alias.second.alias = s.get_location(i).get_arg(j)[0];
      }
    }
  }
}

char *popNewLine(char *line)
{
  int i = 0;
  while (line[i] != '\n')
    i++;
  line[i] = 0;
  return line;
}

void checkEncoding(std::string &encoding)
{
	size_t start;
	size_t end = 0;
  std::vector<std::string> v;

	while ((start = encoding.find_first_not_of(' ', end)) != std::string::npos)
	{
		end = encoding.find(' ', start);
		v.push_back(encoding.substr(start, end - start - 1));
	}
  for (size_t i = 0; i < v.size(); i++)
    if (Encoding::isSupported(Encoding::typeByString(v[i].c_str())))
    {
        encoding = v[i];
        return ;
    }
  encoding = "";
}

bool checkMethods(std::vector<std::string> &methods, HTTPMethod currentMethod)
{
  if (methods.empty())
    return true;

  std::string currMethod;
  if (currentMethod == HTTP_METHOD_OPTIONS)
    currMethod = "OPTIONS";
  else if (currentMethod == HTTP_METHOD_GET)
    currMethod = "GET";
  else if (currentMethod == HTTP_METHOD_HEAD)
    currMethod = "HEAD";
  else if (currentMethod == HTTP_METHOD_POST)
    currMethod = "POST";
  else if (currentMethod == HTTP_METHOD_PUT)
    currMethod = "PUT";
  else if (currentMethod == HTTP_METHOD_DELETE)
    currMethod = "DELETE";
  else if (currentMethod == HTTP_METHOD_TRACE)
    currMethod = "TRACE";
  else if (currentMethod == HTTP_METHOD_CONNECT)
    currMethod = "CONNECT";

  for (size_t i = 0; i < methods.size(); i++)
    if (methods[i] == currMethod)
      return true;
  
  return false;
}

void create_directory(std::string streamFile)
{
  std::string buff;
  std::vector<std::string> spl = split(streamFile, "/");

  buff = spl[0];
  struct stat buffStat;
  for (size_t i = 1; i < spl.size() - 1; i++)
  {
    buff += "/" + spl[i];
    if (stat(buff.c_str(), &buffStat) == -1)
    {
      if (mkdir(buff.c_str(), 0777) == -1)
        throw BaseException("error while mkdir", strerror(errno));
      usleep(5000000);
    }
  }
}

bool checkCgi(t_cgi &cgi, std::string &streamFile, HTTPMethod currentMethod)
{
  if (!checkCgi2(cgi.ext, streamFile))
    return 0;
  if (!cgi.methods.empty() && !checkMethods(cgi.methods, currentMethod))
    return 0;
  return 1;
}