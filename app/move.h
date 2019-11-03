#ifndef LOLTAXX_MOVE_H
#define LOLTAXX_MOVE_H

#include <algorithm>
#include <optional>
#include <vector>

#include "square.h"

namespace loltaxx {

class Move {
   private:
    enum BitOpLookup : std::uint32_t
    {
        TO_SQUARE_SHIFT = 6
    };

   public:
    using value_type = int;

    constexpr Move() : value_(49u | (49u << TO_SQUARE_SHIFT)) {
    }
    constexpr explicit Move(std::uint32_t value) : value_(value) {
    }
    constexpr Move(Square from_square, Square to_square)
        : value_(from_square.value() | (to_square.value() << TO_SQUARE_SHIFT)) {
    }
    constexpr explicit Move(Square square)
        : value_(square.value() | (square.value() << TO_SQUARE_SHIFT)) {
    }

    static std::optional<Move> from(const std::string& str) {
        auto strlen = str.length();
        if (strlen == 2) {
            auto piece_sq = Square::from(str.substr(0, 2));
            if (!piece_sq) {
                return std::nullopt;
            }

            return Move{*piece_sq};
        } else if (strlen == 4) {
            if (str == "0000") {
                return Move{Square{49}};
            }

            auto from = Square::from(str.substr(0, 2));
            auto to = Square::from(str.substr(2, 4));
            if (!(from && to)) {
                return std::nullopt;
            }

            return Move{*from, *to};
        } else {
            return std::nullopt;
        }
    }

    [[nodiscard]] std::string to_str() const {
        Square from_sq = from_square();
        if (from_sq == constants::SQUARE_NULL) {
            return "0000";
        }

        Square to_sq = to_square();
        if (from_sq == to_sq) {
            return from_sq.to_str();
        }

        std::string move_str = from_sq.to_str() + to_sq.to_str();
        return move_str;
    }

    [[nodiscard]] constexpr bool operator==(const Move rhs) const {
        return value() == rhs.value();
    }
    [[nodiscard]] constexpr bool operator!=(const Move rhs) const {
        return value() != rhs.value();
    }

    [[nodiscard]] constexpr Square from_square() const {
        return Square{value() & 0x3f};
    }
    [[nodiscard]] constexpr Square to_square() const {
        return Square{(value() & 0xfc0) >> 6};
    }

    [[nodiscard]] constexpr value_type value() const {
        return value_;
    }

   private:
    value_type value_;
};

namespace constants {

constexpr Move MOVE_NULL = Move{SQUARE_NULL};

}  // namespace constants

class MoveList {
   public:
    using value_type = Move[256];
    using iterator = Move*;

    MoveList() : size_(0) {
    }

    iterator begin() {
        return values_;
    }
    iterator end() {
        return values_ + size();
    }

    void pop_back() {
        --size_;
    }
    void add(Move move) {
        values_[size_] = move;
        ++size_;
    }
    template <class F>
    void sort(F move_evaluator) {
        auto& moves = values_mut_ref();
        int scores[256];
        for (int i = 0; i < size(); ++i) {
            scores[i] = move_evaluator(moves[i]);
        }
        for (int i = 1; i < size(); ++i) {
            Move moving_move = moves[i];
            int moving_score = scores[i];
            int j = i;
            for (; j > 0; --j) {
                if (scores[j - 1] < moving_score) {
                    scores[j] = scores[j - 1];
                    moves[j] = moves[j - 1];
                } else {
                    break;
                }
            }
            scores[j] = moving_score;
            moves[j] = moving_move;
        }
    }
    bool empty() const noexcept {
        return size_ == 0;
    }
    int size() const {
        return size_;
    }
    const value_type& values() const {
        return values_;
    }
    bool contains(Move move) const {
        for (int i = 0; i < size_; ++i) {
            if (move == values_[i]) {
                return true;
            }
        }
        return false;
    }

   protected:
    value_type& values_mut_ref() {
        return values_;
    }

   private:
    value_type values_;
    int size_;
};

inline std::ostream& operator<<(std::ostream& ostream, Move move) {
    return ostream << move.to_str();
}

}  // namespace loltaxx

namespace std {

template <>
struct hash<loltaxx::Move> : public hash<loltaxx::Move::value_type> {};

}  // namespace std

#endif  // LOLTAXX_MOVE_H
