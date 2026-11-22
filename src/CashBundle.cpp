#include "CashBundle.h"
#include "Exceptions.h"
#include "TextUtil.h"

#include <algorithm>
#include <functional>
#include <numeric>

CashBundle::CashBundle(const std::array<int, KINDS>& counts) : counts_(counts) {}

int CashBundle::count(int index) const { return counts_.at(static_cast<std::size_t>(index)); }

int CashBundle::totalBills() const {
    return std::accumulate(counts_.begin(), counts_.end(), 0);
}

long long CashBundle::totalAmount() const {
    return std::inner_product(counts_.begin(), counts_.end(), DENOMINATIONS.begin(), 0LL,
                              std::plus<>(),
                              [](int count, int denom) { return static_cast<long long>(count) * denom; });
}

void CashBundle::add(const CashBundle& other) {
    for (std::size_t i = 0; i < counts_.size(); ++i) counts_[i] += other.counts_[i];
}

bool CashBundle::contains(const CashBundle& other) const {
    for (std::size_t i = 0; i < counts_.size(); ++i) {
        if (counts_[i] < other.counts_[i]) return false;
    }
    return true;
}

void CashBundle::remove(const CashBundle& other) {
    if (!contains(other)) throw InsufficientCashException(MsgId::ERR_INSUFFICIENT_CASH);
    for (std::size_t i = 0; i < counts_.size(); ++i) counts_[i] -= other.counts_[i];
}

DispenseResult CashBundle::planDispense(long long amount, int maxBills, CashBundle& out) const {
    if (amount <= 0) return DispenseResult::NOT_POSSIBLE;

    std::array<int, KINDS> plan{};
    long long remaining = amount;
    for (std::size_t i = 0; i < counts_.size(); ++i) {
        const long long denom = DENOMINATIONS[i];
        const long long take = std::min<long long>(counts_[i], remaining / denom);
        plan[i] = static_cast<int>(take);
        remaining -= take * denom;
    }
    if (remaining != 0) return DispenseResult::NOT_POSSIBLE;

    CashBundle result(plan);
    if (result.totalBills() > maxBills) return DispenseResult::TOO_MANY_BILLS;
    out = result;
    return DispenseResult::OK;
}

std::string CashBundle::toString(bool skipZero) const {
    std::string out;
    for (std::size_t i = 0; i < counts_.size(); ++i) {
        if (skipZero && counts_[i] == 0) continue;
        if (!out.empty()) out += ' ';
        out += text::formatWon(DENOMINATIONS[i]) + "x" + std::to_string(counts_[i]);
    }
    return out.empty() ? "-" : out;
}
