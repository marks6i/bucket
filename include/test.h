#include <iostream>
#include <map>
#include <list>
#include <string>

// Define your triplet_type (assuming it's a tuple with three elements)
using triplet_type = std::tuple<int, int, std::list<std::string>>;

// Define Traits (you can replace this with your actual Traits)
struct Traits
{
  static bool lt(int a, int b) { return a < b; }
  static bool eq(int a, int b) { return a == b; }
  static void assign(int &a, int b) { a = b; }
};

int main()
{
  std::map<std::pair<int, int>, std::list<std::string>> buckets;

  // ... Populate your map with data ...

  int l = 5;  // Example low value
  int h = 15; // Example high value

  // Check for overlaps to slice buckets
  for (auto it = buckets.begin(); it != buckets.end(); ++it)
  {
    if (!Traits::lt(l, h))
      break; // All done since range is null

    const auto &triplet = it->first;

    // Step 1: Isolate the part of the new range before the current bucket
    if (Traits::lt(l, triplet.first))
    {
      if (Traits::lt(triplet.first, h))
      { // Overlap
        buckets[{l, triplet.first}] = it->second;
        Traits::assign(l, triplet.first);
      }
      else
      { // No overlap
        buckets[{l, h}] = it->second;
        Traits::assign(l, triplet.first);
        continue; // It all ends before the current bucket, so we're done
      }
    }

    // Step 2: Isolate the part remaining starting at the current bucket
    if (Traits::eq(l, triplet.first))
    {
      if (Traits::lt(h, triplet.second))
      {
        buckets[{l, h}] = it->second;
        Traits::assign(l, h);
      }
      else
      {
        // Already have a bucket from l to triplet.second (the current bucket), so adjust l
        Traits::assign(l, triplet.second);
      }
    }
  }

  // Print the resulting buckets (for demonstration)
  for (const auto &entry : buckets)
  {
    const auto &range = entry.first;
    std::cout << "Range: [" << range.first << ", " << range.second << ")" << std::endl;
  }

  return 0;
}

#include <iostream>
#include <map>
#include <list>
#include <string>

// Define your triplet_type (assuming it's a tuple with three elements)
using triplet_type = std::tuple<int, int, std::list<std::string>>;

// Define Traits (you can replace this with your actual Traits)
struct Traits
{
  static bool lt(int a, int b) { return a < b; }
  static bool eq(int a, int b) { return a == b; }
  static void assign(int &a, int b) { a = b; }
};

int main()
{
  std::map<std::pair<int, int>, std::list<std::string>> buckets;

  // ... Populate your map with data ...

  int l = 5;  // Example low value
  int h = 15; // Example high value

  // Check for overlaps to slice buckets
  for (auto it = buckets.begin(); it != buckets.end(); ++it)
  {
    if (!Traits::lt(l, h))
      break; // All done since range is null

    const auto &triplet = it->first;

    // Step 1: Isolate the part of the new range before the current bucket
    if (Traits::lt(l, triplet.first))
    {
      if (Traits::lt(triplet.first, h))
      { // Overlap
        buckets[{l, triplet.first}] = it->second;
        Traits::assign(l, triplet.first);
      }
      else
      { // No overlap
        buckets[{l, h}] = it->second;
        Traits::assign(l, triplet.first);
        continue; // It all ends before the current bucket, so we're done
      }
    }

    // Step 2: Isolate the part remaining starting at the current bucket
    if (Traits::eq(l, triplet.first))
    {
      if (Traits::lt(h, triplet.second))
      {
        buckets[{l, h}] = it->second;
        Traits::assign(l, h);
      }
      else
      {
        // Already have a bucket from l to triplet.second (the current bucket), so adjust l
        Traits::assign(l, triplet.second);
      }
    }
  }

  // Print the resulting buckets (for demonstration)
  for (const auto &entry : buckets)
  {
    const auto &range = entry.first;
    std::cout << "Range: [" << range.first << ", " << range.second << ")" << std::endl;
  }

  return 0;
}
