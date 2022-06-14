
#ifndef CAPTURE_ERROR_H
#define CAPTURE_ERROR_H

#include <string>
#include <memory>


class Error
{
public:
  static std::shared_ptr<Error> create(const std::string& errorText, std::shared_ptr<Error> previous = {});

  [[nodiscard]] std::string get_error_text() const { return mErrorText; }

private:
  std::shared_ptr<Error> m_parentError;
  std::string mErrorText;
};


#endif //CAPTURE_ERROR_H
