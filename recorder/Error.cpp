
#include "Error.h"


std::shared_ptr<Error> Error::create(const std::string& errorText, std::shared_ptr<Error> previous)
{
  auto error = std::make_shared<Error>();
  error->mErrorText = errorText;
  error->m_parentError = std::move(previous);
  return error;
}
