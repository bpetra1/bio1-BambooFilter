#ifndef BAMBOO_FILTER_HPP
#define BAMBOO_FILTER_HPP

#include "segment.hpp"
#include <vector>
#include <string>

class BambooFilter
{
public:
    BambooFilter(size_t initial_capacity, size_t seg_size);
    ~BambooFilter();

    bool insert(const std::string &element);
    bool lookup(const std::string &element) const;
    bool remove(const std::string &element);

private:
    size_t hash(const std::string &element) const;
    size_t get_segment_index(size_t hash_value) const;
    bool insert_hash(const size_t &hash_val);
    bool lookup_hash(const size_t &hash_val) const;
    bool remove_hash(const size_t &hash_val);

    void expand();
    void compress();
    void update_thresholds();

    size_t segment_size;
    size_t elements_count;
    size_t current_segment_index;
    size_t expansion_threshold;
    size_t compression_threshold;

    std::vector<Segment> segments;
};

#endif // BAMBOO_FILTER_HPP
