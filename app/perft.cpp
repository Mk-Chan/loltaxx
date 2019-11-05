#include <atomic>
#include <iostream>
#include <thread>

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

std::uint64_t perft_parallel(Position pos, int depth, int num_threads) {
    if (depth == 1) {
        return count_moves(pos);
    }

    MoveList move_list = pos.legal_moves();
    std::atomic_int movenum{0};

    std::atomic_uint64_t count{0};

    std::thread threads[16];
    for (int t = 0; t < num_threads; ++t) {
        threads[t] = std::thread{[&]() {
            while (true) {
                int move_idx = movenum.fetch_add(1);
                if (move_idx >= move_list.size()) {
                    break;
                }

                Move move = move_list[move_idx];
                Position child_pos{pos};
                child_pos.make_move(move);
                std::uint64_t thread_perft_count = perft(child_pos, depth - 1);
                count += thread_perft_count;
            }
        }};
    }

    for (int t = 0; t < num_threads; ++t) {
        threads[t].join();
    }

    return count;
}

int main(int argc, char** argv) {
    std::string fen{loltaxx::constants::STARTPOS_FEN};
    int depth = 1;
    int num_threads = 1;

    CLI::App app{"An Ataxx perft tool."};
    app.add_option("-f,--fen", fen, "FEN to calculate perft for.", true);
    app.add_option("-d,--depth", depth, "Depth to calculate perft for.")->required(true);
    app.add_option("-t,--threads", num_threads, "Number of threads to use.");
    CLI11_PARSE(app, argc, argv);

    Position position{fen};
    depth = std::max(1, std::min(depth, 100));
    num_threads = std::max(1, std::min(num_threads, 16));

    std::uint64_t count = perft_parallel(position, depth, num_threads);
    std::cout << count << "\n";
    return 0;
}
