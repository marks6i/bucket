// Copyright 2024 Mark Solinski
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef MASUTILS_OPTIONAL_H_
#define MASUTILS_OPTIONAL_H_

namespace masutils {

// This is a very simple implementation of std::optional. It is not a complete implementation.
// I generated it with Microsoft Copilot and then modified it to fit my needs. I created the
// the default_value member to allow for a default value to be returned if the optional is not
// set. I did this to avoid the uninitialized value problem. I also added the operator<< to allow
// for the optional to be printed; the '<?>' is printed if the optional is not set.

template <typename _T>
class optional {
private:
	_T default_value = {};

public:
	bool _hasValue;
	_T _value;

	constexpr optional() : _hasValue(false), _value(default_value) {}

	optional(_T value) : _hasValue(true), _value(value) {}

	optional(const optional& other) : _hasValue(other._hasValue), _value(other._value) {}

	optional& operator=(const optional& other) {
		_hasValue = other._hasValue;
		_value = other._value;
		return *this;
	}

	optional& operator=(_T value) {
		_hasValue = true;
		_value = value;
		return *this;
	}

	_T value() const {
		return _value;
	}

	operator bool() const {
		return _hasValue;
	}

	friend std::ostream& operator<<(std::ostream& os, const optional<_T>& obj) {
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