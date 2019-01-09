#ifndef __PARSER_GAME_HPP__
#define __PARSER_GAME_HPP__

#include <string>


// The Game class represents a single match between two bots.
//
// name: your bot's name on the engine
// opponent_name: the opponent's bot's name on the engine
// round_stack: the number of chips you have available at the start of every hand
// big_blind: the size of the big blind
// num_rounds: the number of hands that will be played during this match
// time_bank: the total amount of seconds your bot has to play this game.
struct Game {
    std::string name;
    std::string opponent_name;
    int round_stack;
    int big_blind;
    int num_hands;
    float time_bank;
};


// The Round class represents a single round of the game above
//
// hand_num: the round #. Ranges from 1 to Game.num_rounds
// bankroll: the total amount you've gained or lost from the beginning of the game to the start of this round
// opponent_bankroll: the total amount your opponent has gained or lost. Usually the negative of the above
// big_blind: True if you had the big blind, False otherwise.
struct Round {
    int hand_num;
    int bankroll;
    int opponent_bankroll;
    bool big_blind;
};


// The Pot class represents the amount of money in the pot
//
// pip: the amount you have added to the pot via betting during this betting round
// bets: the amount you have added to the pot via betting during previous rounds
// exchanges: the amount you have added to the pot via exchanges
// num_exchanges: the number of times you have exchanged
// total: the total amount you have added to the pot
// opponent_bets: the total amount your opponent has added to the pot via betting
// opponent_exchanges: the total amount your opponent has added to the pot via exchanges
// opponent_num_exchanges: the number of times your opponent has exchanged
// opponent_total: the total amount your opponent has added to the pot
// grand_total: the total size of the pot
struct Pot {
    int _pip;
    int _bets;
    int _num_exchanges;
    int _opponent_bets;
    int _opponent_num_exchanges;

    int pip(void) const;
    int bets(void) const;
    int exchanges(void) const;
    int num_exchanges(void) const;
    int total(void) const;
    int opponent_bets(void) const;
    int opponent_exchanges(void) const;
    int opponent_num_exchanges(void) const;
    int opponent_total(void) const;
    int grand_total(void) const;
};

#endif  // __PARSER_GAME_HPP__
