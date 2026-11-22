#ifndef SESSION_H
#define SESSION_H

#include <memory>
#include <vector>

class ATM;
class System;
class UserCard;
class Account;
class Messages;
class Transaction;

// 일반 사용자 session 1회: 인증 후 ~ 카드 반환 (REQ2)
class Session {
public:
    Session(ATM& atm, System& system, UserCard& card, const Messages& msg);

    // 거래 메뉴 반복. ATMException을 catch하면 오류 출력 후 종료 (REQ9.1)
    // 끝나면 성공 거래가 있을 때만 요약 출력 (REQ2.3)
    void run();

    ATM& atm() const { return *atm_; }
    System& system() const { return *system_; }
    UserCard& card() const { return *card_; }
    Account& account() const;
    const Messages& messages() const { return *msg_; }

    int withdrawCount() const { return withdrawCount_; }
    void countWithdrawal() { ++withdrawCount_; }       // REQ5.6

    // 성공한 거래를 계좌·ATM 기록·session 기록에 한 번에 남김
    void record(std::shared_ptr<const Transaction> tx);

private:
    void handleDeposit();          // 입력을 받아 DepositTransaction 생성 → execute → record
    void handleWithdrawal();
    void handleTransfer();
    void printSummary() const;

    ATM* atm_;
    System* system_;
    UserCard* card_;
    const Messages* msg_;
    int withdrawCount_ = 0;
    std::vector<std::shared_ptr<const Transaction>> txs_;
};

#endif
