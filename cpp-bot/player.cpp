#include <iostream>
#include "player.hpp"

using namespace omp;

Player::Player() {
}

void Player::handle_new_game(const Game& new_game) {
  HandEvaluator eval;
  Hand h = Hand::empty();
  Hand p = Hand::empty();
  p += Hand(0) + Hand(1);
  h += Hand(4) + Hand(5) + Hand(8);
  Hand w = p + h;
  Hand w2 = Hand(0) + Hand(1) + Hand(2) + Hand(5) + Hand(8) ;
  std::cout << "NEW GAME TEST: " << eval.evaluate(w2) << " : " << eval.evaluate(w) << "\n";
}

void Player::handle_new_round(const Game& game, const Round& new_round) {
  std::cout << "NEW ROUND: " << game.num_hands << " : " << new_round.bankroll << "\n";
}

void Player::handle_round_over(
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
Action Player::get_action(
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
) {
  int call_cost = action_cost(pot, CallAction());

  HandEvaluator eval;
  Hand pocket = Hand::empty();
  Hand board = Hand::empty();
  Hand whole = Hand::empty();

  for (int i=0; i<cards.size(); i++) {
    std::cout << cards[i];
    pocket += Hand(rank_map[std::string(1,cards[i].at(0))]*3 + suit_map[std::string(1,cards[i].at(1))]);
    whole += Hand(rank_map[std::string(1,cards[i].at(0))]*3 + suit_map[std::string(1,cards[i].at(1))]);
  }
  for (int i=0; i<board_cards.size(); i++) {
    std::cout << board_cards[i];
    board += Hand(suit_map[std::string(1,board_cards[i].at(1))]*13 + rank_map[std::string(1,board_cards[i].at(0))]);
    whole += Hand(suit_map[std::string(1,board_cards[i].at(1))]*13 + rank_map[std::string(1,board_cards[i].at(0))]);
  }
  std::cout << "\n";

  std::cout << eval.evaluate(pocket) << " : " << eval.evaluate(board) << " : " << eval.evaluate(whole) << "\n";

  int score = eval.evaluate(whole);
  int board_score = eval.evaluate(board);

  // preflop
  //================================
  if (board_cards.size() == 0) {
    // Pocket Pair
    if (score > 8000) {
      if (legal_move_mask & BET_ACTION_TYPE) {
        return BetAction(300);
      } else if (legal_move_mask & RAISE_ACTION_TYPE) {
        return RaiseAction(300);
      } else if (legal_move_mask & CALL_ACTION_TYPE) {
        return CallAction();
      }
    }

    // Face Card
    if (score > 4475) {
      if (legal_move_mask & BET_ACTION_TYPE) {
        return BetAction(300);
      } else if (legal_move_mask & RAISE_ACTION_TYPE) {
        return RaiseAction(300);
      } else if (legal_move_mask & CALL_ACTION_TYPE) {
        return CallAction();
      }
    }

    if (legal_move_mask & CHECK_ACTION_TYPE) {
      return CheckAction();
    } else if (legal_move_mask & CALL_ACTION_TYPE && call_cost < 4) {
      return CallAction();
    } else {
      return FoldAction();
    }
  } //preflop

  // FLOP
  // ==================
  if (board_cards.size() >= 3) {
    //better than board by 2 order
    if (score - board_score > 9000) {
      if (legal_move_mask & BET_ACTION_TYPE) {
        return BetAction(100);
      } else if (legal_move_mask & RAISE_ACTION_TYPE) {
        return RaiseAction(100);
      } else if (legal_move_mask & CALL_ACTION_TYPE) {
        return CallAction();
      }
    }
    //better than board by 1 order
    if (score - board_score > 5000) {
      if (legal_move_mask & BET_ACTION_TYPE) {
        return BetAction(50);
      } else if (legal_move_mask & RAISE_ACTION_TYPE) {
        return RaiseAction(50);
      } else if (legal_move_mask & CALL_ACTION_TYPE) {
        return CallAction();
      }
    }
    if (score - board_score > 1000) {
      if (legal_move_mask & BET_ACTION_TYPE) {
        return BetAction(10);
      } else if (legal_move_mask & RAISE_ACTION_TYPE) {
        return RaiseAction(10);
      }
    }

    if ((float)(pot.total())/(float)(call_cost) < 2 && score > 8000 && legal_move_mask & CALL_ACTION_TYPE) {
      return CallAction();
    }
  }

  // if (score > 12300) {
  //   if (legal_move_mask & BET_ACTION_TYPE) {
  //     std::cout << "BET\n";
  //     return BetAction(50);
  //   } else if (legal_move_mask & RAISE_ACTION_TYPE) {
  //     std::cout << "RAISE\n";
  //     return RaiseAction(50);
  //   } else if (legal_move_mask & CALL_ACTION_TYPE) {
  //     return CallAction();
  //   }
  // }

  // Shitty
  // if (score < 4300) {
  //   if (legal_move_mask & CHECK_ACTION_TYPE) {
  //     return CheckAction();
  //   } else {
  //     return FoldAction();
  //   }
  // }

  if (legal_move_mask & CALL_ACTION_TYPE && call_cost < 10) {
    return CallAction();
  } else if (legal_move_mask & CHECK_ACTION_TYPE) {
    return CheckAction();
  } else {
    return FoldAction();
  }
}

Action Player::bet_raise(const int amount, const ActionType legal_move_mask) {
  if (legal_move_mask & BET_ACTION_TYPE) {
    return BetAction(amount);
  } else if (legal_move_mask & RAISE_ACTION_TYPE) {
    return RaiseAction(amount);
  } else if (legal_move_mask & CALL_ACTION_TYPE) {
    return CallAction();
  }
  return check_fold(legal_move_mask);
}

Action Player::check_fold(const ActionType legal_move_mask) {
  if (legal_move_mask & CHECK_ACTION_TYPE) {
    return CheckAction();
  } else {
    return FoldAction();
  }
}