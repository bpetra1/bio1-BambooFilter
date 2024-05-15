#include <iostream>
#include "segment.hpp"

using namespace std;

int main() {
    Segment* s = new Segment();
    s->insert(2);
    s->insert(3);
    s->insert(4);
    cout << s->lookup(1) << '\n';
    cout << s->lookup(2) << '\n';
    cout << s->lookup(4) << '\n';
    s->remove(4);
    cout << s->lookup(4) << '\n';
    delete s;
}