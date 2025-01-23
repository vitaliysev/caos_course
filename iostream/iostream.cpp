#include "iostream.hpp"
#include "unistd.h"
#include <math.h>
#include <iostream>
namespace stdlike {
istream &istream::operator>>(long long &value) {
    auto temp = static_cast<int64_t>(value);
    operator>>(temp);
    value = temp;
    return *this;
}

istream &istream::operator>>(int64_t &value) {
    ptr_to_ostream->flush();
    value = 0;
    skip_symbols();
    int num = 1;
    read_new();
    if (buffer[pos] == '-') {
        num = -1;
        ++pos;
        read_new();
    }
    if (!('0' <= buffer[pos] && buffer[pos] <= '9')) {
        failbit = true;
    }
    while ('0' <= buffer[pos] && buffer[pos] <= '9') {
        value = value * 10 + num * (buffer[pos] - '0');
        ++pos;
        read_new();
    }
    return *this;
}

istream &istream::operator>>(bool &value) {
    ptr_to_ostream->flush();
    skip_symbols();
    read_new();
    value = buffer[pos] == '1';
    ++pos;
    read_new();
    return *this;
}

istream &istream::operator>>(int &value) {
    auto temp = static_cast<int64_t>(value);
    operator>>(temp);
    value = temp;
    return *this;
}

istream &istream::operator>>(unsigned long long &value) {
    auto temp = static_cast<uint64_t>(value);
    operator>>(temp);
    value = temp;
    return *this;
}
istream &istream::operator>>(uint64_t &value) {
    ptr_to_ostream->flush();
    value = 0;
    skip_symbols();
    read_new();
    if (buffer[pos] == '-') {
        ++pos;
        read_new();
    }
    if (!('0' <= buffer[pos] && buffer[pos] <= '9')) {
        failbit = true;
    }
    while ('0' <= buffer[pos] && buffer[pos] <= '9') {
        value = value * 10 + (buffer[pos] - '0');
        ++pos;
        read_new();
    }
    return *this;
}

istream &istream::operator>>(unsigned int &value) {
    auto temp = static_cast<uint64_t>(value);
    operator>>(temp);
    value = temp;
    return *this;
}

istream &istream::operator>>(char &value) {
    ptr_to_ostream->flush();
    skip_symbols();
    read_new();
    if (!failbit) {
        value = buffer[pos];
        ++pos;
    }
    read_new();
    return *this;
}

istream &istream::operator>>(double &value) {
    ptr_to_ostream->flush();
    value = 0;
    skip_symbols();
    read_new();
    int num = 1;
    if (buffer[pos] == '-') {
        num = -1;
        ++pos;
        read_new();
    }
    while ('0' <= buffer[pos] && buffer[pos] <= '9') {
        value = value * 10 + num * (buffer[pos] - '0');
        ++pos;
        read_new();
    }
    ++pos;
    read_new();
    double mul = 10.0;
    while ('0' <= buffer[pos] && buffer[pos] <= '9') {
        value += num * (buffer[pos] - '0') / mul;
        mul *= 10.0;
        ++pos;
        read_new();
    }
    return *this;
}

istream &istream::operator>>(float &value) {
    return operator>>(reinterpret_cast<double &>(value));
}

char istream::get() {
    read_new();
    char ans = buffer[pos];
    ++pos;
    read_new();
    return ans;
}

char istream::peek() {
    return buffer[pos];
}

void istream::read_new() {
    if (pos == size) {
        size = read(fd, buffer, buffer_capacity);
        failbit = size == -1;
        if (size == -1) {
            size = 0;
        }
        pos = 0;
    }
}

void istream::skip_symbols() {
    while (buffer[pos] == ' ' || buffer[pos] == '\n') {
        ++pos;
    }
}
bool istream::fail() const {
    return failbit;
}
void ostream::increase_pos() {
    failbit = false;
    ++pos;
    if (pos == buffer_capacity) {
        failbit = write(fd, buffer, sizeof(buffer)) == -1;
        pos = 0;
    }
}
bool ostream::fail() const {
    return failbit;
}
ostream &ostream::flush() {
    if (pos > 0) {
        int cnt(0);
        while (!failbit && pos > 0) {
            int written = write(fd, buffer + cnt, pos);
            failbit = written == -1;
            cnt += written;
            pos -= written;
        }
    }
    pos = 0;
    return *this;
}

ostream &ostream::operator<<(long long value) {
    return operator<<(static_cast<int64_t>(value));
}
ostream &ostream::operator<<(int64_t value) {
    long long del = 1;
    long long copy = value;
    while (copy != 0) {
        copy /= 10;
        del *= 10;
    }
    if (value < 0) {
        buffer[pos] = '-';
        increase_pos();
    }
    del /= 10;
    while (del > 0) {
        buffer[pos] = abs(value / del) + '0';
        value %= del;
        del /= 10;
        increase_pos();
    }
    return *this;
}

ostream &ostream::operator<<(unsigned long long value) {
    return operator<<(static_cast<uint64_t>(value));
}
ostream &ostream::operator<<(uint64_t value) {
    unsigned long long del = 1;
    unsigned long long copy = value;
    while (copy != 0) {
        copy /= 10;
        del *= 10;
    }
    del /= 10;
    while (del > 0) {
        buffer[pos] = value / del + '0';
        value %= del;
        del /= 10;
        increase_pos();
    }
    return *this;
}

ostream &ostream::operator<<(int value) {
    return operator<<(static_cast<int64_t>(value));
}

ostream &ostream::operator<<(unsigned int value) {
    return operator<<(static_cast<uint64_t>(value));
}

ostream &ostream::operator<<(char value) {
    buffer[pos] = value;
    increase_pos();
    return *this;
}

ostream &ostream::put(char value) {
    buffer[pos] = value;
    increase_pos();
    return *this;
}

ostream &ostream::operator<<(const void *ptr) {
    buffer[pos] = '0';
    increase_pos();
    buffer[pos] = 'x';
    increase_pos();
    int sz = sizeof(ptr) * 2;
    char alphabet[6] = {'a', 'b', 'c', 'd', 'e', 'f'};
    for (int i = 2; i < sz; ++i) {
        int ch = reinterpret_cast<intptr_t>(ptr) >> ((sz - i) * 4) & 0xf;
        if (ch < 10) {
            buffer[pos] = ch + '0';
        } else {
            buffer[pos] = alphabet[ch % 10];
        }
        increase_pos();
    }
    return *this;
}

ostream &ostream::operator<<(const char *ptr) {
    for (int i = 0; ptr[i] != '\0'; ++i) {
        buffer[pos] = ptr[i];
        increase_pos();
    }
    return *this;
}

ostream &ostream::operator<<(double value) {
    if (value < 0) {
        buffer[pos] = '-';
        increase_pos();
    }
    value = abs(value);
    auto integer_part = static_cast<long long>(value);
    double float_part = value - integer_part;
    operator<<(integer_part);
    operator<<('.');
    int count = 0;
    while (float_part != 0 && count < 10) {
        float_part *= 10;
        buffer[pos] = static_cast<int>(float_part) + '0';
        float_part -= static_cast<int>(float_part);
        increase_pos();
        ++count;
    }
    return *this;
}

ostream &ostream::operator<<(bool value) {
    if (value) {
        buffer[pos] = '1';
        increase_pos();
    } else {
        buffer[pos] = '0';
        increase_pos();
    }
    return *this;
}

ostream &ostream::operator<<(float value) {
    return operator<<(static_cast<double>(value));
}
ostream cout;
istream cin{&cout};

}  // namespace stdlike
