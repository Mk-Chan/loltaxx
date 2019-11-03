#ifndef LOLTAXX_POSITION_H
#define LOLTAXX_POSITION_H

#include <cassert>
#include <sstream>

#include "bitboard.h"
#include "color.h"
#include "move.h"

namespace loltaxx {

namespace constants {

static std::string STARTPOS_FEN = "x5o/7/7/7/7/7/o5x x 0";

}  // namespace constants

class Position {
   private:
    Position() : side_to_move_(constants::CROSS), halfmoves_(0) {
    }

   public:
    explicit Position(const std::string& fen_str) : Position() {
        piece_bb_[0] = Bitboard{std::uint64_t(0)};
        piece_bb_[1] = Bitboard{std::uint64_t(0)};
        gaps_ = Bitboard{std::uint64_t(0)};

        std::istringstream fen_stream{fen_str};
        std::string fen_part;

        // Piece list
        fen_stream >> fen_part;
        Square curr_sq = constants::A7;
        for (char c : fen_part) {
            if (c >= '1' && c <= '7') {
                curr_sq += (c - '0');
            } else if (c == '/') {
                curr_sq -= 14;
            } else if (c == '-') {
                gaps_ |= Bitboard{curr_sq};
                ++curr_sq;
            } else {
                auto piece = Piece::from(c);
                if (piece) {
                    piece_bb_[*piece] |= Bitboard{curr_sq};
                }
                ++curr_sq;
            }
        }

        // Side to move
        fen_stream >> fen_part;
        auto piece = Piece::from(fen_part[0]);
        if (piece) {
            side_to_move_ = *piece;
        }

        // Halfmoves
        if (fen_stream >> fen_part) {
            const char* fen_part_cstr = fen_part.c_str();
            char* end;
            halfmoves_ = std::strtol(fen_part_cstr, &end, 10);
        }
    }

    [[nodiscard]] MoveList legal_moves() const {
        MoveList move_list;

        Bitboard us = piece_bb_[side_to_move_];
        Bitboard occupancy = us | piece_bb_[!side_to_move_];
        Bitboard empty = ~(occupancy | gaps_);

        Bitboard put_piece_bb = us.adjacent() & empty;
        for (Square sq : Bitboard::Iterator{put_piece_bb}) {
            move_list.add(Move{sq});
        }

        for (Square from : Bitboard::Iterator{us}) {
            Bitboard move_piece_bb = Bitboard{from}.jumps() & empty;
            for (Square to : Bitboard::Iterator{move_piece_bb}) {
                move_list.add(Move{from, to});
            }
        }

        if (move_list.empty() && us) {
            move_list.add(constants::MOVE_NULL);
        }

        return move_list;
    }

    void make_move(Move move) {
        if (move == constants::MOVE_NULL) {
            side_to_move_ = !side_to_move_;
            return;
        }

        Square from = move.from_square();
        Square to = move.to_square();

        Bitboard captured = Bitboard{to}.adjacent() & piece_bb_[!side_to_move_];
        piece_bb_[!side_to_move_] ^= captured;
        piece_bb_[side_to_move_] ^= (Bitboard{from}) | (Bitboard{to}) | captured;

        if (captured || from == to) {
            halfmoves_ = 0;
        }

        side_to_move_ = !side_to_move_;
    }

   public:
    Bitboard piece_bb_[2];
    Bitboard gaps_;
    Piece side_to_move_;
    int halfmoves_;
};

}  // namespace loltaxx

#endif  // LOLTAXX_POSITION_H
