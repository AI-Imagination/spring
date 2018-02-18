#ifndef SPRING_STATUS_H
#define SPRING_STATUS_H

#include <string>

namespace spring {

class Status {
 public:
  Status() {}
  Status(char code) : code_(code) {}
  Status(char code, std::string&& msg) : code_(code), msg_(std::move(msg)) {}
  Status(char code, const std::string& msg) : code_(code), msg_(msg) {}

  bool ok() const { return code_ == 0; }

  const std::string& msg() const { return msg_; }

 private:
  char code_{0};
  std::string msg_;
};

#define SP_LOG(msg)                                                  \
  std::cerr << __DATE__ << " " << __TIME__ << " " << __FILE__ << " " \
            << __LINE__ << "\t" << msg << std::endl;
#define SP_LOG_IF(cond, msg) \
  { SP_LOG(cond, msg); }     \
  while (false)              \
    ;
#define SP_CHECK_OK(status) \
  if (!status.ok()) {       \
    SP_LOG(status.msg());   \
  }
#define SP_CHECK(cond, errno) \
  if (!(cond)) {              \
    SP_LOG("error");          \
    return Status(errno);     \
  }

}  // namespace spring

#endif  // SPRING_STATUS_H
