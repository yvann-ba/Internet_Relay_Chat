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

int countWords(const std::string &str)
{
    std::istringstream stream(str);
    std::string word;
    int count = 0;

    while (stream >> word) {
        count++;
    }
    return count;
}

bool isDisplayable(const std::string &str) {
    for (size_t i = 0; i < str.length(); i++) {
        if (!isprint(str[i])) {
            return false;
        }
    }
    return true;
}