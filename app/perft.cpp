#include <iostream>

#include "CLI11/CLI11.hpp"

#include "position.h"

using loltaxx::Bitboard;
using loltaxx::Move;
using loltaxx::MoveList;
using loltaxx::Position;
using loltaxx::Square;

int count_moves(Position pos) {
    Bitboard us = pos.piece_bb_[pos.side_to_move_];
    Bitboard them = pos.piece_bb_[!pos.side_to_move_];

    if (!us || !them) {
        return 0;
    }

    Bitboard occupancy = us | them;
    Bitboard empty = ~(occupancy | pos.gaps_);

    Bitboard put_piece_bb = us.adjacent() & empty;
    int num_moves = put_piece_bb.popcount();

    for (Square from : Bitboard::Iterator{us}) {
        Bitboard move_piece_bb = Bitboard{from}.jumps() & empty;
        num_moves += move_piece_bb.popcount();
    }

    if (!num_moves) {
        num_moves = 1;
    }

    return num_moves;
}

std::uint64_t perft(Position pos, int depth) {
    if (depth == 1) {
        return count_moves(pos);
    }

    MoveList move_list = pos.legal_moves();

    std::uint64_t count = 0;
    for (Move move : move_list) {
        Position child_pos{pos};
        child_pos.make_move(move);
        count += perft(child_pos, depth - 1);
    }

    return count;
}

int main(int argc, char** argv) {
    std::string fen{loltaxx::constants::STARTPOS_FEN};
    int depth = 1;

    CLI::App app{"An Ataxx perft tool."};
    app.add_option("-f,--fen", fen, "FEN to calculate perft for.", true);
    app.add_option("-d,--depth", depth, "Depth to calculate perft for.")->required(true);

    CLI11_PARSE(app, argc, argv);

    Position position{fen};

    std::uint64_t count = perft(position, depth);
    std::cout << count << "\n";
    return 0;
}
