#ifndef BANK_H
#define BANK_H

#include "Account.h"
#include <string>
#include <vector>

class Bank {
private:
    std::string bankName;
    std::vector<Account*> accounts;

public:
    Bank(const std::string& name);
    ~Bank();

    std::string getBankName() const { return bankName; }
    void addAccount(Account* acc);
    const std::vector<Account*>& getAccounts() const { return accounts; }

    // ATM에서 호출하는 인증 및 조회 메서드 (REQ3)
    Account* findAccountByCard(const std::string& cardNum) const;
    Account* findAccountByNumber(const std::string& accNum) const;
    Account* authenticate(const std::string& cardNum, int password) const;
};

#endif
