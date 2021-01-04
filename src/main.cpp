/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: najimehdi <najimehdi@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/18 16:35:17 by skybt             #+#    #+#             */
/*   Updated: 2020/06/17 00:01:47 by najimehdi        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Context.hpp"
#include <fstream>

void strreplace(std::string &str, const std::string search, const std::string replace )
{
  for(size_t pos = 0; ; pos += replace.length())
  {
    pos = str.find( search, pos );
    if(pos == std::string::npos)
          break;
    str.erase( pos, search.length() );
    str.insert( pos, replace);
  }
}

int launchDefaultConf()
{
  std::ifstream in("model-default.conf");
  std::ofstream out("default.conf");
  std::string line;

  char cwd[200];
  if (getcwd(cwd, sizeof(cwd)) == NULL) {
      perror("getcwd() error");
      return 1;
  }

  while (getline(in, line))
  {
      strreplace(line, "<pwd>", cwd);
      out << line << std::endl;
  }
  return 0;
}

int main(int argc, char** argv, char** env) {
  if (argc != 2)
  {
    if (argc == 1)
    {
      if (launchDefaultConf())
        return 1;
    }
    else
    {
      std::cout << "usage : ./webserv file.conf" << std::endl;
      return 0;
    }
  }
  
  saveEnv(env);
  Context ctx;

  try {
    if (argc == 2)
      ctx.setConfig(new Config(argv[1]));
    else
      ctx.setConfig(new Config("default.conf"));
    Config& config = ctx.config();
    std::cout << "---------------- config : ---------------" << std::endl;

    display_config(config);

    ctx.setServer(new SocketServer(&ctx));
    SocketServer& serv = ctx.server();

    serv.start();

  } catch (BaseException& e) {
    e.prettyPrint("initializing server");
  } catch (std::exception& e) {
    std::cout << "Unknown exception: " << e.what() << std::endl;
  }
}
