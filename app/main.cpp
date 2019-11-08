#include <iostream>

#include "search.h"

using namespace loltaxx;

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cout.setf(std::ios::unitbuf);

    Position position{constants::STARTPOS_FEN};
    search::SearchGlobals search_globals = search::SearchGlobals::new_search_globals();
    auto position_handler = [&position](const UAIPositionParameters& position_parameters) {
        position = Position{position_parameters.fen()};
        if (!position_parameters.move_list()) {
            return;
        }
        for (auto& move_str : position_parameters.move_list()->move_list()) {
            position.make_move(*Move::from(move_str));
        }
    };
    auto go_handler = [&position, &search_globals](const UAIGoParameters& go_parameters) {
        search_globals.set_go_parameters(go_parameters);
        auto best_move = search::best_move_search(position, &search_globals);
        if (best_move) {
            UAIService::bestmove(best_move->to_str());
        } else {
            UAIService::bestmove("0000");
        }
    };
    auto stop_handler = [&search_globals]() { search_globals.set_stop_flag(true); };
    // auto display_handler = [&position](const std::istringstream&) { position.display(); };

    UAIService uai_service{"loltaxx", "Manik Charan"};
    uai_service.register_position_handler(position_handler);
    uai_service.register_go_handler(go_handler);
    uai_service.register_stop_handler(stop_handler);
    // uai_service.register_handler("d", display_handler);
    // uai_service.register_handler("tune", tune_handler);

    std::string line;
    while (true) {
        std::getline(std::cin, line);
        if (line == "uai") {
            uai_service.run();
            break;
        } else {
            std::cout << "Supported Protocols: uai\n";
        }
    }

    return 0;
}
