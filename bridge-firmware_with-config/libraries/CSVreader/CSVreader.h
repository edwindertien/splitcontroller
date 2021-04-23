#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "Arduino.h"

class CSVreader {
  public:
      typedef std::input_iterator_tag     iterator_category;
      typedef CSVRow                      value_type;
      typedef std::size_t                 difference_type;
      typedef CSVRow*                     pointer;
      typedef CSVRow&                     reference;
  private:
      std::istream*       m_str;
      CSVRow              m_row;
};

class CSVRange
{
    std::istream&   stream;
};
