#include <limits>

#include <thallium.hpp>
#include <thallium/pool.hpp>
#include <thallium/scheduler.hpp>
#include <thallium/serialization/stl/vector.hpp>

#include "logging.hpp"

namespace tl = thallium;

#include <deque>
#include <mutex>
#include <thread>

using Message = std::vector<uint8_t>;

std::deque<Message> OutboundQueue;
tl::engine myEngine;
std::mutex Mutex;

void PostSend(tl::remote_procedure proc, tl::endpoint server, Message &&data) {
  bool idle = false;
  {
    std::lock_guard<std::mutex> lock(Mutex);
    idle = OutboundQueue.empty();
    OutboundQueue.emplace_back(data);
  }
  if (idle) {
    myEngine.get_handler_pool().make_thread(
        [proc, server]() {
          bool idle = false;
          while (!idle) {
            auto &data = OutboundQueue.front();
            uint8_t h = proc.on(server)(data);
            std::cout << "Got " << int(h) << std::endl;
            {
              std::lock_guard<std::mutex> lock(Mutex);
              OutboundQueue.pop_front();
              idle = OutboundQueue.empty();
            }
          }
          std::cout << "Queue empty !" << std::endl;
        },
        thallium::anonymous{});
  }
}

int main(int argc, char **argv) {
  auto logger = my_logger();
  tl::logger::set_global_logger(&logger);
  tl::logger::set_global_log_level(tl::logger::level::info);

  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <address>" << std::endl;
    exit(0);
  }

  margo_set_environment(nullptr);
  tl::abt a;
  auto rpc_pool =
      tl::pool::create(tl::pool::access::mpsc, tl::pool::kind::fifo_wait);
  auto rpc_xstream =
      tl::xstream::create(tl::scheduler::predef::basic_wait, *rpc_pool);
  auto progress_pool =
      tl::pool::create(tl::pool::access::mpsc, tl::pool::kind::fifo_wait);
  auto progress_xstream =
      tl::xstream::create(tl::scheduler::predef::basic_wait, *progress_pool);

  myEngine = tl::engine("tcp", THALLIUM_CLIENT_MODE, *progress_pool, *rpc_pool);
  myEngine.set_logger(&logger);
  myEngine.set_log_level(tl::logger::level::info);
  tl::remote_procedure hello = myEngine.define("hello"); //.disable_response();
  tl::endpoint server = myEngine.lookup(argv[1]);

  // for (int i = 1; i < std::numeric_limits<uint8_t>::max(); ++i) {
  for (int i = 1; i < 6; ++i) {
    std::vector<uint8_t> data;
    if (i % 2) {
      data.resize(10);
    } else {
      data.resize(10000);
    }
    std::fill(data.begin(), data.end(), i);
    std::cout << "Invoking hello(" << +data[0] << ")" << std::endl;
    PostSend(hello, server, std::move(data));
    //std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  std::cout << "Done" << std::endl;

  return 0;
}
