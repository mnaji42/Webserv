#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <unistd.h>

void create_directory(std::string streamFile)
{
  std::stringstream ss(streamFile);
  std::string buff;
  std::vector<std::string> split;
  while (std::getline(ss, buff, '/'))
    split.push_back(buff);

  buff = split[0];
  struct stat buffStat;
  for (int i = 1; i < split.size(); i++)
  {
    if (stat(buff.c_str(), &buffStat) == -1)
	{
      if (mkdir(buff.c_str(), 777) == -1)
		  printf("ERROR mkdir 1\n");
	  usleep(800000);
	}
	buff += "/" + split[i];
  }
  if (stat(buff.c_str(), &buffStat) == -1)
  {
      if (mkdir(buff.c_str(), 777) == -1)
		  printf("ERROR mkdir 2\n");
	  usleep(100000);
  }
}

int main()
{
	std::string streamFile = "./directory/bonjour/test";
	struct stat buff;
	// while (stat(streamFile.c_str(), &buff) == -1)
		create_directory(streamFile);
	// mkdir("bonjour/aaa", 777);
	int fd = open("./directory/bonjour/test/lololo", O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
	if (fd == -1)
		std::cout << "ERROR OPEN\n";
	close(fd);
	return 0;
}