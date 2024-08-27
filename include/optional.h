/**
 * @file  optional.h
 * @copyright
 * Copyright 2024 Mark Solinski
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @brief An optional template class.
 *
 * This is a very simple implementation of std::optional. It is not a complete implementation.
 * I generated it with Microsoft Copilot and then modified it to fit my needs. I created the
 * default_value member to allow for a default value to be returned if the optional is not
 * set. I did this to avoid the uninitialized value problem. I also added the operator<< to allow
 * for the optional to be printed; the '<?>' is printed if the optional is not set.
 */

#ifndef MASUTILS_OPTIONAL_H_
#define MASUTILS_OPTIONAL_H_

namespace masutils {

/**
 * @brief 
 * @tparam T 
 */
template <typename T>
class optional {
private:
	T default_value = {}; /** < */

public:
	bool has_value_; /** < */
	T value_; /** < */

	constexpr optional() : has_value_(false), value_(default_value) {} /** < */

	explicit optional(T value) : has_value_(true), value_(value) {} /** < */

	optional(const optional& other) : has_value_(other.has_value_), value_(other.value_) {} /** < */

	/**
	 * @brief 
	 * @param other 
	 * @return 
	 */
	optional& operator=(const optional& other) {
		has_value_ = other.has_value_;
		value_ = other.value_;
		return *this;
	}

	/**
	 * @brief
	 * @param value
	 * @return
	 */
	optional& operator=(T value) {
		has_value_ = true;
		value_ = value;
		return *this;
	}

	/**
	 * @brief 
	 * @return 
	 */
	T value() const {
		return value_;
	}

	/**
	 * @brief
	 * @return
	 */
	explicit operator bool() const {
		return has_value_;
	}

	/**
	 * @brief
	 * @return
	 */
	friend std::ostream& operator<<(std::ostream& os, const optional<T>& obj) {
		if (obj) {
			os << obj.value();
		}
		else {
			os << "<?>";
		}
		return os;
	}
};

} // namespace masutils

#endif // MASUTILS_OPTIONAL_H_