/*
 ***********************************************************************
 * Note: This library has been deprecated in favour of the C++ String  *
 * Toolkit Library (StrTk).                                            *
 * URL: http://www.partow.net/programming/strtk/index.html             *
 ***********************************************************************
 */

#include "StringTokenizer.h"

StringTokenizer::StringTokenizer(const std::string& _str, const std::string& _delim)
{
    if ((_str.length() == 0) || (_delim.length() == 0)) return;

    token_str = _str;
    delim = _delim;

    /*
      Remove sequential delimiter
    */
    unsigned int curr_pos = 0;

    while (true) {
        if ((curr_pos = token_str.find(delim, curr_pos)) <= token_str.length()) {
            curr_pos += delim.length();

            while (token_str.find(delim, curr_pos) == curr_pos) {
                token_str.erase(curr_pos, delim.length());
            }
        } else
            break;
    }

    /*
      Trim leading delimiter
    */
    if (token_str.find(delim, 0) == 0) {
        token_str.erase(0, delim.length());
    }

    /*
      Trim ending delimiter
    */
    curr_pos = 0;
    if ((curr_pos = token_str.rfind(delim)) <= token_str.length()) {
        if (curr_pos != (token_str.length() - delim.length())) return;
        token_str.erase(token_str.length() - delim.length(), delim.length());
    }
}

int StringTokenizer::countTokens()
{
    // Early return for empty strings
    if (token_str.empty()) {
        return 0;
    }

    // Start with 1 token (a string with n delimiters has n+1 tokens)
    int num_tokens = 1;
    size_t curr_pos = 0;

    // Use proper string::npos comparison for "not found" condition
    while ((curr_pos = token_str.find(delim, curr_pos)) != std::string::npos) {
        num_tokens++;
        curr_pos += delim.length();
    }

    return num_tokens;
}

bool StringTokenizer::hasMoreTokens()
{
    return (token_str.length() > 0);
}

std::string StringTokenizer::nextToken()
{
    if (token_str.length() == 0) return "";

    std::string tmp_str = "";
    unsigned int pos = token_str.find(delim, 0);

    if (pos <= token_str.length()) {
        tmp_str = token_str.substr(0, pos);
        token_str = token_str.substr(pos + delim.length(), token_str.length() - pos);
    } else {
        tmp_str = token_str.substr(0, token_str.length());
        token_str = "";
    }
    return tmp_str;
}

int StringTokenizer::nextIntToken()
{
    return atoi(nextToken().c_str());
}

double StringTokenizer::nextFloatToken()
{
    return atof(nextToken().c_str());
}

std::string StringTokenizer::nextToken(const std::string& delimiter)
{
    if (token_str.length() == 0) return "";

    std::string tmp_str = "";
    unsigned int pos = token_str.find(delimiter, 0);

    if (pos <= token_str.length()) {
        tmp_str = token_str.substr(0, pos);
        token_str = token_str.substr(pos + delimiter.length(), token_str.length() - pos);
    } else {
        tmp_str = token_str.substr(0, token_str.length());
        token_str = "";
    }

    return tmp_str;
}

std::string StringTokenizer::remainingString()
{
    return token_str;
}

std::string StringTokenizer::filterNextToken(const std::string& filter_string)
{
    std::string tmp_str = nextToken();
    unsigned int currentPos = 0;

    while ((currentPos = tmp_str.find(filter_string, currentPos)) <= token_str.length()) {
        tmp_str.erase(currentPos, filter_string.length());
    }

    return tmp_str;
}
