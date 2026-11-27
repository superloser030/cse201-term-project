#ifndef FEE_POLICY_H
#define FEE_POLICY_H

// 수수료 표 (REQ1.8)와 거래 한도 한 곳에 모음
// "primary" = 계좌 은행이 그 ATM의 주거래 은행과 같음
class FeePolicy {
public:
    static long long depositFee(bool accountIsPrimary);              // 0 / 2,000 (현금 추가 투입)
    static long long withdrawalFee(bool accountIsPrimary);           // 2,000 / 3,000 (계좌에서 차감)
    static long long accountTransferFee(bool sourceIsPrimary,
                                        bool destIsPrimary);         // 2,000 / 3,000 / 5,000 (보내는 계좌에서 차감)
    static long long cashTransferFee();                              // 1,000 (투입 현금에서 차감)

    static constexpr long long MAX_WITHDRAWAL_AMOUNT = 500000;       // REQ5.7, 수수료 미포함 (결정 9)
    static constexpr int MAX_WITHDRAWALS_PER_SESSION = 3;            // REQ5.6
    static constexpr long long MIN_CHECK_AMOUNT = 100000;            // REQ1.10
    static constexpr long long WITHDRAWAL_UNIT = 1000;               // 최소 권종
    static constexpr int MAX_PASSWORD_ATTEMPTS = 3;                  // REQ3.5
};

#endif
