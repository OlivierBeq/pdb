/**
* Main.hpp
*
* A simple C++ software to retrieve files
* from the Protein Data Bank.
*
* Written by Olivier Béquignon
* Published under the MIT License
*/
#ifndef STRINGMAGIC_HPP
#define STRINGMAGIC_HPP

#include <string>

std::string replace_substring(std::string originalString, std::string stringToReplace, std::string replacementString);
std::string remove_char(std::string originalString, std::string charsToRemove);
std::string to_upper(std::string originalString);
std::string to_lower(std::string originalString);

#endif