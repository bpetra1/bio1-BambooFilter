#include <iostream>
#include "bamboo_filter.hpp"

using namespace std;

void test_insert_and_lookup(BambooFilter &filter, uint32_t element)
{
    filter.insert(element);
    cout << "Inserted " << element << ": " << filter.lookup(element) << '\n';
}

void test_remove(BambooFilter &filter, uint32_t element)
{
    filter.remove(element);
    cout << "Removed " << element << ": " << !filter.lookup(element) << '\n';
}

int main()
{
    size_t initial_capacity = 16;
    size_t segment_size = 4;

    BambooFilter filter(initial_capacity, segment_size);

    cout << "Testing Bamboo Filter:" << endl;

    test_insert_and_lookup(filter, 2);  // 1
    test_insert_and_lookup(filter, 3);  // 1
    test_insert_and_lookup(filter, 4);  // 1
    test_insert_and_lookup(filter, 17); // 1

    cout << "Lookup 1: " << filter.lookup(1) << '\n'; // 0
    cout << "Lookup 4: " << filter.lookup(4) << '\n'; // 1

    test_remove(filter, 4);                                         // 1
    cout << "Lookup 4 after removal: " << filter.lookup(4) << '\n'; // 0

    cout << "Lookup 2: " << filter.lookup(2) << '\n';   // 1
    cout << "Lookup 3: " << filter.lookup(3) << '\n';   // 1
    cout << "Lookup 17: " << filter.lookup(17) << '\n'; // 1

    filter.remove(2);
    filter.remove(3);
    filter.remove(17);
    cout << "After remove:" << endl;
    cout << "Lookup 2: " << filter.lookup(2) << '\n';   // 0
    cout << "Lookup 3: " << filter.lookup(3) << '\n';   // 0
    cout << "Lookup 17: " << filter.lookup(17) << '\n'; // 0

    return 0;
}
