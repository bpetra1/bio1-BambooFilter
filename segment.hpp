#ifndef SEGMENT_H
#define SEGMENT_H

#include <cstdint>
#include <vector>

using namespace std;

class Segment
{
private:
    // two dimensional array with kNumOfBuckets rows and kBucketSize columns; implemented as one dimensional to achieve memory locality
    size_t *table_;
    // pointer to this segment's overflow segment
    Segment *overflow_;

public:
    Segment();
    ~Segment();
    bool insert(size_t element);
    bool lookup(size_t element) const;
    bool remove(size_t element);
    void collect_elements(vector<size_t> &elements, size_t segment_index) const;
    void clear();
};

#endif