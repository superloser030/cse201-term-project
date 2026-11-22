#ifndef CONSOLE_IO_H
#define CONSOLE_IO_H

#include <functional>
#include <string>

// 모든 콘솔 입력을 담당. System이 1개 소유하고 System::io()로 제공
// 공통 규칙
//   '/'          → snapshot handler 호출 후 다시 입력 대기 (REQ10.1)
//   입력 끝(EOF) → InputClosedException (무한 루프 방지)
// handler는 생성 시 받으므로 비어 있을 수 없음
class ConsoleIO {
public:
    explicit ConsoleIO(std::function<void()> snapshotHandler);

    // session 안에서 쓰는 입력: 'c' 입력 시 CancelException (결정 13)
    std::string readToken();      // 공백 전까지 한 단어 (카드 번호, 계좌번호 등)
    long long readNumber();       // 정수. 숫자가 아니면 InvalidInputException(ERR_INVALID_INPUT)

    // 취소를 적용하지 않는 입력 (파일 이름 입력, ATM 선택 등 session 밖)
    std::string readLine();       // 한 줄 전체, 빈 줄 허용 (Enter = 기본 파일)
    long long readNumberNoCancel();

    static constexpr const char* CANCEL_TOKEN = "c";

private:
    std::function<void()> snapshotHandler_;
};

#endif
