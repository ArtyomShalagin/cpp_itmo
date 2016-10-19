#include <iostream>
#include <vector>
#include <list>
#include <set>

using namespace std;

struct node_based {

};

struct array_based {

};

template<typename T>
struct container_traits {
    typedef node_based category;
};

template<typename E>
struct container_traits<vector<E>> {
    typedef array_based category;
};

template<typename E>
struct container_traits<list<E>> {
    typedef node_based category;
};

template<typename E>
struct container_traits<set<E>> {
    typedef node_based category;
};


template<typename C, typename F>
void erase_if(C& c, F pred) {
    erase_if_impl(c, pred, container_traits<C>::category());
};

template<typename C, typename F>
void erase_if_impl(C& c, F pred, node_based) {
    typedef typename C::iterator iter;
    iter curr = c.begin();
    while (curr != c.end()) {
        if (pred(*curr)) {
            curr = c.erase(curr);
        } else {
            curr++;
        }
    }
}

template<typename C, typename F>
void erase_if_impl(C& c, F pred, array_based) {
    typedef typename C::iterator iter;
    c.resize(remove_if(c.begin(), c.end(), pred) - c.begin());
}

int main() {
    vector<int> v;
    return 0;
}