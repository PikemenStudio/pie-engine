//
// Created by Full-Hat on 17/12/2023.
//

#pragma once

#include <boost/process.hpp>

#include "Utils/UtilsMacros.hpp"

namespace bp = boost::process;

void execute(const std::string _command) {
    try {
        bp::ipstream output;
        LOG("EXECUTE", _command);
        bp::child process(_command, bp::std_out > output);
        process.wait();

        std::string line;
        while (std::getline(output, line)) {
            LOG("COMMAND RUN", line);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}
