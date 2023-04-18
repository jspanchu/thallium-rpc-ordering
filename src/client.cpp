#include <limits>

#include <deque>
#include <mutex>
#include <thallium.hpp>
#include <thallium/pool.hpp>
#include <thallium/scheduler.hpp>
#include <thallium/serialization/stl/vector.hpp>
#include <thread>

#include "args.hpp"
#include "log.hpp"

namespace tl = thallium;

using Message = std::vector<uint8_t>;

std::deque<Message> OutboundQueue;
tl::engine myEngine;
std::mutex Mutex;
std::vector<tl::managed<tl::thread>> threads;

void PostSend(tl::remote_procedure rpc, tl::endpoint server, Message&& data)
{
  bool idle = false;
  {
    std::lock_guard<std::mutex> lock(Mutex);
    idle = OutboundQueue.empty();
    OutboundQueue.emplace_back(data);
  }
  LOG_DEBUG("is_idle=" << idle);
  if (idle)
  {
    threads.emplace_back(myEngine.get_handler_pool().make_thread(
      [rpc, server]()
      {
        bool idle = false;
        while (!idle)
        {
          auto& data = OutboundQueue.front();
          LOG_DEBUG("Make rpc call " << int(data[0]));
          rpc.on(server)(data);
          LOG_DEBUG("Server received message");
          {
            std::lock_guard<std::mutex> lock(Mutex);
            OutboundQueue.pop_front();
            idle = OutboundQueue.empty();
          }
        }
        LOG_DEBUG("Queue empty!");
      }));
  }
}

int main(int argc, char** argv)
{
  args::ArgumentParser parser("Thallium server program.");
  args::HelpFlag help(parser, "help", "Display usage", { 'h', "help" });
  args::ValueFlag<std::string> url(parser, "url", "Connect to server URL, ex: tcp://localhost:1234",
    { "url" }, args::Options::Required);
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
  auto protocol = url->substr(0, url->find_first_of(':'));
  myEngine = tl::engine(protocol, THALLIUM_CLIENT_MODE, *progress_pool, *rpc_pool);
  myEngine.set_logger(&logger_instance);
  myEngine.set_log_level(log_level);

  tl::remote_procedure send_message_rpc = myEngine.define("send_message");
  tl::endpoint server = myEngine.lookup(url->c_str());

  for (int i = 1; i < 6; ++i)
  {
    std::vector<uint8_t> data;
    if (i % 2)
    {
      data.resize(10);
    }
    else
    {
      data.resize(10000);
    }
    std::fill(data.begin(), data.end(), i);
    LOG_INFO("Post send_message(" << +data[0] << ")");
    PostSend(send_message_rpc, server, std::move(data));
  }

  for (auto& thread: threads) {
    thread->join();
  }

  tl::remote_procedure exit_server_rpc = myEngine.define("exit_server");
  int exitCode = exit_server_rpc.on(server)();
  myEngine.finalize();
  return exitCode;
}
