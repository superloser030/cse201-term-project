#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "Common.h"
#include <string>

class Transaction {
private:
    static int nextId;          // 시스템 전체 고유 ID 카운터 (REQ2.4)
    int txId;
    int sessionId;
    std::string atmSerial;
    std::string cardNumber;
    TxType type;
    int amount;
    int fee;
    std::string sourceAccount;  // 입금/현금이체 시 빈 문자열("")
    std::string destAccount;    // 출금 시 빈 문자열("")

public:
    Transaction(int sessionId, const std::string& atmSerial, const std::string& cardNum,
                TxType type, int amount, int fee,
                const std::string& srcAcc = "", const std::string& dstAcc = "");

    int getTxId() const { return txId; }
    int getSessionId() const { return sessionId; }
    std::string formatSummary(Language lang) const; // 세션 요약 및 관리자 출력용 포맷 문자열
};

#endif
