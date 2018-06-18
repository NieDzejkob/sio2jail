#pragma once

#include <string>
#include <vector>

namespace s2j {

/**
 * Creates a c-style string and returns pointer it. Caller is responsible for
 * calling delete on returned pointer.
 */
char* stringToCStr(const std::string& str);

/**
 * Splits string using given delimeter.
 */
std::vector<std::string> split(const std::string& str, const std::string& delimeter);

/**
 * Creates a temporary directory and returns path to it.
 *
 * As it uses mktemp, last six characters of template must be XXXXXX.
 */
std::string createTemporaryDirectory(const std::string& directoryTemplate = "/tmp/sio2jail-XXXXXX");

/**
 * Default to_string in s2j namespace, usefull for various overloads.
 */
template<typename T>
std::string to_string(const T& t) {
    return std::to_string(t);
}

bool checkKernelVersion(int major, int minor);

}
