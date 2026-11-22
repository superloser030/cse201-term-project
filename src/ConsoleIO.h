#ifndef CONSOLE_IO_H
#define CONSOLE_IO_H

#include <functional>
#include <string>

// 모든 콘솔 입력을 담당. '/' 입력 시 snapshot handler 호출 후 다시 입력 대기 (REQ10.1)
// System에 직접 의존하지 않고 std::function callback으로 연결
class ConsoleIO {
public:
    static void setSnapshotHandler(std::function<void()> handler);

    static std::string readToken();      // 공백 전까지 한 단어 (카드 번호, 계좌번호 등)
    static std::string readLine();       // 한 줄 전체 (파일 이름 입력 등, 빈 줄 허용)
    static long long readNumber();       // 정수. 숫자가 아니면 InvalidInputException

private:
    static std::function<void()> snapshotHandler_;
};

#endif
