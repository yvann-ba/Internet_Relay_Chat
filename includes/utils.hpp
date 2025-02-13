#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

std::string normalizeSpaces(const std::string &input);
int         countWords(const std::string &str);
bool        isDisplayable(const std::string &str);

#endif