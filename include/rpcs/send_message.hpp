#pragma once

#include <log.hpp>
#include <thallium.hpp>

namespace tl = thallium;

static uint8_t last_head = 0;
static void send_message(const tl::request& req, const std::vector<uint8_t>& data)
{
  req.respond();
  const uint8_t& head = data.front();
  LOG_DEBUG("Received " << +head);

  if (head - last_head != 1)
  {
    std::stringstream msg;
    msg << "Unexpected head! [" << +head << '-' << +last_head << " != " << 1 << ']';
    // throw std::runtime_error(msg.str());
    LOG_CRITICAL(msg.str());
  }
  last_head = head;
}
