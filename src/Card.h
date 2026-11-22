#ifndef CARD_H
#define CARD_H

#include <string>

class Bank;
class Account;
class ATM;
class System;
class Messages;

// 카드 (용어 정의: 관리자 카드는 "특수한 종류의 카드", 결정 5)
// 카드를 넣으면(카드 번호 입력) 카드 종류에 맞는 session을 시작 (polymorphism)
class Card {
public:
    virtual ~Card() = default;

    const std::string& number() const { return number_; }
    Bank& issuer() const { return *issuer_; }

    // ATM이 카드를 받은 뒤 호출. 수락 검사 → (인증) → session 진행
    virtual void beginSession(ATM& atm, System& system, const Messages& msg) = 0;

protected:
    Card(const std::string& number, Bank& issuer);

    std::string number_;
    Bank* issuer_;
};

// 고객 카드: 계좌 1개와 연결, 비밀번호 있음 (REQ3)
class UserCard : public Card {
public:
    UserCard(const std::string& number, Bank& issuer, Account& account, const std::string& password);

    Account& account() const { return *account_; }
    void beginSession(ATM& atm, System& system, const Messages& msg) override;

private:
    friend class Bank;                  // 비밀번호 확인은 Bank만 가능 (REQ3.3)
    Account* account_;
    std::string password_;              // string: 앞자리 0 보존
};

// 관리자 카드: "ADMIN-<은행 이름>", 비밀번호 없음, 발급 은행의 ATM에서만 사용 (결정 2·3·4)
class AdminCard : public Card {
public:
    explicit AdminCard(Bank& issuer);   // number = "ADMIN-" + 은행 이름

    void beginSession(ATM& atm, System& system, const Messages& msg) override;
};

#endif
