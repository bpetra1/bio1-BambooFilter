#ifndef BAMBOO_FILTER_H
#define BAMBOO_FILTER_H

#include "segment.hpp"
#include <vector>
#include <string>

class BambooFilter
{
public:
    BambooFilter(size_t initial_capacity, size_t segment_size);
    ~BambooFilter(); // Declaration of destructor

    bool insert(const std::string &element);
    bool lookup(const std::string &element) const;
    bool remove(const std::string &element);

private:
    std::vector<Segment> segments;
    size_t segment_size;
    size_t elements_count;
    size_t expansion_threshold;
    size_t compression_threshold;

    size_t hash(const std::string &element) const;
    size_t get_segment_index(size_t hash_value) const;

    bool insert_hash(const size_t &hash_val);
    bool lookup_hash(const size_t &hash_val) const;
    bool remove_hash(const size_t &hash_val);
    void expand();
    void compress();
};

#endif // BAMBOO_FILTER_H
