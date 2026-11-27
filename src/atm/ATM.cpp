#include "atm/ATM.h"
#include "bank/Bank.h"
#include "bank/Card.h"
#include "system/ConsoleIO.h"
#include "common/Exceptions.h"
#include "transaction/FeePolicy.h"
#include "common/Messages.h"
#include "atm/Session.h"
#include "system/System.h"

#include <iostream>

ATM::ATM(const std::string& serial, Bank& primaryBank, bool bilingual)
    : serial_(serial), primaryBank_(&primaryBank), bilingual_(bilingual) {}   // 현금 0으로 시작, 은행이 공급 (REQ1.4)

// ── 현금 ────────────────────────────────────────────────────────────────
void ATM::acceptCash(const CashBundle& bills) { cash_.add(bills); }

DispenseResult ATM::planDispense(long long amount, CashBundle& out) const {
    return cash_.planDispense(amount, maxBills_, out);
}

void ATM::dispenseCash(const CashBundle& bills) { cash_.remove(bills); }

// ── session ─────────────────────────────────────────────────────────────
void ATM::startSession(System& system) {
    ConsoleIO& io = system.io();
    const Messages* msg = &Messages::english();          // Unilingual은 영어 고정 (REQ1.3)
    bool cardInserted = false;

    std::cout << "\n===== ATM " << serial_ << " (" << primaryBank_->name() << ", " << typeName()
              << ", " << (bilingual_ ? "Bilingual" : "Unilingual") << ") =====\n";
    try {
        if (bilingual_) {                                 // 결정 12: 카드 투입 전 언어 선택
            std::cout << msg->get(MsgId::SELECT_LANGUAGE) << '\n' << msg->get(MsgId::SELECT_PROMPT);
            if (io.readNumber(1, 2) == 2) msg = &Messages::korean();
        }
        std::cout << msg->get(MsgId::CANCEL_HINT) << '\n' << msg->get(MsgId::INSERT_CARD);
        const std::string cardNumber = io.readToken();
        cardInserted = true;                              // REQ2.1: 카드 투입 = session 시작

        Card* card = system.findCard(cardNumber);
        if (card == nullptr || !card->canBeUsedAt(*this))  // REQ3.1
            throw InvalidCardException(MsgId::ERR_INVALID_CARD);

        if (card->isAdmin()) {
            runAdminSession(system, *msg);
        } else {
            UserCard* user = card->asUserCard();
            if (user == nullptr) throw InvalidCardException(MsgId::ERR_INVALID_CARD);
            if (!authenticate(system, *user, *msg)) throw ATMException(MsgId::ERR_PASSWORD_LOCKED);
            runUserSession(system, *user, *msg);
        }
    } catch (const ATMException& e) {
        std::cout << describeError(*msg, e) << '\n';
    } catch (const CancelException&) {
        std::cout << msg->get(MsgId::CANCELLED) << '\n';
    }
    if (cardInserted) std::cout << msg->get(MsgId::CARD_RETURNED) << '\n';
}

bool ATM::authenticate(System& system, UserCard& card, const Messages& msg) {
    for (int attempt = 1; attempt <= FeePolicy::MAX_PASSWORD_ATTEMPTS; ++attempt) {
        std::cout << msg.get(MsgId::ENTER_PASSWORD);
        const std::string password = system.io().readToken();
        if (card.issuer().verifyPassword(card, password)) return true;     // REQ3.3: 은행이 확인
        const int left = FeePolicy::MAX_PASSWORD_ATTEMPTS - attempt;
        if (left > 0)                                                       // REQ3.4
            std::cout << describeError(msg, ATMException(MsgId::ERR_WRONG_PASSWORD, left)) << '\n';
    }
    return false;                                                           // REQ3.5
}

void ATM::runAdminSession(System& system, const Messages& msg) {
    std::cout << msg.get(MsgId::ADMIN_MENU) << '\n' << msg.get(MsgId::SELECT_PROMPT);   // REQ7.1
    system.io().readNumber(1, 1);

    system.log().printForAtm(std::cout, msg, serial_);                     // REQ7.2: 이 ATM의 거래 (결정 1)
    const std::string path = "ATM_" + serial_ + "_transaction_history.txt";
    if (system.log().saveForAtm(path, msg, serial_))                       // REQ7.3
        std::cout << msg.format(MsgId::HISTORY_SAVED, path) << '\n';
}

void ATM::runUserSession(System& system, UserCard& card, const Messages& msg) {
    Session session(*this, system, card, msg);
    session.run();
}

// ── Single / Multi ──────────────────────────────────────────────────────
SingleBankATM::SingleBankATM(const std::string& serial, Bank& primaryBank, bool bilingual)
    : ATM(serial, primaryBank, bilingual) {}

bool SingleBankATM::acceptsBank(const Bank& bank) const { return isPrimary(bank); }

MultiBankATM::MultiBankATM(const std::string& serial, Bank& primaryBank, bool bilingual)
    : ATM(serial, primaryBank, bilingual) {}

bool MultiBankATM::acceptsBank(const Bank& /*bank*/) const { return true; }
