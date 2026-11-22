#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "CashBundle.h"
#include <string>

class Session;
class Messages;
class Account;

// 거래 1건. 실행(execute)과 기록(describe)을 함께 가짐 (command pattern)
// execute() 성공 후에만 계좌/ATM/session 기록에 남김 (REQ7.2: 성공 거래만)
class Transaction {
public:
    virtual ~Transaction() = default;

    virtual void execute(Session& session) = 0;                    // 실패 시 ATMException 계열 throw
    virtual std::string describe(const Messages& msg) const = 0;   // 거래 종류별로 다른 정보 출력 (REQ7.2)

    int id() const { return id_; }                                 // 모든 session 통틀어 고유 (REQ2.4)
    const std::string& cardNumber() const { return cardNumber_; }
    long long amount() const { return amount_; }
    long long fee() const { return fee_; }

protected:
    Transaction(const std::string& cardNumber, long long amount);
    void assignId();                                               // execute 성공 시 호출 → 성공 거래만 ID 소비

    int id_ = 0;
    std::string cardNumber_;
    long long amount_;
    long long fee_ = 0;

private:
    static int nextId_;
};

// ── 입금 (REQ4) ─────────────────────────────────────────────────────────
// 공통: 입금 계좌, 수수료 지폐(현금으로 별도 투입, REQ4.4)
class DepositTransaction : public Transaction {
protected:
    DepositTransaction(const std::string& cardNumber, Account& target, long long amount,
                       const CashBundle& feeBills);
    Account* target_;
    CashBundle feeBills_;
};

// 현금 입금: 입금 지폐 + 수수료 지폐 합쳐서 50장 이하 (결정 10), ATM 현금 증가 (REQ4.5)
class CashDepositTransaction : public DepositTransaction {
public:
    CashDepositTransaction(const std::string& cardNumber, Account& target,
                           const CashBundle& bills, const CashBundle& feeBills);
    void execute(Session& session) override;
    std::string describe(const Messages& msg) const override;
private:
    CashBundle bills_;
};

// 수표 입금: 100,000원 이상, 1장 (REQ1.10), ATM 현금 증가 없음 (REQ4.6, 수수료 지폐만 증가)
class CheckDepositTransaction : public DepositTransaction {
public:
    CheckDepositTransaction(const std::string& cardNumber, Account& target,
                            long long checkAmount, const CashBundle& feeBills);
    void execute(Session& session) override;
    std::string describe(const Messages& msg) const override;
};

// ── 출금 (REQ5) ─────────────────────────────────────────────────────────
class WithdrawalTransaction : public Transaction {
public:
    WithdrawalTransaction(const std::string& cardNumber, Account& source, long long amount);
    void execute(Session& session) override;
    std::string describe(const Messages& msg) const override;
    const CashBundle& dispensed() const { return dispensed_; }
private:
    Account* source_;
    CashBundle dispensed_;          // 실제로 내준 권종별 장수
};

// ── 현금 이체 (REQ6) ────────────────────────────────────────────────────
class CashTransferTransaction : public Transaction {
public:
    // inserted: 이체 금액 + 수수료를 한꺼번에 넣은 현금. amount = 총액 - 수수료
    CashTransferTransaction(const std::string& cardNumber, Account& dest, const CashBundle& inserted);
    void execute(Session& session) override;
    std::string describe(const Messages& msg) const override;
private:
    Account* dest_;
    CashBundle inserted_;
};

// ── 계좌 이체 (REQ6) ────────────────────────────────────────────────────
class AccountTransferTransaction : public Transaction {
public:
    AccountTransferTransaction(const std::string& cardNumber, Account& source, Account& dest,
                               long long amount);
    void execute(Session& session) override;
    std::string describe(const Messages& msg) const override;
private:
    Account* source_;
    Account* dest_;
};

#endif
