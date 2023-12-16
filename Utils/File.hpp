//
// Created by Full-Hat on 16/12/2023.
//

#pragma once

#include <fstream>

#include "Utils/UtilsMacros.hpp"

class File {
public:
    static bool read(const std::string& file_name, std::vector<char>& file_buffer) {
        try {
            std::ifstream file(file_name, std::ios::binary);
            if (!file.is_open()) {
                throw std::ios_base::failure("Unable to open file");
            }

            file.seekg(0, std::ios::end);
            std::streamsize file_size = file.tellg();
            file.seekg(0, std::ios::beg);

            if (file_size < 0) {
                throw std::ios_base::failure("Invalid file size");
            }

            file_buffer.resize(static_cast<std::size_t>(file_size));
            file.read(file_buffer.data(), file_size);

            if (!file) {
                throw std::ios_base::failure("Error reading file");
            }

            file.close();
            return true;
        } catch (const std::exception& e) {
            LOG("File read error: ", e.what(), " - File: '", file_name, "'");
            return false;
        }
    }

    static void printContents(const std::vector<char>& file_buffer) {
        std::cout << "File Contents:\n";
        for (char character : file_buffer) {
            std::cout << character;
        }
        std::cout << std::endl;
    }
};
