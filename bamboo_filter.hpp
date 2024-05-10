#ifndef BAMBOO_FILTER_H
#define BAMBOO_FILTER_H

#include "cuckoo_filter.hpp"
#include <vector>

class BambooFilter
{
public:
    BambooFilter(size_t initial_capacity, size_t segment_size);
    ~BambooFilter();

    bool insert(const int &element);
    bool lookup(const int &element) const;
    bool remove(const int &element);
    void expand();

private:
    struct Segment
    {
        CuckooFilter cuckoo; // Cuckoo filter used as segment
    };

    std::vector<Segment> segments;
    size_t segment_size;

    size_t hash(const int &element) const;
    size_t get_segment_index(size_t hash_value) const;
};

#endif
