#ifndef SEGMENT_H
#define SEGMENT_H

#include <cstdint>
#include <vector>

using namespace std;

class Segment
{
private:
    // two dimensional array with kNumOfBuckets rows and kBucketSize columns; implemented as one dimensional to achieve memory locality
    uint32_t *table_;
    // pointer to this segment's overflow segment
    Segment *overflow_;

public:
    Segment();
    ~Segment();
    bool insert(uint32_t element);
    bool lookup(uint32_t element) const;
    bool remove(uint32_t element);
    void collect_elements(vector<uint32_t> &elements, uint32_t segment_index);
    void clear();
};

#endif