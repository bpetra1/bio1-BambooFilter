#ifndef SEGMENT_H
#define SEGMENT_H

#include <cstdint>

class Segment
{
private:
    // two dimensional array with kNumOfBuckets rows and kBucketSize columns; implemented as one dimensional to achieve memory locality
    uint32_t* table_;
    // pointer to this segment's overflow segment
    Segment* overflow_;
public:
    Segment();
    ~Segment();
    bool insert(uint32_t element);
    bool lookup(uint32_t element);
    bool remove(uint32_t element);
};

#endif