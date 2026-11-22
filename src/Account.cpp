#include "Account.h"
#include "Exceptions.h"
#include "TextUtil.h"

#include <utility>

Account::Account(Bank& bank, const std::string& ownerName, const std::string& accountNumber,
                 long long balance)
    : bank_(&bank), ownerName_(ownerName), accountNumber_(accountNumber),
      key_(text::normalizeKey(accountNumber)), balance_(balance) {}

void Account::deposit(long long amount) {
    if (amount <= 0) throw InvalidInputException(MsgId::ERR_INVALID_AMOUNT);
    balance_ += amount;
}

void Account::withdraw(long long amount) {
    if (amount <= 0) throw InvalidInputException(MsgId::ERR_INVALID_AMOUNT);
    if (balance_ < amount) throw InsufficientFundsException(MsgId::ERR_INSUFFICIENT_FUNDS, balance_);
    balance_ -= amount;
}

void Account::addHistory(std::shared_ptr<const Transaction> tx) {
    history_.push_back(std::move(tx));
}
