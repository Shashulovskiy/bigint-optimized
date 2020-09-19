#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <vector>
#include <string>
#include <zconf.h>
#include <cstdint>
#include <cstddef>
#include <iosfwd>
#include <algorithm>
#include <functional>
#include "buffer.h"

struct big_integer {
	big_integer();
	big_integer(big_integer const &other);
	big_integer(uint32_t a);
	big_integer(int a);
	explicit big_integer(std::string const &str);
	~big_integer();
	big_integer &operator=(big_integer const &other);

	big_integer &operator+=(big_integer const &rhs);

	big_integer &operator-=(big_integer const &rhs);
	big_integer &operator*=(big_integer const &rhs);
	big_integer &operator/=(big_integer const &rhs);
	big_integer &operator%=(big_integer const &rhs);
	big_integer &operator&=(big_integer const &rhs);

	big_integer &operator|=(big_integer const &rhs);
	big_integer &operator^=(big_integer const &rhs);
	big_integer &operator<<=(uint32_t rhs);

	big_integer &operator>>=(uint32_t rhs);
	friend big_integer operator+(big_integer a, big_integer const &b);

	friend big_integer operator-(big_integer a, big_integer const &b);
	friend big_integer operator*(big_integer const &a, big_integer const &b);
	friend big_integer operator/(big_integer const &a, big_integer const &b);
	friend big_integer operator%(big_integer const &a, big_integer const &b);
	friend big_integer operator&(big_integer const &a, big_integer const &b);

	friend big_integer operator|(big_integer const &a, big_integer const &b);
	friend big_integer operator^(big_integer const &a, big_integer const &b);
	friend big_integer operator<<(big_integer const &a, uint32_t b);

	uint32_t &operator[](size_t pos);

	friend big_integer operator>>(big_integer const &a, uint32_t b);
	big_integer operator+() const;

	big_integer operator-() const;
	big_integer operator~() const;
	big_integer &operator++();

	big_integer operator++(int);
	big_integer &operator--();

	big_integer operator--(int);
	friend bool operator==(big_integer const &a, big_integer const &b);

	friend bool operator!=(big_integer const &a, big_integer const &b);
	friend bool operator<(big_integer const &a, big_integer const &b);
	friend bool operator>(big_integer const &a, big_integer const &b);
	friend bool operator<=(big_integer const &a, big_integer const &b);
	friend bool operator>=(big_integer const &a, big_integer const &b);
	friend std::string to_string(big_integer const &a);

 private:
	big_integer(bool sign, buffer &data);
	big_integer(bool sign, std::vector<uint32_t> &data);

	bool sign_;
	buffer data_;

	static std::pair<big_integer, big_integer> divide(big_integer const &a, big_integer const &b);
	static std::pair<big_integer, big_integer> divide_by_short(big_integer const &a, big_integer const &b);
	bool shifted_less_than(big_integer const &rhs, size_t shift);
	void shifted_subtract(big_integer const &rhs, size_t shift);

	big_integer &trim();

	static big_integer convert_to_complementary(big_integer const &a);

	static big_integer bitwise_operator(big_integer const &lhs,
										big_integer const &rhs,
										std::function<uint32_t(uint32_t, uint32_t)> f);
};

big_integer operator+(big_integer a, big_integer const &b);
big_integer operator-(big_integer a, big_integer const &b);
big_integer operator*(big_integer const &a, big_integer const &b);
big_integer operator/(big_integer const &a, big_integer const &b);
big_integer operator%(big_integer const &a, big_integer const &b);

big_integer operator&(big_integer const &a, big_integer const &b);
big_integer operator|(big_integer const &a, big_integer const &b);
big_integer operator^(big_integer const &a, big_integer const &b);

big_integer operator<<(big_integer const &a, uint32_t b);
big_integer operator>>(big_integer const &a, uint32_t b);

bool operator==(big_integer const &a, big_integer const &b);
bool operator!=(big_integer const &a, big_integer const &b);
bool operator<(big_integer const &a, big_integer const &b);
bool operator>(big_integer const &a, big_integer const &b);
bool operator<=(big_integer const &a, big_integer const &b);
bool operator>=(big_integer const &a, big_integer const &b);

std::string to_string(big_integer const &a);
std::ostream &operator<<(std::ostream &s, big_integer const &a);

#endif // BIG_INTEGER_H
