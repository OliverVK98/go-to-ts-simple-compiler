//
// Created by oliver on 3/25/24.
//

#ifndef GO_TO_TS_SIMPLE_COMPILER_LOGGER_H
#define GO_TO_TS_SIMPLE_COMPILER_LOGGER_H
#include "string"
#include <fstream>

class logger {
public:
    explicit logger()
            : log_file("logs.txt", std::ios::app) {
        if (!log_file.is_open()) {
            throw std::runtime_error("Failed to open log file");
        }
    }

    void log(unsigned long message) {
        log_file << message << std::endl;
    }

    void log(std::string message) {
        log_file << message << std::endl;
    }

    void log(int message) {
        log_file << message << std::endl;
    }


    void log(bool message) {
        log_file << message << std::endl;
    }

    ~logger() {
        if (log_file.is_open()) {
            log_file.close();
        }
    }

private:
    std::ofstream log_file;
};

#endif //GO_TO_TS_SIMPLE_COMPILER_LOGGER_H
