# bucket_map API Documentation

## Overview

`bucket_map` is a template class that provides a map-like container for storing ordered ranges with associated values. Each bucket represents a non-overlapping range on an ordered axis, with gaps allowed if there are no values in the given range. The container automatically handles range splitting and merging operations.

## Range Behavior

The bucket containers use half-open ranges [low, high) where:
- The low bound is inclusive
- The high bound is exclusive
- Empty ranges are not allowed (high must be greater than low)

For example:
```cpp
bucket_map<int, std::string> map;
map.spread(0, 5, "A");  // Covers [0, 1, 2, 3, 4] but not 5
map.spread(5, 10, "B"); // Covers [5, 6, 7, 8, 9] but not 10
// No gap between ranges - they meet at 5
```

## Template Parameters

```cpp
template<
    typename Indices,
    typename Values,
    typename CompareTraits = bucket_compare_traits<Indices>,
    typename ValueTraits = bucket_value_traits<Values>
>
```

### Indices
The type used for range indices. Must satisfy one of:
- Arithmetic type (int, float, etc.)
- Class type implementing:
  ```cpp
  class CustomIndex {
      // Required operations (via CompareTraits)
      bool operator<(const CustomIndex& other) const;  // For ordering
      bool operator==(const CustomIndex& other) const; // For equality
      // Optional: Custom serialization, hashing, etc.
  };
  ```

### Values
The type of values to store in buckets. Can be:
- Simple types (int, string, etc.)
- Complex types (must be copyable or moveable)
- Custom classes:
  ```cpp
  class CustomValue {
      // Must be either copyable or moveable
      CustomValue(const CustomValue&) = default;  // or
      CustomValue(CustomValue&&) = default;
      
      // Optional: Custom value operations via ValueTraits
      void merge(const CustomValue& other);  // For combining values
      void split(CustomValue& other);        // For splitting ranges
  };
  ```

### CompareTraits
Traits class defining comparison operations for indices. Default implementation:
```cpp
template<typename T>
struct bucket_compare_traits {
    static bool lt(const T& a, const T& b) { return a < b; }
    static bool eq(const T& a, const T& b) { return a == b; }
    static void assign(T& dest, const T& src) { dest = src; }
};
```

Custom traits example:
```cpp
struct TimeCompareTraits {
    static bool lt(const std::chrono::system_clock::time_point& a,
                  const std::chrono::system_clock::time_point& b) {
        return a < b;
    }
    static bool eq(const std::chrono::system_clock::time_point& a,
                  const std::chrono::system_clock::time_point& b) {
        return a == b;
    }
    static void assign(std::chrono::system_clock::time_point& dest,
                      const std::chrono::system_clock::time_point& src) {
        dest = src;
    }
};
```

### ValueTraits
Traits class defining operations for the value container. Default implementation:
```cpp
template<typename T>
struct bucket_value_traits {
    using value_container = std::vector<T>;
    
    static void add(value_container& container, const T& value) {
        container.push_back(value);
    }
    static void append(value_container& dest, const value_container& src) {
        dest.insert(dest.end(), src.begin(), src.end());
    }
};
```

Custom traits example:
```cpp
struct SetValueTraits {
    using value_container = std::set<std::string>;
    
    static void add(value_container& container, const std::string& value) {
        container.insert(value);
    }
    static void append(value_container& dest, const value_container& src) {
        dest.insert(src.begin(), src.end());
    }
};
```

## Public Types

```cpp
using index_type = Indices;                  // Type for range bounds
using value_type = Values;                   // Type for stored values
using value_container = typename ValueTraits::value_container;           // Container type for values
using const_value_container = const typename ValueTraits::value_container;
using bucket_type = bucket_object<index_type, value_container>;         // Type representing a bucket
using bucket_type_map = std::map<index_type, bucket_type>;             // Internal storage type
```

## Public Interface

### Constructors and Assignment

```cpp
// Default constructor - creates an unconstrained bucket map
explicit bucket_map() noexcept;

// Constructor with range constraints - enforces value bounds
// Throws std::invalid_argument if high < low
explicit bucket_map(index_type low, index_type high);

// Default destructor
~bucket_map() = default;

// Move assignment operator
bucket_map& operator=(bucket_map&&) noexcept = default;
```

### Range Operations

```cpp
// Spread a value across a range [low, high), potentially splitting existing buckets
// Returns the number of buckets affected by the operation (created, modified, or merged)
// Throws:
//   - std::invalid_argument if high <= low
//   - std::out_of_range if range outside constrained bounds
//   - std::bad_alloc if memory allocation fails
[[nodiscard]] int spread(index_type low, index_type high, value_type value);

// Cover a range [low, high) with a value, replacing any existing values
// Returns the number of buckets affected by the operation (created, modified, or merged)
// Throws:
//   - std::invalid_argument if high <= low
//   - std::out_of_range if range outside constrained bounds
//   - std::bad_alloc if memory allocation fails
[[nodiscard]] int cover(index_type low, index_type high, value_type value);

// Erase all values in the specified range [low, high)
// Returns true if any values were erased, false if range was empty
// Throws:
//   - std::invalid_argument if high <= low
//   - std::out_of_range if range outside constrained bounds
[[nodiscard]] bool erase(index_type low, index_type high);

// Erase all values from the map
// Returns true if any values were erased, false if container was empty
[[nodiscard]] bool erase();

// Check if a bucket contains the given index
// Returns true if a bucket's range [low, high) contains the index, false otherwise
// No exceptions thrown
[[nodiscard]] bool contains(index_type index) const;

// Get the value container for the bucket containing the given index
// Returns a reference to the value container
// Throws std::out_of_range if no bucket contains the index
// Note: It is recommended to call contains() first to check if the index exists
[[nodiscard]] value_container& at(index_type index);
[[nodiscard]] const value_container& at(index_type index) const;

// Find a bucket containing the specified index
// Returns an iterator to the bucket containing the index
// Throws std::out_of_range if no bucket contains the index
// Note: It is recommended to call contains() first to check if the index exists
[[nodiscard]] iterator find(index_type index);
[[nodiscard]] const_iterator find(index_type index) const;

// Find the next bucket relative to the given index
// Returns iterator to the current bucket if index is in it, otherwise the next bucket
// Returns end() if no such bucket exists
// No exceptions thrown
[[nodiscard]] iterator next(index_type index);
[[nodiscard]] const_iterator next(index_type index) const;

// Find the previous bucket relative to the given index
// Returns iterator to the current bucket if index is in it, otherwise the previous bucket
// Returns end() if no such bucket exists before the index
// Returns the last bucket if index is beyond all buckets
// No exceptions thrown
[[nodiscard]] iterator previous(index_type index);
[[nodiscard]] const_iterator previous(index_type index) const;

// Create a view over a range of buckets [start, end)
// Returns a range view that can be used with range-based for loops
// No exceptions thrown
bucket_range<bucket_map, false> range(index_type start, index_type end);
bucket_range<bucket_map, true> range(index_type start, index_type end) const;
```

### Bound Operations

The container provides two sets of methods for accessing bounds:

1. **Direct Bound Access** - For internal use and advanced scenarios
   ```cpp
   // Get the raw bounds of a constrained map
   // Throws std::runtime_error if unconstrained
   [[nodiscard]] index_type low() const;   // Raw lower bound
   [[nodiscard]] index_type high() const;  // Raw upper bound
   ```

2. **Logical Bound Access** - Preferred for general use
   ```cpp
   // Get the logical bounds of a constrained map
   // Throws std::runtime_error if unconstrained
   [[nodiscard]] index_type lower_bound() const;  // Inclusive lower bound
   [[nodiscard]] index_type upper_bound() const;  // Exclusive upper bound
   ```

The difference between these methods:
- `low()/high()` provide direct access to internal bound storage
- `lower_bound()/upper_bound()` represent the logical range boundaries
- For most use cases, prefer `lower_bound()/upper_bound()`
- Both pairs throw `std::runtime_error` if the container is unconstrained

Example:
```cpp
bucket_map<int, std::string> map(0, 10);  // Constrained [0, 10)

// These pairs are equivalent for bucket_map
assert(map.low() == map.lower_bound());       // Both return 0
assert(map.high() == map.upper_bound());      // Both return 10

// But they may differ in derived classes or future implementations
// Always use lower_bound()/upper_bound() unless you specifically need
// the internal representation
```

### Iteration

```cpp
// Iterator types for traversing buckets in order
using iterator = bucket_iterator_base<bucket_type_map, bucket_type, false>;
using const_iterator = bucket_iterator_base<bucket_type_map, bucket_type, true>;
using reverse_iterator = std::reverse_iterator<iterator>;
using const_reverse_iterator = std::reverse_iterator<const_iterator>;

// Forward iteration methods
[[nodiscard]] iterator begin();             // Iterator to first bucket
[[nodiscard]] const_iterator begin() const;
[[nodiscard]] const_iterator cbegin() const;
[[nodiscard]] iterator end();               // Iterator past last bucket
[[nodiscard]] const_iterator end() const;
[[nodiscard]] const_iterator cend() const;

// Reverse iteration methods
[[nodiscard]] reverse_iterator rbegin();    // Reverse iterator to last bucket
[[nodiscard]] const_reverse_iterator rbegin() const;
[[nodiscard]] const_reverse_iterator crbegin() const;
[[nodiscard]] reverse_iterator rend();      // Reverse iterator past first bucket
[[nodiscard]] const_reverse_iterator rend() const;
[[nodiscard]] const_reverse_iterator crend() const;
```

### Accessors

```cpp
// Get the number of buckets in the map
[[nodiscard]] constexpr std::size_t size() const noexcept;

// Check if the map contains no buckets
[[nodiscard]] constexpr bool empty() const noexcept;

// Check if the map has constrained bounds
[[nodiscard]] constexpr bool constrained() const noexcept;
```

## Error Handling

The container provides strong exception guarantees for its operations:

1. **Invalid Arguments**
   - Throws `std::invalid_argument` when:
     - `high < low` in constrained constructor
     - `high < low` in any range operation (spread, cover, erase)
     - `high == low` in any range operation (empty ranges not allowed)

2. **Out of Range**
   - Throws `std::out_of_range` when:
     - Operating on ranges outside constrained bounds
     - `low < lower_bound()` or `high > upper_bound()` in constrained containers
     - Attempting to access values at invalid indices

3. **Unconstrained Access**
   - Throws `std::runtime_error` when:
     - Calling `low()`, `high()`, `lower_bound()`, or `upper_bound()` on an unconstrained container

4. **Iterator Operations**
   - Throws `std::out_of_range` when:
     - Dereferencing end iterators
     - Dereferencing invalidated iterators

5. **Memory Allocation**
   - May throw `std::bad_alloc` during:
     - Container resizing
     - Value container operations
     - Range splitting operations

## Implementation Notes

- Range operations automatically handle splitting and merging of buckets
- Constrained maps enforce value bounds on all operations
- Iterator invalidation follows standard map container rules
- All modifying operations maintain the non-overlapping range invariant

## Usage Patterns

### Basic Range Operations
```cpp
bucket_map<int, std::string> map;

// Simple value assignment
map.spread(0, 10, "section A");  // [0, 10] -> "section A"
map.spread(20, 30, "section B"); // [20, 30] -> "section B"

// Overlapping ranges
map.cover(5, 15, "overlap");     // [0, 5] -> "section A"
                                 // [5, 15] -> "overlap"
                                 // [20, 30] -> "section B"

// Range erasure
map.erase(10, 25);              // [0, 5] -> "section A"
                                // [5, 10] -> "overlap"
                                // [25, 30] -> "section B"
```

### Working with Custom Types
```cpp
struct TimeRange {
    std::chrono::system_clock::time_point start;
    std::chrono::system_clock::time_point end;
    bool operator<(const TimeRange& other) const { return start < other.start; }
    bool operator==(const TimeRange& other) const { return start == other.start && end == other.end; }
};

struct Activity {
    std::string name;
    int priority;
    std::vector<std::string> participants;
};

bucket_map<TimeRange, Activity, TimeCompareTraits> schedule;

// Schedule activities
schedule.spread(
    {now, now + 1h},
    {"Meeting", 1, {"Alice", "Bob"}}
);

// Find overlapping activities
auto it = schedule.find({now + 30min});
if (it != schedule.end()) {
    const auto& activity = it->second;
    std::cout << "Found: " << activity.name << "\n";
}
```

### Constrained Maps
```cpp
// Create a constrained map for a specific range
bucket_map<float, int> grades(0.0f, 100.0f);

// Attempts to add values outside range will throw
try {
    grades.spread(-1.0f, 50.0f, 75);  // Throws std::out_of_range
} catch (const std::out_of_range& e) {
    std::cerr << "Invalid grade range: " << e.what() << "\n";
}
```

### Range Views and Iteration
```cpp
bucket_map<int, std::string> map;
map.spread(0, 10, "A");
map.spread(10, 20, "B");
map.spread(20, 30, "C");

// Forward iteration
for (const auto& [range, value] : map) {
    std::cout << "[" << range.first << ", " << range.second << "] -> "
              << value << "\n";
}

// Reverse iteration
for (auto it = map.rbegin(); it != map.rend(); ++it) {
    const auto& [range, value] = *it;
    // Process in reverse order
}

// Range view over subset
auto view = map.range(5, 25);
for (const auto& [range, value] : view) {
    // Process only ranges that overlap with [5, 25]
}
```

### Value Container Customization
```cpp
// Using sets for unique values
bucket_map<int, std::string, bucket_compare_traits<int>, SetValueTraits> unique_map;

// Values in each range will be unique
unique_map.spread(0, 10, "A");
unique_map.spread(0, 10, "A");  // Value not duplicated

// Custom value merging
struct MergeableValue {
    double sum = 0;
    int count = 0;
    double average() const { return count ? sum / count : 0; }
};

struct AverageValueTraits {
    using value_container = std::vector<MergeableValue>;
    
    static void add(value_container& container, const MergeableValue& value) {
        if (container.empty()) {
            container.push_back(value);
        } else {
            container[0].sum += value.sum;
            container[0].count += value.count;
        }
    }
    
    static void append(value_container& dest, const value_container& src) {
        for (const auto& value : src) {
            add(dest, value);
        }
    }
};

bucket_map<int, MergeableValue, bucket_compare_traits<int>, AverageValueTraits> stats;
stats.spread(0, 10, {100.0, 1});
stats.spread(0, 10, {200.0, 1});  // Values are merged: {300.0, 2}
```

### Specialized Use Cases

#### Memory Allocation Tracking
```cpp
struct MemoryBlock {
    size_t size;
    bool allocated;
    std::string owner;
};

// Custom traits for memory block merging
struct MemoryTraits : public bucket_value_traits<MemoryBlock> {
    static void append(value_container& dest, const value_container& src) {
        // Merge contiguous free blocks
        if (!dest.empty() && !src.empty() &&
            !dest.back().allocated && !src.front().allocated) {
            dest.back().size += src.front().size;
            dest.insert(dest.end(), src.begin() + 1, src.end());
        } else {
            dest.insert(dest.end(), src.begin(), src.end());
        }
    }
};

bucket_map<uintptr_t, MemoryBlock, bucket_compare_traits<uintptr_t>, MemoryTraits> memory_map;

// Allocate memory
void allocate(uintptr_t start, size_t size, const std::string& owner) {
    memory_map.spread(start, start + size, {size, true, owner});
}

// Free memory and merge contiguous blocks
void free(uintptr_t address) {
    auto it = memory_map.find(address);
    if (it != memory_map.end()) {
        it->second.allocated = false;
        it->second.owner = "";
    }
}
```

#### Version Control System Diff
```cpp
struct FileVersion {
    int version;
    std::string content;
    std::string author;
    std::chrono::system_clock::time_point timestamp;
};

// Custom traits for version history
struct VersionTraits : public bucket_value_traits<FileVersion> {
    static void add(value_container& container, const FileVersion& value) {
        // Keep versions sorted by timestamp
        auto pos = std::lower_bound(container.begin(), container.end(),
            value, [](const auto& a, const auto& b) {
                return a.timestamp < b.timestamp;
            });
        container.insert(pos, value);
    }
};

bucket_map<size_t, FileVersion, bucket_compare_traits<size_t>, VersionTraits> file_history;

// Record changes to line ranges
void record_change(size_t start_line, size_t end_line, 
                  const std::string& content, const std::string& author) {
    file_history.spread(start_line, end_line, {
        static_cast<int>(file_history.size() + 1),
        content,
        author,
        std::chrono::system_clock::now()
    });
}
```

### Container Interoperability

#### Converting Between bucket_map and bucket_list
```cpp
// Helper function to convert bucket_map to bucket_list
template<typename Index, typename Value, typename CompareTraits, typename ValueTraits>
bucket_list<Index, Value, CompareTraits, ValueTraits>
to_bucket_list(const bucket_map<Index, Value, CompareTraits, ValueTraits>& map) {
    bucket_list<Index, Value, CompareTraits, ValueTraits> list;
    
    // Transfer ranges in order
    for (const auto& [range, value] : map) {
        list.spread(range.first, range.second, value);
    }
    return list;
}

// Helper function to convert bucket_list to bucket_map
template<typename Index, typename Value, typename CompareTraits, typename ValueTraits>
bucket_map<Index, Value, CompareTraits, ValueTraits>
to_bucket_map(const bucket_list<Index, Value, CompareTraits, ValueTraits>& list) {
    bucket_map<Index, Value, CompareTraits, ValueTraits> map;
    
    // Transfer all ranges
    for (const auto& [range, value] : list) {
        map.spread(range.first, range.second, value);
    }
    return map;
}
```

#### Integration with Standard Containers
```cpp
// Store bucket ranges in standard containers
std::vector<std::pair<std::pair<int, int>, std::string>> 
extract_ranges(const bucket_map<int, std::string>& map) {
    std::vector<std::pair<std::pair<int, int>, std::string>> ranges;
    for (const auto& [range, value] : map) {
        ranges.push_back({{range.first, range.second}, value});
    }
    return ranges;
}

// Reconstruct from standard containers
bucket_map<int, std::string> 
from_ranges(const std::vector<std::pair<std::pair<int, int>, std::string>>& ranges) {
    bucket_map<int, std::string> map;
    for (const auto& [range, value] : ranges) {
        map.spread(range.first, range.second, value);
    }
    return map;
}
```

### Complex Custom Traits

#### Composite Value Traits
```cpp
// Value type combining multiple properties
struct CompositeValue {
    std::variant<int, double, std::string> primary_value;
    std::vector<std::string> tags;
    std::map<std::string, std::string> metadata;
};

// Complex traits handling multiple value types
struct CompositeTraits {
    using value_container = std::vector<CompositeValue>;
    
    static void add(value_container& container, const CompositeValue& value) {
        container.push_back(value);
    }
    
    static void append(value_container& dest, const value_container& src) {
        // Merge values with tag combination and metadata aggregation
        for (const auto& value : src) {
            if (dest.empty()) {
                dest.push_back(value);
                continue;
            }
            
            auto& last = dest.back();
            
            // Combine primary values if possible
            if (value.primary_value.index() == last.primary_value.index()) {
                std::visit([&](auto&& v) {
                    using T = std::decay_t<decltype(v)>;
                    if constexpr (std::is_arithmetic_v<T>) {
                        std::get<T>(last.primary_value) += std::get<T>(value.primary_value);
                    }
                }, value.primary_value);
            } else {
                dest.push_back(value);
            }
            
            // Merge tags without duplicates
            std::set<std::string> unique_tags(last.tags.begin(), last.tags.end());
            unique_tags.insert(value.tags.begin(), value.tags.end());
            last.tags.assign(unique_tags.begin(), unique_tags.end());
            
            // Merge metadata
            last.metadata.insert(value.metadata.begin(), value.metadata.end());
        }
    }
};
```

#### Range-Aware Compare Traits
```cpp
// Custom index type with range awareness
struct RangeAwareIndex {
    int value;
    int tolerance;  // Allowed deviation for comparison
    
    bool overlaps(const RangeAwareIndex& other) const {
        return std::abs(value - other.value) <= (tolerance + other.tolerance);
    }
};

// Traits implementing fuzzy comparison
struct RangeAwareTraits {
    static bool lt(const RangeAwareIndex& a, const RangeAwareIndex& b) {
        return a.value + a.tolerance < b.value - b.tolerance;
    }
    
    static bool eq(const RangeAwareIndex& a, const RangeAwareIndex& b) {
        return a.overlaps(b);
    }
    
    static void assign(RangeAwareIndex& dest, const RangeAwareIndex& src) {
        dest = src;
    }
};

// Example usage with fuzzy range matching
bucket_map<RangeAwareIndex, std::string, RangeAwareTraits> fuzzy_map;
fuzzy_map.spread({100, 5}, {200, 5}, "tolerant range");
auto it = fuzzy_map.find({198, 3});  // Will find the range due to overlap
```

#### Hierarchical Compare Traits
```cpp
// Index type representing hierarchical structure
struct HierarchicalIndex {
    std::vector<int> levels;
    
    std::string to_string() const {
        std::string result;
        for (int level : levels) {
            result += std::to_string(level) + ".";
        }
        return result;
    }
};

// Traits for hierarchical comparison
struct HierarchicalTraits {
    static bool lt(const HierarchicalIndex& a, const HierarchicalIndex& b) {
        const size_t min_size = std::min(a.levels.size(), b.levels.size());
        for (size_t i = 0; i < min_size; ++i) {
            if (a.levels[i] != b.levels[i]) {
                return a.levels[i] < b.levels[i];
            }
        }
        return a.levels.size() < b.levels.size();
    }
    
    static bool eq(const HierarchicalIndex& a, const HierarchicalIndex& b) {
        return a.levels == b.levels;
    }
    
    static void assign(HierarchicalIndex& dest, const HierarchicalIndex& src) {
        dest = src;
    }
};

// Example usage with hierarchical indices
bucket_map<HierarchicalIndex, std::string, HierarchicalTraits> hierarchy;
hierarchy.spread({{1,1}}, {{1,3}}, "Section 1.1-1.3");
hierarchy.spread({{2,1}}, {{2,5}}, "Section 2.1-2.5");
```