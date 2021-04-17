/*
* 2021 Lonn,  github.com/lonnwolf031
*
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

class ReadFile {

private:

  typedef std::map<std::string, std::string> ConfigInfo;
  std::ifstream myFile_Handler;
  std::string myLine;
  std::vector <std::string> lines;
  std::string line;
  std::istringstream is_line(std::string);
  std::string key;
  std::string value;


public:

  ReadFile();
  ~ReadFile();
  //void                                    checkMap(std::map<std::string, uint8_t>&, std::map<std::string, std::string>&);
  std::map<std::string,std::string>       readConfigFile();

};
