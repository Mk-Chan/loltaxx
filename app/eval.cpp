#include "eval.h"

namespace loltaxx {

int eval(Position* pos) {
    int eval = 0;

    for (Piece piece : constants::PIECES) {
        Bitboard bb = pos->pieces(piece);

        eval += MATERIAL * bb.popcount();

        eval = -eval;
    }

    if (pos->side_to_move() == constants::KNOT) {
        eval = -eval;
    }

    return eval;
}

}  // namespace loltaxx
