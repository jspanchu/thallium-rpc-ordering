#include <limits>

#include <thallium.hpp>
#include <thallium/pool.hpp>
#include <thallium/scheduler.hpp>
#include <thallium/serialization/stl/vector.hpp>

#include "logging.hpp"

namespace tl = thallium;

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
  auto rpc_pool = tl::pool::create(tl::pool::access::mpsc, tl::pool::kind::fifo_wait);
  auto rpc_xstream = tl::xstream::create(tl::scheduler::predef::basic_wait, *rpc_pool);
  auto progress_pool = tl::pool::create(tl::pool::access::mpsc, tl::pool::kind::fifo_wait);
  auto progress_xstream = tl::xstream::create(tl::scheduler::predef::basic_wait, *progress_pool);

  tl::engine myEngine("tcp", THALLIUM_CLIENT_MODE, *progress_pool, *rpc_pool);
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
    hello.on(server)(data);
  }

  return 0;
}
