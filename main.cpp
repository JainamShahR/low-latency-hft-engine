#include "order_book.hpp"

#include <chrono>
#include <iostream>

int main() {
    hft::OrderBook book;

    book.add_limit_order(1, hft::Side::Buy, 10000, 100);
    book.add_limit_order(2, hft::Side::Buy, 9900, 50);
    book.add_limit_order(3, hft::Side::Sell, 10100, 80);

    const auto start = std::chrono::steady_clock::now();
    const auto trades = book.add_limit_order(4, hft::Side::Sell, 10000, 70);
    const auto end = std::chrono::steady_clock::now();

    std::cout << "Trades:\n";
    for (const auto& trade : trades) {
        std::cout << "BUY #" << trade.buy_order_id
                  << " x SELL #" << trade.sell_order_id
                  << " | price=" << trade.price
                  << " | qty=" << trade.quantity << '\n';
    }

    const auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    std::cout << "Matching time: " << ns << " ns\n";
    std::cout << "Active orders: " << book.active_order_count() << '\n';

    return 0;
}
