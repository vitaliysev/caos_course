#pragma once

#include <cstdlib>
#include <cstdint>
namespace stdlike {
const int buffer_capacity = 2048;
class ostream {
public:
    ostream &operator<<(int value);
    ostream &operator<<(float value);
    ostream &operator<<(double value);
    ostream &operator<<(char value);
    ostream &operator<<(const void *ptr);
    ostream &operator<<(const char *ptr);
    ostream &operator<<(long long value);
    ostream &operator<<(int64_t value);
    ostream &operator<<(uint64_t value);
    ostream &operator<<(unsigned int value);
    ostream &operator<<(unsigned long long value);
    ostream &operator<<(bool value);
    ostream &put(char value);
    ostream &flush();
    bool fail() const;

private:
    char buffer[buffer_capacity]{};
    int fd = 1;
    int pos = 0;
    void increase_pos();
    bool failbit = false;
};
class istream {
public:
    explicit istream(ostream *ptr) : ptr_to_ostream(ptr){};
    istream &operator>>(int &value);
    istream &operator>>(float &value);
    istream &operator>>(double &value);
    istream &operator>>(char &value);
    //istream &operator>>(long &value);
    istream &operator>>(long long &value);
    istream &operator>>(int64_t &value);
    istream &operator>>(unsigned int &value);
    istream &operator>>(uint64_t &value);
    istream &operator>>(unsigned long long &value);
    istream &operator>>(bool &value);
    bool fail() const;
    char get();
    char peek();

private:
    void skip_symbols();
    char buffer[buffer_capacity]{};
    int size = 0;
    int fd = 0;
    int pos = 0;
    void read_new();
    ostream *ptr_to_ostream;
    bool failbit = false;
};

extern istream cin;
extern ostream cout;
}  // namespace stdlike

