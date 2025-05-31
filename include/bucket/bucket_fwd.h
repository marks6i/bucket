/**
 * @file  bucket_fwd.h
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
 * @brief Forward declarations to resolve circular references.
 */

 #pragma once

namespace masutils {

// Forward declarations
template <typename Container, bool IsConst>
class bucket_range;

template <typename Indices, typename Values, typename CompareTraits, typename ValueTraits>
class bucket_map;

} // namespace masutils 