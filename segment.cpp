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
    if (overflow_ != nullptr) {
        delete overflow_;
    }
}

bool Segment::insert(uint32_t element)
{
    // calculate fingerprint and buckets for this element
    uint32_t fingerprint = element >> (kBucketIndexBitLength + kInitialSegmentIndexBitLength);
    uint32_t i1 = element % kNumOfBuckets;
    uint32_t i2 = (i1 ^ fingerprint) % kNumOfBuckets;

    // try to put the element in the first bucket
    for (int j = 0; j < kBucketSize; j++) {
        if (table_[i1 * kNumOfBuckets + j] == 0) {
            table_[i1 * kNumOfBuckets + j] = element;
            return true;
        }
    }

    // try the second bucket
    for (int j = 0; j < kBucketSize; j++) {
        if (table_[i2 * kNumOfBuckets + j] == 0) {
            table_[i2 * kNumOfBuckets + j] = element;
            return true;
        }
    }

    // both buckets are full, we need to kick out a random element from one of the buckets
    default_random_engine generator;
    uniform_int_distribution<int> distribution(0, 1);
    uint32_t i = distribution(generator) == 0 ? i1 : i2;

    // distribution for selecting random entries in bucket
    distribution = uniform_int_distribution<int>(0, kBucketSize);

    for (int n = 0; n < kMaxNumKicks; n++) {
        // get random entry
        uint32_t e = table_[i * kNumOfBuckets + distribution(generator)];

        // put fingerprint in that spot
        table_[i * kNumOfBuckets + distribution(generator)] = element;
        element = e;
        fingerprint = element >> (kBucketIndexBitLength + kInitialSegmentIndexBitLength);

        // calculate alternative bucket of element that was kicked out and try to put it there
        i = (i ^ fingerprint) % kNumOfBuckets;
        for (int j = 0; j < kBucketSize; j++) {
            if (table_[i * kNumOfBuckets + j] == 0) {
                table_[i * kNumOfBuckets + j] = element;
                return true;
            }
        }
    }

    // store in overflow segment
    if (overflow_ == nullptr) {
        overflow_ = new Segment();
    }
    return overflow_->insert(element);

    // table is full
    return false;
}

bool Segment::lookup(uint32_t element)
{
    // calculate fingerprint and buckets for this element
    uint32_t fingerprint = element >> (kBucketIndexBitLength + kInitialSegmentIndexBitLength);
    uint32_t i1 = element % kNumOfBuckets;
    uint32_t i2 = (i1 ^ fingerprint) % kNumOfBuckets;

    // try to find the element in the first bucket
    for (int j = 0; j < kBucketSize; j++) {
        if (table_[i1 * kNumOfBuckets + j] == element) {
            return true;
        }
    }

    // try the second bucket
    for (int j = 0; j < kBucketSize; j++) {
        if (table_[i2 * kNumOfBuckets + j] == element) {
            return true;
        }
    }
    
    // try the overflow segment
    if (overflow_ != nullptr) {
        return overflow_->lookup(element);
    }

    // element is not present
    return false;
}

bool Segment::remove(uint32_t element)
{
    // calculate fingerprint and buckets for this element
    uint32_t fingerprint = element >> (kBucketIndexBitLength + kInitialSegmentIndexBitLength);
    uint32_t i1 = element % kNumOfBuckets;
    uint32_t i2 = (i1 ^ fingerprint) % kNumOfBuckets;

    // try to find the element in the first bucket
    for (int j = 0; j < kBucketSize; j++) {
        if (table_[i1 * kNumOfBuckets + j] == element) {
            table_[i1 * kNumOfBuckets + j] = 0;
            return true;
        }
    }

    // try the second bucket
    for (int j = 0; j < kBucketSize; j++) {
        if (table_[i2 * kNumOfBuckets + j] == element) {
            table_[i2 * kNumOfBuckets + j] = 0;
            return true;
        }
    }
    
    // try the overflow segment
    if (overflow_ != nullptr) {
        return overflow_->remove(element);
    }

    // element was not present
    return false;
}
