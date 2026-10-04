#include "order_book.hpp"

#include <algorithm>
#include <stdexcept>

namespace hft {

std::string to_string(Side side) {
    return side == Side::Buy ? "BUY" : "SELL";
}

std::string to_string(OrderType type) {
    return type == OrderType::Limit ? "LIMIT" : "MARKET";
}

std::string to_string(OrderStatus status) {
    switch (status) {
        case OrderStatus::New: return "NEW";
        case OrderStatus::PartiallyFilled: return "PARTIALLY_FILLED";
        case OrderStatus::Filled: return "FILLED";
        case OrderStatus::Cancelled: return "CANCELLED";
    }
    return "UNKNOWN";
}

void OrderBook::update_status(OrderPtr order) {
    if (order->remaining == 0) {
        order->status = OrderStatus::Filled;
    } else if (order->remaining < order->quantity) {
        order->status = OrderStatus::PartiallyFilled;
    } else {
        order->status = OrderStatus::New;
    }
}

std::vector<Trade> OrderBook::add_limit_order(std::uint64_t id, Side side,
                                              std::int64_t price,
                                              std::uint64_t quantity) {
    if (quantity == 0 || price <= 0) throw std::invalid_argument("Invalid order");
    if (orders_.count(id)) throw std::invalid_argument("Duplicate order id");

    auto [it, inserted] = orders_.emplace(id, Order{id, side, OrderType::Limit, price,
                                                     quantity, quantity, next_sequence_++});
    OrderPtr incoming = &it->second;
    auto trades = match(incoming, false);

    if (incoming->remaining > 0) {
        if (side == Side::Buy) buys_[price].push_back(incoming);
        else sells_[price].push_back(incoming);
        update_status(incoming);
    }
    return trades;
}

std::vector<Trade> OrderBook::add_market_order(std::uint64_t id, Side side,
                                               std::uint64_t quantity) {
    if (quantity == 0) throw std::invalid_argument("Invalid order");
    if (orders_.count(id)) throw std::invalid_argument("Duplicate order id");

    auto [it, inserted] = orders_.emplace(id, Order{id, side, OrderType::Market, 0,
                                                     quantity, quantity, next_sequence_++});
    OrderPtr incoming = &it->second;
    auto trades = match(incoming, true);
    update_status(incoming);
    if (incoming->remaining > 0) incoming->status = OrderStatus::Cancelled;
    return trades;
}

std::vector<Trade> OrderBook::match(OrderPtr incoming, bool is_market) {
    std::vector<Trade> trades;

    while (incoming->remaining > 0) {
        if (incoming->side == Side::Buy) {
            if (sells_.empty()) break;
            auto level_it = sells_.begin();
            const auto price = level_it->first;
            if (!is_market && price > incoming->price) break;

            auto &level = level_it->second;
            auto *resting = level.front();
            const auto fill = std::min(incoming->remaining, resting->remaining);
            trades.push_back({incoming->id, resting->id, price, fill});
            incoming->remaining -= fill;
            resting->remaining -= fill;
            update_status(resting);
            if (resting->remaining == 0) {
                resting->status = OrderStatus::Filled;
                orders_.erase(resting->id);
                level.erase(level.begin());
            }
            if (level.empty()) sells_.erase(level_it);
        } else {
            if (buys_.empty()) break;
            auto level_it = buys_.begin();
            const auto price = level_it->first;
            if (!is_market && price < incoming->price) break;

            auto &level = level_it->second;
            auto *resting = level.front();
            const auto fill = std::min(incoming->remaining, resting->remaining);
            trades.push_back({resting->id, incoming->id, price, fill});
            incoming->remaining -= fill;
            resting->remaining -= fill;
            update_status(resting);
            if (resting->remaining == 0) {
                resting->status = OrderStatus::Filled;
                orders_.erase(resting->id);
                level.erase(level.begin());
            }
            if (level.empty()) buys_.erase(level_it);
        }
    }
    return trades;
}

bool OrderBook::cancel_order(std::uint64_t id) {
    auto it = orders_.find(id);
    if (it == orders_.end()) return false;
    OrderPtr order = &it->second;
    remove_from_level(order);
    order->status = OrderStatus::Cancelled;
    orders_.erase(it);
    return true;
}

void OrderBook::remove_from_level(OrderPtr order) {
    if (order->type != OrderType::Limit || order->remaining == 0) return;
    if (order->side == Side::Buy) {
        auto level_it = buys_.find(order->price);
        if (level_it == buys_.end()) return;
        auto &level = level_it->second;
        level.erase(std::remove(level.begin(), level.end(), order), level.end());
        if (level.empty()) buys_.erase(level_it);
    } else {
        auto level_it = sells_.find(order->price);
        if (level_it == sells_.end()) return;
        auto &level = level_it->second;
        level.erase(std::remove(level.begin(), level.end(), order), level.end());
        if (level.empty()) sells_.erase(level_it);
    }
}

const Order* OrderBook::find_order(std::uint64_t id) const {
    auto it = orders_.find(id);
    return it == orders_.end() ? nullptr : &it->second;
}

} // namespace hft
