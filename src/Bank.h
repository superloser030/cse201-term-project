#ifndef BANK_H
#define BANK_H

#include "Account.h"
#include "Card.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

// 은행 1개: 계좌와 카드를 소유하고 비밀번호를 확인 (REQ1.5, REQ3.3)
class Bank {
public:
    explicit Bank(const std::string& name);

    Bank(const Bank&) = delete;                 // 소유 객체 중복 해제 방지
    Bank& operator=(const Bank&) = delete;

    const std::string& name() const { return name_; }

    // ── 파일 로드 시 사용 ─────────────────────────────────────
    Account& openAccount(const std::string& ownerName, const std::string& accountNumber,
                         long long balance);
    UserCard& issueCard(const std::string& cardNumber, Account& account, const std::string& password);
    AdminCard& issueAdminCard();               // "ADMIN-" + name_ (결정 3)

    // ── 조회 / 인증 (번호는 '-'·공백·대소문자 무관) ─────────
    Card* findCard(const std::string& cardNumber) const;          // 없으면 nullptr
    Account* findAccount(const std::string& accountNumber) const; // 없으면 nullptr
    bool verifyPassword(const UserCard& card, const std::string& password) const;  // REQ3.3

    const std::vector<std::unique_ptr<Account>>& accounts() const { return accounts_; }
    const AdminCard* adminCard() const { return adminCard_; }

private:
    std::string name_;
    std::vector<std::unique_ptr<Account>> accounts_;               // 파일 순서 유지 (snapshot 출력)
    std::map<std::string, Account*> accountIndex_;                 // 정규화 번호 → 계좌
    std::map<std::string, std::unique_ptr<Card>> cards_;           // 정규화 번호 → 카드
    AdminCard* adminCard_ = nullptr;
};

#endif
