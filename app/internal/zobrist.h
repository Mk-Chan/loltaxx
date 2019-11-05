#ifndef LOLTAXX_INTERNAL_ZOBRIST_H
#define LOLTAXX_INTERNAL_ZOBRIST_H

#include <array>
#include <cstdint>
#include <random>

namespace loltaxx {

inline std::uint64_t rand_u64() {
    static std::mt19937 rng{std::uint64_t(9823710830529454)};
    static std::uniform_int_distribution<std::uint64_t> dist;
    return dist(rng);
}

namespace init {

inline std::array<std::array<std::uint64_t, 49>, 2> piece_square_keys() {
    std::array<std::array<std::uint64_t, 49>, 2> keys{};
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 49; ++j) {
            keys[i][j] = rand_u64();
        }
    }
    return keys;
}

}  // namespace init

namespace constants {

inline std::array<std::array<std::uint64_t, 49>, 2> PIECE_SQUARE_KEYS = init::piece_square_keys();
inline std::uint64_t SIDE_TO_MOVE_KEY = rand_u64();

}

}  // namespace loltaxx

#endif  // LOLTAXX_INTERNAL_ZOBRIST_H
