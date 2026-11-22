#ifndef ATM_H
#define ATM_H

#include "CashBundle.h"
#include "TransactionLog.h"
#include <memory>
#include <string>

class Bank;
class UserCard;
class System;
class Messages;
class Transaction;

// ATM 1대 (REQ1.1~1.3). session 진행의 주체
// Single/Multi 차이는 일반 카드 수락 규칙(acceptsBank)뿐 (REQ1.2)
class ATM {
public:
    virtual ~ATM() = default;

    ATM(const ATM&) = delete;
    ATM& operator=(const ATM&) = delete;

    // 언어 선택(Bilingual만, 결정 12) → 카드 번호 입력(session 시작, REQ2.1)
    // → card.canBeUsedAt(*this) 확인 → 관리자 카드면 runAdminSession()
    //   일반 카드면 authenticate()(최대 3회) 후 runUserSession()
    // session 전 단계의 ATMException / CancelException은 여기서 받아 종료
    void startSession(System& system);

    virtual bool acceptsBank(const Bank& bank) const = 0;                     // 일반 카드 발급 은행 수락 여부
    bool isPrimary(const Bank& bank) const { return &bank == primaryBank_; }   // 수수료, 관리자 카드 판단

    const std::string& serial() const { return serial_; }
    Bank& primaryBank() const { return *primaryBank_; }
    bool isBilingual() const { return bilingual_; }
    virtual std::string typeName() const = 0;       // "Single" / "Multi" (snapshot 출력용)

    // ── 현금: 용도별 함수로만 변경 (encapsulation) ────────────
    const CashBundle& cash() const { return cash_; }                     // 읽기 전용 (snapshot)
    void acceptCash(const CashBundle& bills);                           // 투입 현금 보관 (REQ4.5, 6.6)
    bool planDispense(long long amount, CashBundle& out) const;         // 내줄 지폐 계산만 (REQ5.1, 결정 8)
    void dispenseCash(const CashBundle& bills);                         // 실제 배출 (REQ5.5). 부족하면 InsufficientCashException

    // ── 거래 기록 ─────────────────────────────────────────────
    const TransactionLog& log() const { return log_; }                  // 읽기 전용 (관리자 출력)
    void record(std::shared_ptr<const Transaction> tx);                 // 성공 거래 추가 (결정 1)

protected:
    ATM(const std::string& serial, Bank& primaryBank, bool bilingual, const CashBundle& initialCash);

private:
    void runAdminSession(System& system, const Messages& msg);                 // REQ7: 거래 내역 메뉴만
    void runUserSession(System& system, UserCard& card, const Messages& msg);  // Session 생성 후 run()
    bool authenticate(System& system, UserCard& card, const Messages& msg);    // REQ3.3~3.5

    std::string serial_;
    Bank* primaryBank_;
    bool bilingual_;
    CashBundle cash_;
    TransactionLog log_;
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
