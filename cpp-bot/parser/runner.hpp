#ifndef __PARSER_RUNNER_HPP__
#define __PARSER_RUNNER_HPP__

#include <boost/asio/ip/tcp.hpp>

#include "./bot.hpp"

using boost::asio::ip::tcp;

class Runner {
private:
    tcp::iostream* stream;
    Bot* bot;
    Game current_game;
    Round current_round;
    Pot current_pot;
    std::vector<std::string> current_cards;
    std::vector<std::string> move_history;

public:
    Runner(tcp::iostream* stream, Bot* bot);

    void run();

private:
    void send(const std::string& outgoing);
    void receive(const std::string& packet);
    void handle_newgame(const std::vector<std::string>& command);
    void handle_newhand(const std::vector<std::string>& command);
    void handle_exchange(const std::vector<std::string>& command);
    void update_pot(const std::vector<std::string>& new_moves, const int new_pot_total);
    Action handle_getaction(const std::vector<std::string>& command);
    void handle_action(const Action& action);
    void handle_handover(const std::vector<std::string>& command);
};


#endif  // __PARSER_RUNNER_HPP__
