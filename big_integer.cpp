#include "big_integer.h"

#include <utility>
#include <vector>
#include <string>
#include <cstdint>
#include <cstddef>
#include <iosfwd>
#include <iostream>
#include <algorithm>
#include <functional>

/* * * * * * * * * Constructors & destructor * * * * * * * * * */

big_integer::big_integer() : sign_(false), data_(0) {}

big_integer::big_integer(big_integer const &other) {
	sign_ = other.sign_;
	data_ = other.data_;
}

big_integer::big_integer(int a) : sign_(false), data_() {
	if (a == INT32_MIN) {
		sign_ = true;
		data_ = buffer(static_cast<uint32_t>(INT32_MAX) + 1);
	} else {
		if (a < 0) {
			sign_ = true;
			a = -a;
		} else {
			sign_ = false;
		}

		data_.push_back(a);
	}
}

big_integer::big_integer(std::string const &str) {
	big_integer res = big_integer();
	sign_ = false;
	size_t i = 0;
	if (str[i] == '-') {
		sign_ = true;
		++i;
	}
	if (str[i] == '+') {
		++i;
	}
	for (; i < str.size(); ++i) {
		res = res * 10 + (str[i] - '0');
	}
	data_ = res.data_;
	trim();
}

big_integer::big_integer(uint32_t a) : sign_(false), data_(a) {}

big_integer::big_integer(bool sign, buffer &data) {
	sign_ = sign;
	data_ = data;
}

big_integer::~big_integer() = default;

/* * * * * * * * * Assignment operators * * * * * * * * * */

big_integer &big_integer::operator=(big_integer const &other) = default;

big_integer &big_integer::operator+=(big_integer const &rhs) {
	return *this = *this + rhs;
}

big_integer &big_integer::operator-=(big_integer const &rhs) {
	return *this = *this - rhs;
}

big_integer &big_integer::operator*=(big_integer const &rhs) {
	return *this = *this * rhs;
}

big_integer &big_integer::operator/=(big_integer const &rhs) {
	return *this = *this / rhs;
}

big_integer &big_integer::operator%=(big_integer const &rhs) {
	return *this = *this % rhs;
}

big_integer &big_integer::operator&=(big_integer const &rhs) {
	return *this = *this & rhs;
}

big_integer &big_integer::operator|=(big_integer const &rhs) {
	return *this = *this | rhs;
}

big_integer &big_integer::operator^=(big_integer const &rhs) {
	return *this = *this ^ rhs;
}

big_integer &big_integer::operator<<=(uint32_t rhs) {
	return *this = *this << rhs;
}

big_integer &big_integer::operator>>=(uint32_t rhs) {
	return *this = *this >> rhs;
}

/* * * * * * * * * Unary operators * * * * * * * * * */

big_integer big_integer::operator+() const {
	return *this;
}

big_integer big_integer::operator-() const {
	big_integer tmp(*this);
	tmp.sign_ = !tmp.sign_;
	return tmp.trim();
}

big_integer big_integer::operator~() const {
	return -*this - 1;
}

big_integer &big_integer::operator++() {
	return *this += 1;
}

big_integer big_integer::operator++(int) {
	big_integer res = *this;
	++*this;
	return res;
}

big_integer &big_integer::operator--() {
	return *this -= 1;
}

big_integer big_integer::operator--(int) {
	big_integer res = *this;
	--*this;
	return res;
}

/* * * * * * * * * Binary operators (+, -, *) * * * * * * * * * */

big_integer operator+(big_integer a, big_integer const &b) {
	if (a.sign_ ^ b.sign_) {
		return a - -b;
	}

	uint32_t carry = 0;
	size_t res_size = std::max(a.data_.size(), b.data_.size());
	a.data_.resize(res_size);
	for (size_t i = 0; i < res_size; ++i) {
		uint32_t a_digit = a.data_[i];
		uint32_t b_digit = i < b.data_.size() ? b.data_[i] : 0;

		uint32_t curr_carry = carry;
		uint64_t res_digit = a_digit + b_digit;

		if (a_digit > UINT32_MAX - b_digit || res_digit > UINT32_MAX - curr_carry) {
			carry = 1;
			res_digit &= UINT32_MAX;
		} else {
			carry = 0;
		}

		a.data_[i] = res_digit + curr_carry;
	}

	a.data_.push_back(carry);
	return a.trim();
}

big_integer operator-(big_integer a, big_integer const &b) {
	if (a.sign_ == 0 && b.sign_ == 1) {
		return a += -b;
	}
	if (a.sign_ == 1 && b.sign_ == 0) {
		return -(-a + b);
	}
	if (a.sign_ == 1 && b.sign_ == 1) {
		return -(-a - -b);
	}
	if (a < b) {
		return -(b - a);
	}

	uint32_t carry = 0;
	for (size_t i = 0; i < a.data_.size(); ++i) {
		uint32_t a_digit = a.data_[i];
		uint32_t b_digit = i < b.data_.size() ? b.data_[i] : 0;

		uint32_t curr_carry = carry;
		uint64_t res_digit = a_digit - b_digit;

		if (b_digit > a_digit || res_digit < carry) {
			carry = 1;
		} else {
			carry = 0;
		}

		a.data_[i] = res_digit - curr_carry;
	}
	return a.trim();
}

big_integer operator*(big_integer const &a, big_integer const &b) {
	big_integer res;
	res.data_.resize(a.data_.size() + b.data_.size() + 1);

	for (size_t i = 0; i < a.data_.size(); ++i) {
		uint64_t carry = 0;
		for (size_t j = 0; j < b.data_.size() || carry; ++j) {
			uint32_t a_digit = i < a.data_.size() ? a.data_[i] : 0;
			uint32_t b_digit = j < b.data_.size() ? b.data_[j] : 0;
			uint64_t ans_digit = res.data_[i + j] + a_digit * 1ULL * b_digit + carry;

			res.data_[i + j] = ans_digit & UINT32_MAX;
			carry = ans_digit >> 32;
		}
	}

	res.sign_ = a.sign_ ^ b.sign_;
	return res.trim();
}

/* * * * * * * * * Binary operators (div, mod) * * * * * * * * * */

big_integer operator/(big_integer const &a, big_integer const &b) {
	return big_integer::divide(a, b).first;
}

big_integer operator%(big_integer const &a, big_integer const &b) {
	return big_integer::divide(a, b).second;
}

std::pair<big_integer, big_integer> big_integer::divide(big_integer const &u, big_integer const &v) {
	big_integer a = u.sign_ ? -u : u, b = v.sign_ ? -v : v;
	size_t res_sign = u.sign_ ^v.sign_;

	if (a < b) {
		return std::make_pair(0, u.sign_ == v.sign_ ? u : u + v);
	}

	if (b.data_.size() == 1) {
		return big_integer::divide_by_short(u, v);
	}

	uint32_t shift = (static_cast<uint64_t>(UINT32_MAX) + 1) / (static_cast<uint64_t>(b.data_[b.data_.size() - 1]) + 1);
	a *= shift;
	b *= shift;
	a.data_.push_back(0);
	big_integer res;
	res.sign_ = res_sign;
	res.data_.resize(a.data_.size() - b.data_.size());

	for (ptrdiff_t i = res.data_.size() - 1; i >= 0; --i) {
		size_t sza = a.data_.size();
		size_t szb = b.data_.size();

		uint64_t lhs = static_cast<uint64_t>(a.data_[sza - 1]) << 32;
		if (sza >= 2) {
			lhs |= static_cast<uint64_t>(a.data_[sza - 2]);
		}
		uint32_t rhs = b.data_[szb - 1];

		res.data_[i] = std::min(lhs / rhs, static_cast<uint64_t>(UINT32_MAX));
		big_integer to_subtract = res.data_[i] * b;

		while (a.shifted_less_than(to_subtract, i)) {
			to_subtract -= b;
			res.data_[i]--;
		}
		a.shifted_subtract(to_subtract, i);
	}

	a.sign_ = u.sign_;
	return std::make_pair(res.trim(), a / shift);
}

std::pair<big_integer, big_integer> big_integer::divide_by_short(big_integer const &a, big_integer const &other) {
	uint32_t b = other.data_[0];
	big_integer res;
	res.data_.resize(a.data_.size());
	uint64_t curr = 0;
	for (ptrdiff_t i = res.data_.size() - 1; i >= 0; --i) {
		curr = a.data_[i] + curr * static_cast<uint64_t>(UINT32_MAX + 1LL);
		res.data_[i] = static_cast<uint32_t>(curr / b);
		curr %= b;
	}
	res.sign_ = a.sign_ ^ other.sign_;
	big_integer remainder = static_cast<uint32_t>(curr);
	remainder.sign_ = a.sign_;
	return std::make_pair(res.trim(), remainder.trim());
}

bool big_integer::shifted_less_than(big_integer const &rhs, size_t shift) {
	if (rhs == 0) {
		return false;
	}
	if (data_.size() != (rhs.data_.size() + shift)) {
		return data_.size() < (rhs.data_.size() + shift);
	} else {
		for (ptrdiff_t i = rhs.data_.size() - 1; i >= 0; --i) {
			if (data_[i + shift] != rhs.data_[i]) {
				return data_[i + shift] < rhs.data_[i];
			}
		}
	}
	return false;
}

void big_integer::shifted_subtract(big_integer const &rhs, size_t shift) {
	uint32_t carry = 0;
	for (size_t i = shift; i < data_.size(); ++i) {
		uint32_t a_digit = data_[i];
		uint32_t b_digit = (i - shift < rhs.data_.size() ? rhs.data_[i - shift] : 0);

		uint32_t curr_carry = carry;
		uint64_t res_digit = a_digit - b_digit;

		if (b_digit > a_digit || res_digit < carry) {
			carry = 1;
		} else {
			carry = 0;
		}

		data_[i] = res_digit - curr_carry;
	}

	trim();
}

/* * * * * * * * * Bitwise binary operators (&, |, ^) * * * * * * * * * */

big_integer operator&(big_integer const &a, big_integer const &b) {
	return big_integer::bitwise_operator(a, b, [](uint32_t a, uint32_t b) { return a & b; });
}

big_integer operator|(big_integer const &a, big_integer const &b) {
	return big_integer::bitwise_operator(a, b, [](uint32_t a, uint32_t b) { return a | b; });
}

big_integer operator^(big_integer const &a, big_integer const &b) {
	return big_integer::bitwise_operator(a, b, [](uint32_t a, uint32_t b) { return a ^ b; });
}

big_integer big_integer::convert_to_complementary(big_integer const &a) {
	buffer res = a.data_;
	for (size_t i = 0; i < res.size(); ++i) {
		res[i] = ~res[i];
	}
	return big_integer(false, res) + 1;
}

big_integer big_integer::bitwise_operator(big_integer const &lhs,
										  big_integer const &rhs,
										  std::function<uint32_t(uint32_t, uint32_t)> f) {
	big_integer a_complementary = lhs.sign_ ? convert_to_complementary(lhs) : lhs;
	big_integer b_complementary = rhs.sign_ ? convert_to_complementary(rhs) : rhs;

	big_integer res;
	res.data_.resize(std::max(a_complementary.data_.size(), b_complementary.data_.size()));

	for (size_t i = 0; i < res.data_.size(); ++i) {
		uint32_t a_digit = i < a_complementary.data_.size() ? a_complementary.data_[i] : (lhs.sign_ ? UINT32_MAX : 0);
		uint32_t b_digit = i < b_complementary.data_.size() ? b_complementary.data_[i] : (rhs.sign_ ? UINT32_MAX : 0);
		res.data_[i] = f(a_digit, b_digit);
	}

	res.sign_ = f(lhs.sign_, rhs.sign_);

	if (res.sign_) {
		res = convert_to_complementary(res);
		res.sign_ = f(lhs.sign_, rhs.sign_);
	}

	return res.trim();
}

/* * * * * * * * * Bit shift operators (>>, <<) * * * * * * * * * */

big_integer operator<<(big_integer const &a, uint32_t b) {
	uint32_t shift_digits = b / 32;
	uint32_t shift_number = b % 32;

	big_integer shifted = a * static_cast<uint32_t>(1 << shift_number);

	big_integer res;
	res.sign_ = a.sign_;
	res.data_.resize(shifted.data_.size() + shift_digits);
	for (size_t i = 0; i < shift_digits; ++i) {
		res.data_[i] = 0;
	}
	for (size_t i = 0; i < shifted.data_.size(); ++i) {
		res.data_[i + shift_digits] = shifted.data_[i];
	}
	return res;
}

big_integer operator>>(big_integer const &a, uint32_t b) {
	uint32_t shift_digits = b / 32;
	uint32_t shift_number = b % 32;
	uint32_t mod = 1 << shift_number;

	big_integer shifted = a / mod;

	size_t shifted_length = shifted.data_.size() - shift_digits;
	if (shifted_length <= 0) {
		return 0;
	} else {
		big_integer res;
		res.sign_ = a.sign_;
		res.data_.resize(shifted_length);
		for (size_t i = 0; i < shifted_length; ++i) {
			res.data_[i] = shifted.data_[i + shift_digits];
		}
		if (a.sign_) {
			--res;
		}
		return res;
	}
}

/* * * * * * * * * Сomparison operators * * * * * * * * * */

bool operator==(big_integer const &a, big_integer const &b) {
	return (a.data_ == b.data_) && (a.sign_ == b.sign_);
}

bool operator!=(big_integer const &a, big_integer const &b) {
	return !(a == b);
}

bool operator<(big_integer const &a, big_integer const &b) {
	if (a.sign_ != b.sign_) {
		return a.sign_ > b.sign_;
	}

	if (a.sign_ == 1 && b.sign_ == 1) {
		return (-a > -b);
	}

	if (a.data_.size() != b.data_.size()) {
		return a.data_.size() < b.data_.size();
	}

	for (ptrdiff_t i = a.data_.size() - 1; i >= 0; i--) {
		if (a.data_[i] != b.data_[i]) {
			return a.data_[i] < b.data_[i];
		}
	}
	return false;
}

bool operator>(big_integer const &a, big_integer const &b) {
	if (a.sign_ != b.sign_) {
		return a.sign_ < b.sign_;
	}

	if (a.sign_ == 1 && b.sign_ == 1) {
		return (-a < -b);
	}

	if (a.data_.size() != b.data_.size()) {
		return a.data_.size() > b.data_.size();
	}

	for (ptrdiff_t i = a.data_.size() - 1; i >= 0; i--) {
		if (a.data_[i] != b.data_[i]) {
			return a.data_[i] > b.data_[i];
		}
	}
	return false;
}

bool operator<=(big_integer const &a, big_integer const &b) {
	return !(a > b);
}

bool operator>=(big_integer const &a, big_integer const &b) {
	return !(a < b);
}

/* * * * * * * * * String related operation * * * * * * * * * */

std::string to_string(big_integer const &a) {
	if (a == 0) {
		return "0";
	}

	big_integer a_copy(a);
	std::string res;
	while (a_copy != 0) {
		std::pair<big_integer, big_integer> digits = big_integer::divide_by_short(a_copy, 10);
		res += static_cast<char>(digits.second.data_[0] + '0');
		a_copy = digits.first;
	}
	if (a.sign_) res += '-';
	reverse(res.begin(), res.end());
	return res;
}

/* * * * * * * * * Helper functions * * * * * * * * * */

big_integer &big_integer::trim() {
	while (data_.size() > 1) {
		if (data_.back() == 0) {
			data_.pop_back();
		} else {
			break;
		}
	}
	if (data_.size() == 1 && data_.back() == 0) {
		sign_ = false;
	}

	return *this;
}

uint32_t &big_integer::operator[](size_t pos) {
	return data_[pos];
}

/* * * * * * * * * ostream operators * * * * * * * * * */

std::ostream &operator<<(std::ostream &s, big_integer const &a) {
	return s << to_string(a);
}
