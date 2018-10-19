#pragma once
#include <iostream>
#include <exception>

class TowerTypeException : public std::exception {
public:
	TowerTypeException(std::string error) : error(error) {}

	virtual const char* what() const throw() {
		return error.c_str();
	}

private:
	std::string error;
};