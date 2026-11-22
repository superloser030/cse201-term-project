#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <memory>
#include <string>
#include <vector>

class Bank;
class Transaction;

// 계좌 1개 (REQ1.5). 잔액은 deposit()/withdraw()로만 바뀜 (encapsulation)
class Account {
public:
    Account(Bank& bank, const std::string& ownerName, const std::string& accountNumber,
            long long balance);

    Bank& bank() const { return *bank_; }
    const std::string& ownerName() const { return ownerName_; }
    const std::string& accountNumber() const { return accountNumber_; }
    long long balance() const { return balance_; }

    void deposit(long long amount);
    void withdraw(long long amount);                    // 잔액 부족 시 InsufficientFundsException

    void addHistory(std::shared_ptr<const Transaction> tx);
    const std::vector<std::shared_ptr<const Transaction>>& history() const { return history_; }

private:
    Bank* bank_;
    std::string ownerName_;
    std::string accountNumber_;
    long long balance_;
    std::vector<std::shared_ptr<const Transaction>> history_;
};

#endif
