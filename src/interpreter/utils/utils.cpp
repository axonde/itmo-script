#include "utils.h"

namespace Utils {

template<typename Comp>
std::vector<std::string> Split(const std::string& string, const Comp& comp) {
    std::vector<std::string> splitted_string;
    int l = 0;
    for (int r = 0; r != string.size() + 1; r++) {
        if (r == string.size() || comp(string[r])) {
            splitted_string.push_back(string.substr(l, r - l));
            l = r + 1;
        }
    }
    return splitted_string;
}

} // end Utils
