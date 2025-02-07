#pragma once
#include <string>

class Buffer {
public:
    Buffer();
    ~Buffer();

    void append(const char* str, int size);

    ssize_t size();

    const char* c_str();

    void clear();

    void get_line();

private:
    std::string _buffer;
};
