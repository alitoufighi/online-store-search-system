#include <string>
#include <sstream>
#include <cstdlib>
#include "defines.hpp"
#include "globals.hpp"
using namespace std;

std::string sort_type;
int sort_field_index, num_of_fields;

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
    locale loc;
    string res = "";
    for (string::size_type i = 0; i < str.length(); ++i)
        res += tolower(str[i], loc);
    return res;
}

// used in sort
// determines if a string is a number or not
bool is_num(string s1) {
    for (size_t i = 0; i < s1.length(); ++i)
        if (s1[i] > '9' || s1[i] < '0')
            return false;
    return true;
}

// used in sort
bool compare(string s1, string s2) {
    stringstream ss1(s1), ss2(s2);
    string field1, field2;
    int i = 0;
    while (i < num_of_fields) {
        getline(ss1, field1, WS);
        getline(ss2, field2, WS);
        if (i == sort_field_index) {
            if (is_num(field1) && is_num(field2)) {
                int f1 = atoi(field1.c_str()), f2 = atoi(field2.c_str());
                return sort_type == ASCEND ? (f1 < f2) : (f1 > f2);
            } else
                return sort_type == ASCEND ? (field1 < field2) : (field1 > field2);
        }
        i++;
    }
    return false; // if sorting not provided by user, this does not do anything :D
}
