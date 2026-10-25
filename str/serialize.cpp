#include "serialize.h"

#include <cstring>
#include <iomanip>
#include <istream>
#include <ostream>
#include <stdexcept>

using namespace std;

namespace ser {

namespace {

void check_read(const istream& in, const char* what) {
    if (!in) {
        throw runtime_error(string("Ошибка чтения: ") + what);
    }
}

}

void write_tag(ostream& out, const char (&tag)[5]) {
    out.write(tag, 4);
}

void read_tag(istream& in, const char (&tag)[5]) {
    char buf[4] = {};
    in.read(buf, 4);
    check_read(in, "тег");
    if (memcmp(buf, tag, 4) != 0) {
        throw runtime_error(string("Неверный тег, ожидался ") + tag);
    }
}

void write_size(ostream& out, size_t n) {
    const auto v = static_cast<uint64_t>(n);
    out.write(reinterpret_cast<const char*>(&v), sizeof(v));
}

size_t read_size(istream& in) {
    uint64_t v = 0;
    in.read(reinterpret_cast<char*>(&v), sizeof(v));
    check_read(in, "размер");
    if (v > MAX_ITEMS) {
        throw runtime_error("Слишком большой размер в файле");
    }
    return static_cast<size_t>(v);
}

void write_string(ostream& out, const string& s) {
    write_size(out, s.size());
    out.write(s.data(), static_cast<streamsize>(s.size()));
}

string read_string(istream& in) {
    const size_t len = read_size(in);
    string s(len, '\0');
    in.read(s.data(), static_cast<streamsize>(len));
    check_read(in, "строка");
    return s;
}

void write_int(ostream& out, int value) {
    const auto v = static_cast<int32_t>(value);
    out.write(reinterpret_cast<const char*>(&v), sizeof(v));
}

int read_int(istream& in) {
    int32_t v = 0;
    in.read(reinterpret_cast<char*>(&v), sizeof(v));
    check_read(in, "число");
    return v;
}

void write_text_header(ostream& out, const string& tag, size_t n) {
    out << tag << ' ' << n << '\n';
}

size_t read_text_header(istream& in, const string& tag) {
    string got;
    uint64_t n = 0;
    in >> got >> n;
    check_read(in, "заголовок");
    if (got != tag) {
        throw runtime_error("Неверный заголовок, ожидался " + tag);
    }
    if (n > MAX_ITEMS) {
        throw runtime_error("Слишком большой размер в файле");
    }
    return static_cast<size_t>(n);
}

void write_text_string(ostream& out, const string& s) {
    out << quoted(s) << '\n';
}

string read_text_string(istream& in) {
    string s;
    in >> quoted(s);
    check_read(in, "строка");
    return s;
}

void write_text_int(ostream& out, int value) {
    out << value << '\n';
}

int read_text_int(istream& in) {
    int v = 0;
    in >> v;
    check_read(in, "число");
    return v;
}

}
