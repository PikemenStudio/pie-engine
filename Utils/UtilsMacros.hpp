#pragma once

#include <sstream>

const std::ostringstream log_out;

#define PRINT_DEBUG(message) log_out << message << std::endl;
