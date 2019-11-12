#include "eval.h"

namespace loltaxx {

int eval(Position* pos) {
    int eval = 0;

    Bitboard crosses = pos->pieces(constants::CROSS);
    Bitboard knots = pos->pieces(constants::KNOT);

    eval += 1000 * (crosses.popcount() - knots.popcount());

    if (pos->side_to_move() == constants::KNOT) {
        eval = -eval;
    }

    return eval;
}

}  // namespace loltaxx
