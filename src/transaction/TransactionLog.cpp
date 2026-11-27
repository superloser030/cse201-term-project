#include "transaction/TransactionLog.h"
#include "common/Messages.h"
#include "transaction/Transaction.h"

#include <fstream>
#include <ostream>
#include <utility>

void TransactionLog::add(std::shared_ptr<const Transaction> tx) {
    entries_.push_back(std::move(tx));
}

void TransactionLog::printForAtm(std::ostream& out, const Messages& msg, const std::string& atmSerial) const {
    out << msg.format(MsgId::HISTORY_TITLE, atmSerial) << '\n';
    bool any = false;
    for (const auto& tx : entries_) {
        if (tx->atmSerial() != atmSerial) continue;
        out << tx->describe(msg) << '\n';             // 거래 종류별 describe (polymorphism)
        any = true;
    }
    if (!any) out << msg.get(MsgId::NO_TRANSACTIONS) << '\n';
}

bool TransactionLog::saveForAtm(const std::string& path, const Messages& msg, const std::string& atmSerial) const {
    std::ofstream file(path);
    if (!file) return false;
    printForAtm(file, msg, atmSerial);
    return static_cast<bool>(file);
}
