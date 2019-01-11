#include "./bot.hpp"

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
