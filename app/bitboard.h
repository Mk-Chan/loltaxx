#ifndef LOLTXX_BITBOARD_H
#define LOLTXX_BITBOARD_H

#include <cstdint>
#include <iostream>

#include "square.h"

namespace loltaxx {

class Bitboard {
   private:
    enum Constants : std::uint64_t
    {
        BB_FULL = std::uint64_t(0x1FFFFFFFFFFFF),
        BB_FULL_SANS_FILE_A = BB_FULL ^ std::uint64_t(0x0040810204081),
        BB_FULL_SANS_FILE_G = BB_FULL ^ std::uint64_t(0x1020408102040),
    };

   public:
    using value_type = std::uint64_t;

    constexpr Bitboard() : value_(0) {
    }
    constexpr explicit Bitboard(int shift) : value_(std::uint64_t(1) << shift) {
    }
    constexpr explicit Bitboard(unsigned shift) : value_(std::uint64_t(1) << shift) {
    }
    constexpr explicit Bitboard(value_type value) : value_(value) {
    }
    constexpr explicit Bitboard(Square square) : Bitboard(square.value()) {
    }

    constexpr operator value_type() const {
        return value_;
    }

    [[nodiscard]] constexpr Bitboard operator<<(int shift) const {
        return Bitboard{(value_ << unsigned(shift))};
    }
    [[nodiscard]] constexpr Bitboard operator<<(unsigned shift) const {
        return Bitboard{value_ << shift};
    }
    [[nodiscard]] constexpr Bitboard operator>>(int shift) const {
        return Bitboard{value_ >> unsigned(shift)};
    }
    [[nodiscard]] constexpr Bitboard operator>>(unsigned shift) const {
        return Bitboard{value_ >> shift};
    }
    [[nodiscard]] constexpr Bitboard operator|(value_type rhs) const {
        return Bitboard{value_ | rhs};
    }
    [[nodiscard]] constexpr Bitboard operator&(value_type rhs) const {
        return Bitboard{value_ & rhs};
    }
    [[nodiscard]] constexpr Bitboard operator^(value_type rhs) const {
        return Bitboard{value_ ^ rhs};
    }
    [[nodiscard]] constexpr Bitboard operator|(Bitboard rhs) const {
        return Bitboard{value_ | rhs};
    }
    [[nodiscard]] constexpr Bitboard operator&(Bitboard rhs) const {
        return Bitboard{value_ & rhs};
    }
    [[nodiscard]] constexpr Bitboard operator^(Bitboard rhs) const {
        return Bitboard{value_ ^ rhs};
    }

    [[nodiscard]] constexpr Bitboard operator~() const {
        return Bitboard{~value_};
    }
    [[nodiscard]] constexpr bool operator!() const {
        return value_ == 0;
    }
    [[nodiscard]] constexpr Bitboard operator-() const {
        return Bitboard{~value_};
    }
    constexpr Bitboard& operator<<=(int shift) {
        value_ <<= unsigned(shift);
        return *this;
    }
    constexpr Bitboard& operator<<=(unsigned shift) {
        value_ <<= shift;
        return *this;
    }
    constexpr Bitboard& operator>>=(int shift) {
        value_ >>= unsigned(shift);
        return *this;
    }
    constexpr Bitboard& operator>>=(unsigned shift) {
        value_ >>= shift;
        return *this;
    }
    constexpr Bitboard& operator|=(value_type rhs) {
        value_ |= rhs;
        return *this;
    }
    constexpr Bitboard& operator&=(value_type rhs) {
        value_ &= rhs;
        return *this;
    }
    constexpr Bitboard& operator^=(value_type rhs) {
        value_ ^= rhs;
        return *this;
    }

    [[nodiscard]] constexpr Bitboard north() const {
        return (*this << 7) & Bitboard{BB_FULL};
    }
    [[nodiscard]] constexpr Bitboard south() const {
        return *this >> 7;
    }
    [[nodiscard]] constexpr Bitboard east() const {
        return (*this << 1) & Bitboard{BB_FULL_SANS_FILE_A};
    }
    [[nodiscard]] constexpr Bitboard west() const {
        return (*this >> 1) & Bitboard{BB_FULL_SANS_FILE_G};
    }

    [[nodiscard]] constexpr Bitboard adjacent() const {
        return north() | south() | east() | west() | north().east() | north().west() |
               south().east() | south().west();
    }
    [[nodiscard]] constexpr Bitboard jumps() const {
        return north().north() | north().north().east() | north().north().west() | south().south() |
               south().south().east() | south().south().west() | east().east() |
               east().north().east() | east().south().east() | west().west() |
               west().north().west() | west().south().west() | north().east().north().east() |
               north().west().north().west() | south().east().south().east() |
               south().west().south().west();
    }

    [[nodiscard]] constexpr int popcount() const {
        return __builtin_popcountll(value_);
    }
    [[nodiscard]] constexpr Square forward_bitscan() const {
        return Square{__builtin_ctzll(value_)};
    }
    [[nodiscard]] constexpr Square reverse_bitscan() const {
        return Square{48 - __builtin_clzll(value_)};
    }
    constexpr void forward_popbit() {
        value_ &= value_ - 1;
    }
    constexpr void reverse_popbit() {
        value_ ^= Bitboard{reverse_bitscan() + 1};
    }

    class Iterator {
       public:
        using iterator = Bitboard::Iterator;

        constexpr explicit Iterator(value_type value) : value_(value) {
        }

        constexpr iterator begin() {
            return *this;
        }
        constexpr iterator end() {
            return Iterator{0};
        }
        constexpr iterator operator++() {
            value_ &= value_ - 1;
            return *this;
        }
        constexpr Square operator*() {
            return Square{__builtin_ctzll(value_)};
        }
        constexpr bool operator!=(const iterator& rhs) const {
            return value_ != rhs.value_;
        }

       private:
        value_type value_;
    };

   private:
    value_type value_;
};

}  // namespace loltaxx

namespace std {

template <>
struct hash<loltaxx::Bitboard> : public hash<loltaxx::Bitboard::value_type> {};

}  // namespace std

#endif  // LOLTXX_BITBOARD_H
