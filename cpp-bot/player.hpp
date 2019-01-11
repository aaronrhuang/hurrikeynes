#ifndef __PLAYER_HPP__
#define __PLAYER_HPP__
#include <boost/assign/list_of.hpp>
#include <map>
#include <string>
#include "./parser/bot.hpp"
#include "./parser/game.hpp"
#include "./parser/actions.hpp"
#include "./omp/HandEvaluator.h"
#include <set> 
using namespace omp;

class Player : public Bot {
private:
    // Your private instance variables go here.
    std::map<std::string, int> suit_map = boost::assign::map_list_of("s",0)("h",1)("c",2)("d",3);
    std::map<std::string, int> rank_map = boost::assign::map_list_of("2",0)("3",1)("4",2)("5",3)("6",4)("7",5)("8",6)("9",7)("10",8)("J",9)("Q",10)("K",11)("A",12);

public:
    Player();

    // Called when a new game starts. Called exactly once.
    //
    // Arguments:
    // new_game: the Game object.
    void handle_new_game(const Game& new_game);

    // Called when a new round starts. Called Game.num_rounds times.
    //
    // Arguments:
    // game: the Game object for the new round.
    // new_round: the new Round object.
    void handle_new_round(const Game& game, const Round& new_round);

    // Called when a round ends. Called Game.num_rounds times.
    //
    // Arguments:
    // game: the Game object.
    // round: the Round object.
    // pot: the Pot object.
    // cards: the cards you held when the round ended.
    // opponent_cards: the cards your opponent held when the round ended, or a length 0 vector if they never showed.
    // board_cards: the cards on the board when the round ended.
    // result: 'win', 'loss' or 'tie'
    // new_bankroll: your total bankroll at the end of this round.
    // new_opponent_bankroll: your opponent's total bankroll at the end of this round.
    // move_history: a list of moves that occurred during this round, earliest moves first.
    void handle_round_over(
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
    );

    // Where the magic happens - your code should implement this function.
    // Called any time the server needs an action from your bot.

    // Arguments:
    // game: the Game object.
    // round: the Round object.
    // pot: the Pot object.
    // cards: an array of your cards, in common format.
    // board_cards: an array of cards on the board. This list has length 0, 3, 4, or 5.
    // legal_moves: a set of the move classes that are legal to make.
    // cost_func: a function that takes a move, and returns additional cost of that move. Your returned move will raise your pot.contribution by this amount.
    // move_history: a list of moves that have occurred during this round so far, earliest moves first.
    // time_left: a float of the number of seconds your bot has remaining in this match (not round).
    // min_amount: if BetAction or RaiseAction is valid, the smallest amount you can bet or raise to (i.e. the smallest you can increase your pip).
    // max_amount: if BetAction or RaiseAction is valid, the largest amount you can bet or raise to (i.e. the largest you can increase your pip).
    Action get_action(
        const Game& game,
        const Round& round,
        const Pot& pot,
        const std::vector<std::string>& cards,
        const std::vector<std::string>& board_cards,
        const ActionType legal_move_mask,
        const std::vector<std::string>& move_history,
        const float time_left,
        const int min_amount,
        const int max_amount
    );
    float win_chance(const Hand pocket, const Hand board, const Hand whole, std::set<int> card_idxs);
    Action bet_raise(const int amount, const int call_cost, const ActionType legal_move_mask);
    Action check_fold(const ActionType legal_move_mask);
};

#endif  // __PLAYER_HPP__
