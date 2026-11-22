#ifndef MESSAGES_H
#define MESSAGES_H

#include <string>

// 출력 문구 식별자. 문구 안의 "{}"는 format()이 값으로 바꿈
enum class MsgId {
    // ── 안내 ─────────────────────────────────────────────
    SELECT_LANGUAGE, SELECT_PROMPT, INSERT_CARD, CANCEL_HINT, ENTER_PASSWORD,
    CARD_RETURNED, CANCELLED, CASH_RETURNED,
    MAIN_MENU, ADMIN_MENU,
    DEPOSIT_TYPE, ENTER_BILLS, BILL_COUNT, ENTER_CHECK_COUNT, ENTER_CHECK_AMOUNT, ENTER_FEE_BILLS,
    ENTER_WITHDRAWAL_AMOUNT, DISPENSED,
    TRANSFER_TYPE, ENTER_DEST_ACCOUNT, ENTER_TRANSFER_CASH, CONFIRM_TRANSFER_AMOUNT, ENTER_TRANSFER_AMOUNT,
    TRANSACTION_SUCCESS, BALANCE, SESSION_SUMMARY, SUMMARY_ACCOUNT,
    HISTORY_TITLE, NO_TRANSACTIONS, HISTORY_SAVED,

    // ── 거래 기록 표시용 label ───────────────────────────
    MONEY, LBL_TX, LBL_CARD, LBL_AMOUNT, LBL_FEE, LBL_FROM, LBL_TO, LBL_BILLS, LBL_CHECK_COUNT,
    LBL_DEPOSIT_CASH, LBL_DEPOSIT_CHECK, LBL_WITHDRAWAL, LBL_CASH_TRANSFER, LBL_ACCOUNT_TRANSFER,

    // ── 오류 (ATMException이 담는 id, REQ9.1) ────────────
    ERROR_PREFIX,
    ERR_INVALID_CARD,           // REQ3.2
    ERR_WRONG_PASSWORD,         // REQ3.4 (detail: 남은 시도 횟수)
    ERR_PASSWORD_LOCKED,        // REQ3.5
    ERR_INSUFFICIENT_FUNDS,     // REQ5.2 (detail: 현재 잔액)
    ERR_INSUFFICIENT_CASH,      // REQ5.2
    ERR_TOO_MANY_BILLS,         // REQ4.2, 결정 8·10 (detail: 최대 장수)
    ERR_TOO_MANY_CHECKS,        // REQ4.2 (detail: 최대 장수)
    ERR_WITHDRAWAL_LIMIT,       // REQ5.7 (detail: 최대 금액)
    ERR_WITHDRAWAL_COUNT,       // REQ5.6 (detail: 최대 횟수)
    ERR_INVALID_CHECK,          // REQ1.10 (detail: 최소 금액)
    ERR_INVALID_AMOUNT,         // 0 이하 등
    ERR_NOT_MULTIPLE_OF_1000,   // 출금액 단위
    ERR_NO_CASH,                // 투입 현금 없음
    ERR_ACCOUNT_NOT_FOUND,      // REQ6.2
    ERR_SAME_ACCOUNT,           // 보내는 계좌 = 받는 계좌
    ERR_FEE_MISMATCH,           // 결정 11 (detail: 필요한 수수료)
    ERR_INVALID_INPUT,          // 숫자가 아닌 입력, 범위 밖 메뉴 번호
    ERR_UNEXPECTED              // 그 밖의 예외 (마지막 방어선)
};

// 언어별 문구 (REQ8). Unilingual ATM은 항상 english()
class Messages {
public:
    virtual ~Messages() = default;

    virtual std::string get(MsgId id) const = 0;               // 언어별 문구 (polymorphism)
    std::string format(MsgId id, long long number) const;      // "{}" → "1,234"
    std::string format(MsgId id, const std::string& text) const;

    static const Messages& english();
    static const Messages& korean();
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
