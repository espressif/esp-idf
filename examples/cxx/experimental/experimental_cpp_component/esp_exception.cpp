#ifdef __cpp_exceptions

#include "esp_exception.hpp"

namespace idf {

ESPException::ESPException(esp_err_t error) : error(error) { }

} // namespace idf

#endif // __cpp_exceptions
