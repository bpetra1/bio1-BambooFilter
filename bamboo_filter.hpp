#ifndef BAMBOO_FILTER_H
#define BAMBOO_FILTER_H

#include "segment.hpp"
#include <vector>

class BambooFilter
{
public:
    BambooFilter(size_t initial_capacity, size_t segment_size);
    ~BambooFilter(); // Deklaracija destruktora

    bool insert(const uint32_t &element);
    bool lookup(const uint32_t &element) const;
    bool remove(const uint32_t &element);
    void expand();
    void compress();

private:
    std::vector<Segment> segments;
    size_t segment_size;
    size_t elements_count;
    size_t expansion_threshold;
    size_t compression_threshold;

    size_t hash(const uint32_t &element) const;
    size_t get_segment_index(size_t hash_value) const;
};

#endif // BAMBOO_FILTER_H
