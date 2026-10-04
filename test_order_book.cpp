#include "order_book.hpp"
#include <cassert>
#include <iostream>

int main() {
    hft::OrderBook book;

    book.add_limit_order(1, hft::Side::Buy, 10000, 100);
    auto trades = book.add_limit_order(2, hft::Side::Sell, 10000, 40);
    assert(trades.size() == 1);
    assert(trades[0].quantity == 40);
    assert(trades[0].price == 10000);
    assert(trades[0].buy_order_id == 1);
    assert(trades[0].sell_order_id == 2);

    const auto* remaining = book.find_order(1);
    assert(remaining != nullptr);
    assert(remaining->remaining == 60);

    book.add_limit_order(3, hft::Side::Buy, 9900, 20);
    assert(book.cancel_order(3));
    assert(book.find_order(3) == nullptr);

    auto market_trades = book.add_market_order(4, hft::Side::Sell, 60);
    assert(market_trades.size() == 1);
    assert(market_trades[0].quantity == 60);
    assert(book.find_order(1) == nullptr);

    std::cout << "All tests passed.\n";
}
