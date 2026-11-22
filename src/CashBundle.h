#ifndef CASH_BUNDLE_H
#define CASH_BUNDLE_H

#include <array>

// 권종별 지폐 장수 묶음 (REQ1.10). ATM 보유 현금, 투입 현금, 배출 현금 모두 이 타입
// index 순서: [0]=50,000 [1]=10,000 [2]=5,000 [3]=1,000
class CashBundle {
public:
    static constexpr int KINDS = 4;
    static constexpr std::array<int, KINDS> DENOMINATIONS = { 50000, 10000, 5000, 1000 };
    static constexpr int MAX_BILLS_PER_TRANSACTION = 50;     // 화면 요구사항, 결정 8·10

    CashBundle() = default;
    explicit CashBundle(const std::array<int, KINDS>& counts);

    int count(int index) const;          // 해당 권종 장수
    int totalBills() const;              // 전체 장수
    long long totalAmount() const;       // 전체 금액

    void add(const CashBundle& other);
    void remove(const CashBundle& other);       // 부족하면 InsufficientCashException
    bool contains(const CashBundle& other) const;

    // 최소 장수로 amount를 만드는 조합 계산 (REQ5.1). 이 객체는 바꾸지 않음
    // 만들 수 있고 50장 이하면 out에 결과를 넣고 true (결정 8)
    bool tryDispense(long long amount, CashBundle& out) const;

private:
    std::array<int, KINDS> counts_{};
};

#endif
