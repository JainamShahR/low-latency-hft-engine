#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace hft {

enum class Side { Buy, Sell };
enum class OrderType { Limit, Market };

enum class OrderStatus { New, PartiallyFilled, Filled, Cancelled };

struct Order {
    std::uint64_t id{};
    Side side{};
    OrderType type{};
    std::int64_t price{};       // price in integer ticks (e.g. paise/cents)
    std::uint64_t quantity{};
    std::uint64_t remaining{};
    std::uint64_t sequence{};   // arrival sequence for price-time priority
    OrderStatus status{OrderStatus::New};
};

struct Trade {
    std::uint64_t buy_order_id{};
    std::uint64_t sell_order_id{};
    std::int64_t price{};
    std::uint64_t quantity{};
};

class OrderBook {
public:
    std::vector<Trade> add_limit_order(std::uint64_t id, Side side,
                                       std::int64_t price, std::uint64_t quantity);

    std::vector<Trade> add_market_order(std::uint64_t id, Side side,
                                        std::uint64_t quantity);

    bool cancel_order(std::uint64_t id);
    const Order* find_order(std::uint64_t id) const;

    std::size_t active_order_count() const { return orders_.size(); }
    std::uint64_t next_sequence() const { return next_sequence_; }

private:
    using OrderPtr = Order*;
    using PriceLevel = std::vector<OrderPtr>;
    using BuyBook = std::map<std::int64_t, PriceLevel, std::greater<>>;
    using SellBook = std::map<std::int64_t, PriceLevel, std::less<>>;

    std::vector<Trade> match(OrderPtr incoming, bool is_market);
    void remove_from_level(OrderPtr order);
    void erase_if_empty(Side side, std::int64_t price);
    static void update_status(OrderPtr order);

    BuyBook buys_;
    SellBook sells_;
    std::unordered_map<std::uint64_t, Order> orders_;
    std::uint64_t next_sequence_{1};
};

std::string to_string(Side side);
std::string to_string(OrderType type);
std::string to_string(OrderStatus status);

} // namespace hft
