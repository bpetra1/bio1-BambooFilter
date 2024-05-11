#include "bamboo_filter.hpp"
#include <iostream>
#include <cmath>

BambooFilter::BambooFilter(size_t initial_capacity, size_t seg_size)
    : segment_size(seg_size)
{
    segments.reserve(initial_capacity / segment_size);
    for (size_t i = 0; i < initial_capacity / segment_size; ++i)
    {
        // segments.push_back({CuckooFilter(seg_size, 4, 10)});  // Assumed bucket size and fingerprint size
    }
}

bool BambooFilter::insert(const int &element)
{
    size_t hash_val = hash(element);
    size_t segment_index = get_segment_index(hash_val);

    // return segments[segment_index].insert(element);
}

bool BambooFilter::lookup(const int &element) const
{
    size_t hash_val = hash(element);
    size_t segment_index = get_segment_index(hash_val);

    // return segments[segment_index].lookup(element);
}

bool BambooFilter::remove(const int &element)
{
    size_t hash_val = hash(element);
    size_t segment_index = get_segment_index(hash_val);

    // return segments[segment_index].remove(element);
}

void BambooFilter::expand()
{
}

size_t BambooFilter::hash(const int &element) const
{
    return std::hash<int>()(element);
}

size_t BambooFilter::get_segment_index(size_t hash_value) const
{
    return hash_value % segments.size();
}
