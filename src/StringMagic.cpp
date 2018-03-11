/**
* Main.cpp
*
* A simple C++ software to retrieve files
* from the Protein Data Bank.
*
* Written by Olivier Béquignon
* Published under the MIT License
*/
#include <algorithm>
#include "StringMagic.hpp"

std::string replace_substring(std::string originalString, std::string stringToReplace, std::string replacementString) {
    size_t index = 0;
    while (true) {
        // Locate the substring to replace.
        index = originalString.find(stringToReplace, index);
        if (index == std::string::npos) break;

        // Make the replacement.
        originalString = originalString.replace(index, stringToReplace.length(), replacementString);

        // Advance index so the next iteration does not pick it up as well.
        index += stringToReplace.length();
    }
    return originalString;
}

std::string remove_char(std::string originalString, std::string charsToRemove) {
    for (int i = 0; i < charsToRemove.length(); ++i) {
        originalString.erase(std::remove(originalString.begin(), originalString.end(), charsToRemove.at(i)), originalString.end());
    }
    return originalString;
}

std::string to_upper(std::string originalString) {
    std::transform(originalString.begin(), originalString.end(), originalString.begin(), ::toupper);

    return originalString;
}

std::string to_lower(std::string originalString) {
    std::transform(originalString.begin(), originalString.end(), originalString.begin(), ::tolower);

    return originalString;

}
