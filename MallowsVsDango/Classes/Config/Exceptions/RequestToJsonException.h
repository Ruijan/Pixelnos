#pragma once
#include <iostream>
#include <exception>


class RequestToJsonException : public std::exception
{
public:
	RequestToJsonException(std::string error): error(error) {

	}

	virtual const char* what() const throw()
	{
		return error.c_str();
	}

private:
	std::string error;
};