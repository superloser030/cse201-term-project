#ifndef TRANSACTION_LOG_H
#define TRANSACTION_LOG_H

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

class Transaction;
class Messages;

// ATM 1대의 성공 거래 기록 (결정 1: 관리자는 그 ATM의 모든 사용자 거래를 봄)
class TransactionLog {
public:
    void add(std::shared_ptr<const Transaction> tx);
    bool empty() const { return entries_.empty(); }

    void print(std::ostream& out, const Messages& msg) const;          // REQ7.2
    void saveToFile(const std::string& path, const Messages& msg) const; // REQ7.3

private:
    std::vector<std::shared_ptr<const Transaction>> entries_;
};

#endif
