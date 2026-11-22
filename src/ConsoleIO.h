#ifndef CONSOLE_IO_H
#define CONSOLE_IO_H

#include <functional>
#include <iosfwd>
#include <string>

// 모든 콘솔 입력을 담당. System이 1개 소유하고 System::io()로 제공
// 입력은 항상 한 줄 단위로 읽음 (>> 와 getline 혼용 문제 방지)
// 공통 규칙
//   '/'          → snapshot handler 호출 후 다시 입력 대기 (REQ10.1)
//   입력 끝(EOF) → InputClosedException (무한 루프 방지)
//   표준 입력이 파일(redirect)이면 읽은 값을 화면에 다시 출력(echo)해 실행 기록을 읽기 쉽게 함
class ConsoleIO {
public:
    explicit ConsoleIO(std::function<void()> snapshotHandler);

    // session 안에서 쓰는 입력: 'c' 입력 시 CancelException (결정 13)
    std::string readToken();                              // 한 줄 (카드 번호, 계좌번호, 비밀번호 등)
    long long readNumber(long long min, long long max);   // 범위 밖·숫자 아님 → InvalidInputException

    // 취소를 적용하지 않는 입력 (파일 이름 입력, ATM 선택 등 session 밖)
    std::string readLine();                               // 빈 줄 허용 (Enter = 기본 파일)
    long long readNumberNoCancel(long long min, long long max);

    static constexpr const char* CANCEL_TOKEN = "c";

private:
    std::string nextLine(bool allowCancel);
    static long long parseNumber(const std::string& s, long long min, long long max);

    std::function<void()> snapshotHandler_;
    bool echo_;
};

#endif
