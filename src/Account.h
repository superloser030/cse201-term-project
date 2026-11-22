#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "Transaction.h"
#include <string>
#include <vector>

class Account {
private:
    std::string bankName;
    std::string userName;
    std::string accountNumber;
    int balance;
    std::string cardNumber;
    int password;
    std::vector<Transaction> history;

public:
    Account(const std::string& bankName, const std::string& userName,
            const std::string& accountNumber, int balance,
            const std::string& cardNumber, int password);

    // Getters
    std::string getBankName() const { return bankName; }
    std::string getUserName() const { return userName; }
    std::string getAccountNumber() const { return accountNumber; }
    int getBalance() const { return balance; }
    std::string getCardNumber() const { return cardNumber; }

    // 검증 및 잔액 연산
    bool verifyPassword(int inputPwd) const;
    void deposit(int amount);
    bool withdraw(int amount); // 잔액 부족 시 false 반환
    void addHistory(const Transaction& tx);
};

#endif
