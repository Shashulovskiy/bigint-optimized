//
// Created by Artem Shashulovskiy on 04.07.2020.
//

#ifndef BIGINT__BUFFER_H_
#define BIGINT__BUFFER_H_

#include <cstddef>
#include <cstdint>
#include <memory>
#include "shared_vector.h"

class buffer {
	static size_t constexpr STATIC_SIZE = sizeof(shared_vector) / sizeof(uint32_t);

	size_t size_;
	bool is_small;
	union {
		uint32_t static_data[STATIC_SIZE];
		shared_vector *dynamic_data;
	};

	void ensure_uniqueness() {
		if (!is_small) {
			dynamic_data = dynamic_data->unshare();
		}
	}

	void ensure_type(size_t sz) {
		if (sz >= STATIC_SIZE) {
			is_small = false;
			std::vector<uint32_t> static_data_vector(static_data, static_data + STATIC_SIZE);
			dynamic_data = new shared_vector(static_data_vector);
		}
	}

	void copy_static_buffer(buffer const &other) {
		std::copy(other.static_data, other.static_data + other.size_, static_data);
	}

	void add_reference(buffer const &other) {
		dynamic_data = other.dynamic_data;
		dynamic_data->add_reference();
	}

	void copy_buffer(buffer const &other) {
		if (is_small) {
			copy_static_buffer(other);
		} else {
			add_reference(other);
		}
	}

 public:
	~buffer() {
		if (!is_small) {
			dynamic_data->destroy();
		}
	}

	buffer() : size_(0), is_small(true) {}

	explicit buffer(uint32_t a) : size_(1), is_small(true) {
		static_data[0] = a;
	}

	buffer(buffer const &other) : size_(other.size_), is_small(other.is_small) {
		copy_buffer(other);
	}

	buffer &operator=(buffer const &other) {
		if (this == &other) {
			return *this;
		}
		this->~buffer();
		size_ = other.size_;
		is_small = other.is_small;
		copy_buffer(other);
		return *this;
	}

	uint32_t &operator[](size_t pos) {
		if (is_small) {
			return static_data[pos];
		} else {
			ensure_uniqueness();
			return (*dynamic_data)[pos];
		}
	}

	uint32_t const &operator[](size_t pos) const {
		if (is_small) {
			return static_data[pos];
		} else {
			return (*dynamic_data)[pos];
		}
	}

	friend bool operator==(buffer const &a, buffer const &b) {
		if (a.size_ != b.size_) {
			return false;
		}
		for (size_t i = 0; i < a.size_; ++i) {
			if (a[i] != b[i]) {
				return false;
			}
		}
		return true;
	}

	size_t size() const {
		return size_;
	}

	void resize(size_t sz) {
		if (sz > size_) {
			ensure_uniqueness();
			ensure_type(sz);
			if (!is_small) {
				dynamic_data->resize(sz);
				for (size_t i = size_; i < STATIC_SIZE; ++i) {
					(*dynamic_data)[i] = 0;
				}
			} else {
				for (size_t i = size_; i < sz; ++i) {
					static_data[i] = 0;
				}
			}
			size_ = sz;
		}
	}

	void push_back(uint32_t element) {
		ensure_uniqueness();
		if (is_small) {
			ensure_type(size_);
		}

		if (is_small) {
			static_data[size_] = element;
		} else {
			dynamic_data->push_back(element);
		}

		++size_;
	}

	uint32_t const &back() const {
		if (is_small) {
			return static_data[size_ - 1];
		} else {
			return dynamic_data->back();
		}
	}

	void pop_back() {
		ensure_uniqueness();
		--size_;
		if (!is_small) {
			dynamic_data->pop_back();
		}
	}

};

#endif //BIGINT__BUFFER_H_
