#include "bamboo_filter.hpp"
#include "constants.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <cmath>

BambooFilter::BambooFilter()
    : elements_count(0), current_segment_index(0),
      expansion_threshold(2 * kNumOfBuckets), compression_threshold(0)
{
    srand(time(0));
    segments.reserve(kInitialNumOfSegments);
    for (uint32_t i = 0; i < kInitialNumOfSegments; ++i)
    {
        segments.push_back(new Segment());
    }
    std::cout << "BambooFilter initialized with capacity: " << kInitialNumOfSegments * kNumOfBuckets * kBucketSize << " and segment size: " << kNumOfBuckets * kBucketSize << std::endl;
}

BambooFilter::~BambooFilter()
{
    for (auto &segment : segments)
    {
        delete segment;
    }
    segments.clear();
}

bool BambooFilter::insert(const std::string &element)
{
    uint32_t hash_val = hash(element);
    return insert_hash(hash_val);
}

bool BambooFilter::insert_hash(const uint32_t &hash_val)
{
    uint32_t segment_index = get_segment_index(hash_val);

    if (segment_index >= segments.size())
    {
        std::cerr << "Error: Segment index " << segment_index << " out of bounds (total segments: " << segments.size() << ")" << std::endl;
        return false;
    }

    if (segments[segment_index]->insert(hash_val))
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
    uint32_t hash_val = hash(element);
    return lookup_hash(hash_val);
}

bool BambooFilter::lookup_hash(const uint32_t &hash_val) const
{
    uint32_t segment_index = get_segment_index(hash_val);
    if (segment_index >= segments.size())
    {
        return false;
    }
    return segments[segment_index]->lookup(hash_val);
}

bool BambooFilter::remove(const std::string &element)
{
    uint32_t hash_val = hash(element);
    return remove_hash(hash_val);
}

bool BambooFilter::remove_hash(const uint32_t &hash_val)
{
    uint32_t segment_index = get_segment_index(hash_val);

    if (segment_index >= segments.size())
    {
        std::cerr << "Error: Segment index " << segment_index << " out of bounds (total segments: " << segments.size() << ")" << std::endl;
        return false;
    }

    if (segments[segment_index]->remove(hash_val))
    {
        elements_count--;
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
    // Add a new segment
    segments.push_back(new Segment());

    // Rehash elements only from the current segment
    std::vector<uint32_t> elements_to_rehash;
    segments[current_segment_index]->collect_elements(elements_to_rehash, current_segment_index);
    segments[current_segment_index]->clear();
    elements_count -= elements_to_rehash.size();

    // Update thresholds
    compression_threshold = expansion_threshold;
    expansion_threshold += 2 * kNumOfBuckets;

    for (auto &element : elements_to_rehash)
    {
        insert_hash(element);
    }

    // Move to the next segment for expansion
    current_segment_index++;
    if (current_segment_index * 2 == segments.size())
    {
        // This round is over
        current_segment_index = 0;
    }
}

void BambooFilter::compress()
{
    if (segments.size() <= 1)
    {
        return; // No compression if only one segment remains
    }

    // Collect elements from the current last segment for compression
    std::vector<uint32_t> elements_to_rehash;
    segments.back()->collect_elements(elements_to_rehash, current_segment_index);
    elements_count -= elements_to_rehash.size();

    // Remove the last segment
    delete segments.back();
    segments.pop_back();

    // Update thresholds
    expansion_threshold = compression_threshold;
    compression_threshold -= 2 * kNumOfBuckets;

    // Rehash elements into the remaining segments
    for (auto &element : elements_to_rehash)
    {
        insert_hash(element);
    }

    // Update the current segment index for extension
    current_segment_index--;
    if (current_segment_index < 0)
    {
        current_segment_index = segments.size() / 2;
    }
}

uint32_t BambooFilter::hash(const std::string &element) const
{
    return std::hash<std::string>()(element);
}

uint32_t BambooFilter::get_segment_index(uint32_t hash_value) const
{
    uint32_t num_segments = segments.size();
    uint32_t segment_bit_length = kInitialSegmentIndexBitLength;

    // Calculate the number of bits needed to represent the number of segments
    while ((1ULL << segment_bit_length) < num_segments)
    {
        ++segment_bit_length;
    }

    // Shift the hash value right by the number of bits for the bucket index
    uint32_t shifted_hash = hash_value >> kBucketIndexBitLength;

    // Mask for the segment index bits
    uint32_t mask = (1ULL << segment_bit_length) - 1;

    // Apply the mask to get the segment index
    uint32_t segment_index = shifted_hash & mask;

    if (segment_index >= num_segments)
    {
        // We need to exclude left-most segment bit
        --segment_bit_length;
        mask = (1ULL << segment_bit_length) - 1;
        segment_index = shifted_hash & mask;
    }
    return segment_index;
}
