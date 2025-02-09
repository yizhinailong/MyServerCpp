#include "Buffer.h"

#include <iostream>

Buffer::Buffer() {}

Buffer::~Buffer() {}

void Buffer::append(const char* str, int size) {
    for (int i = 0; i < size; i++) {
        if (str[i] == '\0') {
            break;
        }
        _buffer.push_back(str[i]);
    }
}

ssize_t Buffer::size() {
    return ssize_t(_buffer.size());
}

const char* Buffer::c_str() {
    return _buffer.c_str();
}

void Buffer::clear() {
    _buffer.clear();
}

void Buffer::get_line() {
    _buffer.clear();
    std::getline(std::cin, _buffer);
}

void Buffer::set_buf(const char* buffer) {
    _buffer.clear();
    _buffer.append(buffer);
}
