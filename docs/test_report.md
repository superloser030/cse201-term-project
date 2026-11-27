# Prototype 테스트 결과 (2026-11-23 작성, 2026-11-28 조교 답변 반영)

- branch: `prototype`
- build: MSVC (VS 2022 Build Tools) `/std:c++17 /EHsc /W4 /permissive- /utf-8` → **오류 0, warning 0**
- 자동 테스트: `bash test/run_tests.sh` → **47개 중 47개 통과** (2026-11-28, 조교 답변 반영 후)
- 메모리 검사: `/fsanitize=address`(AddressSanitizer)로 build해 전체 입력 실행 → **ASan 오류 보고 0건**
  (파일 오류 테스트 33~37은 의도대로 종료 코드 1). 조교 답변 반영 후 47개로 다시 실행해도 0건
- 실행 기록: `test/output/<case>.out`

## 테스트 목록과 대응 요구사항
초기 조건: 별도 표시가 없으면 `sample_initial_condition.txt`, `rich`는 `test/data/rich_condition.txt`
(은행 3, 계좌 4, ATM 3: 000111 Kakao Single Uni / 000222 IM Multi Bi / 000333 Shinhan Multi Bi).

| # | 테스트 | REQ / 결정 | 확인 내용 |
|---|---|---|---|
| 01 | tc1_sample | **Sample Test Case 1**, REQ4.1·4.3·4.5, 10.1 | 4,000 → 8,000, ATM1 1,000원권 3 → 7, 수수료 0원이면 수수료 입력 생략 |
| 02 | invalid_card_single | REQ1.2, 3.1, 3.2 | Single ATM에 타행 카드 → Invalid card |
| 03 | wrong_password_3 | REQ3.4, 3.5 | 남은 횟수 2, 1 → 3회 실패 시 session 중단 |
| 04 | password_retry_success | REQ3.3, 3.4 | 1회 틀린 뒤 성공 |
| 05 | nonprimary_deposit_fee | REQ1.8, 4.4, 4.5 | 타행 입금 수수료 2,000원 현금 별도 투입, ATM 현금 증가 |
| 06 | fee_mismatch | 결정 11 | 수수료 불일치 → 오류, 투입금 반환, 잔액·ATM 변화 없음 |
| 07 | check_deposit | REQ4.1, 4.6, 결정 14 | 수표 1장 금액만 입력(장수 묻지 않음), ATM 현금 불변 |
| 08 | check_below_min | REQ1.10 | 99,999원 수표 → 오류 |
| 09 | check_nonprimary_fee | REQ4.4, 4.6 | 수표 입금 수수료는 현금, ATM에는 수수료 지폐만 증가 |
| 10 | withdraw_greedy_17000 | REQ5.1, 5.3, 5.4 | 17,000 = 10,000×1 + 5,000×1 + 1,000×2, 수수료 2,000 |
| 11 | withdraw_3_limit | REQ5.6 | 4번째 출금 → 오류, 3건 요약 출력 |
| 12 | withdraw_limit_500k | REQ5.7 | 501,000 → 오류 / 500,000 → 50,000×10 |
| 13 | withdraw_not_multiple | 출금 단위 | 1,500원 → 오류 |
| 14 | insufficient_funds | REQ5.2 | 잔액 500원 계좌 1,000원 출금 → 오류(잔액 표시) |
| 15 | atm_insufficient_cash | REQ5.2, 5.5 | 500,000 성공 후 200,000 → ATM 현금 부족 |
| 16 | withdraw_too_many_bills | 결정 8 | 1,000원권만 있는 ATM에서 55,000 → 50장 초과 오류 |
| 17 | deposit_bills_with_fee_limit | REQ4.2, 결정 10 | 입금 51장 → 초과 / 입금 50장 + 수수료 2장 → 성공 (수수료 지폐는 한도 제외) |
| 18 | cash_transfer | REQ6.1~6.3, 6.6, 6.7 | 11,000 투입 → 10,000 이체, 수수료 1,000, ATM 현금 증가 |
| 19 | cash_transfer_cancel | REQ6.3, 결정 13 | 금액 확인에서 취소 → 투입금 반환, 변화 없음 |
| 20 | account_transfer_fees | REQ1.8, 6.4, 6.5, 6.7, 추가 4 | 수수료 2,000 / 3,000 / 5,000, 하이픈 없는 계좌번호 입력 |
| 21 | same_account | 이체 검사 | 같은 계좌로 이체 → 오류 |
| 22 | account_not_found | REQ6.2 | 없는 계좌 → 오류 |
| 23 | admin_history | REQ7.1~7.3, 결정 1·2, 추가 4 | 그 ATM의 거래 표시, ATM별 파일 저장, `admin-kakao`(소문자) 인식 |
| 24 | admin_no_transactions | REQ7.1, 7.2 | IM 관리자가 IM ATM에서 조회, 거래 없을 때 안내 |
| 25 | korean | REQ8.1, 8.2, 2.3 | 한국어 메뉴·오류·요약 |
| 26 | korean_invalid_card | REQ8.2, 3.2 | 한국어 오류 문구 |
| 27 | cancel | 결정 13, REQ2.2 | 메뉴에서 취소, 카드 입력 전 취소 |
| 28 | snapshot_midsession | REQ10.1 | 비밀번호 입력 중 `/` → snapshot 후 계속 |
| 29 | eof | crash 방지 | 입력이 끝나면 정상 종료 |
| 30 | invalid_input | REQ9.1 | 범위 밖 ATM 번호, 숫자 아닌 메뉴 입력 |
| 31 | hyphenless_card | 추가 4 | `111111111111`, `1111 1111 1111` 모두 인식 |
| 32 | file_not_found_retry | 결정 6 | 없는 파일 → 다시 묻기 → 기본 파일 |
| 33 | file_duplicate_card | 결정 7 | 하이픈만 다른 카드 번호도 중복으로 처리 → 종료 |
| 34 | file_unknown_bank | 결정 7 | 목록에 없는 은행 → 종료 |
| 35 | file_bad_type | 결정 7 | ATM 종류 값 오류 → 종료 |
| 36 | file_truncated | 결정 7 | 줄 부족 → 종료 |
| 37 | file_admin_collision | 결정 7 | 관리자 카드 번호와 같은 카드 → 종료 |
| 38 | crlf_file | 입력 견고성 | Windows 줄바꿈(CRLF) 파일 정상 로드 |
| 39 | explicit_filename | REQ1.11 | 파일 이름 직접 입력 |
| 40 | withdraw_nonprimary_fee | REQ1.8 | 타행 출금 수수료 3,000 |
| 41 | withdraw_count_new_session | REQ5.6 | 새 session에서 출금 다시 가능 |
| 42 | cash_transfer_not_enough | REQ6.3 | 수수료 이하 투입 → 오류, 반환 |
| 43 | deposit_no_cash | 입력 검사 | 0장 입금 → 오류 |
| 44 | admin_korean | REQ7, 8.2 | 한국어 관리자 화면 (ATM 000222의 거래) |
| 45 | cancel_during_bills | 결정 13 | 지폐 입력 도중 취소 → 투입금 반환 안내 |
| 46 | admin_other_bank_atm | 결정 2 | 다른 은행 ATM에 관리자 카드 → Invalid card |
| 47 | admin_only_this_atm | 결정 1 (조교 답변 a) | ATM2의 거래가 ATM1 관리자 화면·파일에 나오지 않음, ATM2 관리자에게는 나옴 |

## 확인하지 못한 것
- **실제 console 창의 한국어 표시**: 테스트는 출력을 파일로 받아 UTF-8 문자열을 비교한 것이다.
  실제 cmd/PowerShell/Windows Terminal 창에서 한글이 깨지지 않는지는 확인하지 않았다(확인 안 됨).
  프로그램은 시작 시 `SetConsoleOutputCP(CP_UTF8)`을 호출한다. demo 노트북에서 직접 실행해 확인할 것.
- **다른 compiler(g++, clang)**: 이 PC에 없어서 MSVC로만 build했다.
- **사람이 직접 입력하는 대화형 실행**: 자동 테스트는 입력을 파일로 넣었다. 파일 입력일 때는 입력값을 화면에 다시 출력(echo)하므로,
  대화형 실행 화면은 테스트 출력과 입력 줄 표시 방식이 다르다.
