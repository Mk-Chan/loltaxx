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
        TO_SQUARE_SHIFT = 6,
        MOVE_TYPE_SHIFT = 12,
        IS_CAPTURE_SHIFT = 13,

        MOVE_TYPE_MASK = 3 << MOVE_TYPE_SHIFT,
        IS_CAPTURE_MASK = 3 << IS_CAPTURE_SHIFT
    };

   public:
    enum class Type : std::uint8_t
    {
        NONE,
        MOVE,
        PUT
    };
    enum class IsCapture : std::uint8_t
    {
        NONE,
        FALSE,
        TRUE
    };

    using value_type = int;

    constexpr explicit Move(std::uint32_t value) : value_(value) {
    }
    constexpr Move(Square from_square, Square to_square, IsCapture is_capture = IsCapture::NONE)
        : value_(from_square.value() | (to_square.value() << TO_SQUARE_SHIFT) |
                 (std::uint32_t(is_capture) << IS_CAPTURE_SHIFT)) {
    }
    constexpr Move(Square square, IsCapture is_capture = IsCapture::NONE)
        : value_(square.value() | (std::uint32_t(49) << TO_SQUARE_SHIFT) |
                 (std::uint32_t(1) << MOVE_TYPE_SHIFT) |
                 (std::uint32_t(is_capture) << IS_CAPTURE_SHIFT)) {
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
        auto from_sq = from_square();
        if (from_sq == constants::SQUARE_NULL) {
            return "0000";
        }

        auto to_sq = to_square();
        if (to_sq == constants::SQUARE_NULL) {
            return from_sq.to_str();
        }

        std::string move_str = from_square().to_str() + to_square().to_str();
        return move_str;
    }

    [[nodiscard]] constexpr bool operator==(const Move rhs) const {
        return value_sans_type() == rhs.value_sans_type();
    }
    [[nodiscard]] constexpr bool operator!=(const Move rhs) const {
        return value_sans_type() != rhs.value_sans_type();
    }

    [[nodiscard]] constexpr Square from_square() const {
        return Square{value() & 0x3f};
    }
    [[nodiscard]] constexpr Square to_square() const {
        return Square{(value() & 0xfc0) >> 6};
    }
    [[nodiscard]] constexpr Move::Type type() const {
        return static_cast<Move::Type>((value() & MOVE_TYPE_MASK) >> MOVE_TYPE_SHIFT);
    }
    [[nodiscard]] constexpr Move::IsCapture is_capture() const {
        return static_cast<Move::IsCapture>((value() & IS_CAPTURE_MASK) >> IS_CAPTURE_SHIFT);
    }

    [[nodiscard]] constexpr value_type value_sans_type() const {
        return value_ & ~MOVE_TYPE_MASK;
    }
    [[nodiscard]] constexpr value_type value_sans_capture() const {
        return value_ & ~IS_CAPTURE_MASK;
    }
    [[nodiscard]] constexpr value_type value_sans_type_and_capture() const {
        return value_ & ~IS_CAPTURE_MASK & ~MOVE_TYPE_MASK;
    }
    [[nodiscard]] constexpr value_type value() const {
        return value_;
    }

   private:
    value_type value_;
};

class MoveList {
   public:
    using value_type = std::vector<Move>;
    using iterator = value_type::iterator;
    using const_iterator = value_type::const_iterator;

    MoveList() {
        values_.reserve(32);
    }

    iterator begin() {
        return values_.begin();
    }
    iterator end() {
        return values_.end();
    }
    const_iterator cbegin() const {
        return values_.cbegin();
    }
    const_iterator cend() const {
        return values_.cend();
    }

    void pop_back() {
        values_.pop_back();
    }
    void add(Move move) {
        values_.push_back(move);
    }
    void add(const MoveList& move_list) noexcept {
        values_.reserve(size() + move_list.size());
        for (auto iter = move_list.cbegin(); iter != move_list.cend(); ++iter) {
            add(*iter);
        }
    }
    template <class F>
    void sort(F move_evaluator) {
        auto& moves = values_mut_ref();
        std::vector<int> scores;
        scores.reserve(values_.size());
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
    void clear() noexcept {
        values_.clear();
    }
    bool empty() const noexcept {
        return values_.empty();
    }
    int size() const {
        return values_.size();
    }
    const value_type& values() const {
        return values_;
    }
    bool contains(Move move) const {
        return std::find(cbegin(), cend(), move) != cend();
    }

   protected:
    value_type& values_mut_ref() {
        return values_;
    }

   private:
    value_type values_;
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
