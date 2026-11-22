#ifndef SYSTEM_MANAGER_H
#define SYSTEM_MANAGER_H

#include "Bank.h"
#include "ATM.h"
#include <string>
#include <vector>

class SystemManager {
private:
    std::vector<Bank*> banks;
    std::vector<ATM*> atms;

public:
    ~SystemManager();

    bool loadInitialFile(const std::string& filename); // REQ1.11 파일 로드
    void printSnapshot() const;                        // REQ10.1 '/' 입력 시 스냅샷 출력

    // 은행/계좌 전역 검색 헬퍼
    Bank* findBankByName(const std::string& name) const;
    Account* findAccountByCardGlobal(const std::string& cardNum) const;
    Account* findAccountByNumberGlobal(const std::string& accNum) const;

    void run(); // 프로그램 메인 루프 (사용할 ATM 선택 및 실행)
};

#endif
