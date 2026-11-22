#ifndef MESSAGES_H
#define MESSAGES_H

#include <string>

// 출력 문구 식별자. 구현하면서 필요한 항목을 추가한다
enum class MsgId {
    SELECT_LANGUAGE, INSERT_CARD, ENTER_PASSWORD,
    INVALID_CARD, WRONG_PASSWORD, PASSWORD_LOCKED, CARD_RETURNED,
    MAIN_MENU, ADMIN_MENU,
    DEPOSIT_TYPE, ENTER_BILLS, ENTER_CHECK_AMOUNT, ENTER_FEE_BILLS,
    ENTER_WITHDRAWAL_AMOUNT, TRANSFER_TYPE, ENTER_DEST_ACCOUNT, ENTER_TRANSFER_AMOUNT,
    CONFIRM_TRANSFER_AMOUNT, TRANSACTION_SUCCESS, SESSION_SUMMARY, NO_TRANSACTIONS,
    HISTORY_SAVED, ERROR_PREFIX
};

// 언어별 문구 (REQ8). Unilingual ATM은 항상 EnglishMessages
class Messages {
public:
    virtual ~Messages() = default;
    virtual std::string get(MsgId id) const = 0;
};

class EnglishMessages : public Messages {
public:
    std::string get(MsgId id) const override;
};

class KoreanMessages : public Messages {
public:
    std::string get(MsgId id) const override;
};

#endif
