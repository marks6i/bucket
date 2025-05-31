# Bucket Template Library

A modern C++20 template library for managing ranges with associated values. The library provides two main components:

- `bucket_map`: A map-like container for storing values associated with ranges of indices
- `bucket_list`: A list-like container for storing ordered ranges with associated values

## Features

Common features for both components:
- Range-based operations (spread, cover, erase)
- Half-open range intervals [low, high) where low is inclusive and high is exclusive
- Efficient storage and retrieval of values associated with ranges
- Support for custom index and value types
- Modern C++20 design

### bucket_map
- Map-like interface for range-value associations
- Efficient lookup of values at specific indices
- Support for overlapping ranges
- Automatic range splitting and merging

### bucket_list
- List-like interface for ordered ranges
- Optimized for range-based operations
- Maintains sorted order of ranges
- Support for multiple values per range

## Range Behavior

Both containers use half-open ranges [low, high) where:
- The low bound is inclusive
- The high bound is exclusive
- Empty ranges are not allowed (high must be greater than low)
- Ranges can be adjacent without overlap (e.g., [0,5) and [5,10))

For example:
```cpp
bucket_map<int, std::string> map;
map.spread(0, 5, "A");    // Covers indices [0,1,2,3,4]
map.spread(5, 10, "B");   // Covers indices [5,6,7,8,9]
// Ranges meet at 5 without overlap

bucket_list<int, std::string> list;
list.spread(0, 3, "first");    // Covers indices [0,1,2]
list.spread(3, 6, "second");   // Covers indices [3,4,5]
list.spread(6, 9, "third");    // Covers indices [6,7,8]
// Ranges are stored in order with no gaps
```

## Prerequisites

- C++20 compliant compiler
- CMake 3.15 or higher
- Google Test (automatically downloaded by CMake)

## Building

### Clean Build Process

To perform a clean build with all the correct settings:

```bash
# Remove existing build directory if it exists
rm -rf build    # On Unix-like systems
# OR
rmdir /s /q build    # On Windows

# Create new build directory with correct settings
cmake -B build \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_STANDARD=20 \
    -DCMAKE_CXX_STANDARD_REQUIRED=ON \
    -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDebug

# Build the project
cmake --build build --config Debug

# Run tests
cd build
ctest -C Debug --output-on-failure
```

### Build Options

- `CMAKE_BUILD_TYPE`: Set to `Debug` for development, `Release` for production
- `CMAKE_CXX_STANDARD`: Set to `20` for C++20 features
- `CMAKE_CXX_STANDARD_REQUIRED`: Set to `ON` to enforce C++20
- `CMAKE_MSVC_RUNTIME_LIBRARY`: 
  - `MultiThreadedDebug` for Debug builds
  - `MultiThreaded` for Release builds

### Quick Build (for development)

For quick rebuilds during development, you can use:

```bash
# If build directory already exists with correct settings
cmake --build build --config Debug

# Run tests
cd build
ctest -C Debug --output-on-failure
```

## Running Tests

```bash
# Windows
.\Debug\bucket_list_test.exe
.\Debug\bucket_map_test.exe
.\Debug\bucket_compare_traits_test.exe
.\Debug\bucket_value_traits_test.exe

# Unix-like systems
./bucket_list_test
./bucket_map_test
./bucket_compare_traits_test
./bucket_value_traits_test
```

## Continuous Integration

The project uses GitHub Actions for CI. The pipeline:
- Builds on Windows and Linux
- Runs all tests
- Checks for C++20 compliance
- Validates CMake configuration

## Usage

### bucket_map

```cpp
#include <bucket/bucket_map.h>
#include <string>
#include <iostream>

// Create a bucket map with integer indices and string values
masutils::bucket_map<int, std::string> map;

// Basic range operations
map.spread(0, 5, "value1");  // Associates "value1" with range [0, 5)
map.cover(2, 4, "value2");   // Overwrites range [2, 4) with "value2"
map.erase(1, 3);            // Removes values in range [1, 3)

// Querying values
std::string val = map.at(4);  // Get value at specific index
bool exists = map.contains(2); // Check if index has a value

// Iterating over ranges
for (const auto& [range, value] : map) {
    std::cout << "Range [" << range.first << ", " << range.second << "): " 
              << value << "\n";
}

// Advanced operations
map.clear();  // Clear all ranges
map.spread(0, 10, "base");      // Range [0, 10)
map.cover(3, 7, "middle");      // Range [3, 7) splits existing range
map.spread(5, 8, "overlap");    // Range [5, 8) overlaps with existing ranges

// Using custom types
struct CustomValue {
    int priority;
    std::string data;
};

masutils::bucket_map<double, CustomValue> custom_map;
custom_map.spread(0.0, 1.0, CustomValue{1, "low"});   // Range [0.0, 1.0)
custom_map.spread(1.0, 2.0, CustomValue{2, "high"});  // Range [1.0, 2.0)

// Range lookup operations
auto ranges = custom_map.find_ranges(0.5, 1.5);  // Find all ranges overlapping [0.5, 1.5)
```

### bucket_list

```cpp
#include <bucket/bucket_list.h>
#include <string>
#include <iostream>

// Create a bucket list with integer indices and string values
masutils::bucket_list<int, std::string> list;

// Basic range operations
list.spread(0, 5, "segment1");    // Add range [0, 5)
list.cover(2, 4, "segment2");     // Override range [2, 4)
list.erase(1, 3);                // Remove range [1, 3)

// Querying and accessing
bool has_value = list.contains(4);
std::string val = list.at(4);     // Get value at index

// Iterating through ranges in order
for (const auto& [range, value] : list) {
    std::cout << "Range [" << range.first << ", " << range.second << "): " 
              << value << "\n";
}

// Advanced operations
list.clear();  // Clear all ranges

// Building a sequence of ranges
list.spread(0, 3, "first");    // Range [0, 3)
list.spread(3, 6, "second");   // Range [3, 6)
list.spread(6, 9, "third");    // Range [6, 9)

// Modifying multiple ranges
list.cover(2, 7, "overlap");   // Covers ranges [2, 7)
list.erase(4, 8);             // Removes ranges [4, 8)

// Using with custom types
struct TimeSegment {
    std::string activity;
    int duration;
};

masutils::bucket_list<int, TimeSegment> schedule;
schedule.spread(0, 2, TimeSegment{"meeting", 120});  // Range [0, 2)
schedule.spread(2, 4, TimeSegment{"lunch", 120});    // Range [2, 4)
schedule.spread(4, 8, TimeSegment{"work", 240});     // Range [4, 8)
```

## Advanced Custom Type Examples

### Complex Types with bucket_map

```cpp
#include <bucket/bucket_map.h>
#include <memory>
#include <vector>
#include <string>

// Base class for polymorphic behavior
class Resource {
public:
    virtual ~Resource() = default;
    virtual std::string type() const = 0;
    virtual double usage() const = 0;
};

// Derived classes
class CPUResource : public Resource {
    double cpu_percent_;
    int core_count_;
public:
    CPUResource(double cpu, int cores) : cpu_percent_(cpu), core_count_(cores) {}
    std::string type() const override { return "CPU"; }
    double usage() const override { return cpu_percent_ * core_count_; }
};

class MemoryResource : public Resource {
    size_t bytes_used_;
    size_t total_bytes_;
public:
    MemoryResource(size_t used, size_t total) 
        : bytes_used_(used), total_bytes_(total) {}
    std::string type() const override { return "Memory"; }
    double usage() const override { 
        return static_cast<double>(bytes_used_) / total_bytes_; 
    }
};

// Template class as value type
template<typename T>
class MetricData {
    std::vector<T> samples_;
    std::string unit_;
public:
    MetricData(const std::vector<T>& data, std::string unit)
        : samples_(data), unit_(unit) {}
    
    double average() const {
        if (samples_.empty()) return 0.0;
        return std::accumulate(samples_.begin(), samples_.end(), 0.0) 
               / samples_.size();
    }
};

// Example usage with complex types
void resource_monitoring_example() {
    // Map using polymorphic types with smart pointers
    masutils::bucket_map<int, std::shared_ptr<Resource>> resources;
    
    // Allocate different resources to time ranges
    resources.spread(0, 100, std::make_shared<CPUResource>(75.0, 8));
    resources.spread(50, 150, std::make_shared<MemoryResource>(8'000'000'000, 16'000'000'000));
    
    // Query resource usage at specific points
    if (auto resource = resources.at(75)) {
        std::cout << "Resource type at t=75: " << resource->type() 
                  << ", Usage: " << resource->usage() << "\n";
    }
    
    // Using template class as value type
    masutils::bucket_map<double, MetricData<float>> metrics;
    
    // Store metric samples for different time ranges
    metrics.spread(0.0, 1.0, MetricData<float>({1.2f, 1.3f, 1.1f}, "volts"));
    metrics.spread(1.0, 2.0, MetricData<float>({2.1f, 2.2f, 2.3f}, "volts"));
    
    // Access and process metrics
    auto metric = metrics.at(0.5);
    std::cout << "Average at t=0.5: " << metric.average() << "\n";
}

### Complex Types with bucket_list

```cpp
#include <bucket/bucket_list.h>
#include <variant>
#include <optional>
#include <chrono>

// Complex value type using std::variant
struct TaskPriority {
    enum Level { LOW, MEDIUM, HIGH, CRITICAL };
    Level level;
    std::string justification;
};

struct TaskDuration {
    std::chrono::seconds planned;
    std::optional<std::chrono::seconds> actual;
};

struct Task {
    std::string name;
    TaskPriority priority;
    TaskDuration duration;
    std::variant<
        std::string,              // Simple text status
        std::vector<std::string>, // Multiple status updates
        double                    // Progress percentage
    > status;
};

// Custom comparison traits for chrono types
struct ChronoCompare {
    using TimePoint = std::chrono::system_clock::time_point;
    
    bool less(const TimePoint& a, const TimePoint& b) const {
        return a < b;
    }
    
    bool equal(const TimePoint& a, const TimePoint& b) const {
        return a == b;
    }
};

// Example usage with complex task scheduling
void task_scheduling_example() {
    using TimePoint = std::chrono::system_clock::time_point;
    using namespace std::chrono_literals;
    
    // Create a schedule with custom time point comparison
    masutils::bucket_list<TimePoint, Task, ChronoCompare> schedule;
    
    auto now = std::chrono::system_clock::now();
    
    // Schedule complex tasks
    Task deployment{
        "System Deployment",
        TaskPriority{TaskPriority::CRITICAL, "Production release"},
        TaskDuration{2h, std::nullopt},
        std::vector<std::string>{"Planning", "In Progress"}
    };
    
    Task monitoring{
        "System Monitoring",
        TaskPriority{TaskPriority::HIGH, "Service stability"},
        TaskDuration{4h, 3h + 45min},
        0.75  // 75% complete
    };
    
    // Add tasks to schedule
    schedule.spread(now, now + 2h, deployment);
    schedule.spread(now + 2h, now + 6h, monitoring);
    
    // Query and process tasks
    for (const auto& [range, task] : schedule) {
        std::cout << "Task: " << task.name << "\n";
        std::cout << "Status: ";
        std::visit([](const auto& s) {
            using T = std::decay_t<decltype(s)>;
            if constexpr (std::is_same_v<T, std::string>) {
                std::cout << s;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                std::cout << s.back();
            } else if constexpr (std::is_same_v<T, double>) {
                std::cout << (s * 100) << "% complete";
            }
        }, task.status);
        std::cout << "\n";
    }
}
```

## Error Handling Examples

### Exception Safety and Error Handling with bucket_map

```cpp
#include <bucket/bucket_map.h>
#include <stdexcept>
#include <optional>
#include <cassert>

// Custom exception types
class RangeError : public std::runtime_error {
public:
    explicit RangeError(const std::string& msg) : std::runtime_error(msg) {}
};

class ValueError : public std::runtime_error {
public:
    explicit ValueError(const std::string& msg) : std::runtime_error(msg) {}
};

// Value type with validation
class ValidatedValue {
    int value_;
    static constexpr int MIN_VALUE = 0;
    static constexpr int MAX_VALUE = 100;

public:
    explicit ValidatedValue(int val) {
        if (val < MIN_VALUE || val > MAX_VALUE) {
            throw ValueError("Value must be between 0 and 100");
        }
        value_ = val;
    }
    
    int get() const { return value_; }
};

// Safe wrapper functions for bucket operations
template<typename Index, typename Value>
class SafeBucketMap {
    masutils::bucket_map<Index, Value> map_;

public:
    // Safe spread operation with validation
    bool try_spread(Index start, Index end, const Value& value) noexcept {
        try {
            if (start >= end) {
                return false;  // Invalid range
            }
            map_.spread(start, end, value);
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }

    // Safe access with std::optional
    std::optional<Value> try_at(Index index) const noexcept {
        try {
            if (map_.contains(index)) {
                return map_.at(index);
            }
            return std::nullopt;
        } catch (const std::exception&) {
            return std::nullopt;
        }
    }

    // Exception-safe range modification
    bool modify_range(Index start, Index end, 
                     std::function<void(Value&)> modifier) noexcept {
        try {
            auto ranges = map_.find_ranges(start, end);
            for (auto& [range, value] : ranges) {
                modifier(value);
            }
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }
};

// Example usage of error handling
void error_handling_example() {
    try {
        // Using validated values
        masutils::bucket_map<int, ValidatedValue> validated_map;
        
        // This will succeed
        validated_map.spread(0, 10, ValidatedValue(50));
        
        // This will throw ValueError
        try {
            validated_map.spread(20, 30, ValidatedValue(150));  // Value > 100
        } catch (const ValueError& e) {
            std::cerr << "Validation error: " << e.what() << "\n";
        }

        // Using safe wrapper
        SafeBucketMap<int, int> safe_map;
        
        // Safe operations that won't throw
        bool success = safe_map.try_spread(0, 10, 42);
        assert(success);

        // Invalid range handling
        success = safe_map.try_spread(10, 5, 42);  // start > end
        assert(!success);

        // Safe value access
        if (auto value = safe_map.try_at(5)) {
            std::cout << "Value at 5: " << *value << "\n";
        } else {
            std::cout << "No value at index 5\n";
        }

        // Safe modification of values
        safe_map.modify_range(0, 10, [](int& value) {
            value *= 2;  // Double all values in range
        });

    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << "\n";
    }
}

### Exception Safety and Error Handling with bucket_list

```cpp
#include <bucket/bucket_list.h>
#include <system_error>
#include <type_traits>

// Error code enumeration
enum class BucketError {
    InvalidRange = 1,
    DuplicateEntry,
    OutOfMemory,
    InvalidValue
};

// Error category implementation
class BucketErrorCategory : public std::error_category {
public:
    const char* name() const noexcept override { return "bucket_error"; }
    std::string message(int ev) const override {
        switch (static_cast<BucketError>(ev)) {
            case BucketError::InvalidRange:
                return "Invalid range specified";
            case BucketError::DuplicateEntry:
                return "Duplicate entry detected";
            case BucketError::OutOfMemory:
                return "Out of memory";
            case BucketError::InvalidValue:
                return "Invalid value";
            default:
                return "Unknown error";
        }
    }
};

const BucketErrorCategory& bucket_error_category() {
    static BucketErrorCategory category;
    return category;
}

// Make BucketError work with std::error_code
namespace std {
    template<>
    struct is_error_code_enum<BucketError> : true_type {};
}

std::error_code make_error_code(BucketError e) {
    return {static_cast<int>(e), bucket_error_category()};
}

// Safe bucket list operations with error reporting
template<typename Index, typename Value>
class SafeBucketList {
    masutils::bucket_list<Index, Value> list_;

public:
    // Operation result with error reporting
    struct Result {
        bool success;
        std::error_code error;
    };

    // RAII-style transaction for multiple operations
    class Transaction {
        SafeBucketList& list_;
        std::vector<std::function<void()>> rollback_actions_;
        bool committed_ = false;

    public:
        explicit Transaction(SafeBucketList& list) : list_(list) {}
        
        ~Transaction() {
            if (!committed_) {
                // Rollback on destruction if not committed
                for (auto it = rollback_actions_.rbegin(); 
                     it != rollback_actions_.rend(); ++it) {
                    (*it)();
                }
            }
        }

        Result spread(Index start, Index end, const Value& value) {
            try {
                list_.list_.spread(start, end, value);
                rollback_actions_.push_back([&, start, end]() {
                    list_.list_.erase(start, end);
                });
                return {true, {}};
            } catch (const std::exception&) {
                return {false, make_error_code(BucketError::InvalidRange)};
            }
        }

        void commit() { committed_ = true; }
    };

    // Safe operations with error reporting
    Result try_spread(Index start, Index end, const Value& value) {
        if (start >= end) {
            return {false, make_error_code(BucketError::InvalidRange)};
        }

        try {
            list_.spread(start, end, value);
            return {true, {}};
        } catch (const std::bad_alloc&) {
            return {false, make_error_code(BucketError::OutOfMemory)};
        } catch (const std::exception&) {
            return {false, make_error_code(BucketError::InvalidValue)};
        }
    }

    // Begin a transaction
    Transaction begin_transaction() {
        return Transaction(*this);
    }
};

// Example usage of error handling with bucket_list
void bucket_list_error_handling_example() {
    SafeBucketList<int, std::string> safe_list;

    // Simple operation with error checking
    auto result = safe_list.try_spread(0, 10, "test");
    if (!result.success) {
        std::cerr << "Error: " << result.error.message() << "\n";
    }

    // Using transactions for multiple operations
    {
        auto transaction = safe_list.begin_transaction();
        
        auto r1 = transaction.spread(0, 5, "first");
        if (!r1.success) {
            std::cerr << "First operation failed: " << r1.error.message() << "\n";
            return;  // Transaction will automatically rollback
        }

        auto r2 = transaction.spread(5, 10, "second");
        if (!r2.success) {
            std::cerr << "Second operation failed: " << r2.error.message() << "\n";
            return;  // Transaction will automatically rollback
        }

        transaction.commit();  // All operations successful
    }
}

### Concurrent Access and Logging Examples

```cpp
#include <bucket/bucket_map.h>
#include <bucket/bucket_list.h>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <atomic>

// Thread-safe bucket map with logging
template<typename Index, typename Value>
class ThreadSafeBucketMap {
    mutable std::shared_mutex mutex_;
    masutils::bucket_map<Index, Value> map_;
    std::shared_ptr<spdlog::logger> logger_;
    std::atomic<size_t> active_readers_{0};
    std::atomic<size_t> active_writers_{0};

public:
    ThreadSafeBucketMap() {
        // Initialize logger with daily rotation
        try {
            logger_ = spdlog::daily_logger_mt("bucket_map_logger", "logs/bucket_map.log");
            logger_->set_level(spdlog::level::debug);
            logger_->flush_on(spdlog::level::info);
        } catch (const spdlog::spdlog_ex& ex) {
            std::cerr << "Logger initialization failed: " << ex.what() << std::endl;
        }
    }

    // Write operation with exclusive lock
    bool try_spread(Index start, Index end, const Value& value) {
        try {
            std::unique_lock lock(mutex_);
            active_writers_++;
            
            logger_->debug("Attempting spread operation: [{}, {}]", start, end);
            
            if (start >= end) {
                logger_->warn("Invalid range: start ({}) >= end ({})", start, end);
                active_writers_--;
                return false;
            }

            // Debug info about concurrent access
            logger_->debug("Current concurrent access - Readers: {}, Writers: {}", 
                         active_readers_.load(), active_writers_.load());

            map_.spread(start, end, value);
            
            logger_->info("Successfully spread value over range [{}, {}]", start, end);
            active_writers_--;
            return true;
        } catch (const std::exception& e) {
            active_writers_--;
            logger_->error("Spread operation failed: {}", e.what());
            return false;
        }
    }

    // Read operation with shared lock
    std::optional<Value> try_at(Index index) const {
        try {
            std::shared_lock lock(mutex_);
            active_readers_++;

            logger_->debug("Attempting to read at index: {}", index);
            
            auto result = map_.contains(index) ? 
                         std::optional<Value>(map_.at(index)) : 
                         std::nullopt;

            if (!result) {
                logger_->debug("No value found at index: {}", index);
            }

            active_readers_--;
            return result;
        } catch (const std::exception& e) {
            active_readers_--;
            logger_->error("Read operation failed: {}", e.what());
            return std::nullopt;
        }
    }

    // Debugging helper
    void dump_state() const {
        std::shared_lock lock(mutex_);
        logger_->info("=== Bucket Map State ===");
        logger_->info("Active readers: {}", active_readers_.load());
        logger_->info("Active writers: {}", active_writers_.load());
        
        for (const auto& [range, value] : map_) {
            logger_->info("Range [{}, {}]: {}", range.first, range.second, value);
        }
    }
};

// Deadlock prevention with timeout locks
template<typename Index, typename Value>
class DeadlockSafeBucketList {
    mutable std::shared_mutex mutex_;
    masutils::bucket_list<Index, Value> list_;
    std::shared_ptr<spdlog::logger> logger_;
    static constexpr auto LOCK_TIMEOUT = std::chrono::milliseconds(100);

public:
    DeadlockSafeBucketList() {
        logger_ = spdlog::daily_logger_mt("bucket_list_logger", "logs/bucket_list.log");
        logger_->set_level(spdlog::level::debug);
    }

    // Write operation with timeout
    struct WriteResult {
        bool success;
        enum class Error {
            None,
            Timeout,
            InvalidRange,
            Exception
        } error;
        std::string message;
    };

    WriteResult try_spread_with_timeout(Index start, Index end, const Value& value) {
        logger_->debug("Attempting spread with timeout: [{}, {}]", start, end);

        if (start >= end) {
            logger_->warn("Invalid range parameters: start={}, end={}", start, end);
            return {false, WriteResult::Error::InvalidRange, "Invalid range"};
        }

        // Try to acquire exclusive lock with timeout
        if (!mutex_.try_lock_for(LOCK_TIMEOUT)) {
            logger_->warn("Lock acquisition timeout for spread operation");
            return {false, WriteResult::Error::Timeout, "Operation timed out"};
        }

        // RAII lock guard after successful manual lock
        std::unique_lock<std::shared_mutex> lock(mutex_, std::adopt_lock);

        try {
            list_.spread(start, end, value);
            logger_->info("Successfully spread value over range [{}, {}]", start, end);
            return {true, WriteResult::Error::None, ""};
        } catch (const std::exception& e) {
            logger_->error("Spread operation failed: {}", e.what());
            return {false, WriteResult::Error::Exception, e.what()};
        }
    }
};

// Example usage with debugging and concurrent access
void concurrent_debug_example() {
    ThreadSafeBucketMap<int, std::string> safe_map;
    DeadlockSafeBucketList<int, std::string> safe_list;

    // Set up logging format
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [thread %t] %v");

    // Simulate concurrent access
    std::vector<std::thread> threads;

    // Writer threads
    for (int i = 0; i < 3; ++i) {
        threads.emplace_back([&safe_map, i]() {
            for (int j = 0; j < 5; ++j) {
                safe_map.try_spread(i * 10, i * 10 + 5, 
                    "Value from thread " + std::to_string(i));
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        });
    }

    // Reader threads
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([&safe_map, i]() {
            for (int j = 0; j < 10; ++j) {
                safe_map.try_at(i * 5);
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
        });
    }

    // Debugging thread
    threads.emplace_back([&safe_map]() {
        for (int i = 0; i < 5; ++i) {
            safe_map.dump_state();
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    });

    // Example of deadlock-safe operations
    threads.emplace_back([&safe_list]() {
        for (int i = 0; i < 5; ++i) {
            auto result = safe_list.try_spread_with_timeout(
                i * 10, i * 10 + 5, "Test value");
            
            if (!result.success) {
                switch (result.error) {
                    case DeadlockSafeBucketList<int, std::string>::WriteResult::Error::Timeout:
                        spdlog::warn("Operation timed out, retrying...");
                        break;
                    case DeadlockSafeBucketList<int, std::string>::WriteResult::Error::InvalidRange:
                        spdlog::error("Invalid range specified");
                        break;
                    case DeadlockSafeBucketList<int, std::string>::WriteResult::Error::Exception:
                        spdlog::error("Operation failed: {}", result.message);
                        break;
                    default:
                        break;
                }
            }
        }
    });

    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
}

## Project Structure

```
bucket/
├── include/
│   └── bucket/
│       ├── bucket_map.h
│       ├── bucket_list.h
│       ├── bucket_compare_traits.h
│       └── bucket_value_traits.h
├── tests/
│   ├── bucket_map_test.cpp
│   ├── bucket_list_test.cpp
│   ├── bucket_compare_traits_test.cpp
│   └── bucket_value_traits_test.cpp
├── docs/
│   ├── bucket_map_api.md
│   ├── bucket_list_api.md
│   └── support_files.md
├── CMakeLists.txt
└── README.md
```

## Documentation

- [bucket_map API Documentation](docs/bucket_map_api.md)
- [bucket_list API Documentation](docs/bucket_list_api.md)
- [Support Files Documentation](docs/support_files.md)

## Contributing

1. Fork the repository
2. Create a feature branch from `develop`
3. Make your changes
4. Run tests and ensure they pass
5. Submit a pull request to `develop`

## License

This project is licensed under the Apache License, Version 2.0 - see the LICENSE file for details.

## Installation

The bucket library is header-only, which means there are no binaries to compile or link against. You just need to include the header files in your project.

### Option 1: Download from GitHub Releases

1. Visit the [Releases page](https://github.com/yourusername/bucket/releases) of the bucket library
2. Download the latest release package:
   - `bucket-x.y.z.tar.gz` - Source archive (Unix-like systems)
   - `bucket-x.y.z.zip` - Source archive (Windows)
3. Extract the package:
   ```bash
   # For .tar.gz
   tar xzf bucket-x.y.z.tar.gz
   # For .zip
   unzip bucket-x.y.z.zip
   ```
4. Install using CMake:
   ```bash
   cd bucket-x.y.z
   cmake -B build \
       -DCMAKE_BUILD_TYPE=Release \
       -DCMAKE_INSTALL_PREFIX=/usr/local
   cmake --build build
   sudo cmake --install build
   ```

### Option 2: Building from Source

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/bucket.git
   ```

2. Optional: Create source package
   ```bash
   mkdir build && cd build
   cmake ..
   cpack --config CPackSourceConfig.cmake
   ```

### Using with CMake Projects

Add the library to your project using one of these methods:

1. **FetchContent** (recommended for version control):
   ```cmake
   include(FetchContent)
   FetchContent_Declare(
       bucket
       GIT_REPOSITORY https://github.com/yourusername/bucket.git
       GIT_TAG v1.0.0  # Specify the version you want
   )
   FetchContent_MakeAvailable(bucket)
   
   target_link_libraries(your_target PRIVATE bucket::bucket)
   ```

2. **find_package** (if installed system-wide):
   ```cmake
   find_package(bucket 1.0.0 REQUIRED)
   target_link_libraries(your_target PRIVATE bucket::bucket)
   ```

### Using without CMake

Since this is a header-only library, you can simply:

1. Copy the headers to your project:
   - Copy the `include/bucket` directory to your project's include path
   - Or copy to a system-wide location:
     ```bash
     sudo cp -r include/bucket /usr/local/include/
     ```

2. Include in your C++ code:
   ```cpp
   #include <bucket/bucket_list.h>
   #include <bucket/bucket_map.h>
   ```

3. Compile with C++20 support:
   ```bash
   g++ -std=c++20 your_code.cpp -I/path/to/bucket/include
   ```

### Documentation

Documentation can be found in the `docs/` directory of the source package.
