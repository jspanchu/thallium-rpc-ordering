#pragma once

#include <chrono>
#include <iostream>
#include <mutex>
#include <thallium.hpp>
#include <thallium/logger.hpp>
#include <thread>
#include <vector>

#include "rang.hpp"

namespace tl = thallium;

static std::mutex mtx_cout;

// Asynchronous output
struct acout
{
  std::unique_lock<std::mutex> lk;
  acout()
    : lk(std::unique_lock<std::mutex>(mtx_cout))
  {
  }

  template <typename T>
  acout& operator<<(const T& _t)
  {
    std::cout << _t;
    return *this;
  }

  acout& operator<<(std::ostream& (*fp)(std::ostream&))
  {
    std::cout << fp;
    return *this;
  }
};

class tl_logger : public tl::logger
{

public:
  void trace(const char* msg) const override
  {
    acout() << rang::bg::yellow << "[tid=" << std::this_thread::get_id() << ']' << rang::bg::reset
            << rang::style::dim << rang::style::bold << rang::fg::green << " [trace] "
            << rang::fg::reset << rang::style::reset << rang::style::dim << rang::style::italic
            << msg << rang::style::reset << std::endl;
  }

  void debug(const char* msg) const override
  {
    acout() << rang::bg::yellow << "[tid=" << std::this_thread::get_id() << ']' << rang::bg::reset
            << rang::style::dim << rang::style::bold << rang::fg::cyan << " [debug] "
            << rang::fg::reset << rang::style::reset << rang::style::dim << msg
            << rang::style::reset << std::endl;
  }

  void info(const char* msg) const override
  {
    acout() << rang::bg::yellow << "[tid=" << std::this_thread::get_id() << ']' << rang::bg::reset
            << rang::style::bold << rang::fg::blue << " [info] " << rang::fg::reset
            << rang::style::reset << msg << std::endl;
  }

  void warning(const char* msg) const override
  {
    acout() << rang::bg::yellow << "[tid=" << std::this_thread::get_id() << ']' << rang::bg::reset
            << rang::style::bold << rang::fg::yellow << " [warning] " << rang::fg::reset
            << rang::style::reset << msg << std::endl;
  }

  void error(const char* msg) const override
  {
    acout() << rang::bg::yellow << "[tid=" << std::this_thread::get_id() << ']' << rang::bg::reset
            << rang::style::bold << rang::fg::red << " [error] " << rang::fg::reset
            << rang::style::reset << msg << std::endl;
  }

  void critical(const char* msg) const override
  {
    acout() << rang::bg::yellow << "[tid=" << std::this_thread::get_id() << ']' << rang::bg::reset
            << rang::style::bold << rang::bg::red << " [critical] " << rang::bg::reset
            << rang::style::reset << msg << std::endl;
  }

  static tl::logger::level get_verbosity_from_string(const std::string& level)
  {
    if (level == "trace")
    {
      return tl::logger::level::trace;
    }
    else if (level == "debug")
    {
      return tl::logger::level::debug;
    }
    else if (level == "info")
    {
      return tl::logger::level::info;
    }
    else if (level == "warning")
    {
      return tl::logger::level::warning;
    }
    else if (level == "error")
    {
      return tl::logger::level::error;
    }
    return tl::logger::level::critical;
  }
};

static tl_logger logger_instance = tl_logger();

#define LOG_TRACE(stream_msg)                                                                      \
  {                                                                                                \
    std::stringstream ss;                                                                          \
    ss << stream_msg;                                                                              \
    logger_instance.trace(ss.str().c_str());                                                       \
  }

#define LOG_DEBUG(stream_msg)                                                                      \
  {                                                                                                \
    std::stringstream ss;                                                                          \
    ss << stream_msg;                                                                              \
    logger_instance.debug(ss.str().c_str());                                                       \
  }

#define LOG_INFO(stream_msg)                                                                       \
  {                                                                                                \
    std::stringstream ss;                                                                          \
    ss << stream_msg;                                                                              \
    logger_instance.info(ss.str().c_str());                                                        \
  }

#define LOG_WARNING(stream_msg)                                                                    \
  {                                                                                                \
    std::stringstream ss;                                                                          \
    ss << stream_msg;                                                                              \
    logger_instance.warning(ss.str().c_str());                                                     \
  }

#define LOG_ERROR(stream_msg)                                                                      \
  {                                                                                                \
    std::stringstream ss;                                                                          \
    ss << stream_msg;                                                                              \
    logger_instance.error(ss.str().c_str());                                                       \
  }

#define LOG_CRITICAL(stream_msg)                                                                   \
  {                                                                                                \
    std::stringstream ss;                                                                          \
    ss << stream_msg;                                                                              \
    logger_instance.critical(ss.str().c_str());                                                    \
  }
