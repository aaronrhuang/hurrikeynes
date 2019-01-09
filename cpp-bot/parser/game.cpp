#include "./game.hpp"

int Pot::pip(void) const {
    return _pip;
}

int Pot::bets(void) const {
    return _bets;
}

int Pot::exchanges(void) const {
    return (1 << (_num_exchanges + 1)) - 2;
}

int Pot::num_exchanges(void) const {
    return _num_exchanges;
}

int Pot::total(void) const {
    return pip() + bets() + exchanges();
}

int Pot::opponent_bets(void) const {
    return _opponent_bets;
}

int Pot::opponent_exchanges(void) const {
    return (1 << (_opponent_num_exchanges + 1)) - 2;
}

int Pot::opponent_num_exchanges(void) const {
    return _opponent_num_exchanges;
}

int Pot::opponent_total(void) const {
    return opponent_bets() + opponent_exchanges();
}

int Pot::grand_total(void) const {
    return total() + opponent_total();
}
