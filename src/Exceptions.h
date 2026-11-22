#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include "Messages.h"
#include <stdexcept>
#include <string>

// ════════════════════════════════════════════════════════════════════════
// 예외를 받는 위치 (crash 방지 규칙)
//   Session::run()        : 거래 중 ATMException(FeeMismatchException은 따로), CancelException
//   ATM::startSession()   : session 전 단계(언어·카드·인증)의 ATMException, CancelException
//   System::run()         : 그 밖의 std::exception (마지막 방어선, ATM 선택 화면으로 복귀)
//   main()                : FileFormatException → 오류 출력 후 종료
//                           InputClosedException → 정상 종료
// ════════════════════════════════════════════════════════════════════════

// ── session 중 오류 (REQ9.1) ─────────────────────────────────────────────
// 문구 대신 MsgId를 담음 → 받는 쪽이 선택된 언어로 출력 (REQ8.2)
// detail: 문구의 "{}"에 들어갈 숫자 (잔액, 한도 등). 없으면 -1
class ATMException : public std::runtime_error {
public:
    explicit ATMException(MsgId id, long long detail = -1)
        : std::runtime_error("ATMException(id=" + std::to_string(static_cast<int>(id)) + ")"),
          id_(id), detail_(detail) {}

    MsgId id() const { return id_; }
    long long detail() const { return detail_; }

private:
    MsgId id_;
    long long detail_;
};

class InvalidCardException : public ATMException {         // REQ3.1~3.2
public:
    using ATMException::ATMException;
};

class InsufficientFundsException : public ATMException {   // REQ5.2 계좌 잔액 부족
public:
    using ATMException::ATMException;
};

class InsufficientCashException : public ATMException {    // REQ5.2 ATM 현금 부족
public:
    using ATMException::ATMException;
};

class LimitExceededException : public ATMException {       // REQ4.2, 5.6, 5.7, 결정 8·10
public:
    using ATMException::ATMException;
};

class InvalidInputException : public ATMException {        // 잘못된 입력 범위 (REQ9.1 암시적 예외)
public:
    using ATMException::ATMException;
};

class FeeMismatchException : public ATMException {         // 결정 11: 수수료 현금 불일치 → 투입금 반환
public:
    using ATMException::ATMException;
};

// 선택된 언어로 오류 문구 만들기: "[Error] ..." / "[오류] ..."
inline std::string describeError(const Messages& msg, const ATMException& e) {
    std::string body = e.detail() >= 0 ? msg.format(e.id(), e.detail()) : msg.get(e.id());
    return msg.get(MsgId::ERROR_PREFIX) + body;
}

// ── 취소 (결정 13) ───────────────────────────────────────────────────────
// 오류가 아닌 정상 종료. ConsoleIO가 'c' 입력 시 throw
class CancelException : public std::exception {
public:
    const char* what() const noexcept override { return "Cancelled"; }
};

// ── 입력 끝(EOF) ─────────────────────────────────────────────────────────
// 더 읽을 입력이 없음 → main()에서 받아 프로그램 정상 종료 (무한 루프 방지)
class InputClosedException : public std::runtime_error {
public:
    InputClosedException() : std::runtime_error("Input closed") {}
};

// ── 입력 파일 오류 (결정 7) ──────────────────────────────────────────────
// 언어 선택 전(프로그램 시작 시) 발생 → 영어 문자열 그대로
class FileFormatException : public std::runtime_error {
public:
    explicit FileFormatException(const std::string& msg) : std::runtime_error(msg) {}
};

#endif
