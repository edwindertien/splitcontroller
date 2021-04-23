#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "Arduino.h"

class CSVIterator
{
    public:

        CSVIterator::CSVIterator(std::istream& str)  :m_str(str.good()?&str:NULL) { ++(*this); }
        CSVIterator::CSVIterator()                   :m_str(NULL) {}

        // Pre Increment
        CSVIterator::CSVIterator& operator++()               {if (m_str) { if (!((*m_str) >> m_row)){m_str = NULL;}}return *this;}
        // Post increment
        CSVIterator::CSVIterator operator++(int)             {CSVIterator    tmp(*this);++(*this);return tmp;}
        CSVIterator::CSVRow const& operator*()   const       {return m_row;}
        CSVIterator::CSVRow const* operator->()  const       {return &m_row;}

        bool operator==(CSVIterator const& rhs) {return ((this == &rhs) || ((this->m_str == NULL) && (rhs.m_str == NULL)));}
        bool operator!=(CSVIterator const& rhs) {return !((*this) == rhs);}
};



CSVRange::CSVIterator begin() const {return CSVIterator{stream};}
CSVRange::CSVIterator end()   const {return CSVIterator{};}


int main()
{
    std::ifstream       file("plop.csv");

    for(auto& row: CSVRange(file))
    {
        std::cout << "4th Element(" << row[3] << ")\n";
    }
}
