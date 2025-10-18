#include "Log.h"
#include <filesystem>
#include <iostream>

LogLevel Log::level = LogLevel::INFO;
std::ofstream Log::file;

void Log::initialize(LogLevel startLevel, const std::string& filePath){
    level = startLevel;

    // opens file in location
    file.open(filePath, std::ios::out);

    // checks if file opens / exists
    if(!file.is_open()){
        std::cout << "Could not open file" << std::endl;
    } else{
        std::cout << "Logging Initialized" << std::endl;
    }
}

// sets current log level
void Log::setLevel(LogLevel newLevel){
    level = newLevel;
}

// sends log to console and file
void Log::log(LogLevel msgLevel, const char* message){

    // sends log message to the console
    std::cout << message <<std::endl;

    // sends log to file
    if (file.is_open()){
        file << message << '\n';
    }
}