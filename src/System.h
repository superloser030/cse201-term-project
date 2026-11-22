#ifndef SYSTEM_H
#define SYSTEM_H

#include "ATM.h"
#include "Bank.h"
#include "ConsoleIO.h"
#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

// 시스템 전체: 은행과 ATM을 소유, 파일 로드, snapshot, ATM 선택 루프
class System {
public:
    System();                                              // io_를 snapshot handler와 함께 생성
    System(const System&) = delete;
    System& operator=(const System&) = delete;

    // 파일 이름 입력(Enter = 기본) → 열기 실패 시 다시 묻기 (결정 6)
    void promptAndLoad();

    // 파일 읽기 + 검사. 문제 있으면 FileFormatException (결정 7)
    void load(const std::string& filename);

    void run();                                            // ATM 선택 → atm.startSession()
    void printSnapshot(std::ostream& out) const;          // REQ10.1

    Bank* findBank(const std::string& name) const;
    Card* findCard(const std::string& cardNumber) const;          // 모든 은행에서 검색
    Account* findAccount(const std::string& accountNumber) const; // 이체 받는 계좌 검색

    ConsoleIO& io() { return io_; }                        // ATM, Session이 입력할 때 사용

    static constexpr const char* DEFAULT_FILE = "sample_initial_condition.txt";

private:
    ConsoleIO io_;
    std::vector<std::unique_ptr<Bank>> banks_;
    std::vector<std::unique_ptr<ATM>> atms_;
};

#endif
