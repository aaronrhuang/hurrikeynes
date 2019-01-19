#ifndef __PLAYER_HPP__
#define __PLAYER_HPP__
#include <boost/assign/list_of.hpp>
#include <map>
#include <set>
#include <string>
#include "./parser/bot.hpp"
#include "./parser/game.hpp"
#include "./parser/actions.hpp"
#include "./omp/HandEvaluator.h"
using namespace omp;
using namespace std;

class Player : public Bot {
private:
    HandEvaluator evaluator;
    // Your private instance variables go here.
    map<string, int> suit_map = boost::assign::map_list_of("s",0)("h",1)("c",2)("d",3);
    map<string, int> rank_map = boost::assign::map_list_of("2",0)("3",1)("4",2)("5",3)("6",4)("7",5)("8",6)                                                      ("9",7)("10",8)("J",9)("Q",10)("K",11)("A",12);

    //                      2     3     4     5     6     7     8     9     10    J     Q     K     A
    float pflop[13][13] = {{0.51, 0.35, 0.36, 0.37, 0.37, 0.37, 0.40, 0.42, 0.44, 0.47, 0.49, 0.53, 0.57}, // 2
                           {0.39, 0.55, 0.38, 0.39, 0.39, 0.39, 0.40, 0.43, 0.45, 0.48, 0.50, 0.54, 0.58}, // 3
                           {0.40, 0.42, 0.58, 0.41, 0.41, 0.41, 0.42, 0.43, 0.46, 0.48, 0.51, 0.54, 0.59}, // 4
                           {0.41, 0.43, 0.44, 0.61, 0.43, 0.43, 0.44, 0.45, 0.47, 0.49, 0.52, 0.55, 0.60}, // 5
                           {0.40, 0.42, 0.44, 0.46, 0.64, 0.45, 0.46, 0.47, 0.48, 0.50, 0.53, 0.56, 0.59}, // 6
                           {0.41, 0.43, 0.45, 0.46, 0.48, 0.67, 0.47, 0.48, 0.50, 0.52, 0.54, 0.57, 0.60}, // 7
                           {0.43, 0.43, 0.45, 0.47, 0.49, 0.50, 0.69, 0.50, 0.52, 0.53, 0.55, 0.58, 0.61}, // 8
                           {0.45, 0.46, 0.46, 0.48, 0.50, 0.51, 0.53, 0.72, 0.53, 0.55, 0.57, 0.59, 0.62}, // 9
                           {0.47, 0.48, 0.49, 0.49, 0.51, 0.53, 0.54, 0.56, 0.75, 0.57, 0.59, 0.61, 0.64}, // 10
                           {0.50, 0.50, 0.51, 0.52, 0.53, 0.54, 0.56, 0.57, 0.59, 0.78, 0.59, 0.62, 0.65}, // J
                           {0.52, 0.53, 0.54, 0.55, 0.55, 0.56, 0.58, 0.59, 0.61, 0.61, 0.80, 0.62, 0.65}, // Q
                           {0.55, 0.56, 0.57, 0.58, 0.58, 0.59, 0.60, 0.61, 0.63, 0.64, 0.64, 0.83, 0.66}, // K
                           {0.59, 0.60, 0.61, 0.62, 0.62, 0.63, 0.63, 0.64, 0.66, 0.66, 0.67, 0.68, 0.85}, // A
                         };



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
        const vector<string>& cards,
        const vector<string>& opponent_cards,
        const vector<string>& board_cards,
        const string& result,
        const int new_bankroll,
        const int new_opponent_bankroll,
        const vector<string>& move_history
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
        const vector<string>& cards,
        const vector<string>& board_cards,
        const ActionType legal_move_mask,
        const vector<string>& move_history,
        const float time_left,
        const int min_amount,
        const int max_amount
    );
    float hand_strength(int fc, int sc);
    float win_chance(const Hand pocket, const Hand board, const Hand whole, const set<int> card_idxs);
    float opp_odds(const Hand whole, const Hand board, const set<int> card_idxs);
    Action bet_raise(const int amount, const int call_cost, const ActionType legal_move_mask);
    Action check_fold(const ActionType legal_move_mask);
};

#endif  // __PLAYER_HPP__
