#include <assert.h>
#include "bimap.h"
//#include <iostream>

//#define c std::cout
//#define el std::endl

bimap::left_iterator::left_iterator(bimap::node const *link, bimap const *map) : map(map), link(link) {

}

bimap::left_iterator::~left_iterator() {

}

bimap::left_t const &bimap::left_iterator::operator*() const {
    assert (link != map->end_left().link);
    return link->left_data;
}

bimap::left_iterator &bimap::left_iterator::operator++() {      //++var
    bimap::left_data_handler handler(map);
    bimap::node const *next_node = map->next(link, &handler);
    assert (next_node != nullptr);
    link = next_node;
    return *this;
}

bimap::left_iterator bimap::left_iterator::operator++(int) {    //var++
    bimap::left_iterator result(link, map);
    ++(*this);
    return result;
}

bimap::left_iterator &bimap::left_iterator::operator--() {
    bimap::left_data_handler handler(map);
    bimap::node const *next_node = map->prev(link, &handler);
    assert (next_node != nullptr);
    link = next_node;
    return *this;
}

bimap::left_iterator bimap::left_iterator::operator--(int) {
    bimap::left_iterator result(link, map);
    --(*this);
    return result;
}

bimap::right_iterator bimap::left_iterator::flip() const {
    return bimap::right_iterator(link, map);
}

bool bimap::left_iterator::operator==(const bimap::left_iterator rhs) const {
    return link == rhs.link && map == rhs.map;
}

bool bimap::left_iterator::operator!=(const bimap::left_iterator rhs) const {
    return !(*this == rhs);
}

//______________________________________________________________________________________________________________________
//left iterator ends here
//______________________________________________________________________________________________________________________

bimap::right_iterator::right_iterator(bimap::node const *link, bimap const *map) : map(map), link(link) {

}

bimap::right_iterator::~right_iterator() {

}

bimap::left_t const &bimap::right_iterator::operator*() const {
    assert (link != map->end_right().link);
    return link->right_data;
}

bimap::right_iterator &bimap::right_iterator::operator++() {
    bimap::right_data_handler handler(map);
    bimap::node const *next_node = map->next(link, &handler);
    assert (next_node != nullptr);
    link = next_node;
    return *this;
}

bimap::right_iterator bimap::right_iterator::operator++(int) {
    bimap::right_iterator result(link, map);
    ++(*this);
    return result;
}

bimap::right_iterator &bimap::right_iterator::operator--() {
    bimap::right_data_handler handler(map);
    bimap::node const *next_node = map->prev(link, &handler);
    assert (next_node != nullptr);
    link = next_node;
    return *this;
}

bimap::right_iterator bimap::right_iterator::operator--(int) {
    bimap::right_iterator result(link, map);
    --(*this);
    return result;
}

bimap::left_iterator bimap::right_iterator::flip() const {
    return bimap::left_iterator(link, map);
}

bool bimap::right_iterator::operator==(const bimap::right_iterator rhs) const {
    return link == rhs.link && map == rhs.map;
}

bool bimap::right_iterator::operator!=(const bimap::right_iterator rhs) const {
    return !(*this == rhs);
}

//______________________________________________________________________________________________________________________
//right iterator ends here
//______________________________________________________________________________________________________________________

bimap::bimap() {
//    fake_node.left_data = "left fake";
//    fake_node.right_data = "right fake";
}

bimap::~bimap() {
    left_iterator iter = begin_left();
    while (iter != end_left()) {
        left_iterator iter_delete = iter++;
        erase(iter_delete);
    }
}

template<typename T>
bimap::node const *bimap::min(bimap::node const *curr_node, T *handler) const {
    if (handler->get_left(curr_node) == nullptr) {
        return curr_node;
    }
    return min(handler->get_left(curr_node), handler);
}

template<typename T>
bimap::node const *bimap::max(bimap::node const *node, T *handler) const {
    if (handler->get_right(node) == nullptr) {
        return node;
    }
    return max(handler->get_right(node), handler);
}

template<typename T>
bimap::node const *bimap::next(bimap::node const *curr_node, T *handler) const {
    if (handler->get_right(curr_node) != nullptr) {
        return min(handler->get_right(curr_node), handler);
    }
    bimap::node const *parent = handler->get_parent(curr_node);
    if (parent == nullptr) {
        return nullptr;
    }
    while (parent != &fake_node && curr_node == handler->get_right(parent)) {
        curr_node = parent;
        parent = handler->get_parent(parent);
    }
    return parent;
}

template<typename T>
bimap::node const *bimap::prev(bimap::node const *node, T *handler) const {
    if (handler->get_left(node) != nullptr) {
        return max(handler->get_left(node), handler);
    }
    bimap::node const *parent = handler->get_parent(node);
    if (parent == nullptr) {
        return nullptr;
    }
    while (parent != &fake_node && node == handler->get_left(parent)) {
        node = parent;
        parent = handler->get_parent(parent);
    }
    return parent;
}

bimap::left_iterator bimap::insert(left_t const &left, right_t const &right) {
    node *new_node = new node(left, right);
    bool ok = insert_left(new_node);
    if (ok) {
        ok &= insert_right(new_node);
    }
    if (ok) {
        return bimap::left_iterator(new_node, this);
    } else {
        delete new_node;
        return end_left();
    }
}

template<typename T>
bool bimap::insert_node_rec(bimap::node *new_node, bimap::node *curr_node,
                            T *handler) {
    if (handler->get_data(new_node) == handler->get_data(curr_node) && curr_node != &fake_node) {
        if (handler->id == 1) {
            remove_left(curr_node);
        }
        //need that in case if left insert was already done,
        //but right key already exists. checking that before inserting
        //would take too much time
        return false;
    } else if (handler->get_data(new_node) < handler->get_data(curr_node) || //have to go left
               curr_node == handler->get_fake()) {;
        if (handler->get_left(curr_node) == nullptr) {                       //doesn't have left child
            handler->set_left(curr_node, new_node);
            handler->set_parent(new_node, curr_node);
            return true;
        } else {
            return insert_node_rec(new_node, handler->get_left(curr_node), handler);
        }
    } else {                                                                 //have to go right
        if (handler->get_right(curr_node) == nullptr) {                      //doesn't have right child
            handler->set_right(curr_node, new_node);
            handler->set_parent(new_node, curr_node);
            return true;
        } else {
            return insert_node_rec(new_node, handler->get_right(curr_node), handler);
        }
    }
}

template<typename T>
void bimap::remove_node(bimap::node const *curr_node, T *handler) { //not removing pointers here!!!
    bimap::node *parent = handler->get_parent(curr_node);
    bimap::node *left_child = handler->get_left(curr_node);
    bimap::node *right_child = handler->get_right(curr_node);
    if (left_child == nullptr && right_child == nullptr) {                          //node is leaf
        if (handler->get_left(parent) == curr_node) {                               //is left child
            handler->set_left(parent, nullptr);
        } else {                                                                    //is right child
            handler->set_right(parent, nullptr);
        }
    } else if (left_child == nullptr ^ right_child == nullptr) {                    //has the only child
        bimap::node *child = left_child != nullptr ? left_child : right_child;
        if (handler->get_left(parent) == curr_node) {                               //is left child
            handler->set_left(parent, child);
        } else {                                                                    //is right child
            handler->set_right(parent, child);
        }
        handler->set_parent(child, parent);
    } else {                                                                        //both children exist
        bimap::node *next_node = (node *) next(curr_node, handler);                 //TODO
        remove_node(next_node, handler);                                            //0 or 1 child for sure
        handler->set_parent(next_node, parent);
        if (handler->get_left(parent) == curr_node) {                               //is left child
            handler->set_left(parent, next_node);
        } else {                                                                    //is right child
            handler->set_right(parent, next_node);
        }
        left_child = handler->get_left(curr_node);
        right_child = handler->get_right(curr_node);
        if (left_child != nullptr) {
            handler->set_parent(left_child, next_node);
        }
        if (right_child != nullptr) {
            handler->set_parent(right_child, next_node);
        }
        handler->set_left(next_node, left_child);
        handler->set_right(next_node, right_child);
    }
    //at that point tree is ok but node still has pointers to elements he must not access anymore
}

bool bimap::insert_left(bimap::node *new_node) {
    bimap::left_data_handler handler(this);
    return insert_node_rec(new_node, &fake_node, &handler);
}

bool bimap::insert_right(bimap::node *new_node) {
    bimap::right_data_handler handler(this);
    return insert_node_rec(new_node, &fake_node, &handler);
}

void bimap::remove_left(bimap::node const *node) {
    bimap::left_data_handler handler(this);
    remove_node(node, &handler);
}

void bimap::remove_right(bimap::node const *node) {
    bimap::right_data_handler handler(this);
    remove_node(node, &handler);
}

void bimap::erase(bimap::left_iterator it) {
    bimap::node const *deleted_node = it.link;
    remove_left(deleted_node);
    remove_right(deleted_node);
    delete deleted_node;
}

void bimap::erase(bimap::right_iterator it) {
    bimap::node const *deleted_node = it.link;
    remove_left(deleted_node);
    remove_right(deleted_node);
    delete deleted_node;
}

bimap::left_iterator bimap::find_left(left_t const &left) const {
    bimap::left_data_handler handler(this);
    bimap::node const *result = find(&fake_node, &handler, left);
    if (result == nullptr) {
        return end_left();
    } else {
        return bimap::left_iterator(result, this);
    }
}

bimap::right_iterator bimap::find_right(right_t const &right) const {
    bimap::right_data_handler handler(this);
    bimap::node const *result = find(&fake_node, &handler, right);
    if (result == nullptr) {
        return end_right();
    } else {
        return bimap::right_iterator(result, this);
    }
}

template<typename T, typename U>
bimap::node const *bimap::find(bimap::node const *curr_node,
                               T *handler, U const &key) const {
    if (handler->get_data(curr_node) == key && curr_node != &fake_node) {
        return curr_node;
    }
    if (curr_node == nullptr) {
        return nullptr;
    }
    if (key < handler->get_data(curr_node) || curr_node == &fake_node) {
        return find(handler->get_left(curr_node), handler, key);
    } else {
        return find(handler->get_right(curr_node), handler, key);
    }
}

bimap::left_iterator bimap::begin_left() const {
    bimap::left_data_handler handler(this);
    bimap::node const *min_node = bimap::min(&fake_node, &handler);
    return bimap::left_iterator(min_node, this);
}

bimap::left_iterator bimap::end_left() const {
    return bimap::left_iterator(&fake_node, this);
}

bimap::right_iterator bimap::begin_right() const {
    bimap::right_data_handler handler(this);
    bimap::node const *min_node = bimap::min(&fake_node, &handler);
    return bimap::right_iterator(min_node, this);
}

bimap::right_iterator bimap::end_right() const {
    return bimap::right_iterator(&fake_node, this);
}