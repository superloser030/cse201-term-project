#include "system/ConsoleIO.h"
#include "common/Exceptions.h"
#include "common/TextUtil.h"

#include <cstdio>
#include <iostream>
#include <utility>

#ifdef _WIN32
#include <io.h>
namespace { bool stdinIsTerminal() { return _isatty(_fileno(stdin)) != 0; } }
#else
#include <unistd.h>
namespace { bool stdinIsTerminal() { return isatty(fileno(stdin)) != 0; } }
#endif

ConsoleIO::ConsoleIO(std::function<void()> snapshotHandler)
    : snapshotHandler_(std::move(snapshotHandler)), echo_(!stdinIsTerminal()) {}

std::string ConsoleIO::nextLine(bool allowCancel) {
    while (true) {
        std::string line;
        if (!std::getline(std::cin, line)) throw InputClosedException();
        const std::string value = text::trim(line);
        if (echo_) std::cout << value << '\n';           // 파일 입력일 때 실행 기록에 입력값 표시

        if (value == "/") {                              // REQ10.1: 어느 입력에서든 snapshot
            if (snapshotHandler_) snapshotHandler_();
            std::cout << "(continue input) ";
            continue;
        }
        if (allowCancel && text::toLower(value) == CANCEL_TOKEN) throw CancelException();
        return value;
    }
}

long long ConsoleIO::parseNumber(const std::string& s, long long min, long long max) {
    if (s.empty()) throw InvalidInputException(MsgId::ERR_INVALID_INPUT);
    long long value = 0;
    try {
        std::size_t pos = 0;
        value = std::stoll(s, &pos);
        if (pos != s.size()) throw InvalidInputException(MsgId::ERR_INVALID_INPUT);
    } catch (const InvalidInputException&) {
        throw;
    } catch (const std::exception&) {                    // invalid_argument, out_of_range
        throw InvalidInputException(MsgId::ERR_INVALID_INPUT);
    }
    if (value < min || value > max) throw InvalidInputException(MsgId::ERR_INVALID_INPUT);
    return value;
}

std::string ConsoleIO::readToken() { return nextLine(true); }

long long ConsoleIO::readNumber(long long min, long long max) {
    return parseNumber(nextLine(true), min, max);
}

std::string ConsoleIO::readLine() { return nextLine(false); }

long long ConsoleIO::readNumberNoCancel(long long min, long long max) {
    return parseNumber(nextLine(false), min, max);
}
