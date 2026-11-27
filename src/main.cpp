#include "common/Exceptions.h"
#include "system/System.h"

#include <iostream>

#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);    // 한국어 출력 (소스는 UTF-8, /utf-8로 compile)
    SetConsoleCP(CP_UTF8);
#endif

    System system;
    try {
        system.promptAndLoad();                     // REQ1.11
        system.printAdminCards(std::cout);
        system.run();
    } catch (const FileFormatException& e) {        // 결정 7: 입력 파일 오류 → 종료
        std::cout << "[Error] Invalid initial condition file: " << e.what() << "\nProgram terminated.\n";
        return 1;
    } catch (const InputClosedException&) {         // 입력 끝 → 정상 종료
        std::cout << "\n[Input ended] Program terminated.\n";
        return 0;
    } catch (const std::exception& e) {
        std::cout << "[Error] " << e.what() << "\nProgram terminated.\n";
        return 1;
    }
    return 0;
}
