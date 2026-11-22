#include "Messages.h"
#include "TextUtil.h"

namespace {

std::string replacePlaceholder(std::string pattern, const std::string& value) {
    const std::size_t pos = pattern.find("{}");
    if (pos != std::string::npos) pattern.replace(pos, 2, value);
    return pattern;
}

}  // namespace

std::string Messages::format(MsgId id, long long number) const {
    return replacePlaceholder(get(id), text::formatWon(number));
}

std::string Messages::format(MsgId id, const std::string& value) const {
    return replacePlaceholder(get(id), value);
}

const Messages& Messages::english() {
    static const EnglishMessages instance;
    return instance;
}

const Messages& Messages::korean() {
    static const KoreanMessages instance;
    return instance;
}

// ── English ──────────────────────────────────────────────────────────────
std::string EnglishMessages::get(MsgId id) const {
    switch (id) {
    case MsgId::SELECT_LANGUAGE:         return "Select language / 언어 선택: 1) English  2) 한국어";
    case MsgId::SELECT_PROMPT:           return "Select: ";
    case MsgId::INSERT_CARD:             return "Insert your card (enter card number): ";
    case MsgId::CANCEL_HINT:             return "(Enter 'c' at any prompt to cancel)";
    case MsgId::ENTER_PASSWORD:          return "Enter password: ";
    case MsgId::CARD_RETURNED:           return "Card returned. Thank you.";
    case MsgId::CANCELLED:               return "Cancelled by user.";
    case MsgId::CASH_RETURNED:           return "Inserted cash has been returned.";
    case MsgId::MAIN_MENU:               return "1) Deposit  2) Withdrawal  3) Transfer  4) End session";
    case MsgId::ADMIN_MENU:              return "[Admin Menu] 1) Transaction History";
    case MsgId::DEPOSIT_TYPE:            return "1) Cash  2) Check";
    case MsgId::ENTER_BILLS:             return "Insert cash: enter the number of bills for each denomination.";
    case MsgId::BILL_COUNT:              return "  Number of {} KRW bills: ";
    case MsgId::ENTER_CHECK_COUNT:       return "Number of checks: ";
    case MsgId::ENTER_CHECK_AMOUNT:      return "  Amount of check #{}: ";
    case MsgId::ENTER_FEE_BILLS:         return "A fee of {} KRW is required. Insert the fee in cash separately.";
    case MsgId::ENTER_WITHDRAWAL_AMOUNT: return "Amount to withdraw (KRW): ";
    case MsgId::DISPENSED:               return "Dispensed bills: {}";
    case MsgId::TRANSFER_TYPE:           return "1) Cash transfer  2) Account transfer";
    case MsgId::ENTER_DEST_ACCOUNT:      return "Destination account number: ";
    case MsgId::ENTER_TRANSFER_CASH:     return "Insert the cash to transfer, including the fee of {} KRW.";
    case MsgId::CONFIRM_TRANSFER_AMOUNT: return "Amount to be transferred (fee excluded): {} KRW.  1) OK  2) Cancel";
    case MsgId::ENTER_TRANSFER_AMOUNT:   return "Amount to transfer (KRW): ";
    case MsgId::TRANSACTION_SUCCESS:     return "Transaction completed. (Transaction ID: {})";
    case MsgId::BALANCE:                 return "Current balance: {} KRW";
    case MsgId::SESSION_SUMMARY:         return "===== Session Summary =====";
    case MsgId::SUMMARY_ACCOUNT:         return "Account: {}";
    case MsgId::HISTORY_TITLE:           return "===== Transaction History (all ATMs, since system start) =====";
    case MsgId::NO_TRANSACTIONS:         return "No transactions.";
    case MsgId::HISTORY_SAVED:           return "Transaction history saved to file: {}";

    case MsgId::MONEY:                   return "{} KRW";
    case MsgId::LBL_TX:                  return "TX";
    case MsgId::LBL_CARD:                return "Card";
    case MsgId::LBL_AMOUNT:              return "Amount";
    case MsgId::LBL_FEE:                 return "Fee";
    case MsgId::LBL_FROM:                return "From";
    case MsgId::LBL_TO:                  return "To";
    case MsgId::LBL_BILLS:               return "Bills";
    case MsgId::LBL_CHECK_COUNT:         return "Checks";
    case MsgId::LBL_DEPOSIT_CASH:        return "Cash Deposit";
    case MsgId::LBL_DEPOSIT_CHECK:       return "Check Deposit";
    case MsgId::LBL_WITHDRAWAL:          return "Withdrawal";
    case MsgId::LBL_CASH_TRANSFER:       return "Cash Transfer";
    case MsgId::LBL_ACCOUNT_TRANSFER:    return "Account Transfer";

    case MsgId::ERROR_PREFIX:            return "[Error] ";
    case MsgId::ERR_INVALID_CARD:        return "Invalid card.";
    case MsgId::ERR_WRONG_PASSWORD:      return "Wrong password. Attempts left: {}";
    case MsgId::ERR_PASSWORD_LOCKED:     return "Wrong password entered 3 times in a row. Session aborted.";
    case MsgId::ERR_INSUFFICIENT_FUNDS:  return "Insufficient account balance. (Balance: {} KRW)";
    case MsgId::ERR_INSUFFICIENT_CASH:   return "This ATM does not have enough cash to dispense the amount.";
    case MsgId::ERR_TOO_MANY_BILLS:      return "Too many bills. Maximum {} bills per transaction.";
    case MsgId::ERR_TOO_MANY_CHECKS:     return "Too many checks. Maximum {} check(s) per transaction.";
    case MsgId::ERR_WITHDRAWAL_LIMIT:    return "Exceeds the withdrawal limit of {} KRW per transaction.";
    case MsgId::ERR_WITHDRAWAL_COUNT:    return "Maximum of {} withdrawals per session reached.";
    case MsgId::ERR_INVALID_CHECK:       return "Invalid check. The minimum check amount is {} KRW.";
    case MsgId::ERR_INVALID_AMOUNT:      return "Invalid amount.";
    case MsgId::ERR_NOT_MULTIPLE_OF_1000:return "The amount must be a multiple of 1,000 KRW.";
    case MsgId::ERR_NO_CASH:             return "No cash was inserted.";
    case MsgId::ERR_ACCOUNT_NOT_FOUND:   return "Account not found.";
    case MsgId::ERR_SAME_ACCOUNT:        return "The source and destination accounts are the same.";
    case MsgId::ERR_FEE_MISMATCH:        return "The inserted fee does not match the required fee of {} KRW.";
    case MsgId::ERR_INVALID_INPUT:       return "Invalid input.";
    case MsgId::ERR_UNEXPECTED:          return "Unexpected error.";
    }
    return "";
}

// ── 한국어 ───────────────────────────────────────────────────────────────
std::string KoreanMessages::get(MsgId id) const {
    switch (id) {
    case MsgId::SELECT_LANGUAGE:         return "Select language / 언어 선택: 1) English  2) 한국어";
    case MsgId::SELECT_PROMPT:           return "선택: ";
    case MsgId::INSERT_CARD:             return "카드를 넣어 주세요 (카드 번호 입력): ";
    case MsgId::CANCEL_HINT:             return "(취소하려면 언제든 'c'를 입력하세요)";
    case MsgId::ENTER_PASSWORD:          return "비밀번호를 입력하세요: ";
    case MsgId::CARD_RETURNED:           return "카드가 반환되었습니다. 감사합니다.";
    case MsgId::CANCELLED:               return "사용자가 취소했습니다.";
    case MsgId::CASH_RETURNED:           return "투입한 현금을 반환했습니다.";
    case MsgId::MAIN_MENU:               return "1) 입금  2) 출금  3) 이체  4) 거래 종료";
    case MsgId::ADMIN_MENU:              return "[관리자 메뉴] 1) 거래 내역";
    case MsgId::DEPOSIT_TYPE:            return "1) 현금  2) 수표";
    case MsgId::ENTER_BILLS:             return "현금을 넣어 주세요: 권종별 장수를 입력하세요.";
    case MsgId::BILL_COUNT:              return "  {}원권 장수: ";
    case MsgId::ENTER_CHECK_COUNT:       return "수표 장수: ";
    case MsgId::ENTER_CHECK_AMOUNT:      return "  {}번째 수표 금액: ";
    case MsgId::ENTER_FEE_BILLS:         return "수수료 {}원이 필요합니다. 수수료를 현금으로 따로 넣어 주세요.";
    case MsgId::ENTER_WITHDRAWAL_AMOUNT: return "출금할 금액(원): ";
    case MsgId::DISPENSED:               return "지급한 지폐: {}";
    case MsgId::TRANSFER_TYPE:           return "1) 현금 이체  2) 계좌 이체";
    case MsgId::ENTER_DEST_ACCOUNT:      return "받는 계좌번호: ";
    case MsgId::ENTER_TRANSFER_CASH:     return "수수료 {}원을 포함해 이체할 현금을 넣어 주세요.";
    case MsgId::CONFIRM_TRANSFER_AMOUNT: return "이체될 금액(수수료 제외): {}원.  1) 확인  2) 취소";
    case MsgId::ENTER_TRANSFER_AMOUNT:   return "이체할 금액(원): ";
    case MsgId::TRANSACTION_SUCCESS:     return "거래가 완료되었습니다. (거래 번호: {})";
    case MsgId::BALANCE:                 return "현재 잔액: {}원";
    case MsgId::SESSION_SUMMARY:         return "===== 거래 요약 =====";
    case MsgId::SUMMARY_ACCOUNT:         return "계좌: {}";
    case MsgId::HISTORY_TITLE:           return "===== 거래 내역 (전체 ATM, 시스템 시작 이후) =====";
    case MsgId::NO_TRANSACTIONS:         return "거래 내역이 없습니다.";
    case MsgId::HISTORY_SAVED:           return "거래 내역을 파일로 저장했습니다: {}";

    case MsgId::MONEY:                   return "{}원";
    case MsgId::LBL_TX:                  return "거래";
    case MsgId::LBL_CARD:                return "카드";
    case MsgId::LBL_AMOUNT:              return "금액";
    case MsgId::LBL_FEE:                 return "수수료";
    case MsgId::LBL_FROM:                return "출금 계좌";
    case MsgId::LBL_TO:                  return "입금 계좌";
    case MsgId::LBL_BILLS:               return "지폐";
    case MsgId::LBL_CHECK_COUNT:         return "수표";
    case MsgId::LBL_DEPOSIT_CASH:        return "현금 입금";
    case MsgId::LBL_DEPOSIT_CHECK:       return "수표 입금";
    case MsgId::LBL_WITHDRAWAL:          return "출금";
    case MsgId::LBL_CASH_TRANSFER:       return "현금 이체";
    case MsgId::LBL_ACCOUNT_TRANSFER:    return "계좌 이체";

    case MsgId::ERROR_PREFIX:            return "[오류] ";
    case MsgId::ERR_INVALID_CARD:        return "유효하지 않은 카드입니다.";
    case MsgId::ERR_WRONG_PASSWORD:      return "비밀번호가 틀렸습니다. 남은 횟수: {}";
    case MsgId::ERR_PASSWORD_LOCKED:     return "비밀번호를 3회 연속 틀려 거래를 중단합니다.";
    case MsgId::ERR_INSUFFICIENT_FUNDS:  return "계좌 잔액이 부족합니다. (잔액: {}원)";
    case MsgId::ERR_INSUFFICIENT_CASH:   return "ATM에 지급할 현금이 부족합니다.";
    case MsgId::ERR_TOO_MANY_BILLS:      return "지폐가 너무 많습니다. 거래당 최대 {}장입니다.";
    case MsgId::ERR_TOO_MANY_CHECKS:     return "수표가 너무 많습니다. 거래당 최대 {}장입니다.";
    case MsgId::ERR_WITHDRAWAL_LIMIT:    return "1회 출금 한도 {}원을 초과했습니다.";
    case MsgId::ERR_WITHDRAWAL_COUNT:    return "세션당 최대 출금 횟수({}회)를 초과했습니다.";
    case MsgId::ERR_INVALID_CHECK:       return "유효하지 않은 수표입니다. 최소 금액은 {}원입니다.";
    case MsgId::ERR_INVALID_AMOUNT:      return "잘못된 금액입니다.";
    case MsgId::ERR_NOT_MULTIPLE_OF_1000:return "금액은 1,000원 단위여야 합니다.";
    case MsgId::ERR_NO_CASH:             return "투입된 현금이 없습니다.";
    case MsgId::ERR_ACCOUNT_NOT_FOUND:   return "계좌를 찾을 수 없습니다.";
    case MsgId::ERR_SAME_ACCOUNT:        return "보내는 계좌와 받는 계좌가 같습니다.";
    case MsgId::ERR_FEE_MISMATCH:        return "넣은 수수료가 필요한 수수료 {}원과 다릅니다.";
    case MsgId::ERR_INVALID_INPUT:       return "잘못된 입력입니다.";
    case MsgId::ERR_UNEXPECTED:          return "예상하지 못한 오류가 발생했습니다.";
    }
    return "";
}
