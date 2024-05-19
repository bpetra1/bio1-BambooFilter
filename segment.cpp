#include "segment.hpp"
#include "constants.hpp"
#include <functional>
#include <random>

using namespace std;

Segment::Segment() : overflow_(nullptr)
{
    this->table_ = new uint32_t[kNumOfBuckets * kBucketSize];
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
    return element >> (kBucketIndexBitLength + kInitialSegmentIndexBitLength) | INT32_MIN;
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
    default_random_engine generator;
    uniform_int_distribution<int> distribution(0, 1);
    uint32_t i = distribution(generator) == 0 ? i1 : i2;

    // distribution for selecting random entries in bucket
    distribution = uniform_int_distribution<int>(0, kBucketSize);

    for (int n = 0; n < kMaxNumKicks; n++)
    {
        // get random entry
        uint32_t e = table_[i * kBucketSize + distribution(generator)];

        // put fingerprint in that spot
        table_[i * kBucketSize + distribution(generator)] = fingerprint;
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
    return overflow_->insert(element);

    // table is full
    return false;
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

void Segment::collect_elements(std::vector<uint32_t> &elements, size_t segment_index) const
{
    for (int i = 0; i < kNumOfBuckets; i++)
    {
        for (int j = 0; j < kBucketSize; j++)
        if (table_[i * kBucketSize + j] != 0)
        {
            uint32_t element = table_[i * kBucketSize + j] != 0;
            element <<= kBucketIndexBitLength + kInitialSegmentIndexBitLength;
            element |= segment_index << kBucketIndexBitLength;
            element |= i;
            elements.push_back(element);
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
