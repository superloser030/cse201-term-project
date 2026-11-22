#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "CashBundle.h"
#include <string>
#include <vector>

class Messages;
class Account;
class ATM;

// 거래 1건. 실행(execute)과 기록(describe)을 함께 가짐 (command pattern)
//
// 규칙
//   - 수수료: 각 class의 static requiredFee()가 유일한 계산 위치 (FeePolicy 표 사용)
//             Session은 안내용으로, 생성자는 fee 설정용으로 같은 함수를 호출
//   - execute()는 non-virtual (template method):
//       ① validate()  모든 검사. 실패하면 throw, 상태 변경 없음
//       ② apply()     계좌·ATM 상태 변경
//       ③ ID 발급     성공 거래만 ID를 받음 (REQ2.4, REQ7.2)
class Transaction {
public:
    virtual ~Transaction() = default;

    void execute(ATM& atm);

    virtual std::string describe(const Messages& msg) const = 0;   // 거래 종류별로 다른 정보 출력 (REQ7.2)
    virtual std::vector<Account*> affectedAccounts() const = 0;    // 잔액이 바뀐 계좌 (임시 결정 4)

    int id() const { return id_; }                                 // 모든 session 통틀어 고유 (REQ2.4)
    const std::string& atmSerial() const { return atmSerial_; }
    const std::string& cardNumber() const { return cardNumber_; }
    long long amount() const { return amount_; }
    long long fee() const { return fee_; }

protected:
    Transaction(const std::string& cardNumber, const ATM& atm, long long amount, long long fee);

    virtual void validate(const ATM& atm) const = 0;
    virtual void apply(ATM& atm) = 0;

    std::string header(const Messages& msg) const;      // "[TX n] ATM 000111 | Card ...."
    std::string moneyPart(const Messages& msg) const;   // " | Amount ... | Fee ..."

private:
    static int nextId_;
    int id_ = 0;
    std::string atmSerial_;
    std::string cardNumber_;
    long long amount_;
    long long fee_;
};

// ── 입금 (REQ4) ─────────────────────────────────────────────────────────
// 공통: 입금 계좌, 수수료 지폐(현금으로 별도 투입, REQ4.4)
class DepositTransaction : public Transaction {
public:
    static long long requiredFee(const ATM& atm, const Account& target);    // 0 / 2,000
    std::vector<Account*> affectedAccounts() const override { return { target_ }; }

protected:
    DepositTransaction(const std::string& cardNumber, const ATM& atm, Account& target,
                       long long amount, const CashBundle& feeBills);
    void validateFee(const ATM& atm, int otherBills) const;     // 지폐 한도(합산, 결정 10) + 수수료 일치(결정 11)
    Account& target() const { return *target_; }
    const CashBundle& feeBills() const { return feeBills_; }

private:
    Account* target_;
    CashBundle feeBills_;
};

// 현금 입금: ATM 현금 증가 (REQ4.5)
class CashDepositTransaction : public DepositTransaction {
public:
    CashDepositTransaction(const std::string& cardNumber, const ATM& atm, Account& target,
                           const CashBundle& bills, const CashBundle& feeBills);
    std::string describe(const Messages& msg) const override;
protected:
    void validate(const ATM& atm) const override;
    void apply(ATM& atm) override;
private:
    CashBundle bills_;
};

// 수표 입금: 100,000원 이상 (REQ1.10), 장수 한도 (REQ4.2), ATM 현금 증가 없음 (REQ4.6, 수수료 지폐만 증가)
class CheckDepositTransaction : public DepositTransaction {
public:
    CheckDepositTransaction(const std::string& cardNumber, const ATM& atm, Account& target,
                            const std::vector<long long>& checks, const CashBundle& feeBills);
    std::string describe(const Messages& msg) const override;
protected:
    void validate(const ATM& atm) const override;
    void apply(ATM& atm) override;
private:
    std::vector<long long> checks_;
};

// ── 출금 (REQ5) ─────────────────────────────────────────────────────────
class WithdrawalTransaction : public Transaction {
public:
    static long long requiredFee(const ATM& atm, const Account& source);    // 2,000 / 3,000

    WithdrawalTransaction(const std::string& cardNumber, const ATM& atm, Account& source,
                          long long amount);
    std::string describe(const Messages& msg) const override;
    std::vector<Account*> affectedAccounts() const override { return { source_ }; }
    const CashBundle& dispensed() const { return dispensed_; }
protected:
    void validate(const ATM& atm) const override;
    void apply(ATM& atm) override;
private:
    Account* source_;
    CashBundle dispensed_;          // 실제로 내준 권종별 장수
};

// ── 현금 이체 (REQ6) ────────────────────────────────────────────────────
class CashTransferTransaction : public Transaction {
public:
    static long long requiredFee();                                          // 1,000

    // inserted: 이체 금액 + 수수료를 한꺼번에 넣은 현금. amount = 총액 - 수수료 (REQ6.3)
    CashTransferTransaction(const std::string& cardNumber, const ATM& atm, Account& dest,
                            const CashBundle& inserted);
    std::string describe(const Messages& msg) const override;
    std::vector<Account*> affectedAccounts() const override { return { dest_ }; }
protected:
    void validate(const ATM& atm) const override;
    void apply(ATM& atm) override;
private:
    Account* dest_;
    CashBundle inserted_;
};

// ── 계좌 이체 (REQ6) ────────────────────────────────────────────────────
class AccountTransferTransaction : public Transaction {
public:
    static long long requiredFee(const ATM& atm, const Account& source,
                                 const Account& dest);                       // 2,000 / 3,000 / 5,000

    AccountTransferTransaction(const std::string& cardNumber, const ATM& atm, Account& source,
                               Account& dest, long long amount);
    std::string describe(const Messages& msg) const override;
    std::vector<Account*> affectedAccounts() const override { return { source_, dest_ }; }
protected:
    void validate(const ATM& atm) const override;
    void apply(ATM& atm) override;
private:
    Account* source_;
    Account* dest_;
};

#endif
