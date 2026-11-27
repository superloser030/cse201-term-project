#include "Card.h"
#include "ATM.h"
#include "Bank.h"
#include "TextUtil.h"

Card::Card(const std::string& number, Bank& issuer)
    : number_(number), key_(text::normalizeKey(number)), issuer_(&issuer) {}

UserCard::UserCard(const std::string& number, Bank& issuer, Account& account, const std::string& password)
    : Card(number, issuer), account_(&account), password_(password) {}

bool UserCard::canBeUsedAt(const ATM& atm) const {
    return atm.acceptsBank(issuer());          // Single: 주거래 은행만 / Multi: 모든 은행 (REQ1.2)
}

AdminCard::AdminCard(Bank& issuer) : Card("ADMIN-" + issuer.name(), issuer) {}

bool AdminCard::canBeUsedAt(const ATM& atm) const {
    return atm.isPrimary(issuer());            // 결정 2: 발급 은행이 관리하는 ATM에서만
}
