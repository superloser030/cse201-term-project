#ifndef ATM_H
#define ATM_H

#include "CashBundle.h"
#include "TransactionLog.h"
#include <string>

class Bank;
class Card;
class System;

// ATM 1대 (REQ1.1~1.3). Single/Multi 차이는 일반 카드 수락 규칙뿐 (REQ1.2)
class ATM {
public:
    virtual ~ATM() = default;

    ATM(const ATM&) = delete;
    ATM& operator=(const ATM&) = delete;

    // 언어 선택(Bilingual만) → 카드 번호 입력 → card.beginSession()
    void startSession(System& system);

    bool acceptsUserCard(const Card& card) const;   // acceptsBank(card.issuer())
    bool acceptsAdminCard(const Card& card) const;  // 발급 은행 == 주거래 은행 (결정 2)
    bool isPrimary(const Bank& bank) const { return &bank == primaryBank_; }   // 수수료 판단

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

    virtual bool acceptsBank(const Bank& bank) const = 0;

    std::string serial_;
    Bank* primaryBank_;
    bool bilingual_;
    CashBundle cash_;
    TransactionLog log_;
};

class SingleBankATM : public ATM {
public:
    SingleBankATM(const std::string& serial, Bank& primaryBank, bool bilingual, const CashBundle& initialCash);
    std::string typeName() const override { return "Single"; }
protected:
    bool acceptsBank(const Bank& bank) const override;   // 주거래 은행만
};

class MultiBankATM : public ATM {
public:
    MultiBankATM(const std::string& serial, Bank& primaryBank, bool bilingual, const CashBundle& initialCash);
    std::string typeName() const override { return "Multi"; }
protected:
    bool acceptsBank(const Bank& bank) const override;   // 모든 은행
};

#endif
