#ifndef A2DD_H
#define A2DD_H

class cuckoo_filter
{
private:
    int capacity;
    int bucket_size;

public:
    cuckoo_filter(/* args */);
    ~cuckoo_filter();
};

#endif