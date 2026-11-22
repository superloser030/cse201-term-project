#ifndef ATM_H
#define ATM_H

#include "Common.h"
#include "Bank.h"
#include "Transaction.h"
#include <string>
#include <vector>

class SystemManager;

class ATM {
private:
    std::string primaryBankName;
    std::string serialNumber;       // 6자리 문자열 (예: "000111")
    bool isSingleBank;              // true: Single, false: Multi
    bool isUnilingual;              // true: English only, false: Bilingual
    int cash[4];                    // [50000, 10000, 5000, 1000] 매수

    SystemManager* sysManager;      // 전체 은행 조회 및 '/' 입력 처리용

    int sessionCounter;             // 누적 세션 번호

    std::vector<Transaction> atmHistory; // 전체 거래 기록 (관리자 메뉴용)

    // 조원 B가 내부 구현할 세부 헬퍼 함수들
    void runAdminSession(Language lang);
    void runUserSession(Account* userAcc, Language lang);
    bool handleDeposit(Account* userAcc, int sessionId, Language lang,
                       std::vector<Transaction>& sessionTxs);
    bool handleWithdrawal(Account* userAcc, int sessionId, Language lang, int& withdrawCount,
                          std::vector<Transaction>& sessionTxs);
    bool handleTransfer(Account* userAcc, int sessionId, Language lang,
                        std::vector<Transaction>& sessionTxs);
    bool dispenseCashGreedy(int amount, int dispensed[4]); // 최소 매수 출금 계산

public:
    ATM(const std::string& primaryBank, const std::string& serial,
        bool singleBank, bool unilingual, const int initialCash[4], SystemManager* sys);

    // Getters (스냅샷 출력용)
    std::string getPrimaryBankName() const { return primaryBankName; }
    std::string getSerialNumber() const { return serialNumber; }
    int getCashCount(int idx) const { return cash[idx]; }
    int getTotalCash() const;

    // 메인 실행 진입점: 카드 삽입부터 세션 종료까지 한 사이클 실행
    void startSession();
};

#endif
