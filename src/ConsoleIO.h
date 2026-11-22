#ifndef CONSOLE_IO_H
#define CONSOLE_IO_H

#include <functional>
#include <string>

// 모든 콘솔 입력을 담당. '/' 입력 시 snapshot handler 호출 후 다시 입력 대기 (REQ10.1)
// System이 1개 소유하고 System::io()로 제공. handler는 생성 시 받으므로 비어 있을 수 없음
class ConsoleIO {
public:
    explicit ConsoleIO(std::function<void()> snapshotHandler);

    std::string readToken();      // 공백 전까지 한 단어 (카드 번호, 계좌번호 등)
    std::string readLine();       // 한 줄 전체 (파일 이름 입력 등, 빈 줄 허용)
    long long readNumber();       // 정수. 숫자가 아니면 InvalidInputException

private:
    std::function<void()> snapshotHandler_;
};

#endif
