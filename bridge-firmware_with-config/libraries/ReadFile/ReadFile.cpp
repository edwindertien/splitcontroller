/*
* 2021 Lonn,  github.com/lonnwolf031
*
*/

#include <iostream>
#include <fstream>
#include "ReadFile.h"

//using namespace std;

#include <string>
#include <vector>
#include <map>

std::map<std::string,uint8_t> m_ps4_actions =
{
  {"joyRXax2", 2},
  {"joyRYax1", 3},
  {"joyRZb11", 4},
  {"joyLZb12", 7},
  {"joyLYax4", 6},
  {"joyLXax3", 5},
  {"sqbutton", 26},
  {"circbutton", 28},
  {"psbutton", 30},
  {"l2button", 32},
  {"r2button", 34},
  {"tributton", 36},
  {"xbutton", 38},
  {"lhat", 40},
  {"rhat", 42},
  {"uhat", 44},
  {"dhat", 46},
  {"r1button6", 48},
  {"l1button5", 50},
  {"optbutton",  52}
};

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


int main() {
  ReadFile myRead;
  //myRead.checkMap(test1Map, test2Map);
  std::map<std::string,std::string> cfgValues = myRead.readConfigFile();
  myRead.checkMap(test1Map, cfgValues);
  return 0;
}

ReadFile::ReadFile() {
}

ReadFile::~ReadFile() {
}

std::map<std::string,std::string> ReadFile::readConfigFile() {
  std::map<std::string,std::string> configValues;
  // File Open in the Read Mode
  myFile_Handler.open("File_1.config");
  if(myFile_Handler.is_open())
  {
    //ConfigInfo configValues;

     while (std::getline(myFile_Handler, line))
     {

         if (std::getline(myFile_Handler, key, '='))
         {
             if (key[0] == '#') {
               // get text after #, if left or right and save
               std::cout << "Found # " << std::endl;
             }
             else if (key[0] == '$') {
                  // get text after $, find color and save
                std::cout << "Found $ " << std::endl;
             }
             if (std::getline(myFile_Handler, value))
             {
               std::cout << value << std::endl;
               configValues.insert({key, value});;
                // configValues[key] = value;
             }
         }
     }
     myFile_Handler.close();
   }
   else
   {
       std::cout << "Unable to open the file!";
   }
   return configValues;
 }

/*
void ReadFile::checkMap(std::map<std::string, uint8_t> &psMap, std::map<std::string, std::string> &customMap) {
  /*
  for (auto const& x : customMap)
  {
    std::cout << x.first  // string (key)
              << ':'
              << x.second // string's value
              << std::endl;
  }


}


    std::map<std::string,std::string>::iterator custMapIt = customMap.begin();
    while (custMapIt != customMap.end())
    {
        std::cout << custMapIt->first << ":" << custMapIt->second << std::endl;
        itPsMap = customMap.find('b');
        if (itPsMap != customMap.end())
          // here was erase
        custMapIt++;
    }
}
*/
