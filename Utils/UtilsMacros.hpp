#pragma once

#include <sstream>
#include <ostream>
#include <iostream>

template <typename... Args>
concept Printable = requires(std::ostream& os, Args&&... args) {
    ((os << std::forward<Args>(args)), ...);
};

class Logger {
public:
    explicit Logger() : output_stream(&std::cout) {

    }

    void set_output_stream(std::ostream& os) {
        output_stream = &os;
    }

    template <Printable... Args>
    void operator()(Args&&... args) const {
        (((*output_stream) << std::forward<Args>(args) << " "), ...);
        (*output_stream) << std::endl;
    }

private:
    std::ostream* output_stream;
};

const Logger LOG;
