#include "common/TextUtil.h"

#include <algorithm>
#include <cctype>

namespace text {

std::string trim(const std::string& s) {
    const char* ws = " \t\r\n";
    const std::size_t begin = s.find_first_not_of(ws);
    if (begin == std::string::npos) return "";
    const std::size_t end = s.find_last_not_of(ws);
    return s.substr(begin, end - begin + 1);
}

std::string toLower(const std::string& s) {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return result;
}

std::string normalizeKey(const std::string& s) {
    std::string result;
    for (unsigned char c : s) {
        if (c == '-' || std::isspace(c)) continue;
        result.push_back(static_cast<char>(std::toupper(c)));
    }
    return result;
}

std::string formatWon(long long value) {
    const bool negative = value < 0;
    const unsigned long long magnitude = negative
        ? 0ULL - static_cast<unsigned long long>(value)
        : static_cast<unsigned long long>(value);
    const std::string digits = std::to_string(magnitude);

    std::string out;
    int count = 0;
    for (auto it = digits.rbegin(); it != digits.rend(); ++it) {
        if (count > 0 && count % 3 == 0) out.push_back(',');
        out.push_back(*it);
        ++count;
    }
    if (negative) out.push_back('-');
    std::reverse(out.begin(), out.end());
    return out;
}

}  // namespace text
