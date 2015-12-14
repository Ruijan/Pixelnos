#ifndef  _FUNCTIONS_H_
#define  _FUNCTIONS_H_

#include <iostream>
#include <vector>
#include <sstream>

template <typename T>
std::string to_string(T value)
{
  //create an output string stream
  std::ostringstream os ;

  //throw the value into the string stream
  os << value ;

  //convert the string stream into a string and return
  return os.str() ;
}
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);


#endif
