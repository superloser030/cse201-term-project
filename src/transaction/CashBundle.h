#ifndef CASH_BUNDLE_H
#define CASH_BUNDLE_H

#include <array>
#include <string>

// 최소 장수 출금 계산 결과
enum class DispenseResult { OK, NOT_POSSIBLE, TOO_MANY_BILLS };

// 권종별 지폐 장수 묶음 (REQ1.10). ATM 보유 현금, 투입 현금, 배출 현금 모두 이 타입
// index 순서: [0]=50,000 [1]=10,000 [2]=5,000 [3]=1,000
class CashBundle {
public:
    static constexpr int KINDS = 4;
    static constexpr std::array<int, KINDS> DENOMINATIONS = { 50000, 10000, 5000, 1000 };

    CashBundle() = default;
    explicit CashBundle(const std::array<int, KINDS>& counts);

    int count(int index) const;          // 해당 권종 장수
    int totalBills() const;              // 전체 장수
    long long totalAmount() const;       // 전체 금액
    bool empty() const { return totalBills() == 0; }

    void add(const CashBundle& other);
    void remove(const CashBundle& other);       // 부족하면 InsufficientCashException
    bool contains(const CashBundle& other) const;

    // 최소 장수로 amount를 만드는 조합 계산 (REQ5.1). 이 객체는 바꾸지 않음
    // 큰 권종이 작은 권종의 배수이므로 큰 권종부터 최대한 쓰는 greedy로 최소 장수가 됨
    DispenseResult planDispense(long long amount, int maxBills, CashBundle& out) const;

    // "50,000x2 10,000x3 ..." (skipZero면 0장인 권종 생략)
    std::string toString(bool skipZero = false) const;

private:
    std::array<int, KINDS> counts_{};
};

#endif
