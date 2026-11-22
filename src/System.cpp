#include "System.h"
#include "Exceptions.h"
#include "Messages.h"
#include "TextUtil.h"

#include <algorithm>
#include <climits>
#include <fstream>
#include <iostream>
#include <set>

System::System() : io_([this]() { printSnapshot(std::cout); }) {}

// ── 파일 로드 ───────────────────────────────────────────────────────────
void System::promptAndLoad() {
    while (true) {
        std::cout << "Initial condition file name (Enter = " << DEFAULT_FILE << "): ";
        std::string name = io_.readLine();
        if (name.empty()) name = DEFAULT_FILE;

        std::ifstream probe(name);
        if (!probe) {                                                    // 결정 6: 다시 묻기
            std::cout << "[Error] Cannot open file: " << name << " (please try again)\n";
            continue;
        }
        probe.close();
        load(name);

        std::size_t accountCount = 0;
        for (const auto& bank : banks_) accountCount += bank->accounts().size();
        std::cout << "Loaded " << banks_.size() << " bank(s), " << accountCount << " account(s), "
                  << atms_.size() << " ATM(s) from " << name << '\n';
        return;
    }
}

void System::load(const std::string& filename) {
    std::ifstream fin(filename);
    if (!fin) throw FileFormatException("Cannot open file: " + filename);

    // 공백 단위로 읽음 (CRLF의 '\r'도 공백으로 처리됨)
    auto next = [&fin](const std::string& what) {
        std::string token;
        if (!(fin >> token)) throw FileFormatException("Unexpected end of file while reading " + what);
        return token;
    };
    auto nextNumber = [&next](const std::string& what, long long min, long long max) {
        const std::string token = next(what);
        long long value = 0;
        bool ok = true;
        try {
            std::size_t pos = 0;
            value = std::stoll(token, &pos);
            ok = (pos == token.size());
        } catch (const std::exception&) {
            ok = false;
        }
        if (!ok || value < min || value > max)
            throw FileFormatException("Invalid number for " + what + ": " + token);
        return value;
    };

    const long long bankCount = nextNumber("bank count", 1, 1000);
    const long long accountCount = nextNumber("account count", 0, 100000);
    const long long atmCount = nextNumber("ATM count", 1, 1000);

    std::set<std::string> bankNames, cardKeys, accountKeys, serials;

    // 1) 은행 + 은행별 관리자 카드 (결정 3)
    for (long long i = 0; i < bankCount; ++i) {
        const std::string name = next("bank name");
        if (!bankNames.insert(name).second) throw FileFormatException("Duplicate bank name: " + name);
        banks_.push_back(std::make_unique<Bank>(name));
        AdminCard& admin = banks_.back()->issueAdminCard();
        cardKeys.insert(admin.key());
    }

    // 2) 계좌 + 카드 (REQ1.11 순서: 은행, 이름, 계좌번호, 잔액, 카드 번호, 비밀번호)
    for (long long i = 0; i < accountCount; ++i) {
        const std::string bankName = next("account bank name");
        const std::string owner = next("account user name");
        const std::string accountNumber = next("account number");
        const long long balance = nextNumber("account balance", 0, LLONG_MAX / 4);
        const std::string cardNumber = next("card number");
        const std::string password = next("card password");

        Bank* bank = findBank(bankName);
        if (bank == nullptr) throw FileFormatException("Unknown bank for account " + accountNumber + ": " + bankName);
        if (!accountKeys.insert(text::normalizeKey(accountNumber)).second)
            throw FileFormatException("Duplicate account number: " + accountNumber);
        if (!cardKeys.insert(text::normalizeKey(cardNumber)).second)
            throw FileFormatException("Duplicate card number: " + cardNumber);

        Account& account = bank->openAccount(owner, accountNumber, balance);
        bank->issueCard(cardNumber, account, password);
    }

    // 3) ATM (주거래 은행, 일련번호, 종류, 언어, 초기 현금 4개)
    for (long long i = 0; i < atmCount; ++i) {
        const std::string bankName = next("ATM primary bank");
        const std::string serial = next("ATM serial number");
        const std::string type = text::toLower(next("ATM type"));
        const std::string language = text::toLower(next("ATM language"));
        std::array<int, CashBundle::KINDS> counts{};
        for (std::size_t k = 0; k < counts.size(); ++k)
            counts[k] = static_cast<int>(nextNumber("ATM initial cash", 0, INT_MAX / 2));

        Bank* bank = findBank(bankName);
        if (bank == nullptr) throw FileFormatException("Unknown bank for ATM " + serial + ": " + bankName);
        if (!serials.insert(serial).second) throw FileFormatException("Duplicate ATM serial number: " + serial);
        if (type != "single" && type != "multi") throw FileFormatException("Invalid ATM type for " + serial + ": " + type);
        if (language != "unilingual" && language != "bilingual")
            throw FileFormatException("Invalid ATM language for " + serial + ": " + language);

        const bool bilingual = (language == "bilingual");
        const CashBundle cash(counts);
        if (type == "single") atms_.push_back(std::make_unique<SingleBankATM>(serial, *bank, bilingual, cash));
        else                  atms_.push_back(std::make_unique<MultiBankATM>(serial, *bank, bilingual, cash));
    }
}

void System::printAdminCards(std::ostream& out) const {
    out << "Admin cards (no password):";
    for (const auto& bank : banks_) {
        if (bank->adminCard() != nullptr) out << "  " << bank->adminCard()->number() << " (" << bank->name() << ")";
    }
    out << '\n';
}

// ── 메인 루프 ───────────────────────────────────────────────────────────
void System::run() {
    while (true) {
        try {
            std::cout << "\n===== Select ATM =====\n";
            for (std::size_t i = 0; i < atms_.size(); ++i) {
                const ATM& atm = *atms_[i];
                std::cout << (i + 1) << ") " << atm.serial() << "  " << atm.primaryBank().name() << "  "
                          << atm.typeName() << "  " << (atm.isBilingual() ? "Bilingual" : "Unilingual") << '\n';
            }
            std::cout << "0) Exit program\n(Enter '/' at any prompt to print the snapshot)\nSelect: ";
            const long long choice = io_.readNumberNoCancel(0, static_cast<long long>(atms_.size()));
            if (choice == 0) {
                std::cout << "Program terminated.\n";
                return;
            }
            atms_[static_cast<std::size_t>(choice - 1)]->startSession(*this);
        } catch (const InputClosedException&) {
            throw;                                                       // main에서 정상 종료
        } catch (const ATMException& e) {
            std::cout << describeError(Messages::english(), e) << '\n';
        } catch (const std::exception& e) {                              // 마지막 방어선
            std::cout << "[Error] Unexpected error: " << e.what() << '\n';
        }
    }
}

void System::printSnapshot(std::ostream& out) const {
    out << "\n===== Snapshot =====\n[ATMs]\n";
    for (const auto& atm : atms_) {
        out << "  ATM " << atm->serial() << " (" << atm->primaryBank().name() << ", " << atm->typeName()
            << "): " << atm->cash().toString() << " | Total " << text::formatWon(atm->cash().totalAmount())
            << " KRW\n";
    }
    out << "[Accounts]\n";
    for (const auto& bank : banks_) {
        for (const auto& acc : bank->accounts()) {
            out << "  " << bank->name() << " " << acc->accountNumber() << " (" << acc->ownerName()
                << "): " << text::formatWon(acc->balance()) << " KRW\n";
        }
    }
    out << "====================\n";
}

// ── 검색 ────────────────────────────────────────────────────────────────
Bank* System::findBank(const std::string& name) const {
    const auto it = std::find_if(banks_.begin(), banks_.end(),
                                 [&name](const std::unique_ptr<Bank>& b) { return b->name() == name; });
    return it == banks_.end() ? nullptr : it->get();
}

Card* System::findCard(const std::string& cardNumber) const {
    for (const auto& bank : banks_) {
        if (Card* card = bank->findCard(cardNumber)) return card;
    }
    return nullptr;
}

Account* System::findAccount(const std::string& accountNumber) const {
    for (const auto& bank : banks_) {
        if (Account* acc = bank->findAccount(accountNumber)) return acc;
    }
    return nullptr;
}
