#include <iostream>
#include "./bot.hpp"


void Bot::handle_new_game(const Game &new_game) {
    std::cout << "NEW GAME: " << new_game.name << " : " << new_game.num_hands << "\n";
}

void Bot::handle_new_round(const Game& game, const Round& new_round) {
    std::cout << "NEW ROUND: " << game.num_hands << " : " << new_round.bankroll << "\n";
}

void Bot::handle_round_over(
    const Game& game,
    const Round& round,
    const Pot& pot,
    const std::vector<std::string>& cards,
    const std::vector<std::string>& opponent_cards,
    const std::vector<std::string>& board_cards,
    const std::string& result,
    const int new_bankroll,
    const int new_opponent_bankroll,
    const std::vector<std::string>& move_history
) {
    std::cout << "OVER: " << result << "\n";
}

Action Bot::get_action(
    const Game &game,
    const Round &round,
    const Pot &pot,
    const std::vector<std::string> &cards,
    const std::vector<std::string> &board_cards,
    const ActionType legal_move_mask,
    const std::vector<std::string> &move_history,
    const float time_left,
    const int min_amount,
    const int max_amount
) {
    return FoldAction();
}

int Bot::action_cost(const Pot& pot, const Action& action) {
    switch (action.type) {
        case FOLD_ACTION_TYPE:
        case CHECK_ACTION_TYPE:
            return 0;
        case CALL_ACTION_TYPE:
            return pot.opponent_bets() - pot.bets() - pot.pip();
        case EXCHANGE_ACTION_TYPE:
            return (1 << (pot.num_exchanges() + 1));
        case BET_ACTION_TYPE:
        case RAISE_ACTION_TYPE:
            return action.amount - pot.pip();
        default:
            return 0;
    }
}
