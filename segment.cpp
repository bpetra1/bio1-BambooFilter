#include "segment.hpp"
#include "constants.hpp"
#include <functional>
#include <random>
#include <iostream>

Segment::Segment() : overflow_(nullptr)
{
    table_ = new uint32_t[kNumOfBuckets * kBucketSize];
    for (int i = 0; i < kNumOfBuckets * kBucketSize; i++)
    {
        table_[i] = 0;
    }
}

Segment::~Segment()
{
    delete[] table_;
    if (overflow_ != nullptr)
    {
        delete overflow_;
    }
}

inline uint32_t calclulate_fingerprint(uint32_t element)
{
    // left-most bit set to 1 to indicate that bucket entry is not empty
    uint32_t bitmask = ((uint32_t)-1 >> 1) + 1;
    return element >> (kBucketIndexBitLength + kInitialSegmentIndexBitLength) | bitmask;
}

bool Segment::insert(uint32_t element)
{
    // calculate fingerprint and buckets for this element
    uint32_t fingerprint = calclulate_fingerprint(element);
    uint32_t i1 = element % kNumOfBuckets;
    uint32_t i2 = (i1 ^ fingerprint) % kNumOfBuckets;

    // try to put the element in the first bucket
    for (int j = 0; j < kBucketSize; j++)
    {
        if (table_[i1 * kBucketSize + j] == 0)
        {
            table_[i1 * kBucketSize + j] = fingerprint;
            return true;
        }
    }

    // try the second bucket
    for (int j = 0; j < kBucketSize; j++)
    {
        if (table_[i2 * kBucketSize + j] == 0)
        {
            table_[i2 * kBucketSize + j] = fingerprint;
            return true;
        }
    }

    // both buckets are full, we need to kick out a random element from one of the buckets
    uint32_t i = rand() & 1 == 0 ? i1 : i2;

    for (int n = 0; n < kMaxNumKicks; n++)
    {
        // get random entry
        int bucket = rand() % kBucketSize;
        uint32_t e = table_[i * kBucketSize + bucket];

        // put fingerprint in that spot
        table_[i * kBucketSize + bucket] = fingerprint;
        fingerprint = e;

        // calculate alternative bucket of element that was kicked out and try to put it there
        i = (i ^ fingerprint) % kNumOfBuckets;
        for (int j = 0; j < kBucketSize; j++)
        {
            if (table_[i * kBucketSize + j] == 0)
            {
                table_[i * kBucketSize + j] = fingerprint;
                return true;
            }
        }
    }

    // store in overflow segment
    if (overflow_ == nullptr)
    {
        overflow_ = new Segment();
    }
    // note: new_element doesn't have segment index bits set, but they are not important anymore
    uint32_t new_element = fingerprint << (kInitialSegmentIndexBitLength + kBucketIndexBitLength) | i;
    return overflow_->insert(new_element);
}

bool Segment::lookup(uint32_t element) const
{
    // calculate fingerprint and buckets for this element
    uint32_t fingerprint = calclulate_fingerprint(element);
    uint32_t i1 = element % kNumOfBuckets;
    uint32_t i2 = (i1 ^ fingerprint) % kNumOfBuckets;

    // try to find the element in the first bucket
    for (int j = 0; j < kBucketSize; j++)
    {
        if (table_[i1 * kBucketSize + j] == fingerprint)
        {
            return true;
        }
    }

    // try the second bucket
    for (int j = 0; j < kBucketSize; j++)
    {
        if (table_[i2 * kBucketSize + j] == fingerprint)
        {
            return true;
        }
    }

    // try the overflow segment
    if (overflow_ != nullptr)
    {
        return overflow_->lookup(element);
    }

    // element is not present
    return false;
}

bool Segment::remove(uint32_t element)
{
    // calculate fingerprint and buckets for this element
    uint32_t fingerprint = calclulate_fingerprint(element);
    uint32_t i1 = element % kNumOfBuckets;
    uint32_t i2 = (i1 ^ fingerprint) % kNumOfBuckets;

    // try to find the element in the first bucket
    for (int j = 0; j < kBucketSize; j++)
    {
        if (table_[i1 * kBucketSize + j] == fingerprint)
        {
            table_[i1 * kBucketSize + j] = 0;
            return true;
        }
    }

    // try the second bucket
    for (int j = 0; j < kBucketSize; j++)
    {
        if (table_[i2 * kBucketSize + j] == fingerprint)
        {
            table_[i2 * kBucketSize + j] = 0;
            return true;
        }
    }

    // try the overflow segment
    if (overflow_ != nullptr)
    {
        return overflow_->remove(element);
    }

    // element was not present
    return false;
}

void Segment::collect_elements(std::vector<uint32_t> &elements, uint32_t segment_index) const
{
    for (int i = 0; i < kNumOfBuckets; i++)
    {
        for (int j = 0; j < kBucketSize; j++)
        {
            if (table_[i * kBucketSize + j] != 0)
            {
                uint32_t element = table_[i * kBucketSize + j] & ~(((uint32_t)-1 >> 1) + 1); // remove the left-most bit set to 1
                element <<= (kBucketIndexBitLength + kInitialSegmentIndexBitLength);
                element |= (segment_index << kBucketIndexBitLength);
                element |= i;
                elements.push_back(element);
            }
        }
    }
    if (overflow_ != nullptr)
    {
        overflow_->collect_elements(elements, segment_index);
    }
}

void Segment::clear()
{
    for (int i = 0; i < kNumOfBuckets * kBucketSize; ++i)
    {
        table_[i] = 0;
    }
    if (overflow_ != nullptr)
    {
        overflow_->clear();
    }
}
