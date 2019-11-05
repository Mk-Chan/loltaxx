#ifndef LOLTXX_PIECE_H
#define LOLTXX_PIECE_H

#include <iostream>

#include "internal/meta_value_type.h"

namespace loltaxx {

class Piece : public MetaValueType<int> {
   public:
    class Value {
       public:
        enum ColorValue : value_type
        {
            CROSS = 0,
            KNOT = 1
        };
    };
    constexpr explicit Piece(value_type value) : MetaValueType<value_type>(value) {
    }

    constexpr Piece operator!() {
        return Piece{!value()};
    }

    constexpr char to_char() const {
        switch (value()) {
            case Value::CROSS:
                return 'w';
            case Value::KNOT:
                return 'b';
            default:
                return '-';
        }
    }

    constexpr static std::optional<Piece> from(char c) {
        switch (c) {
            case 'x':
                return Piece{Value::CROSS};
            case 'o':
                return Piece{Value::KNOT};
            case 'X':
                return Piece{Value::CROSS};
            case 'O':
                return Piece{Value::KNOT};
            default:
                return {};
        }
    }
};

inline std::ostream& operator<<(std::ostream& ostream, Piece color) {
    return ostream << color.to_char();
}

namespace constants {

constexpr static Piece CROSS{Piece::Value::CROSS};
constexpr static Piece KNOT{Piece::Value::KNOT};
constexpr static Piece PIECES[]{CROSS, KNOT};

}  // namespace constants

}  // namespace loltaxx

namespace std {

template <>
struct hash<loltaxx::Piece> : public hash<loltaxx::Piece::value_type> {};

}  // namespace std

#endif  // LOLTAXX_PIECE_H
