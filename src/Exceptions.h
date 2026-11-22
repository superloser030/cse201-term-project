#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <string>

// ── session 중 예외 (REQ9.1) ─────────────────────────────────────────────
// Session::run()에서 catch → 오류 메시지 출력 → session 종료
class ATMException : public std::runtime_error {
public:
    explicit ATMException(const std::string& msg) : std::runtime_error(msg) {}
};

class InvalidCardException : public ATMException {         // REQ3.1~3.2, 결정 2
public:
    using ATMException::ATMException;
};

class WrongPasswordException : public ATMException {       // REQ3.5 (3회 연속 실패)
public:
    using ATMException::ATMException;
};

class InsufficientFundsException : public ATMException {   // REQ5.2 계좌 잔액 부족
public:
    using ATMException::ATMException;
};

class InsufficientCashException : public ATMException {    // REQ5.2 ATM 현금 부족, 결정 8
public:
    using ATMException::ATMException;
};

class LimitExceededException : public ATMException {       // REQ4.2, 5.6, 5.7, 결정 10
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

// ── 입력 파일 오류 (결정 7) ──────────────────────────────────────────────
// session 예외와 별개. System::load()에서 throw → main에서 오류 출력 후 프로그램 종료
class FileFormatException : public std::runtime_error {
public:
    explicit FileFormatException(const std::string& msg) : std::runtime_error(msg) {}
};

#endif
