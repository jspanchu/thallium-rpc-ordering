#include <thallium.hpp>

namespace tl = thallium;

static std::function<void()> exit_callback;

static void exit_server(const tl::request& req)
{
  exit_callback();
  req.respond(0);
}