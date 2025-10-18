#pragma once
#include <iostream>
#include <string>
#include <fstream>

// different levels to log
enum LogLevel{
    WARNING = 0,
    INFO = 1, 
    ERR = 2
};

class Log{
    private:
    static LogLevel level;
    static std::ofstream file;

    public:
    // open the log file and initialize logging
    static void initialize(LogLevel startLevel, const std::string& filePath);

    // sends message to log
    static void log(LogLevel newLevel, const char* message);

    // set log level
    static void setLevel(LogLevel newLevel);
};