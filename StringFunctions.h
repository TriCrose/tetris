#ifndef STRINGFUNCTIONS_H
#define STRINGFUNCTIONS_H

#include <string>
#include <sstream>

template<class T> std::string ToString(T in) {
    std::stringstream ss;
    ss << in;
    return ss.str();
}

#endif // STRINGFUNCTIONS_H
