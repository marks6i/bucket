//
// pch.h
//

#ifndef PCH_H_
#define PCH_H_

// TODO: reference additional headers your program requires here

#include <stdexcept>
#include <string>
#include <cstdio>
#include <iostream>
#include <vector>
#include <mutex>

#if BUCKET_USING_GTEST_
#include "gtest/gtest.h"
#endif // BUCKET_USING_GTEST_

#if !defined(_MSC_VER)
#pragma GCC diagnostic ignored "-Wwrite-strings"
#endif

#if defined(__MINGW32__) || defined(__MINGW64__)
#define __MINGW__
#endif

#endif // PCH_H_
