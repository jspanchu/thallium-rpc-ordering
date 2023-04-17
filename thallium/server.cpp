#include <iostream>
#include <sstream>
#include <stdexcept>

#include <thallium.hpp>
#include <thallium/serialization/stl/vector.hpp>

#include "logging.hpp"

namespace tl = thallium;

static uint8_t lastHead = 0;

void hello(const tl::request &req, const std::vector<uint8_t> &data) {
  const uint8_t &head = data.front();
  std::cout << "Data[0] " << +head << std::endl;

  if (head - lastHead != 1) {
    std::stringstream msg;
    msg << "Unexpected head! [" << +head << '-' << +lastHead << " != " << 1
        << ']';
    throw std::runtime_error(msg.str());
  }
  lastHead = head;
}

int main(int argc, char *argv[]) {
  auto logger = my_logger();
  margo_set_environment(nullptr);
  tl::abt a;
  tl::logger::set_global_logger(&logger);
  tl::logger::set_global_log_level(tl::logger::level::info);

  auto rpc_pool = tl::pool::create(tl::pool::access::mpsc, tl::pool::kind::fifo_wait);
  auto rpc_xstream = tl::xstream::create(tl::scheduler::predef::basic_wait, *rpc_pool);
  auto progress_pool = tl::pool::create(tl::pool::access::mpsc, tl::pool::kind::fifo_wait);
  auto progress_xstream = tl::xstream::create(tl::scheduler::predef::basic_wait, *progress_pool);

  tl::engine myEngine("tcp", THALLIUM_SERVER_MODE, *progress_pool, *rpc_pool);
  myEngine.set_logger(&logger);
  myEngine.set_log_level(tl::logger::level::info);
  myEngine.define("hello", &hello).disable_response();
  std::cout << "Server running at address " << myEngine.self() << std::endl;

  return 0;
}
