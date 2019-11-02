#ifndef LOLTAXX_RANK_H
#define LOLTAXX_RANK_H

#include <cstdint>
#include <optional>

#include "app/internal/meta_value_type.h"

namespace loltaxx {

class Rank : public MetaValueType<int> {
   public:
    class Value {
       public:
        enum RankValue : value_type
        {
            RANK_1,
            RANK_2,
            RANK_3,
            RANK_4,
            RANK_5,
            RANK_6,
            RANK_7
        };
    };
    constexpr explicit Rank(value_type value) : MetaValueType<value_type>(value) {
    }

    constexpr char to_char() const {
        return '1' + value();
    }

    constexpr static std::optional<Rank> from(char c) {
        int offset = c - '1';
        if (offset < 0 || offset > 6) {
            return std::nullopt;
        }
        return Rank{Value::RANK_1 + offset};
    }
};

namespace constants {

constexpr static Rank RANK_1 = Rank{Rank::Value::RANK_1};
constexpr static Rank RANK_2 = Rank{Rank::Value::RANK_2};
constexpr static Rank RANK_3 = Rank{Rank::Value::RANK_3};
constexpr static Rank RANK_4 = Rank{Rank::Value::RANK_4};
constexpr static Rank RANK_5 = Rank{Rank::Value::RANK_5};
constexpr static Rank RANK_6 = Rank{Rank::Value::RANK_6};
constexpr static Rank RANK_7 = Rank{Rank::Value::RANK_7};

}  // namespace constants

}  // namespace loltaxx

namespace std {

template <>
struct hash<loltaxx::Rank> : public hash<loltaxx::Rank::value_type> {};

}  // namespace std

#endif  // LOLTAXX_RANK_H
