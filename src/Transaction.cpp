#include "Transaction.h"
#include "ATM.h"
#include "Account.h"
#include "Bank.h"
#include "Exceptions.h"
#include "FeePolicy.h"
#include "Messages.h"

#include <numeric>

// ════════════════════════════════════════════════════════════════════════
// Transaction (base)
// ════════════════════════════════════════════════════════════════════════
int Transaction::nextId_ = 1;

Transaction::Transaction(const std::string& cardNumber, const ATM& atm, long long amount, long long fee)
    : atmSerial_(atm.serial()), cardNumber_(cardNumber), amount_(amount), fee_(fee) {}

void Transaction::execute(ATM& atm) {
    validate(atm);      // ① 검사만. 실패하면 throw (상태 변경 없음)
    apply(atm);         // ② 상태 변경
    id_ = nextId_++;    // ③ 성공한 거래만 ID 발급
}

std::string Transaction::header(const Messages& msg) const {
    return "[" + msg.get(MsgId::LBL_TX) + " " + std::to_string(id_) + "] ATM " + atmSerial_ +
           " | " + msg.get(MsgId::LBL_CARD) + " " + cardNumber_;
}

std::string Transaction::moneyPart(const Messages& msg) const {
    return " | " + msg.get(MsgId::LBL_AMOUNT) + " " + msg.format(MsgId::MONEY, amount_) +
           " | " + msg.get(MsgId::LBL_FEE) + " " + msg.format(MsgId::MONEY, fee_);
}

// ════════════════════════════════════════════════════════════════════════
// 입금
// ════════════════════════════════════════════════════════════════════════
long long DepositTransaction::requiredFee(const ATM& atm, const Account& target) {
    return FeePolicy::depositFee(atm.isPrimary(target.bank()));
}

DepositTransaction::DepositTransaction(const std::string& cardNumber, const ATM& atm, Account& target,
                                       long long amount, const CashBundle& feeBills)
    : Transaction(cardNumber, atm, amount, requiredFee(atm, target)),
      target_(&target), feeBills_(feeBills) {}

void DepositTransaction::validateFee(const ATM& atm, int otherBills) const {
    // 결정 10: 입금 지폐 + 수수료 지폐 합쳐서 한도 이하
    if (otherBills + feeBills_.totalBills() > atm.maxBillsPerTransaction())
        throw LimitExceededException(MsgId::ERR_TOO_MANY_BILLS, atm.maxBillsPerTransaction());
    // 결정 11: 수수료 현금이 정확히 일치해야 함
    if (feeBills_.totalAmount() != fee())
        throw FeeMismatchException(MsgId::ERR_FEE_MISMATCH, fee());
}

// ── 현금 입금 ───────────────────────────────────────────────────────────
CashDepositTransaction::CashDepositTransaction(const std::string& cardNumber, const ATM& atm,
                                               Account& target, const CashBundle& bills,
                                               const CashBundle& feeBills)
    : DepositTransaction(cardNumber, atm, target, bills.totalAmount(), feeBills), bills_(bills) {}

void CashDepositTransaction::validate(const ATM& atm) const {
    if (bills_.empty()) throw InvalidInputException(MsgId::ERR_NO_CASH);
    validateFee(atm, bills_.totalBills());
}

void CashDepositTransaction::apply(ATM& atm) {
    target().deposit(amount());               // REQ4.3
    CashBundle all = bills_;
    all.add(feeBills());
    atm.acceptCash(all);                      // REQ4.5: 입금 현금 + 수수료 현금 → ATM 현금 증가
}

std::string CashDepositTransaction::describe(const Messages& msg) const {
    return header(msg) + " | " + msg.get(MsgId::LBL_DEPOSIT_CASH) + moneyPart(msg) +
           " | " + msg.get(MsgId::LBL_TO) + " " + target().accountNumber();
}

// ── 수표 입금 ───────────────────────────────────────────────────────────
CheckDepositTransaction::CheckDepositTransaction(const std::string& cardNumber, const ATM& atm,
                                                 Account& target, const std::vector<long long>& checks,
                                                 const CashBundle& feeBills)
    : DepositTransaction(cardNumber, atm, target, std::accumulate(checks.begin(), checks.end(), 0LL), feeBills),
      checks_(checks) {}

void CheckDepositTransaction::validate(const ATM& atm) const {
    if (checks_.empty()) throw InvalidInputException(MsgId::ERR_INVALID_AMOUNT);
    if (static_cast<int>(checks_.size()) > atm.maxChecksPerTransaction())
        throw LimitExceededException(MsgId::ERR_TOO_MANY_CHECKS, atm.maxChecksPerTransaction());
    for (long long check : checks_) {
        if (check < FeePolicy::MIN_CHECK_AMOUNT)
            throw InvalidInputException(MsgId::ERR_INVALID_CHECK, FeePolicy::MIN_CHECK_AMOUNT);
    }
    validateFee(atm, 0);
}

void CheckDepositTransaction::apply(ATM& atm) {
    target().deposit(amount());               // REQ4.3
    if (!feeBills().empty()) atm.acceptCash(feeBills());   // REQ4.6: 수표는 ATM 현금에 더하지 않음
}

std::string CheckDepositTransaction::describe(const Messages& msg) const {
    return header(msg) + " | " + msg.get(MsgId::LBL_DEPOSIT_CHECK) + " (" + msg.get(MsgId::LBL_CHECK_COUNT) +
           " " + std::to_string(checks_.size()) + ")" + moneyPart(msg) +
           " | " + msg.get(MsgId::LBL_TO) + " " + target().accountNumber();
}

// ════════════════════════════════════════════════════════════════════════
// 출금
// ════════════════════════════════════════════════════════════════════════
long long WithdrawalTransaction::requiredFee(const ATM& atm, const Account& source) {
    return FeePolicy::withdrawalFee(atm.isPrimary(source.bank()));
}

WithdrawalTransaction::WithdrawalTransaction(const std::string& cardNumber, const ATM& atm,
                                             Account& source, long long amount)
    : Transaction(cardNumber, atm, amount, requiredFee(atm, source)), source_(&source) {}

void WithdrawalTransaction::validate(const ATM& atm) const {
    if (amount() <= 0) throw InvalidInputException(MsgId::ERR_INVALID_AMOUNT);
    if (amount() % FeePolicy::WITHDRAWAL_UNIT != 0) throw InvalidInputException(MsgId::ERR_NOT_MULTIPLE_OF_1000);
    if (amount() > FeePolicy::MAX_WITHDRAWAL_AMOUNT)
        throw LimitExceededException(MsgId::ERR_WITHDRAWAL_LIMIT, FeePolicy::MAX_WITHDRAWAL_AMOUNT);
    if (source_->balance() < amount() + fee())
        throw InsufficientFundsException(MsgId::ERR_INSUFFICIENT_FUNDS, source_->balance());

    CashBundle plan;
    switch (atm.planDispense(amount(), plan)) {
    case DispenseResult::OK: break;
    case DispenseResult::NOT_POSSIBLE:
        throw InsufficientCashException(MsgId::ERR_INSUFFICIENT_CASH);
    case DispenseResult::TOO_MANY_BILLS:
        throw LimitExceededException(MsgId::ERR_TOO_MANY_BILLS, atm.maxBillsPerTransaction());
    }
}

void WithdrawalTransaction::apply(ATM& atm) {
    atm.planDispense(amount(), dispensed_);   // validate에서 가능함을 확인함
    source_->withdraw(amount() + fee());      // REQ5.3, 5.4
    atm.dispenseCash(dispensed_);             // REQ5.5
}

std::string WithdrawalTransaction::describe(const Messages& msg) const {
    return header(msg) + " | " + msg.get(MsgId::LBL_WITHDRAWAL) + moneyPart(msg) +
           " | " + msg.get(MsgId::LBL_FROM) + " " + source_->accountNumber() +
           " | " + msg.get(MsgId::LBL_BILLS) + " " + dispensed_.toString(true);
}

// ════════════════════════════════════════════════════════════════════════
// 현금 이체
// ════════════════════════════════════════════════════════════════════════
long long CashTransferTransaction::requiredFee() {
    return FeePolicy::cashTransferFee();
}

CashTransferTransaction::CashTransferTransaction(const std::string& cardNumber, const ATM& atm,
                                                 Account& dest, const CashBundle& inserted)
    : Transaction(cardNumber, atm, inserted.totalAmount() - requiredFee(), requiredFee()),
      dest_(&dest), inserted_(inserted) {}

void CashTransferTransaction::validate(const ATM& atm) const {
    if (inserted_.empty()) throw InvalidInputException(MsgId::ERR_NO_CASH);
    if (inserted_.totalBills() > atm.maxBillsPerTransaction())
        throw LimitExceededException(MsgId::ERR_TOO_MANY_BILLS, atm.maxBillsPerTransaction());
    if (amount() <= 0) throw InvalidInputException(MsgId::ERR_INVALID_AMOUNT);   // 수수료도 안 되는 금액
}

void CashTransferTransaction::apply(ATM& atm) {
    dest_->deposit(amount());                 // REQ6.3, 6.7: 투입 현금 - 수수료
    atm.acceptCash(inserted_);                // REQ6.6
}

std::string CashTransferTransaction::describe(const Messages& msg) const {
    return header(msg) + " | " + msg.get(MsgId::LBL_CASH_TRANSFER) + moneyPart(msg) +
           " | " + msg.get(MsgId::LBL_TO) + " " + dest_->accountNumber();
}

// ════════════════════════════════════════════════════════════════════════
// 계좌 이체
// ════════════════════════════════════════════════════════════════════════
long long AccountTransferTransaction::requiredFee(const ATM& atm, const Account& source, const Account& dest) {
    return FeePolicy::accountTransferFee(atm.isPrimary(source.bank()), atm.isPrimary(dest.bank()));
}

AccountTransferTransaction::AccountTransferTransaction(const std::string& cardNumber, const ATM& atm,
                                                       Account& source, Account& dest, long long amount)
    : Transaction(cardNumber, atm, amount, requiredFee(atm, source, dest)), source_(&source), dest_(&dest) {}

void AccountTransferTransaction::validate(const ATM& /*atm*/) const {
    if (source_ == dest_) throw InvalidInputException(MsgId::ERR_SAME_ACCOUNT);
    if (amount() <= 0) throw InvalidInputException(MsgId::ERR_INVALID_AMOUNT);
    if (source_->balance() < amount() + fee())
        throw InsufficientFundsException(MsgId::ERR_INSUFFICIENT_FUNDS, source_->balance());
}

void AccountTransferTransaction::apply(ATM& /*atm*/) {
    source_->withdraw(amount() + fee());      // REQ6.5, 6.7
    dest_->deposit(amount());
}

std::string AccountTransferTransaction::describe(const Messages& msg) const {
    return header(msg) + " | " + msg.get(MsgId::LBL_ACCOUNT_TRANSFER) + moneyPart(msg) +
           " | " + msg.get(MsgId::LBL_FROM) + " " + source_->accountNumber() +
           " | " + msg.get(MsgId::LBL_TO) + " " + dest_->accountNumber();
}
