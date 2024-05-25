#include <iostream>
#include "bamboo_filter.hpp"

using namespace std;

void test_insert_and_lookup(BambooFilter &filter, const std::string &element)
{
    filter.insert(element);
    cout << "Inserted " << element << ": " << filter.lookup(element) << '\n';
}

void test_remove(BambooFilter &filter, const std::string &element)
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

    test_insert_and_lookup(filter, "ABCDEFGHIJKLMNOPQRS"); // 1
    test_insert_and_lookup(filter, "T");                   // 1
    test_insert_and_lookup(filter, "C");                   // 1
    test_insert_and_lookup(filter, "G");                   // 1
    test_insert_and_lookup(filter, "I");                   // 1
    test_insert_and_lookup(filter, "J");                   // 1

    cout << "Lookup 'ABCDEFGHIJ': " << filter.lookup("ABCDEFGHIJKLMNOPQRS") << '\n'; // 1
    cout << "Lookup 'F': " << filter.lookup("F") << '\n';                            // 0
    cout << "Lookup 'J': " << filter.lookup("J") << '\n';                            // 0

    test_remove(filter, "C");                                           // 1
    cout << "Lookup 'C' after removal: " << filter.lookup("C") << '\n'; // 0

    cout << "Lookup 'ABCDEFGHIJ': " << filter.lookup("ABCDEFGHIJKLMNOPQRS") << '\n'; // 1
    cout << "Lookup 'T': " << filter.lookup("T") << '\n';                            // 1
    cout << "Lookup 'G': " << filter.lookup("G") << '\n';                            // 1

    filter.remove("ABCDEFGHIJKLMNOPQRS");
    filter.remove("T");
    filter.remove("G");
    cout << "After remove:" << endl;
    cout << "Lookup 'ABCDEFGHIJ': " << filter.lookup("ABCDEFGHIJKLMNOPQRS") << '\n'; // 0
    cout << "Lookup 'T': " << filter.lookup("T") << '\n';                            // 0
    cout << "Lookup 'G': " << filter.lookup("G") << '\n';                            // 0

    return 0;
}
