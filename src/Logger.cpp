#include "Logger.h"
#include <cstdarg>

String Logger::buffer;

void Logger::append(const String& str) {
    buffer += str;
    if (buffer.length() > 2048) {
        buffer.remove(0, buffer.length() - 2048);
    }
}

void Logger::printf(const char* fmt, ...) {
    char buf[128];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    Serial.print(buf);
    append(String(buf));
}

void Logger::println(const char* msg) {
    Serial.println(msg);
    append(String(msg) + "\n");
}

String Logger::get() {
    return buffer;
}
