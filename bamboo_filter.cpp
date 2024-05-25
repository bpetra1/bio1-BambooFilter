#include "bamboo_filter.hpp"
#include "constants.hpp"
#include <iostream>
#include <functional>

BambooFilter::BambooFilter(size_t initial_capacity, size_t seg_size)
    : segment_size(seg_size), elements_count(0), current_segment_index(0),
      expansion_threshold(initial_capacity), compression_threshold(initial_capacity / 2)
{
    segments.reserve(initial_capacity / segment_size);
    for (size_t i = 0; i < initial_capacity / segment_size; ++i)
    {
        segments.emplace_back();
    }
    std::cout << "BambooFilter initialized with capacity: " << initial_capacity << " and segment size: " << segment_size << std::endl;
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

    if (segment_index >= segments.size())
    {
        std::cerr << "Error: Segment index " << segment_index << " out of bounds (total segments: " << segments.size() << ")" << std::endl;
        return false;
    }

    if (segments[segment_index].insert(hash_val))
    {
        elements_count++;
        std::cout << "Inserted element, new count: " << elements_count << std::endl;
        if (elements_count > expansion_threshold)
        {
            expand();
        }
        return true;
    }

    return false;
}

bool BambooFilter::lookup(const std::string &element) const
{
    size_t hash_val = hash(element);
    return lookup_hash(hash_val);
}

bool BambooFilter::lookup_hash(const size_t &hash_val) const
{
    size_t segment_index = get_segment_index(hash_val);
    if (segment_index >= segments.size())
    {
        return false;
    }
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

    if (segment_index >= segments.size())
    {
        std::cerr << "Error: Segment index " << segment_index << " out of bounds (total segments: " << segments.size() << ")" << std::endl;
        return false;
    }

    if (segments[segment_index].remove(hash_val))
    {
        elements_count--;
        std::cout << "Removed element, new count: " << elements_count << std::endl;
        if (elements_count < compression_threshold && segments.size() > 1)
        {
            compress();
        }
        return true;
    }
    return false;
}

void BambooFilter::expand()
{
    std::cout << "Expanding..." << std::endl;
    // Add a new segment
    segments.emplace_back();
    std::cout << "Adding new segment. Total segments before expand: " << segments.size() - 1 << std::endl;

    // Update thresholds
    update_thresholds();
}

void BambooFilter::compress()
{
    if (segments.size() <= 1)
    {
        return; // No compression if only one segment remains
    }

    std::cout << "Compressing..." << std::endl;

    // Collect elements from the current segment for compression
    std::vector<size_t> elements_to_rehash;
    segments.back().collect_elements(elements_to_rehash, current_segment_index);

    // Remove the current segment
    segments.pop_back();
    std::cout << "Last segment removed. Total segments: " << segments.size() << std::endl;

    // Rehash elements into the remaining segments
    elements_count = 0;
    for (auto &element : elements_to_rehash)
    {
        insert_hash(element);
    }

    // Update thresholds
    update_thresholds();
}

void BambooFilter::update_thresholds()
{
    expansion_threshold = segments.size() * segment_size;
    compression_threshold = expansion_threshold / 2;
    std::cout << "Updated thresholds. Expansion threshold: " << expansion_threshold << ", Compression threshold: " << compression_threshold << std::endl;
}

size_t BambooFilter::hash(const std::string &element) const
{
    return std::hash<std::string>()(element);
}

size_t BambooFilter::get_segment_index(size_t hash_value) const
{
    size_t num_segments = segments.size();
    size_t segment_bit_length = 0;

    // Calculate the number of bits needed to represent the number of segments
    while ((1ULL << segment_bit_length) < num_segments)
    {
        ++segment_bit_length;
    }

    // Shift the hash value right by the number of bits for the bucket index
    size_t shifted_hash = hash_value >> kBucketIndexBitLength;

    // Mask for the segment index bits
    size_t mask = (1ULL << segment_bit_length) - 1;

    // Apply the mask to get the segment index
    size_t segment_index = shifted_hash & mask;

    // Ensure the segment index is within bounds
    if (segment_index >= num_segments)
    {
        segment_index = num_segments - 1;
    }

    return segment_index;
}
