#include "search.h"
#include "eval.h"
#include "tt.h"
#include "uai_service.h"

namespace loltaxx::search {

TranspositionTable tt{256};

void sort_moves(Position pos, MoveList* move_list, std::optional<Move> tt_move = {}) {
    Bitboard them = pos.pieces(pos.side_to_move());
    move_list->sort([&](Move move) {
        if (tt_move && *tt_move == move) {
            return 10000;
        } else {
            Bitboard captured = Bitboard{move.to_square()}.adjacent() & them;
            if (captured) {
                return 5000 + captured.popcount();
            } else {
                return 0;
            }
        }
    });
}

SearchResult search_impl(Position pos, int alpha, int beta, int depth, int ply, SearchGlobals* sg) {
    sg->increment_nodes();

    if (depth <= 0) {
        return SearchResult{eval(&pos), {}};
    }

    if (ply) {
        if (sg->stop()) {
            return {0, {}};
        }

        if (pos.halfmoves() >= 100) {
            return {0, {}};
        }

        if (ply >= MAX_PLY) {
            return {eval(&pos), {}};
        }

        alpha = std::max((-MATE_SCORE + ply), alpha);
        beta = std::min((MATE_SCORE - ply), beta);
        if (alpha >= beta) {
            return {alpha, {}};
        }
    }

    bool pv_node = alpha != beta - 1;

    auto hash = pos.hash();
    TTEntry tt_entry = tt.probe(hash);
    Move tt_move;
    if (tt_entry.get_key() == hash) {
        tt_move = Move{tt_entry.get_move()};
        int tt_score = tt_entry.get_score();
        int tt_flag = tt_entry.get_flag();
        if (tt_entry.get_depth() == depth) {
            if ((tt_flag == TTConstants::FLAG_LOWER && tt_score >= beta) ||
                (tt_flag == TTConstants::FLAG_UPPER && tt_score <= alpha)) {
                return {tt_score, {}};
            }
        }
    }

    MoveList move_list = pos.legal_moves();
    if (move_list[0] == constants::MOVE_NULL) {
        return SearchResult{eval(&pos), move_list};
    }

    if (move_list.empty()) {
        return SearchResult{-MATE_SCORE + ply, {}};
    }

    sort_moves(pos, &move_list, tt_move);

    MoveList pv;
    int best_score = -INFINITE;
    for (Move move : move_list) {
        Position child = pos;
        child.make_move(move);

        auto result = -search_impl(child, -beta, -alpha, depth - 1, ply + 1, sg);

        if (ply && sg->stop()) {
            return SearchResult{0, {}};
        }

        if (result.score > best_score) {
            best_score = result.score;
            if (best_score > alpha) {
                alpha = best_score;

                if (pv_node) {
                    pv.clear();
                    pv.add(move);
                    if (result.pv) {
                        pv.add(*result.pv);
                    }
                }

                if (alpha >= beta) {
                    break;
                }
            }
        }
    }

    int tt_flag = best_score >= beta ? TTConstants::FLAG_LOWER
                                     : best_score <= alpha ? TTConstants::FLAG_UPPER : FLAG_EXACT;
    tt.write(pv[0].value(), tt_flag, depth, best_score, hash);

    return {best_score, pv};
}

SearchResult search(Position pos, int depth) {
    tt.clear();
    SearchGlobals search_globals = SearchGlobals::new_search_globals();
    int alpha = -INFINITE;
    int beta = +INFINITE;
    SearchResult search_result = search_impl(pos, alpha, beta, depth, 0, &search_globals);
    return search_result;
}

SearchResult search(Position pos, SearchGlobals* search_globals, int depth) {
    int alpha = -INFINITE;
    int beta = +INFINITE;
    SearchResult search_result = search_impl(pos, alpha, beta, depth, 1, search_globals);
    return search_result;
}

std::optional<loltaxx::Move> best_move_search(loltaxx::Position pos,
                                              SearchGlobals* search_globals) {
    std::optional<Move> best_move;
    auto start_time = curr_time();
    search_globals->set_stop_flag(false);
    search_globals->set_side_to_move(pos.side_to_move());
    search_globals->reset_nodes();
    search_globals->set_start_time(start_time);
    for (int depth = 1; depth <= MAX_PLY; ++depth) {
        auto search_result = search(pos, search_globals, depth);

        if (depth > 1 && search_globals->stop()) {
            return best_move;
        }

        auto time_diff = curr_time() - start_time;

        int score = search_result.score;
        auto& pv = search_result.pv;
        if (!pv) {
            break;
        }

        best_move = *pv->begin();

        std::uint64_t time_taken = time_diff.count();
        std::uint64_t nodes = search_globals->nodes();
        std::uint64_t nps = time_taken ? nodes * 1000 / time_taken : nodes;
        UAIInfoParameters info_parameters{{
            {"depth", depth},
            {"score", score},
            {"time", int(time_taken)},
            {"nps", nps},
            {"nodes", nodes},
        }};

        std::vector<std::string> str_move_list;
        str_move_list.reserve(pv->size());
        for (auto move : *pv) {
            str_move_list.push_back(move.to_str());
        }
        info_parameters.set_pv(UAIMoveList{str_move_list});
        UAIService::info(info_parameters);
    }

    return best_move;
}

}  // namespace loltaxx::search
