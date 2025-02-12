#include <sstream>
#include <string>
#include <iostream>
#include "../includes/utils.hpp"

std::string normalizeSpaces(const std::string& input)
{
	std::string result;
	bool inSpaces = false;

	for (std::string::const_iterator it = input.begin(); it != input.end(); ++it) {
		char ch = *it;

		if (ch == ' ') {
			if (!inSpaces) {
				result += ' ';
				inSpaces = true;
			}
		} else {
			result += ch;
			inSpaces = false;
		}
	}

	return result;
}