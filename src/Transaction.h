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
class DepositTransaction : public Transaction {
public:
    enum class Kind { CASH, CHECK };
    // CASH : bills 사용, checkAmount 무시 / CHECK : checkAmount 사용, bills 비어 있음
    DepositTransaction(const std::string& cardNumber, Account& target, Kind kind,
                       const CashBundle& bills, long long checkAmount, const CashBundle& feeBills);
    void execute(Session& session) override;
    std::string describe(const Messages& msg) const override;
private:
    Account* target_;
    Kind kind_;
    CashBundle bills_;
    CashBundle feeBills_;
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
