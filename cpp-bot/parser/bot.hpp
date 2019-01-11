#ifndef __PARSER_BOT_HPP__
#define __PARSER_BOT_HPP__

#include <vector>
#include <string>

#include "./game.hpp"
#include "./actions.hpp"

class Bot {
public:
    // Called when a new game starts. Called exactly once.
    //
    // Arguments:
    // new_game: the Game object.
    virtual void handle_new_game(const Game& new_game) = 0;

    // Called when a new round starts. Called Game.num_rounds times.
    //
    // Arguments:
    // game: the Game object for the new round.
    // new_round: the new Round object.
    virtual void handle_new_round(const Game& game, const Round& new_round) = 0;

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
    virtual void handle_round_over(
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
    ) = 0;

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
    virtual Action get_action(
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
    ) = 0;

    // Returns the cost of doing that action;
    static int action_cost(const Pot& pot, const Action& action);
};

#endif  // __PARSER_BOT_HPP__
