#ifndef CARD_H
#define CARD_H

#include <string>

class Bank;
class Account;
class ATM;

// 카드 (용어 정의: 관리자 카드는 "특수한 종류의 카드", 결정 5)
// 카드는 정보와 "이 ATM에서 쓸 수 있는가" 판단만 담당. session 진행은 ATM이 함
class Card {
public:
    virtual ~Card() = default;

    const std::string& number() const { return number_; }
    Bank& issuer() const { return *issuer_; }

    virtual bool canBeUsedAt(const ATM& atm) const = 0;   // 카드 종류별 수락 규칙 (polymorphism)
    virtual bool isAdmin() const = 0;

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
    bool canBeUsedAt(const ATM& atm) const override;      // atm.acceptsBank(발급 은행): Single/Multi 규칙 (REQ1.2)
    bool isAdmin() const override { return false; }

private:
    friend class Bank;                  // 비밀번호 확인은 Bank만 가능 (REQ3.3)
    Account* account_;
    std::string password_;              // string: 앞자리 0 보존
};

// 관리자 카드: "ADMIN-<은행 이름>", 비밀번호 없음 (결정 2·3·4)
class AdminCard : public Card {
public:
    explicit AdminCard(Bank& issuer);   // number = "ADMIN-" + 은행 이름

    bool canBeUsedAt(const ATM& atm) const override;      // atm.isPrimary(발급 은행): 주거래 은행 ATM만 (결정 2)
    bool isAdmin() const override { return true; }
};

#endif
