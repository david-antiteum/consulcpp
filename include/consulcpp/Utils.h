#pragma once

#include <string>
#include <optional>

namespace consulcpp::utils {
[[nodiscard]] std::optional<unsigned short> asPort( const std::string & portText );
}
