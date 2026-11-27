#include "Bank.h"
#include "ATM.h"
#include "TextUtil.h"

#include <stdexcept>
#include <utility>

Bank::Bank(const std::string& name) : name_(name) {}

Account& Bank::openAccount(const std::string& ownerName, const std::string& accountNumber,
                           long long balance) {
    auto account = std::make_unique<Account>(*this, ownerName, accountNumber, balance);
    Account& ref = *account;
    accounts_.push_back(std::move(account));
    accountIndex_[ref.key()] = &ref;
    return ref;
}

UserCard& Bank::issueCard(const std::string& cardNumber, Account& account, const std::string& password) {
    auto card = std::make_unique<UserCard>(cardNumber, *this, account, password);
    UserCard& ref = *card;
    cards_[ref.key()] = std::move(card);
    return ref;
}

AdminCard& Bank::issueAdminCard() {
    auto card = std::make_unique<AdminCard>(*this);
    AdminCard& ref = *card;
    cards_[ref.key()] = std::move(card);
    adminCard_ = &ref;
    return ref;
}

void Bank::supplyCash(ATM& atm, const CashBundle& cash) const {
    if (!atm.isPrimary(*this))
        throw std::logic_error("Bank " + name_ + " cannot supply cash to ATM " + atm.serial());
    atm.acceptCash(cash);
}

Card* Bank::findCard(const std::string& cardNumber) const {
    const auto it = cards_.find(text::normalizeKey(cardNumber));
    return it == cards_.end() ? nullptr : it->second.get();
}

Account* Bank::findAccount(const std::string& accountNumber) const {
    const auto it = accountIndex_.find(text::normalizeKey(accountNumber));
    return it == accountIndex_.end() ? nullptr : it->second;
}

bool Bank::verifyPassword(const UserCard& card, const std::string& password) const {
    // 이 은행이 발급한 카드만 확인 (REQ3.3: 은행이 검증해서 결과를 돌려줌)
    return &card.issuer() == this && card.password_ == password;
}
