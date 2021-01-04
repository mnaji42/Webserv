/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   folderResponse.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/25 01:45:26 by najimehdi         #+#    #+#             */
/*   Updated: 2020/06/30 17:16:39 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Handler.hpp"
# include "../Context.hpp"

std::string addURL(std::string str)
{
  for (std::string::iterator it = str.begin(); it != str.end(); it++)
  {
		if (*it == '"' || *it == '\\' || *it == '\'')
		{
			it = str.insert(it, '\\');
			it++;
		}
    else if (*it == ' ')
    {
      it = str.insert(it, '%');
			it++;
      it = str.insert(it, '2');
			it++;
      *it = '0';
    }
  }
  return (str);
}

//Mon Oct 21 15:30:56 2019
//21-Oct-2019 13:30

std::string dateLastModif(char *date)
{
  std::string ret;

  ret.push_back(date[8]);
  ret.push_back(date[9]);
  ret.push_back('-');
  ret.push_back(date[4]);
  ret.push_back(date[5]);
  ret.push_back(date[6]);
  ret.push_back('-');
  ret.push_back(date[20]);
  ret.push_back(date[21]);
  ret.push_back(date[22]);
  ret.push_back(date[23]);
  ret.push_back(' ');
  ret.push_back(date[11]);
  ret.push_back(date[12]);
  ret.push_back(':');
  ret.push_back(date[14]);
  ret.push_back(date[15]);
  
  return ret;
}

std::string folderContentHTML(std::string &streamFile, HTTPRequest const& req, std::string &indexOf)
{
  struct dirent *read;
  DIR *path;
  std::string hostPath = "http://" + req.getHeaders()["Host"] + req.getPath();
  std::string contentHTML = "<html><head><title>Index of " + indexOf + "</title></head><body><h1>Index of " + indexOf + "</h1><hr><pre>";
  
  path = opendir(streamFile.c_str());
  read = readdir(path);
  read = readdir(path);
  contentHTML += "\n<a href=\"" + addSlash(hostPath) + addURL(read->d_name) + "\">" + read->d_name + "/</a>";
  while ((read = readdir(path))) 
  {
    if (read->d_name[0] == '.')
      continue;
    struct stat buff;
    // buff.st_mtimespec.tv_sec;
    if (stat((addSlashStr(streamFile) + read->d_name).c_str(), &buff) == -1) {
      closedir(path);
      return "";
    }
    contentHTML += "\n<a href=\"" + addSlash(hostPath) + addURL(read->d_name) + "\">";
    std::string newLine = read->d_name ;
    if (S_ISDIR(buff.st_mode))
      newLine += "/</a>";
    else
      newLine += "</a>";
    while (newLine.size() < 55)
      newLine.push_back(' ');
    newLine += dateLastModif(ctime(&buff.ST_MTIME.tv_sec)); //mtime pour linux !!
    if (S_ISDIR(buff.st_mode))
      newLine += "                    -";
    else
    {
      std::stringstream ss;
      ss << buff.st_size;
      while (newLine.size() < 93 - ss.str().size())
        newLine.push_back(' ');
      newLine += ss.str();
    }
    contentHTML += newLine;
  }
  contentHTML += "</pre><hr></body></html>";
  closedir(path);
  return contentHTML;
}

HTTPResponse& folderResponseGet(HTTPResponse& res, std::string &date, std::string &streamFile, \
  HTTPRequest const& req, std::vector<std::string> &errorPage,\
  std::string &indexOf)
{
  struct stat buff;
  if (stat((streamFile).c_str(), &buff) == -1)
    return errorResponse(res, false, STATUS_NOT_FOUND, date, errorPage);

  std::string contentHTML;
  if ((contentHTML = folderContentHTML(streamFile, req, indexOf)) == "")
    return errorResponse(res, false, STATUS_NOT_FOUND, date, errorPage);

  return res.status(STATUS_OK)
    .content(contentHTML)
    .header("Content-Type", "text/html")
    .header("Last-Modified", popNewLine(ctime(&buff.ST_MTIME.tv_sec)));
}

HTTPResponse& folderResponseHead(HTTPResponse& res, std::string &date, std::string &streamFile, \
  HTTPRequest const& req, std::vector<std::string> &errorPage)
{
  (void)req;
  struct stat buff;
  
  if (stat((streamFile).c_str(), &buff) == -1)
    return errorResponse(res, true, STATUS_NOT_FOUND, date, errorPage);
  // std::string contentHTML;
  // if ((contentHTML = folderContentHTML(streamFile, req)) == "")
  //   return errorResponse(STATUS_NOT_FOUND, date, errorPage);

  return res.status(STATUS_OK)
		.setNoContent()
    .header("Content-Type", "text/html")
    .header("Last-Modified", popNewLine(ctime(&buff.ST_MTIME.tv_sec)));
}