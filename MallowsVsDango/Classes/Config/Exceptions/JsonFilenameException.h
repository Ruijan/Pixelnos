#pragma once

#include <iostream>
#include <exception>

class JsonFilenameException : public std::exception {
public:
	JsonFilenameException(std::string filename) :
		filename(filename)
	{}

	virtual const char* what() const throw() // my call to the std exception class function (doesn't nessasarily have to be virtual).
	{
		return std::string("Json file " + filename + " does not exists.").c_str(); // my error message
	}
protected:
	std::string filename;
};