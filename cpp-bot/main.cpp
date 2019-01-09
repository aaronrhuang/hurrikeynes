#include <iostream>
#include <boost/program_options.hpp>
#include <boost/asio/ip/tcp.hpp>
#include "./parser/runner.hpp"
#include "./player.hpp"

namespace opt = boost::program_options;

using boost::asio::ip::tcp;

int main(int argc, char *argv[])
{

  // Parse out command line options.
  // Usage: pokerbot [-h HOST] PORT
  std::string host = "localhost";
  int port;
  opt::options_description desc("Allowed options");
  desc.add_options()
    ("host,h", opt::value<std::string>(&host), "HOST")
    ("port", opt::value<int>(&port)->required(), "PORT")
  ;

  opt::positional_options_description p;
  p.add("port", 1);

  opt::variables_map vm;
  try {
    opt::store(opt::command_line_parser(argc, argv).
               options(desc).positional(p).run(), vm);
    opt::notify(vm);
  } catch (const std::exception& e) {
    std::cout << "Usage: pokerbot [-h HOST] PORT\n";
    std::cerr << e.what() << std::endl;
    return 1;
  }

  // Connect to the engine.
  tcp::iostream stream;
  char port_buf[5];
  sprintf(port_buf, "%d", port);
  std::cout << "Connecting to " << host << ":" << port << "\n";
  stream.connect(host, port_buf);
  // Important! Set TCP_NODELAY on the stream.
  tcp::no_delay option(true);
  stream.rdbuf()->set_option(option);
  if (!stream) {
    std::cout << "Could not connect to " << host << ":" << port << "\n";
    return 1;
  }

  Player player;
  Runner runner(&stream, &player);

  runner.run();

  return 0;
}
