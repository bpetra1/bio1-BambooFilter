#include "bamboo_filter.hpp"
#include "constants.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <cmath>

BambooFilter::BambooFilter()
    : elements_count_(0), current_segment_index_(0), expansion_round_(0),
      expansion_threshold_(2 * kNumOfBuckets), compression_threshold_(0)
{
    srand(time(0));
    segments_.reserve(kInitialNumOfSegments);
    for (uint32_t i = 0; i < kInitialNumOfSegments; ++i)
    {
        segments_.push_back(new Segment());
    }
    std::cout << "BambooFilter initialized with capacity: " << kInitialNumOfSegments * kNumOfBuckets * kBucketSize << " and segment size: " << kNumOfBuckets * kBucketSize << std::endl;
}

BambooFilter::~BambooFilter()
{
    for (auto &segment : segments_)
    {
        delete segment;
    }
    segments_.clear();
}

bool BambooFilter::insert(const std::string &element)
{
    uint32_t hash_val = hash(element);
    return insert_hash(hash_val);
}

bool BambooFilter::insert_hash(const uint32_t &hash_val)
{
    uint32_t segment_index = get_segment_index(hash_val);

    if (segment_index >= segments_.size())
    {
        std::cerr << "Error: Segment index " << segment_index << " out of bounds (total segments: " << segments_.size() << ")" << std::endl;
        return false;
    }

    if (segments_[segment_index]->insert(hash_val))
    {
        elements_count_++;
        if (elements_count_ > expansion_threshold_)
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
    if (segment_index >= segments_.size())
    {
        return false;
    }
    return segments_[segment_index]->lookup(hash_val);
}

bool BambooFilter::remove(const std::string &element)
{
    uint32_t hash_val = hash(element);
    return remove_hash(hash_val);
}

bool BambooFilter::remove_hash(const uint32_t &hash_val)
{
    uint32_t segment_index = get_segment_index(hash_val);

    if (segment_index >= segments_.size())
    {
        std::cerr << "Error: Segment index " << segment_index << " out of bounds (total segments: " << segments_.size() << ")" << std::endl;
        return false;
    }

    if (segments_[segment_index]->remove(hash_val))
    {
        elements_count_--;
        if (elements_count_ < compression_threshold_ && segments_.size() > 1)
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
    segments_.push_back(new Segment());

    // Rehash elements only from the current segment
    std::vector<uint32_t> elements_to_rehash;
    segments_[current_segment_index_]->collect_elements(elements_to_rehash, current_segment_index_);
    elements_count_ -= elements_to_rehash.size();

    // Update thresholds
    compression_threshold_ = expansion_threshold_;
    expansion_threshold_ += 2 * kNumOfBuckets;

    // Move to the next segment for expansion
    current_segment_index_++;
    if (current_segment_index_ * 2 == segments_.size())
    {
        // This round is over
        expansion_round_++;
        current_segment_index_ = 0;
    }

    for (auto &element : elements_to_rehash)
    {
        insert_hash(element);
    }
}

void BambooFilter::compress()
{
    if (segments_.size() <= 1)
    {
        return; // No compression if only one segment remains
    }

    // Collect elements from the current last segment for compression
    std::vector<uint32_t> elements_to_rehash;
    segments_.back()->collect_elements(elements_to_rehash, current_segment_index_);
    elements_count_ -= elements_to_rehash.size();

    // Remove the last segment
    delete segments_.back();
    segments_.pop_back();

    // Update thresholds
    expansion_threshold_ = compression_threshold_;
    compression_threshold_ -= 2 * kNumOfBuckets;

    // Update the current segment index for extension
    current_segment_index_--;
    if (current_segment_index_ < 0)
    {
        expansion_round_--;
        current_segment_index_ = segments_.size() / 2;
    }

    // Rehash elements into the remaining segments
    for (auto &element : elements_to_rehash)
    {
        insert_hash(element);
    }
}

uint32_t BambooFilter::hash(const std::string &element) const
{
    return std::hash<std::string>()(element);
}

uint32_t BambooFilter::get_segment_index(uint32_t hash_value) const
{
    uint32_t segment_bit_length = kInitialSegmentIndexBitLength + expansion_round_ + (current_segment_index_ != 0);

    // Shift the hash value right by the number of bits for the bucket index
    uint32_t shifted_hash = hash_value >> kBucketIndexBitLength;

    // Mask for the segment index bits
    uint32_t mask = (1ULL << segment_bit_length) - 1;

    // Apply the mask to get the segment index
    uint32_t segment_index = shifted_hash & mask;

    if (segment_index >= segments_.size())
    {
        // We need to exclude left-most segment bit
        --segment_bit_length;
        mask = (1ULL << segment_bit_length) - 1;
        segment_index = shifted_hash & mask;
    }
    return segment_index;
}
