#ifndef MESSAGES_H
#define MESSAGES_H

#include <string>

// 출력 문구 식별자. 구현하면서 필요한 항목을 추가한다
enum class MsgId {
    // ── 안내 ─────────────────────────────────────────────
    SELECT_LANGUAGE, INSERT_CARD, ENTER_PASSWORD, CARD_RETURNED, CANCEL_HINT,
    MAIN_MENU, ADMIN_MENU,
    DEPOSIT_TYPE, ENTER_BILLS, ENTER_CHECK_AMOUNT, ENTER_FEE_BILLS,
    ENTER_WITHDRAWAL_AMOUNT, TRANSFER_TYPE, ENTER_DEST_ACCOUNT, ENTER_TRANSFER_AMOUNT,
    CONFIRM_TRANSFER_AMOUNT, TRANSACTION_SUCCESS, SESSION_SUMMARY, NO_TRANSACTIONS,
    HISTORY_SAVED, CANCELLED,

    // ── 오류 (ATMException이 담는 id, REQ9.1) ────────────
    ERR_INVALID_CARD,           // REQ3.2, 결정 2
    ERR_WRONG_PASSWORD,         // REQ3.4 (detail: 남은 시도 횟수)
    ERR_PASSWORD_LOCKED,        // REQ3.5
    ERR_INSUFFICIENT_FUNDS,     // REQ5.2 (detail: 현재 잔액)
    ERR_INSUFFICIENT_CASH,      // REQ5.2, 결정 8
    ERR_TOO_MANY_BILLS,         // REQ4.2, 결정 10 (detail: 최대 장수)
    ERR_WITHDRAWAL_LIMIT,       // REQ5.7 (detail: 최대 금액)
    ERR_WITHDRAWAL_COUNT,       // REQ5.6 (detail: 최대 횟수)
    ERR_INVALID_CHECK,          // REQ1.10 (detail: 최소 금액)
    ERR_INVALID_AMOUNT,         // 0 이하, 1,000원 단위 아님 등
    ERR_ACCOUNT_NOT_FOUND,      // REQ6.2
    ERR_SAME_ACCOUNT,           // 보내는 계좌 = 받는 계좌
    ERR_FEE_MISMATCH,           // 결정 11 (detail: 필요한 수수료)
    ERR_INVALID_INPUT,          // 숫자가 아닌 입력, 범위 밖 메뉴 번호
    ERR_UNEXPECTED              // 그 밖의 예외 (마지막 방어선)
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
