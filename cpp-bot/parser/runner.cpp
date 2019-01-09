#include <iostream>
#include <string>

#include "./runner.hpp"

Runner::Runner(tcp::iostream* stream, Bot* bot) {
    this->stream = stream;
    this->bot = bot;
}

static std::vector<std::string> split(const std::string& s, char delimiter)
{
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(s);
   while (std::getline(tokenStream, token, delimiter))
   {
      tokens.push_back(token);
   }
   return tokens;
}

void Runner::send(const std::string& outgoing) {
  (*stream) << outgoing << "\n";
}

void Runner::handle_newgame(const std::vector<std::string>& command) {
  current_game.name = command[1];
  current_game.opponent_name = command[2];
  current_game.round_stack = std::stoi(command[3]);
  current_game.big_blind = std::stoi(command[4]);
  current_game.num_hands = std::stoi(command[5]);
  current_game.time_bank = std::stof(command[6]);
  bot->handle_new_game(current_game);
}

void Runner::handle_newhand(const std::vector<std::string>& command) {
  current_round.hand_num = std::stoi(command[1]);
  current_round.big_blind = command[2] == "false";
  current_cards = split(command[3], ',');
  current_round.bankroll = std::stoi(command[4]);
  current_round.opponent_bankroll = std::stoi(command[5]);
  move_history = std::vector<std::string>();

  current_pot._pip = (current_round.big_blind) ? current_game.big_blind : (current_game.big_blind/2);
  current_pot._bets = 0;
  current_pot._num_exchanges = 0;
  current_pot._opponent_bets = (current_round.big_blind) ? (current_game.big_blind/2) : current_game.big_blind;
  current_pot._opponent_num_exchanges = 0;
  bot->handle_new_round(current_game, current_round);
}

void Runner::handle_exchange(const std::vector<std::string>& command) {
  current_cards = split(command[2], ',');
}


static ActionType get_legal_moves(const std::string& legal_move_string, int* min_amount, int* max_amount) {
  const auto legal_move_strings = split(legal_move_string, ';');
  *min_amount = *max_amount = 0;
  int result = 0;
  for (const auto& move_string : legal_move_strings) {
    if (move_string.rfind("CHECK", 0) == 0) {
      result |= CHECK_ACTION_TYPE;
    } else if (move_string.rfind("CALL", 0) == 0) {
      result |= CALL_ACTION_TYPE;
    } else if (move_string.rfind("FOLD", 0) == 0) {
      result |= FOLD_ACTION_TYPE;
    } else if (move_string.rfind("EXCHANGE", 0) == 0) {
      result |= EXCHANGE_ACTION_TYPE;
    } else if (move_string.rfind("BET", 0) == 0) {
      result |= BET_ACTION_TYPE;
    } else if (move_string.rfind("RAISE", 0) == 0) {
      result |= RAISE_ACTION_TYPE;
    }

    if (move_string.rfind("BET", 0) == 0 || move_string.rfind("RAISE", 0) == 0) {
      const auto move_info = split(move_string, ':');
      *min_amount = std::stoi(move_info[1]);
      *max_amount = std::stoi(move_info[2]);
    }
  }

  return (ActionType) result;
}

static bool check_move_validity(const Action& action, const ActionType legal_move_mask, const int min_amount, const int max_amount) {
  if (!(action.type & legal_move_mask)) {
    return false;
  }

  if (action.type & (BET_ACTION_TYPE | RAISE_ACTION_TYPE)) {
    return action.amount >= min_amount && action.amount <= max_amount;
  }

  return true;
}

void Runner::update_pot(const std::vector<std::string>& new_moves, const int new_pot_total) {
  for (const auto& move : new_moves) {
    if (move.rfind("EXCHANGE", 0) == 0) {
      const auto move_info = split(move, ':');
      if (move_info.back() == current_game.opponent_name) {
        current_pot._opponent_num_exchanges += 1;
      }
    } else if (
      move.rfind("DEAL", 0) == 0 ||
      move.rfind("TIE", 0) == 0 ||
      move.rfind("WIN", 0) == 0
    ) {
      current_pot._bets += current_pot.pip();
      current_pot._pip = 0;
    }
  }

  current_pot._opponent_bets = new_pot_total - current_pot.total() - current_pot.opponent_exchanges();
}

Action Runner::handle_getaction(const std::vector<std::string>& command) {
  int new_pot_total = std::stoi(command[1]);
  auto new_moves = split(command[5], ';');
  update_pot(new_moves, new_pot_total);
  move_history.insert(move_history.begin(), new_moves.begin(), new_moves.end());
  auto board = split(command[3], ',');
  if (command[3] == "None") {
    board.clear();
  }
  int min_amount;
  int max_amount;
  ActionType legal_move_mask = get_legal_moves(command[7], &min_amount, &max_amount);
  float time_left = std::stof(command[8]);

  Action action = bot->get_action(
    current_game,
    current_round,
    current_pot,
    current_cards,
    board,
    legal_move_mask,
    move_history,
    time_left,
    min_amount,
    max_amount
  );

  if (!check_move_validity(action, legal_move_mask, min_amount, max_amount)) {
    std::cout << "Error: bot returned invalid move\n";
    if (legal_move_mask & FOLD_ACTION_TYPE) {
      return FoldAction();
    } else {
      return CheckAction();
    }
  }

  return action;
}

void Runner::handle_action(const Action& action) {
  switch (action.type) {
    case FOLD_ACTION_TYPE:
      send("FOLD");
      break;
    case CHECK_ACTION_TYPE:
      send("CHECK");
      break;
    case CALL_ACTION_TYPE:
      current_pot._pip += Bot::action_cost(current_pot, action);
      send("CALL");
      break;
    case EXCHANGE_ACTION_TYPE:
      current_pot._num_exchanges++;
      send("EXCHANGE");
      break;
    case BET_ACTION_TYPE:
      current_pot._pip += Bot::action_cost(current_pot, action);
      send("BET:" + std::to_string(action.amount));
      break;
    case RAISE_ACTION_TYPE:
      current_pot._pip += Bot::action_cost(current_pot, action);
      send("RAISE:" + std::to_string(action.amount));
      break;
    default:
      break;
  }
}

std::string get_result(const std::vector<std::string>& new_moves, const std::string& opponent_name, std::vector<std::string>* opponent_hand) {
  for (const auto& move : new_moves) {
    if (move.rfind("SHOW", 0) == 0) {
      const auto move_info = split(move, ':');
      if (move_info.back() == opponent_name) {
        *opponent_hand = std::vector<std::string>(move_info.begin() + 1, move_info.begin() + 3);
      }
    } else if (move.rfind("TIE", 0) == 0) {
      return "tie";
    } else if (move.rfind("WIN", 0) == 0) {
      const auto move_info = split(move, ':');
      return (move_info.back() == opponent_name) ? "loss" : "win";
    }
  }

  return "uh oh - something wrong - report to organizers";
}

void Runner::handle_handover(const std::vector<std::string>& command) {
  int bankroll = std::stoi(command[1]);
  int opponent_bankroll = std::stoi(command[2]);
  auto board = split(command[4], ',');
  if (command[4] == "None") {
    board.clear();
  }
  auto new_moves = split(command[6], ';');
  int new_pot_total = std::stoi(command[7]);
  update_pot(new_moves, new_pot_total);
  std::vector<std::string> opponent_cards;

  auto result = get_result(new_moves, current_game.opponent_name, &opponent_cards);
  move_history.insert(move_history.begin(), new_moves.begin(), new_moves.end());

  bot->handle_round_over(
    current_game,
    current_round,
    current_pot,
    current_cards,
    opponent_cards,
    board,
    result,
    bankroll,
    opponent_bankroll,
    move_history
  );
}

void Runner::receive(const std::string& packet) {
  auto command = split(packet, ' ');
  auto action = command[0];

  if (action == "NEWGAME") {
    handle_newgame(command);
  } else if (action == "NEWHAND") {
    handle_newhand(command);
  } else if (action == "EXCHANGE") {
    handle_exchange(command);
  } else if (action == "GETACTION") {
    Action act = handle_getaction(command);
    handle_action(act);
  } else if (action == "HANDOVER") {
    handle_handover(command);
  } else if (action == "REQUESTKEYVALUES") {
    send("FINISH");
  }
}

void Runner::run() {
  std::string line;
  while (std::getline(*stream, line)) {
    receive(line);
  }

  std::cout << "Gameover, engine disconnected.\n";
}
