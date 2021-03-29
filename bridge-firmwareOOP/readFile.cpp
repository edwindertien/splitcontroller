#include <iostream>
#include <fstream>

using namespace std;

#include<vector>
#include<map>

std::map<std::string,uint8_t> test1Map =
{
  {"joyRXax2", 2},
  {"joyRYax1", 3},
  {"joyRZb11", 4},
};

std::map<std::string,uint8_t> test2Map =
{
  {"joyRXar2", 6},
  {"joyRYax1", 7},
  {"joyRZb11", 2},
};

void checkMap(std::map<string, string> &destMap, std::map<string, string> &srcMap); //forward declaration

int main() {
  //readConfigFile();
  checkMap(test1Map,test2Map);
  return 0;
}


/*
would like to read key value pairs from a file, while ignoring comment lines.

imagine a file like:

key1=value1
#ignore me!
*/
void readConfigFile() {
  ifstream myFile_Handler;
  string myLine;
  vector <string> lines;

  // File Open in the Read Mode
  myFile_Handler.open("File_1.config");

  if(myFile_Handler.is_open())
  {
    typedef std::map<std::string, std::string> ConfigInfo;
    ConfigInfo configValues;
    std::string line;
     while (std::getline(fileStream, line))
     {
         std::istringstream is_line(line);
         std::string key;
         if (std::getline(is_line, key, '='))
         {
             std::string value;
             if (key[0] == '#')
                 continue;

             if (std::getline(is_line, value))
             {
                 configValues[key] = value;
             }
         }
     }
     myFile_Handler.close();
   }
   else
   {
       cout << "Unable to open the file!";
   }
 }

void checkMap(std::map<string, string> &destMap, std::map<string, string> &srcMap) {
  std::map<std::string,std::string>::iterator it;

  for ( auto srcEntry = srcMap.begin(),
      destEntry = destMap.begin();
      srcEntry != srcMap.end();
      ++srcEntry , ++destEntry //pre-increment might produce better code
    )
{
  //do something
  std::cout << srcEntry->first << " => " << srcEntry->second << endl;
  std::cout << destEntry->first << " => " << srcEntry->second << endl;
}
/*
  for (it = destMap.begin(); it != destMap.end(); it++)
  {
      pinMode(it->second, OUTPUT);
      digitalWrite(it->second, HIGH);

  }
*/
}
