#include <iostream>
#include "player.hpp"

Player::Player() {
}

void Player::handle_new_game(const Game& new_game) {
  Hand w = Hand::empty();
  Hand p = Hand::empty();
  p += Hand(47) + Hand(43);
  w += Hand(51) + Hand(1) + Hand(4) + Hand(5) + Hand(8);
  cout << p.count() << "\n";
  cout << "NEW GAME TEST: " << evaluator.evaluate(p) << " : " << evaluator.evaluate(w) << "\n";
}

void Player::handle_new_round(const Game& game, const Round& new_round) {
  cout << "NEW ROUND: " << game.num_hands << " : " << game.time_bank << "\n";
}

void Player::handle_round_over(
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
) {

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
// all_in: if BetAction or RaiseAction is valid, the largest amount you can bet or raise to (i.e. the largest you can increase your pip).
Action Player::get_action(
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
) {
  if (2*(1000-round.hand_num) < round.bankroll) {
    return check_fold(legal_move_mask);
  }
  int call_cost = action_cost(pot, CallAction());
  int all_in = game.round_stack-pot.total();

  Hand pocket = Hand::empty();
  Hand board = Hand::empty();
  Hand whole = Hand::empty();
  set<int> card_idxs;

  for (int i=0; i<cards.size(); i++) {
    int card = rank_map[string(1,cards[i].at(0))]*4 +
                suit_map[string(1,cards[i].at(1))];
    card_idxs.insert(card);
    pocket += Hand(card);
    whole += Hand(card);
  }
  for (int i=0; i<board_cards.size(); i++) {
    int card = rank_map[string(1,board_cards[i].at(0))]*4 +
                suit_map[string(1,board_cards[i].at(1))];
    card_idxs.insert(card);
    board += Hand(card);
    whole += Hand(card);
  }

  int curr_score = evaluator.evaluate(whole);
  int board_score = evaluator.evaluate(board);
  int delta = curr_score - board_score;
  // cout << curr_score << " : " << board_score << "\n";

  // PREFLOP
  //================================
  if (board_cards.size() == 0) {
    int fc = rank_map[string(1,cards[0].at(0))]*4 +
                suit_map[string(1,cards[0].at(1))];
    int sc = rank_map[string(1,cards[1].at(0))]*4 +
                suit_map[string(1,cards[1].at(1))];
    float strength = hand_strength(fc, sc);

    if (strength > 0.6) {
      return bet_raise((int)(all_in/2*strength), call_cost, legal_move_mask);
    }
    if (legal_move_mask & CALL_ACTION_TYPE && call_cost < 5) {
      return CallAction();
    }
    return check_fold(legal_move_mask);
  } //preflop

  float curr_chance = opp_odds(whole, board, card_idxs);
  cout << "STR " << board_cards.size() << ": " << curr_chance << "\n";

  // FLOP
  // ==================
  if (board_cards.size() >= 3) {
    if (curr_chance  > 0.6) {
      cout << (int)(all_in/2*curr_chance) << '\n';
      return bet_raise((int)(all_in/2*curr_chance), call_cost, legal_move_mask);
    }
    if (curr_chance  > 0.5) {
      return bet_raise((int)(all_in/4*curr_chance), call_cost, legal_move_mask);
    }
  }

  // TURN
  // ================
  if (board_cards.size() == 4) {
    if (curr_chance  > 0.6) {
      return bet_raise((int)(all_in/2*curr_chance), call_cost, legal_move_mask);
    }
    if (curr_chance  > 0.5) {
      return bet_raise((int)(all_in/4*curr_chance), call_cost, legal_move_mask);
    }
  }

  // RIVER
  // ================
  if (board_cards.size() == 4) {
    if (curr_chance  > 0.5) {
      return bet_raise((int)(all_in/2*curr_chance), call_cost, legal_move_mask);
    }
  }

  if ((float)(pot.total())/(float)(call_cost) > 10 && legal_move_mask & CALL_ACTION_TYPE) {
    return CallAction();
  }
  if (call_cost < 10 && legal_move_mask & CALL_ACTION_TYPE) {
    return CallAction();
  }

  return check_fold(legal_move_mask);
}

// float Player::hand_strength(const vector<string> &cards) {
//   bool suited = suit_map[string(1,cards[0].at(1))] ==
//                   suit_map[string(1,cards[1].at(1))];
//   int rank1 = rank_map[string(1,cards[0].at(0))];
//   int rank2 = rank_map[string(1,cards[1].at(0))];
//   float strength = pflop[rank1][rank2];
//   if (suited) {
//     strength += 0.05;
//   } else {
//     strength -= 0.05;
//   }
//   return strength;
// }
// 0 <= fc, sc < 52
float Player::hand_strength(int fc, int sc) {
  bool suited = fc % 4 == sc % 4;
  int rank1 = fc / 4;
  int rank2 = sc / 4;
  float strength = pflop[rank1][rank2];
  if (suited) {
    strength += 0.05;
  } else {
    strength -= 0.05;
  }
  return strength;
}

float Player::win_chance(const Hand pocket, const Hand board, const Hand whole, const set<int> card_idxs) {
    if (board.count() == 0) {
        int fc = -1;
        int sc = -1;
        for (int card_idx : card_idxs) {
            if (fc == -1) {
                fc = card_idx;
            } else {
                sc = card_idx;
                break;
            }
        }
        return hand_strength(fc, sc);
    }
    int numRemCards = 5 - board.count();
    if (numRemCards < 0) {
        printf("Too many cards on board");
        return -1;
    }

    if (numRemCards == 0) {
        return opp_odds(whole, board, card_idxs);
    }
    if (numRemCards == 1) {
        float sum = 0;
        float ct = 0;
        for (int nc = 0; nc < 52; nc++) {
            if (card_idxs.count(nc) > 0) {
                continue;
            }
            Hand full = whole + Hand(nc);
            set<int> newci;
            newci.insert(card_idxs.begin(), card_idxs.end());
            newci.insert(nc);
            sum += opp_odds(full, board, newci);
            ct += 1;
        }
        return sum / ct;
    }
    if (numRemCards == 2) {
        float sum = 0;
        float ct = 0;
        for (int nfc = 0; nfc < 52; nfc++) {
            if (card_idxs.count(nfc) > 0) continue;
            for (int nsc = 0; nsc < 52; nsc++) {
                if (card_idxs.count(nsc) > 0) continue;
                Hand full = whole + Hand(nfc) + Hand(nsc);
                set<int> newci;
                newci.insert(card_idxs.begin(), card_idxs.end());
                newci.insert(nfc);
                newci.insert(nsc);
                sum += opp_odds(full, board,  newci);
                ct += 1;
            }
        }
        return sum / ct;
    }
    if (numRemCards > 2) {
        printf("Too many cards to brute force over");
        return -1;
    }
    return -1;
}

float Player::opp_odds(const Hand whole, const Hand board, const set<int> card_idxs) {
    HandEvaluator evaluator;
    int handScore = evaluator.evaluate(whole);
    float tot = 0;
    float win = 0;
    for (int ofc = 0; ofc < 52; ofc++) {
       if (card_idxs.count(ofc)>0) continue;
       for (int osc = 0; osc < 52; osc++) {
            if (card_idxs.count(osc) > 0) continue;
            Hand oppHand = board + Hand(ofc) + Hand(osc);
            int oppHandScore = evaluator.evaluate(oppHand);
            tot += 1;
            win += handScore > oppHandScore ? 1 : 0;
       }
    }
    return win / tot;
}

Action Player::bet_raise(const int amount, const int call_cost, const ActionType legal_move_mask) {
  if (call_cost > 390 && amount > 390 && legal_move_mask & CALL_ACTION_TYPE) {
    return CallAction();
  }
  if (amount == 0 && legal_move_mask & CHECK_ACTION_TYPE) {
    return CheckAction();
  }
  if (legal_move_mask & BET_ACTION_TYPE) {
    return BetAction(amount);
  } else if (amount > call_cost && legal_move_mask & RAISE_ACTION_TYPE) {
    return RaiseAction(amount);
  } else if (legal_move_mask & CALL_ACTION_TYPE && call_cost < amount) {
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
