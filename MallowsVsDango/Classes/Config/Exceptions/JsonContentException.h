#pragma once
#include <iostream>
#include <exception>


class JsonContentException : public std::exception
{
public:
	JsonContentException(std::string error) : error(error) {}

	virtual const char* what() const throw() {
		return error.c_str();
	}

private:
	std::string error;
};