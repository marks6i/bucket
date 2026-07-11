# bucket_list API Documentation

## Overview

`bucket_list` is a template class that provides a list-like container for storing ordered ranges with associated values. Each bucket represents a non-overlapping range on an ordered axis, with gaps allowed if there are no values in the given range. The container maintains a strict ordering of ranges and automatically handles range splitting and merging operations.

## Range Behavior

The bucket containers use half-open ranges [low, high) where:
- The low bound is inclusive
- The high bound is exclusive
- Empty ranges are not allowed (high must be greater than low)
- Ranges are maintained in sorted order

For example:
```cpp
bucket_list<int, std::string> list;
list.spread(0, 5, "A");   // Covers [0, 1, 2, 3, 4] but not 5
list.spread(5, 10, "B");  // Covers [5, 6, 7, 8, 9] but not 10
// Ranges are stored in order, no gaps between ranges
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

Custom traits example for time-based ranges:
```cpp
struct DurationCompareTraits {
    static bool lt(const std::chrono::seconds& a,
                  const std::chrono::seconds& b) {
        return a < b;
    }
    static bool eq(const std::chrono::seconds& a,
                  const std::chrono::seconds& b) {
        return a == b;
    }
    static void assign(std::chrono::seconds& dest,
                      const std::chrono::seconds& src) {
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

Custom traits example for priority-based values:
```cpp
struct PriorityValueTraits {
    using value_container = std::priority_queue<Task>;
    
    static void add(value_container& container, const Task& value) {
        container.push(value);
    }
    static void append(value_container& dest, const value_container& src) {
        // Merge priority queues maintaining order
        while (!src.empty()) {
            dest.push(src.top());
            src.pop();
        }
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
// Default constructor - creates an unconstrained bucket list
explicit bucket_list() noexcept;

// Constructor with range constraints - enforces value bounds
// Throws std::invalid_argument if high < low
explicit bucket_list(index_type low, index_type high);

// Default destructor
~bucket_list() = default;

// Move assignment operator
bucket_list& operator=(bucket_list&&) noexcept = default;
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

// Erase all values from the list
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
bucket_range<bucket_list, false> range(index_type start, index_type end);
bucket_range<bucket_list, true> range(index_type start, index_type end) const;
```

### bucket_range Operations

The `bucket_range` class provides a view over a range of buckets in a bucket collection. It supports both forward and reverse iteration over buckets that overlap with the specified range.

```cpp
// Create a bucket_range view
bucket_range<bucket_list, false> range(index_type start, index_type end);
bucket_range<bucket_list, true> range(index_type start, index_type end) const;

// bucket_range member functions:

// Check if a bucket contains the given index within the range
// Returns true if a bucket's range [low, high) contains the index, false otherwise
// Returns false if the index is outside the range bounds
[[nodiscard]] bool contains(index_type index) const;

// Find a bucket containing the given index within the range
// Returns iterator to the bucket containing the index, or end() if not found
// Throws std::out_of_range if the index is outside the range bounds
iterator find(index_type index);
const_iterator find(index_type index) const;

// Get a bucket at the given index within the range
// Returns iterator to the bucket containing the index
// Throws std::out_of_range if no bucket contains the index or if index is outside range bounds
iterator at(index_type index);
const_iterator at(index_type index) const;

// Find the next bucket relative to the given index within the range
// Returns iterator to the current bucket if index is in it, otherwise the next bucket
// Returns end() if no suitable bucket exists or if index is outside range bounds
iterator next(index_type index);
const_iterator next(index_type index) const;

// Find the previous bucket relative to the given index within the range
// Returns iterator to the current bucket if index is in it, otherwise the previous bucket
// Returns end() if no suitable bucket exists or if index is outside range bounds
iterator previous(index_type index);
const_iterator previous(index_type index) const;

// Iterator operations
iterator begin();
iterator end();
const_iterator begin() const;
const_iterator end() const;
iterator rbegin();
iterator rend();
const_iterator rbegin() const;
const_iterator rend() const;
```

Example usage:
```cpp
bucket_list<int, std::string> list;
list.spread(0, 5, "A");
list.spread(5, 10, "B");
list.spread(10, 15, "C");

// Create a range view over [3, 12)
auto range = list.range(3, 12);

// Forward iteration
for (const auto& bucket : range) {
    // Will iterate over buckets that overlap with [3, 12)
    std::cout << "Bucket: [" << bucket.low() << ", " << bucket.high() << ")\n";
}

// Reverse iteration
for (auto it = range.rbegin(); it != range.rend(); ++it) {
    // Will iterate over overlapping buckets in reverse order
    std::cout << "Bucket: [" << it->low() << ", " << it->high() << ")\n";
}

// Using range operations
if (range.contains(4)) {
    auto it = range.find(4);  // Returns iterator to bucket containing 4
    auto values = it->values();  // Access the values in the bucket
}

// Finding next/previous buckets
auto it = range.next(6);      // Get bucket containing or after 6
auto prev = range.previous(6); // Get bucket containing or before 6
```

### Bound Operations

The container provides two sets of methods for accessing bounds:

1. **Direct Bound Access** - For internal use and advanced scenarios
   ```cpp
   // Get the raw bounds of a constrained list
   // Throws std::runtime_error if unconstrained
   [[nodiscard]] index_type low() const;   // Raw lower bound
   [[nodiscard]] index_type high() const;  // Raw upper bound
   ```

2. **Logical Bound Access** - Preferred for general use
   ```cpp
   // Get the logical bounds of a constrained list
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
bucket_list<int, std::string> list(0, 10);  // Constrained [0, 10)

// These pairs are equivalent for bucket_list
assert(list.low() == list.lower_bound());       // Both return 0
assert(list.high() == list.upper_bound());      // Both return 10

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
// Get the number of buckets in the list
[[nodiscard]] constexpr std::size_t size() const noexcept;

// Check if the list contains no buckets
[[nodiscard]] constexpr bool empty() const noexcept;

// Check if the list has constrained bounds
[[nodiscard]] constexpr bool constrained() const noexcept;
```

## Error Handling

The container provides strong exception guarantees for its operations:

1. **Invalid Arguments**
   - Throws `std::invalid_argument` when:
     - `high < low` in constrained constructor
     - `high < low` in any range operation (spread, cover, erase)
     - `high == low` in any range operation (empty ranges not allowed)
     - Attempting to insert ranges that would break ordering

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

- Range operations maintain ordered sequence of buckets
- Constrained lists enforce value bounds on all operations
- Iterator invalidation follows standard map container rules
- All modifying operations maintain the non-overlapping range invariant
- Operations preserve the ordering of ranges in the list

## Usage Patterns

### Basic Range Operations
```cpp
bucket_list<int, std::string> list;

// Sequential range assignment
list.spread(0, 10, "first");     // [0, 10) -> "first"
list.spread(10, 20, "second");   // [10, 20) -> "second"
list.spread(20, 30, "third");    // [20, 30) -> "third"

// Range modification
list.cover(15, 25, "overlap");   // [0, 10) -> "first"
                                // [10, 15) -> "second"
                                // [15, 25) -> "overlap"
                                // [25, 30) -> "third"

// Range erasure
list.erase(5, 15);              // [0, 5) -> "first"
                                // [15, 25) -> "overlap"
                                // [25, 30) -> "third"
```

### Time-Based Scheduling
```cpp
using Duration = std::chrono::seconds;
struct Task {
    std::string name;
    int priority;
    bool completed = false;
};

bucket_list<Duration, Task, DurationCompareTraits> schedule;

// Schedule tasks in sequence
schedule.spread(0s, 1800s, {"Setup", 1});      // 0-30 minutes
schedule.spread(1800s, 3600s, {"Meeting", 2}); // 30-60 minutes
schedule.spread(3600s, 5400s, {"Review", 1});  // 60-90 minutes

// Find task at specific time
auto it = schedule.find(2700s);  // Find task at 45 minutes
if (it != schedule.end()) {
    it->second.completed = true;
}
```

### Constrained Lists with Validation
```cpp
// Create a constrained list for a work day (8 hours)
bucket_list<int, std::string> workday(0, 480);  // minutes

try {
    // Attempt to schedule outside work hours
    workday.spread(470, 490, "Late Meeting");  // Throws std::out_of_range
} catch (const std::out_of_range& e) {
    std::cerr << "Cannot schedule outside work hours: " << e.what() << "\n";
}
```

### Ordered Iteration and Processing
```cpp
bucket_list<int, std::string> list;
list.spread(0, 10, "A");
list.spread(10, 20, "B");
list.spread(20, 30, "C");

// Process ranges in order
for (const auto& [range, value] : list) {
    std::cout << "Processing " << value 
              << " from " << range.first 
              << " to " << range.second << "\n";
}

// Process ranges in reverse order
for (auto it = list.rbegin(); it != list.rend(); ++it) {
    // Process from last range to first
}

// Process specific range
auto view = list.range(5, 25);
for (const auto& [range, value] : view) {
    // Process only ranges that overlap with [5, 25)
}
```

### Priority-Based Task Management
```cpp
struct Task {
    std::string description;
    int priority;
    bool operator<(const Task& other) const {
        return priority < other.priority;
    }
};

bucket_list<int, Task, bucket_compare_traits<int>, PriorityValueTraits> tasks;

// Add tasks with priorities
tasks.spread(0, 60, {"Setup", 1});
tasks.spread(0, 60, {"Urgent Fix", 3});
tasks.spread(0, 60, {"Review", 2});

// Tasks in each range are automatically ordered by priority
for (const auto& [range, task_queue] : tasks) {
    // Process highest priority tasks first
    while (!task_queue.empty()) {
        auto task = task_queue.top();
        std::cout << "Processing: " << task.description << "\n";
        task_queue.pop();
    }
}
```

### Specialized Use Cases

#### Task Scheduler with Dependencies
```cpp
struct Task {
    std::string name;
    std::vector<std::string> dependencies;
    int estimated_duration;
    bool completed = false;
};

// Custom traits for task scheduling
struct TaskTraits : public bucket_value_traits<Task> {
    static void append(value_container& dest, const value_container& src) {
        // Merge tasks while preserving dependencies
        std::map<std::string, std::vector<std::string>> dep_map;
        
        // Collect all dependencies
        for (const auto& task : dest) {
            dep_map[task.name] = task.dependencies;
        }
        for (const auto& task : src) {
            auto& deps = dep_map[task.name];
            deps.insert(deps.end(), 
                       task.dependencies.begin(), 
                       task.dependencies.end());
        }
        
        // Update dependencies in destination
        for (auto& task : dest) {
            task.dependencies = dep_map[task.name];
        }
        
        // Append remaining tasks
        dest.insert(dest.end(), src.begin(), src.end());
    }
};

bucket_list<int, Task, bucket_compare_traits<int>, TaskTraits> schedule;

// Schedule tasks with dependencies
void add_task(int start_time, const Task& task) {
    schedule.spread(start_time, 
                   start_time + task.estimated_duration, 
                   task);
}
```

#### Time Series Data Processing
```cpp
struct TimeSeriesData {
    double value;
    std::vector<double> derivatives;
    std::optional<double> moving_average;
};

// Custom traits for time series analysis
struct TimeSeriesTraits : public bucket_value_traits<TimeSeriesData> {
    static void append(value_container& dest, const value_container& src) {
        if (dest.empty() || src.empty()) {
            dest.insert(dest.end(), src.begin(), src.end());
            return;
        }
        
        // Calculate derivatives at boundary
        auto& last = dest.back();
        auto& first = src.front();
        double derivative = first.value - last.value;
        last.derivatives.push_back(derivative);
        first.derivatives.push_back(derivative);
        
        // Update moving averages
        const size_t window_size = 5;
        std::deque<double> values;
        for (const auto& data : dest) {
            values.push_back(data.value);
            if (values.size() > window_size) values.pop_front();
            if (values.size() == window_size) {
                double sum = std::accumulate(values.begin(), values.end(), 0.0);
                data.moving_average = sum / window_size;
            }
        }
        
        dest.insert(dest.end(), src.begin(), src.end());
    }
};
```

### Container Interoperability

#### Integration with Standard Algorithms
```cpp
// Custom iterator adaptor for standard algorithm compatibility
template<typename Iterator>
class bucket_value_iterator {
    Iterator it;
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = typename Iterator::value_type::second_type;
    using difference_type = std::ptrdiff_t;
    using pointer = const value_type*;
    using reference = const value_type&;
    
    explicit bucket_value_iterator(Iterator iter) : it(iter) {}
    
    reference operator*() const { return it->second; }
    bucket_value_iterator& operator++() { ++it; return *this; }
    bucket_value_iterator operator++(int) {
        auto tmp = *this;
        ++it;
        return tmp;
    }
    bool operator==(const bucket_value_iterator& other) const {
        return it == other.it;
    }
    bool operator!=(const bucket_value_iterator& other) const {
        return !(*this == other);
    }
};

// Example usage with standard algorithms
template<typename T>
void process_bucket_list(bucket_list<int, T>& list) {
    using value_iterator = bucket_value_iterator<typename bucket_list<int, T>::iterator>;
    
    // Sort values within each bucket
    std::for_each(list.begin(), list.end(), [](auto& bucket) {
        std::sort(bucket.second.begin(), bucket.second.end());
    });
    
    // Find specific values
    value_iterator begin(list.begin());
    value_iterator end(list.end());
    auto it = std::find_if(begin, end, [](const auto& value) {
        return /* some condition */;
    });
}
```

#### Serialization Support
```cpp
// Serialization helper for bucket_list
template<typename Index, typename Value>
struct BucketRange {
    Index start;
    Index end;
    Value value;
    
    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & start;
        ar & end;
        ar & value;
    }
};

template<typename Index, typename Value>
std::vector<BucketRange<Index, Value>>
serialize_bucket_list(const bucket_list<Index, Value>& list) {
    std::vector<BucketRange<Index, Value>> result;
    for (const auto& [range, value] : list) {
        result.push_back({range.first, range.second, value});
    }
    return result;
}

template<typename Index, typename Value>
bucket_list<Index, Value>
deserialize_bucket_list(const std::vector<BucketRange<Index, Value>>& ranges) {
    bucket_list<Index, Value> list;
    for (const auto& range : ranges) {
        list.spread(range.start, range.end, range.value);
    }
    return list;
}
```

### Complex Custom Traits

#### State Machine Traits
```cpp
// State transition type
struct State {
    enum class Type { Initial, Processing, Final, Error };
    Type type;
    std::string data;
    std::vector<std::string> history;
    
    bool can_transition_to(const State& next) const {
        switch (type) {
            case Type::Initial:
                return next.type == Type::Processing;
            case Type::Processing:
                return next.type == Type::Processing ||
                       next.type == Type::Final ||
                       next.type == Type::Error;
            case Type::Final:
            case Type::Error:
                return false;
        }
        return false;
    }
};

// Traits for state machine logic
struct StateTraits {
    using value_container = std::vector<State>;
    
    static void add(value_container& container, const State& value) {
        if (container.empty() || 
            container.back().can_transition_to(value)) {
            auto new_state = value;
            if (!container.empty()) {
                new_state.history = container.back().history;
                new_state.history.push_back(container.back().data);
            }
            container.push_back(new_state);
        } else {
            throw std::runtime_error("Invalid state transition");
        }
    }
    
    static void append(value_container& dest, const value_container& src) {
        for (const auto& state : src) {
            add(dest, state);
        }
    }
};
```

#### Event Aggregation Traits
```cpp
// Event type with aggregation support
struct Event {
    std::string type;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, double> metrics;
    std::vector<std::string> tags;
};

// Traits for event aggregation
struct EventTraits {
    using value_container = std::vector<Event>;
    
    static void add(value_container& container, const Event& value) {
        container.push_back(value);
    }
    
    static void append(value_container& dest, const value_container& src) {
        // Group events by type
        std::map<std::string, std::vector<Event>> grouped_events;
        
        for (const auto& event : dest) {
            grouped_events[event.type].push_back(event);
        }
        for (const auto& event : src) {
            grouped_events[event.type].push_back(event);
        }
        
        dest.clear();
        for (const auto& [type, events] : grouped_events) {
            // Aggregate metrics
            Event aggregated{type, events.front().timestamp};
            for (const auto& event : events) {
                for (const auto& [key, value] : event.metrics) {
                    aggregated.metrics[key] += value;
                }
                aggregated.tags.insert(aggregated.tags.end(),
                                     event.tags.begin(),
                                     event.tags.end());
            }
            
            // Remove duplicate tags
            std::sort(aggregated.tags.begin(), aggregated.tags.end());
            aggregated.tags.erase(
                std::unique(aggregated.tags.begin(), aggregated.tags.end()),
                aggregated.tags.end()
            );
            
            dest.push_back(aggregated);
        }
    }
};

// Example usage
bucket_list<int, Event, bucket_compare_traits<int>, EventTraits> event_log;

void log_event(int timestamp, const Event& event) {
    event_log.spread(timestamp, timestamp + 1, event);
}
```

#### Conflict Resolution Traits
```cpp
// Value type with conflict resolution
struct VersionedValue {
    std::string content;
    int version;
    std::string author;
    std::vector<VersionedValue> conflicts;
};

// Traits implementing three-way merge
struct VersionTraits {
    using value_container = std::vector<VersionedValue>;
    
    static void add(value_container& container, const VersionedValue& value) {
        if (container.empty()) {
            container.push_back(value);
            return;
        }
        
        auto& current = container.back();
        if (value.version > current.version) {
            // New version supersedes current
            container.push_back(value);
        } else if (value.version == current.version && 
                   value.content != current.content) {
            // Conflict detected
            current.conflicts.push_back(value);
        }
        // Older versions are ignored
    }
    
    static void append(value_container& dest, const value_container& src) {
        for (const auto& value : src) {
            add(dest, value);
        }
    }
};

// Example usage
bucket_list<int, VersionedValue, bucket_compare_traits<int>, VersionTraits> document;

void update_section(int start, int end, const VersionedValue& value) {
    document.spread(start, end, value);
}
```