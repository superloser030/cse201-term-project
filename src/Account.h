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
    const std::string& accountNumber() const { return accountNumber_; }   // 파일에 적힌 형식 (표시용)
    const std::string& key() const { return key_; }                        // 정규화한 번호 (검색용)
    long long balance() const { return balance_; }

    void deposit(long long amount);                     // 0 이하면 InvalidInputException
    void withdraw(long long amount);                    // 잔액 부족 시 InsufficientFundsException

    // 이 계좌의 잔액을 바꾼 성공 거래 (추가 2)
    void addHistory(std::shared_ptr<const Transaction> tx);
    const std::vector<std::shared_ptr<const Transaction>>& history() const { return history_; }

private:
    Bank* bank_;
    std::string ownerName_;
    std::string accountNumber_;
    std::string key_;
    long long balance_;
    std::vector<std::shared_ptr<const Transaction>> history_;
};

#endif
