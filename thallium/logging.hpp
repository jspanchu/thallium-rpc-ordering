#include <thallium.hpp>

namespace tl = thallium;

class my_logger : public tl::logger {

public:
  void trace(const char *msg) const override {
    std::cout << "[trace] " << msg << std::endl;
  }

  void debug(const char *msg) const override {
    std::cout << "[debug] " << msg << std::endl;
  }

  void info(const char *msg) const override {
    std::cout << "[info] " << msg << std::endl;
  }

  void warning(const char *msg) const override {
    std::cout << "[warning] " << msg << std::endl;
  }

  void error(const char *msg) const override {
    std::cout << "[error] " << msg << std::endl;
  }

  void critical(const char *msg) const override {
    std::cout << "[critical] " << msg << std::endl;
  }
};
