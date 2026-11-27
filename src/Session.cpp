#include "Session.h"
#include "ATM.h"
#include "Account.h"
#include "Bank.h"
#include "Card.h"
#include "ConsoleIO.h"
#include "Exceptions.h"
#include "FeePolicy.h"
#include "Messages.h"
#include "System.h"
#include "Transaction.h"

#include <iostream>

namespace {
constexpr long long MAX_INPUT_AMOUNT = 1000000000000LL;   // 입력 범위 상한 (overflow 방지)
constexpr long long MAX_INPUT_BILLS = 10000;              // 권종별 입력 상한
}

Session::Session(ATM& atm, System& system, UserCard& card, const Messages& msg)
    : atm_(&atm), system_(&system), card_(&card), msg_(&msg) {}

Account& Session::account() const { return card_->account(); }

void Session::run() {
    ConsoleIO& io = system_->io();
    bool running = true;
    while (running) {
        cashInHand_ = false;
        try {
            std::cout << '\n' << msg_->get(MsgId::MAIN_MENU) << '\n' << msg_->get(MsgId::SELECT_PROMPT);
            switch (io.readNumber(1, 4)) {
            case 1: handleDeposit(); break;
            case 2: handleWithdrawal(); break;
            case 3: handleTransfer(); break;
            default: running = false; break;                   // REQ2.2: 사용자가 종료
            }
        } catch (const FeeMismatchException& e) {               // 결정 11: 오류 + 투입금 반환
            std::cout << describeError(*msg_, e) << '\n' << msg_->get(MsgId::CASH_RETURNED) << '\n';
            running = false;
        } catch (const ATMException& e) {                       // REQ9.1: 오류 출력 후 session 종료
            std::cout << describeError(*msg_, e) << '\n';
            if (cashInHand_) std::cout << msg_->get(MsgId::CASH_RETURNED) << '\n';
            running = false;
        } catch (const CancelException&) {                      // 결정 13: 취소
            std::cout << msg_->get(MsgId::CANCELLED) << '\n';
            if (cashInHand_) std::cout << msg_->get(MsgId::CASH_RETURNED) << '\n';
            running = false;
        }
    }
    printSummary();                                             // REQ2.3
}

// ── 입금 (REQ4) ─────────────────────────────────────────────────────────
void Session::handleDeposit() {
    ConsoleIO& io = system_->io();
    std::cout << msg_->get(MsgId::DEPOSIT_TYPE) << '\n' << msg_->get(MsgId::SELECT_PROMPT);
    const long long type = io.readNumber(1, 2);
    const long long fee = DepositTransaction::requiredFee(*atm_, account());

    if (type == 1) {
        std::cout << msg_->get(MsgId::ENTER_BILLS) << '\n';
        const CashBundle bills = readBills();
        const CashBundle feeBills = readFeeBills(fee);
        process(std::make_shared<CashDepositTransaction>(card_->number(), *atm_, account(), bills, feeBills));
    } else {
        std::cout << msg_->get(MsgId::ENTER_CHECK_AMOUNT);    // 수표 1장 고정 (결정 14)
        const long long checkAmount = io.readNumber(1, MAX_INPUT_AMOUNT);
        const CashBundle feeBills = readFeeBills(fee);
        process(std::make_shared<CheckDepositTransaction>(card_->number(), *atm_, account(), checkAmount, feeBills));
    }
}

// ── 출금 (REQ5) ─────────────────────────────────────────────────────────
void Session::handleWithdrawal() {
    if (withdrawCount_ >= FeePolicy::MAX_WITHDRAWALS_PER_SESSION)   // REQ5.6
        throw LimitExceededException(MsgId::ERR_WITHDRAWAL_COUNT, FeePolicy::MAX_WITHDRAWALS_PER_SESSION);

    std::cout << msg_->get(MsgId::ENTER_WITHDRAWAL_AMOUNT);
    const long long amount = system_->io().readNumber(1, MAX_INPUT_AMOUNT);
    auto tx = std::make_shared<WithdrawalTransaction>(card_->number(), *atm_, account(), amount);
    process(tx);
    ++withdrawCount_;
    std::cout << msg_->format(MsgId::DISPENSED, tx->dispensed().toString(true)) << '\n';
}

// ── 이체 (REQ6) ─────────────────────────────────────────────────────────
void Session::handleTransfer() {
    ConsoleIO& io = system_->io();
    std::cout << msg_->get(MsgId::TRANSFER_TYPE) << '\n' << msg_->get(MsgId::SELECT_PROMPT);   // REQ6.1
    const long long type = io.readNumber(1, 2);

    std::cout << msg_->get(MsgId::ENTER_DEST_ACCOUNT);                                        // REQ6.2
    Account* dest = system_->findAccount(io.readToken());
    if (dest == nullptr) throw InvalidInputException(MsgId::ERR_ACCOUNT_NOT_FOUND);

    if (type == 1) {                                                                            // 현금 이체
        std::cout << msg_->format(MsgId::ENTER_TRANSFER_CASH, CashTransferTransaction::requiredFee()) << '\n';
        const CashBundle inserted = readBills();
        auto tx = std::make_shared<CashTransferTransaction>(card_->number(), *atm_, *dest, inserted);
        if (tx->amount() > 0) {                                                                 // REQ6.3: 금액 확인
            std::cout << msg_->format(MsgId::CONFIRM_TRANSFER_AMOUNT, tx->amount()) << '\n'
                      << msg_->get(MsgId::SELECT_PROMPT);
            if (io.readNumber(1, 2) == 2) throw CancelException();
        }
        process(tx);
    } else {                                                                                    // 계좌 이체
        std::cout << msg_->get(MsgId::ENTER_TRANSFER_AMOUNT);                                  // REQ6.4
        const long long amount = io.readNumber(1, MAX_INPUT_AMOUNT);
        process(std::make_shared<AccountTransferTransaction>(card_->number(), *atm_, account(), *dest, amount));
    }
}

// ── 공통 ────────────────────────────────────────────────────────────────
void Session::process(const std::shared_ptr<Transaction>& tx) {
    tx->execute(*atm_);                 // 거래 종류와 무관한 공통 경로 (polymorphism)
    record(tx);
    std::cout << msg_->format(MsgId::TRANSACTION_SUCCESS, tx->id()) << '\n'
              << msg_->format(MsgId::BALANCE, account().balance()) << '\n';
}

void Session::record(const std::shared_ptr<const Transaction>& tx) {
    for (Account* acc : tx->affectedAccounts()) acc->addHistory(tx);   // 잔액이 바뀐 계좌 모두 (추가 2)
    system_->log().add(tx);                                              // 시스템 전체 기록 (REQ7.2)
    txs_.push_back(tx);                                                  // 이번 session 기록 (REQ2.3)
}

CashBundle Session::readBills() {
    std::array<int, CashBundle::KINDS> counts{};
    for (std::size_t i = 0; i < counts.size(); ++i) {
        std::cout << msg_->format(MsgId::BILL_COUNT, CashBundle::DENOMINATIONS[i]);
        counts[i] = static_cast<int>(system_->io().readNumber(0, MAX_INPUT_BILLS));
        if (counts[i] > 0) cashInHand_ = true;           // 입력 도중 취소·오류여도 반환 안내가 나오도록
    }
    return CashBundle(counts);
}

CashBundle Session::readFeeBills(long long fee) {
    if (fee == 0) return CashBundle();                                   // 수수료 0원이면 입력 생략
    std::cout << msg_->format(MsgId::ENTER_FEE_BILLS, fee) << '\n';
    return readBills();
}

void Session::printSummary() const {
    if (txs_.empty()) return;                                            // REQ2.3: 성공 거래 없으면 생략
    const Account& acc = account();
    std::cout << '\n' << msg_->get(MsgId::SESSION_SUMMARY) << '\n'
              << msg_->format(MsgId::SUMMARY_ACCOUNT,
                              acc.bank().name() + " " + acc.accountNumber() + " (" + acc.ownerName() + ")")
              << '\n';
    for (const auto& tx : txs_) std::cout << tx->describe(*msg_) << '\n';
}
