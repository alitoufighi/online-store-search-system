#include <string>
#include <sstream>
#include "defines.hpp"
using namespace std;

// removes space from a c_string
string remove_space(char* line) {
    char* i = line, *j = line;
    while (*j != '\0') {
        *i = *j++;
        if(*i != WS)
            i++;
    }
    *i = '\0';
    return line;
}

// converts a string to lowercase
string lower(string str) {
    std::locale loc;
    string res = "";
    for (std::string::size_type i=0; i<str.length(); ++i)
        res += tolower(str[i],loc);
    return res;
}

// used in sort
// determines if a string is a number or not
bool is_num(string s1) {
    for (int i = 0; i < s1.length(); ++i)
        if (s1[i] > '9' || s1[i] < '0')
            return false;
    return true;
}