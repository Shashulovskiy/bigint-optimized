//
// Created by Artem Shashulovskiy on 04.07.2020.
//

#ifndef BIGINT__SHARED_VECTOR_H_
#define BIGINT__SHARED_VECTOR_H_

class shared_vector {
	uint32_t ref_counter;
	std::vector<uint32_t> data;

 public:
	shared_vector() : ref_counter(1), data(0) {};

	shared_vector(std::vector<uint32_t> const &other) : ref_counter(1), data(other) {};
	shared_vector(shared_vector const &other) : ref_counter(1), data(other.data) {};

	~shared_vector() = default;

	uint32_t use_count() {
		return ref_counter;
	}

	shared_vector *unshare() {
		if (use_count() != 1) {
			--ref_counter;
			return new shared_vector(*this);
		}
		return this;
	}

	uint32_t &operator[](size_t pos) {
		return data[pos];
	}

	uint32_t const &operator[](size_t pos) const {
		return data[pos];
	}

	bool operator==(shared_vector const& other) {
		return data == other.data;
	}

	void resize(size_t sz) {
		data.resize(sz);
	}

	void push_back(uint32_t const &element) {
		data.push_back(element);
	}

	uint32_t const &back() const {
		return data.back();
	}

	void pop_back() {
		data.pop_back();
	}

	void destroy() {
		ref_counter--;
		if (use_count() == 0) {
			delete this;
		}
	}

	void add_reference() {
		++ref_counter;
	}
};

#endif //BIGINT__SHARED_VECTOR_H_
