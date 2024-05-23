#include "bamboo_filter.hpp"
#include "constants.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <functional>

BambooFilter::BambooFilter(size_t initial_capacity, size_t seg_size)
    : segment_size(seg_size), elements_count(0),
      expansion_threshold(initial_capacity), compression_threshold(initial_capacity / 2)
{
    segments.reserve(initial_capacity / segment_size);
    for (size_t i = 0; i < initial_capacity / segment_size; ++i)
    {
        segments.emplace_back();
    }
}

BambooFilter::~BambooFilter()
{
    for (auto &segment : segments)
    {
        segment.clear();
    }
    segments.clear();
}

bool BambooFilter::insert(const std::string &element)
{
    size_t hash_val = hash(element);
    return insert_hash(hash_val);
}

bool BambooFilter::insert_hash(const size_t &hash_val)
{
    size_t segment_index = get_segment_index(hash_val);

    if (segments[segment_index].insert(hash_val))
    {
        elements_count++;
        if (elements_count > expansion_threshold)
        {
            expand();
        }
        return true;
    }

    expand();
    return insert_hash(hash_val);
}

bool BambooFilter::lookup(const std::string &element) const
{
    size_t hash_val = hash(element);
    return lookup_hash(hash_val);
}

bool BambooFilter::lookup_hash(const size_t &hash_val) const
{
    size_t segment_index = get_segment_index(hash_val);
    return segments[segment_index].lookup(hash_val);
}

bool BambooFilter::remove(const std::string &element)
{
    size_t hash_val = hash(element);
    return remove_hash(hash_val);
}

bool BambooFilter::remove_hash(const size_t &hash_val)
{
    size_t segment_index = get_segment_index(hash_val);

    if (segments[segment_index].remove(hash_val))
    {
        elements_count--;
        if (elements_count < compression_threshold)
        {
            compress();
        }
        return true;
    }
    return false;
}

void BambooFilter::expand()
{
    std::cout << "Expanding..." << "\n";
    segments.emplace_back();

    elements_count = 0;
    std::vector<size_t> elements_to_rehash;
    for (size_t i = 0; i < segments.size(); i++)
    {
        segments[i].collect_elements(elements_to_rehash, i);
        segments[i].clear();
    }
    for (auto &element : elements_to_rehash)
    {
        insert_hash(element);
    }

    expansion_threshold *= 2;
    compression_threshold = expansion_threshold / 2;
}

void BambooFilter::compress()
{
    std::cout << "Compressing..." << "\n";

    if (!segments.empty())
    {
        segments.pop_back();
    }

    elements_count = 0;
    std::vector<size_t> elements_to_rehash;
    for (size_t i = 0; i < segments.size(); i++)
    {
        segments[i].collect_elements(elements_to_rehash, i);
        segments[i].clear();
    }
    for (auto &element : elements_to_rehash)
    {
        insert_hash(element);
    }

    expansion_threshold /= 2;
    compression_threshold = expansion_threshold / 2;
}

size_t BambooFilter::hash(const std::string &element) const
{
    return std::hash<std::string>()(element);
}

size_t BambooFilter::get_segment_index(size_t hash_value) const
{
    // Extract the segment index from the hash value based on the bit lengths
    size_t segment_index = (hash_value >> kBucketIndexBitLength) & ((1 << kInitialSegmentIndexBitLength) - 1);
    return segment_index;
}
