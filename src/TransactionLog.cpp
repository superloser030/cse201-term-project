#include "TransactionLog.h"
#include "Messages.h"
#include "Transaction.h"

#include <fstream>
#include <ostream>
#include <utility>

void TransactionLog::add(std::shared_ptr<const Transaction> tx) {
    entries_.push_back(std::move(tx));
}

void TransactionLog::print(std::ostream& out, const Messages& msg) const {
    if (entries_.empty()) {
        out << msg.get(MsgId::NO_TRANSACTIONS) << '\n';
        return;
    }
    for (const auto& tx : entries_) out << tx->describe(msg) << '\n';   // 거래 종류별 describe (polymorphism)
}

bool TransactionLog::saveToFile(const std::string& path, const Messages& msg) const {
    std::ofstream file(path);
    if (!file) return false;
    file << msg.get(MsgId::HISTORY_TITLE) << '\n';
    print(file, msg);
    return static_cast<bool>(file);
}
