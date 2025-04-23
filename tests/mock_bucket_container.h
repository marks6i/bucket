/**
 * @file  mock_bucket_container.h
 * @brief Mock bucket container for testing.
 */

#pragma once

#include <vector>
#include <utility>
#include <gmock/gmock.h>

namespace masutils {
namespace test {

template<typename IndexType, typename ValueType>
class MockBucketContainer {
public:
    using index_type = IndexType;
    using value_type = ValueType;
    using bucket_type = std::pair<std::pair<index_type, index_type>, std::vector<value_type>>;
    using iterator = typename std::vector<bucket_type>::iterator;
    using const_iterator = typename std::vector<bucket_type>::const_iterator;
    using reverse_iterator = typename std::vector<bucket_type>::reverse_iterator;
    using const_reverse_iterator = typename std::vector<bucket_type>::const_reverse_iterator;

    // Add the required accessor class as a nested type
    class accessor {
    public:
        static constexpr index_type low(const bucket_type& bucket) noexcept {
            return bucket.first.first;
        }

        static constexpr index_type high(const bucket_type& bucket) noexcept {
            return bucket.first.second;
        }
    };

    MOCK_METHOD(void, spread, (index_type low, index_type high, value_type value), ());
    
    iterator begin() { return buckets_.begin(); }
    iterator end() { return buckets_.end(); }
    const_iterator begin() const { return buckets_.begin(); }
    const_iterator end() const { return buckets_.end(); }
    reverse_iterator rbegin() { return buckets_.rbegin(); }
    reverse_iterator rend() { return buckets_.rend(); }
    const_reverse_iterator rbegin() const { return buckets_.rbegin(); }
    const_reverse_iterator rend() const { return buckets_.rend(); }

    void add_bucket(index_type low, index_type high, value_type value) {
        buckets_.push_back({{low, high}, {value}});
    }

private:
    std::vector<bucket_type> buckets_;
};

} // namespace test
} // namespace masutils 