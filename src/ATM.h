#ifndef ATM_H
#define ATM_H

#include "CashBundle.h"
#include "TransactionLog.h"
#include <string>

class Bank;
class UserCard;
class System;
class Messages;

// ATM 1대 (REQ1.1~1.3). session 진행의 주체
// Single/Multi 차이는 일반 카드 수락 규칙(acceptsBank)뿐 (REQ1.2)
class ATM {
public:
    virtual ~ATM() = default;

    ATM(const ATM&) = delete;
    ATM& operator=(const ATM&) = delete;

    // 언어 선택(Bilingual만) → 카드 번호 입력 → card.canBeUsedAt(*this) 확인
    // → 관리자 카드면 runAdminSession(), 일반 카드면 비밀번호 확인(최대 3회) 후 runUserSession()
    void startSession(System& system);

    virtual bool acceptsBank(const Bank& bank) const = 0;                     // 일반 카드 발급 은행 수락 여부
    bool isPrimary(const Bank& bank) const { return &bank == primaryBank_; }   // 수수료, 관리자 카드 판단

    const std::string& serial() const { return serial_; }
    Bank& primaryBank() const { return *primaryBank_; }
    bool isBilingual() const { return bilingual_; }
    virtual std::string typeName() const = 0;       // "Single" / "Multi" (snapshot 출력용)

    CashBundle& cash() { return cash_; }
    const CashBundle& cash() const { return cash_; }
    TransactionLog& log() { return log_; }
    const TransactionLog& log() const { return log_; }

protected:
    ATM(const std::string& serial, Bank& primaryBank, bool bilingual, const CashBundle& initialCash);

    std::string serial_;
    Bank* primaryBank_;
    bool bilingual_;
    CashBundle cash_;
    TransactionLog log_;

private:
    void runAdminSession(System& system, const Messages& msg);                 // REQ7: 거래 내역 메뉴만
    void runUserSession(System& system, UserCard& card, const Messages& msg);  // Session 생성 후 run()
    bool authenticate(System& system, UserCard& card, const Messages& msg);    // REQ3.3~3.5
};

class SingleBankATM : public ATM {
public:
    SingleBankATM(const std::string& serial, Bank& primaryBank, bool bilingual, const CashBundle& initialCash);
    bool acceptsBank(const Bank& bank) const override;   // 주거래 은행만
    std::string typeName() const override { return "Single"; }
};

class MultiBankATM : public ATM {
public:
    MultiBankATM(const std::string& serial, Bank& primaryBank, bool bilingual, const CashBundle& initialCash);
    bool acceptsBank(const Bank& bank) const override;   // 모든 은행
    std::string typeName() const override { return "Multi"; }
};

#endif
