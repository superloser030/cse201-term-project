#include "transaction/FeePolicy.h"

// REQ1.8 수수료 표
long long FeePolicy::depositFee(bool accountIsPrimary) {
    return accountIsPrimary ? 0 : 2000;
}

long long FeePolicy::withdrawalFee(bool accountIsPrimary) {
    return accountIsPrimary ? 2000 : 3000;
}

long long FeePolicy::accountTransferFee(bool sourceIsPrimary, bool destIsPrimary) {
    if (sourceIsPrimary && destIsPrimary) return 2000;     // 주거래 ↔ 주거래
    if (sourceIsPrimary || destIsPrimary) return 3000;     // 주거래 ↔ 타행
    return 5000;                                           // 타행 ↔ 타행
}

long long FeePolicy::cashTransferFee() {
    return 1000;
}
