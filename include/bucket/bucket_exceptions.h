/**
 * @file bucket_exceptions.h
 * @brief Shared exception types for bucket containers.
 */

#pragma once

#ifndef MASUTILS_BUCKET_EXCEPTIONS_H_
#define MASUTILS_BUCKET_EXCEPTIONS_H_

#include <stdexcept>

namespace masutils {

class bounds_not_constrained_error : public std::logic_error {
public:
  bounds_not_constrained_error()
      : std::logic_error("Bounds are not constrained.") {}
};

class invalid_range_order_error : public std::invalid_argument {
public:
  invalid_range_order_error()
      : std::invalid_argument("high must be greater than low") {}
};

class range_outside_constrained_bounds_error : public std::out_of_range {
public:
  range_outside_constrained_bounds_error()
      : std::out_of_range("range is outside of constrained bounds") {}
};

class bucket_index_not_found_error : public std::out_of_range {
public:
  bucket_index_not_found_error()
      : std::out_of_range("No bucket contains the specified index") {}
};

class range_index_out_of_bounds_error : public std::out_of_range {
public:
  range_index_out_of_bounds_error()
      : std::out_of_range("Index is outside the range bounds") {}
};

} // namespace masutils

#endif  // MASUTILS_BUCKET_EXCEPTIONS_H_
