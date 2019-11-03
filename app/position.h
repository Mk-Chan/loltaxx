#ifndef LOLTAXX_POSITION_H
#define LOLTAXX_POSITION_H

#include <sstream>

#include "bitboard.h"
#include "color.h"
#include "move.h"

namespace loltaxx {

class Position {
   private:
    Position() : side_to_move_(constants::CROSS), halfmoves_(0) {
    }

   public:
    explicit Position(const std::string& fen_str) : Position() {
        piece_bb_[0] = Bitboard{0};
        piece_bb_[1] = Bitboard{0};
        gaps_ = Bitboard{0};

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
                gaps_ ^= Bitboard{curr_sq};
                ++curr_sq;
            } else {
                auto piece = Piece::from(c);
                if (piece) {
                    put_piece(*piece, curr_sq);
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

    [[nodiscard]] MoveList generate_legal_moves() const {
        MoveList move_list;

        Bitboard us = piece_bb_[side_to_move_];
        Bitboard them = piece_bb_[!side_to_move_];

        Bitboard put_piece_bb = us.adjacent() & ~gaps_;
        Bitboard move_piece_bb = us.jumps() & ~gaps_;
        Bitboard opponent_capture_bb = them.adjacent();

        Bitboard capture_put_bb = put_piece_bb & opponent_capture_bb;
        for (Square sq = capture_put_bb.forward_bitscan(); capture_put_bb;
             capture_put_bb.forward_popbit()) {
            move_list.add(Move{sq, Move::IsCapture::TRUE});
        }

        Bitboard non_capture_put_bb = put_piece_bb ^ capture_put_bb;
        for (Square sq = non_capture_put_bb.forward_bitscan(); non_capture_put_bb;
             non_capture_put_bb.forward_popbit()) {
            move_list.add(Move{sq, Move::IsCapture::FALSE});
        }

        for (Square from = us.forward_bitscan(); us; us.forward_popbit()) {
            Bitboard capture_moves = move_piece_bb & opponent_capture_bb;
            for (Square to = capture_moves.forward_bitscan(); capture_moves;
                 capture_moves.forward_popbit()) {
                move_list.add(Move{from, to, Move::IsCapture::TRUE});
            }

            Bitboard non_capture_moves = move_piece_bb ^ capture_moves;
            for (Square to = non_capture_moves.forward_bitscan(); non_capture_moves;
                 non_capture_moves.forward_popbit()) {
                move_list.add(Move{from, to, Move::IsCapture::FALSE});
            }
        }

        return move_list;
    }

   protected:
    constexpr void put_piece(Piece side, Square square) {
        piece_bb_[side] ^= Bitboard{square};
    }
    constexpr void remove_piece(Piece side, Square square) {
        piece_bb_[side] &= ~Bitboard{square};
    }
    constexpr void move_piece(Piece side, Square from, Square to) {
        piece_bb_[side] ^= (Bitboard{from}) | (Bitboard{to});
    }

   private:
    Bitboard piece_bb_[2];
    Bitboard gaps_;
    Piece side_to_move_;
    int halfmoves_;
};

}  // namespace loltaxx

#endif  // LOLTAXX_POSITION_H
