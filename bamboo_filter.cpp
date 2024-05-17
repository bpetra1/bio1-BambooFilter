#include "bamboo_filter.hpp"
#include <iostream>
#include <vector>

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

bool BambooFilter::insert(const uint32_t &element)
{
    size_t hash_val = hash(element);
    size_t segment_index = get_segment_index(hash_val);

    if (segments[segment_index].insert(element))
    {
        elements_count++;
        if (elements_count > expansion_threshold)
        {
            expand();
        }
        return true;
    }

    expand();
    return insert(element);
}

bool BambooFilter::lookup(const uint32_t &element) const
{
    size_t hash_val = hash(element);
    size_t segment_index = get_segment_index(hash_val);

    return segments[segment_index].lookup(element);
}

bool BambooFilter::remove(const uint32_t &element)
{
    size_t hash_val = hash(element);
    size_t segment_index = get_segment_index(hash_val);

    if (segments[segment_index].remove(element))
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
    cout << "Expanding..." << "\n";
    segments.emplace_back();

    elements_count = 0;
    std::vector<uint32_t> elements_to_rehash;
    for (auto &segment : segments)
    {
        segment.collect_elements(elements_to_rehash);
        segment.clear();
    }
    for (auto &element : elements_to_rehash)
    {
        insert(element);
    }

    expansion_threshold *= 2;
    compression_threshold = expansion_threshold / 2;
}

void BambooFilter::compress()
{
    cout << "Compressing..." << "\n";

    if (!segments.empty())
    {
        segments.pop_back();
    }

    elements_count = 0;
    std::vector<uint32_t> elements_to_rehash;
    for (auto &segment : segments)
    {
        segment.collect_elements(elements_to_rehash);
        segment.clear();
    }
    for (auto &element : elements_to_rehash)
    {
        insert(element);
    }

    expansion_threshold /= 2;
    compression_threshold = expansion_threshold / 2;
}

size_t BambooFilter::hash(const uint32_t &element) const
{
    return std::hash<uint32_t>()(element);
}

size_t BambooFilter::get_segment_index(size_t hash_value) const
{
    return hash_value % segments.size();
}
