#pragma once
#include <Arduino.h>

class Logger {
public:
    static void printf(const char* fmt, ...);
    static void println(const char* msg);
    static String get();
private:
    static String buffer;
    static void append(const String& str);
};

#define LOGF Logger::printf
#define LOGLN Logger::println
