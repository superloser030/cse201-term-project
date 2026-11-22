#ifndef TEXT_UTIL_H
#define TEXT_UTIL_H

#include <string>

// 문자열 보조 함수
namespace text {

std::string trim(const std::string& s);           // 앞뒤 공백·CR·LF 제거
std::string toLower(const std::string& s);

// 카드 번호·계좌번호 비교용 정규화: '-'와 공백 제거, 영문 대문자화
// "1111-1111-1111" == "111111111111", "ADMIN-Kakao" == "admin-kakao"
std::string normalizeKey(const std::string& s);

std::string formatWon(long long value);            // 1234567 → "1,234,567"

}  // namespace text

#endif
