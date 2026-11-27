#ifndef SESSION_H
#define SESSION_H

#include "transaction/CashBundle.h"
#include <memory>
#include <vector>

class ATM;
class System;
class UserCard;
class Account;
class Messages;
class Transaction;

// 일반 사용자 session 1회: 인증 후 ~ 카드 반환 (REQ2)
// 역할: 입력을 받아 거래 객체를 만들고 process()로 실행. 거래 규칙은 Transaction이 담당
class Session {
public:
    Session(ATM& atm, System& system, UserCard& card, const Messages& msg);

    // 거래 메뉴 반복
    //   FeeMismatchException → 오류 + 투입금 반환 안내 후 종료 (결정 11)
    //   ATMException          → msg.get(e.id())로 오류 출력 후 종료 (REQ8.2, REQ9.1)
    //   CancelException       → "취소" 안내 후 정상 종료 (결정 13)
    // 끝나면 성공 거래가 있을 때만 요약 출력 (REQ2.3)
    void run();

private:
    void handleDeposit();
    void handleWithdrawal();
    void handleTransfer();

    void process(const std::shared_ptr<Transaction>& tx);   // execute → record → 결과 출력 (모든 거래 공통 경로)
    void record(const std::shared_ptr<const Transaction>& tx);
    CashBundle readBills();                                  // 권종별 장수 4번 입력
    CashBundle readFeeBills(long long fee);                  // 수수료 0원이면 입력 생략
    void printSummary() const;
    Account& account() const;

    ATM* atm_;
    System* system_;
    UserCard* card_;
    const Messages* msg_;
    int withdrawCount_ = 0;                                  // REQ5.6 (성공한 출금만)
    bool cashInHand_ = false;                                // 진행 중 거래에서 현금이 투입되었는지 (오류 시 반환 안내)
    std::vector<std::shared_ptr<const Transaction>> txs_;
};

#endif
