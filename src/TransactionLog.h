#ifndef TRANSACTION_LOG_H
#define TRANSACTION_LOG_H

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

class Transaction;
class Messages;

// 시스템이 시작된 이후의 성공 거래 기록. System이 1개 소유
// 관리자 화면·파일에는 관리자 카드를 넣은 ATM의 거래만 출력 (결정 1, 조교 답변 (a))
class TransactionLog {
public:
    void add(std::shared_ptr<const Transaction> tx);

    // atmSerial ATM에서 일어난 거래만 출력 (REQ7.2)
    void printForAtm(std::ostream& out, const Messages& msg, const std::string& atmSerial) const;
    // 같은 내용을 파일로 저장 (REQ7.3)
    bool saveForAtm(const std::string& path, const Messages& msg, const std::string& atmSerial) const;

private:
    std::vector<std::shared_ptr<const Transaction>> entries_;
};

#endif
