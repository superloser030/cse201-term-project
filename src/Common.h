#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <vector>

enum class Language { ENGLISH, KOREAN };
enum class TxType { DEPOSIT, WITHDRAWAL, CASH_TRANSFER, ACCOUNT_TRANSFER };

// 권종 상수 (5만원, 1만원, 5천원, 1천원)
const int DENOMINATIONS[4] = { 50000, 10000, 5000, 1000 };
const std::string ADMIN_CARD_NUMBER = "0000-0000-0000";

class SystemManager; // 전방 선언

// '/' 입력 시 스냅샷을 출력하고 재입력을 받는 공통 입력 함수
std::string readStringInput(SystemManager* sys);
int readIntInput(SystemManager* sys, bool& isValid);

#endif
