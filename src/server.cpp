#include <iostream>
#include <sstream>
#include <stdexcept>

#include <thallium.hpp>
#include <thallium/serialization/stl/vector.hpp>

#include "args.hpp"
#include "log.hpp"
#include "rpcs/exit_server.hpp"
#include "rpcs/send_message.hpp"

namespace tl = thallium;
static const std::string DEFAULT_URL = "tcp://localhost:1234";

int main(int argc, char* argv[])
{
  args::ArgumentParser parser("Thallium server program.");
  args::HelpFlag help(parser, "help", "Display usage", { 'h', "help" });
  args::ValueFlag<std::string> url(
    parser, "url", "Bind server to port and ip, ex: tcp://localhost:1234", { "url" }, DEFAULT_URL);
  args::ValueFlag<std::string> verbosity(
    parser, "verbosity", "Thallium logger verbosity", { 'v', "verbosity" }, "trace");
  try
  {
    parser.ParseCLI(argc, argv);
  }
  catch (args::Help)
  {
    std::cout << parser;
    return 0;
  }
  catch (args::ParseError e)
  {
    std::cerr << e.what() << std::endl;
    std::cerr << parser;
    return 1;
  }
  catch (args::ValidationError e)
  {
    std::cerr << e.what() << std::endl;
    std::cerr << parser;
    return 1;
  }

  tl::logger::level log_level = tl_logger::get_verbosity_from_string(verbosity->c_str());

  margo_set_environment(nullptr);
  tl::abt a;
  tl::logger::set_global_logger(&logger_instance);
  tl::logger::set_global_log_level(log_level);

  // relevant pools and their execution streams
  auto rpc_pool = tl::pool::create(tl::pool::access::mpsc, tl::pool::kind::fifo_wait);
  auto rpc_xstream = tl::xstream::create(tl::scheduler::predef::basic_wait, *rpc_pool);
  auto progress_pool = tl::pool::create(tl::pool::access::mpsc, tl::pool::kind::fifo_wait);
  auto progress_xstream = tl::xstream::create(tl::scheduler::predef::basic_wait, *progress_pool);

  // engine setup.
  tl::engine myEngine(url->c_str(), THALLIUM_SERVER_MODE, *progress_pool, *rpc_pool);
  myEngine.set_logger(&logger_instance);
  myEngine.set_log_level(log_level);

  // RPCs
  myEngine.define("send_message", &send_message);
  myEngine.define("exit_server", &exit_server);

  // register the function called when client asks server to exit.
  exit_callback = [&myEngine]()
  {
    LOG_INFO("Finalize engine");
    myEngine.finalize();
  };

  LOG_INFO("Server running at address " << myEngine.self());

  return 0;
}
