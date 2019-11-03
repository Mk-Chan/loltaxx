#include <iostream>
#include "position.h"

using loltaxx::Move;
using loltaxx::MoveList;
using loltaxx::Position;

std::uint64_t perft(Position position, int depth) {
    MoveList move_list = position.legal_moves();
    if (depth == 1) {
        return move_list.size();
    }

    std::uint64_t count = 0;
    for (Move move : move_list) {
        Position child_pos{position};
        child_pos.make_move(move);
        count += perft(child_pos, depth - 1);
    }

    return count;
}

int main() {
    Position position{loltaxx::constants::STARTPOS_FEN};

    for (int depth = 6; depth <= 6; ++depth) {
        std::uint64_t count = perft(position, depth);
        std::cout << "depth " << depth << ": " << count << "\n";
    }

    return 0;
}
