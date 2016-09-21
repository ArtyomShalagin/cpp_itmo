#ifndef BIMAP_BIMAP_H
#define BIMAP_BIMAP_H

#include <string>
#include <iostream>

struct bimap {
    typedef std::string left_t;
    typedef std::string right_t;

    struct left_iterator;
    struct right_iterator;

    bimap();

    // Деструктор. Вызывается при удалении объектов bimap.
    // Инвалидирует все итераторы ссылающиеся на элементы этого bimap
    // (включая итераторы ссылающиеся на элементы следующие за последними).
    ~bimap();

    // Вставка пары (left, right), возвращает итератор на left.
    // Если такой left или такой right уже присутствуют в bimap, вставка не
    // производится и возвращается end_left().
    left_iterator insert(left_t const &left, right_t const &right);

    // Удаляет элемент и соответствующий ему парный.
    // erase невалидного итератора неопределен.
    // erase(end_left()) и erase(end_right()) неопределены.
    // Пусть it ссылается на некоторый элемент e.
    // erase инвалидирует все итераторы ссылающиеся на e и на элемент парный к e.
    void erase(left_iterator it);

    void erase(right_iterator it);

    // Возвращает итератор по элементу. В случае если элемент не найден, возвращает
    // end_left()/end_right() соответственно.
    left_iterator find_left(left_t const &left) const;

    right_iterator find_right(right_t const &right) const;

    // Возващает итератор на минимальный по величине left.
    left_iterator begin_left() const;

    // Возващает итератор на следующий за последним по величине left.
    left_iterator end_left() const;

    // Возващает итератор на минимальный по величине right.
    right_iterator begin_right() const;

    // Возващает итератор на следующий за последним по величине right.
    right_iterator end_right() const;

private:
    struct node {
        node() {
            left_left = nullptr;
            left_right = nullptr;
            left_parent = nullptr;
            right_left = nullptr;
            right_right = nullptr;
            right_parent = nullptr;
        }

        node(left_t left, right_t right) :
                left_data(left), right_data(right) {
            left_left = nullptr;
            left_right = nullptr;
            left_parent = nullptr;
            right_left = nullptr;
            right_right = nullptr;
            right_parent = nullptr;
        }

        ~node() {
            left_left = left_right = left_parent = right_left = right_right = right_parent = nullptr;
        }

        left_t left_data;
        node *left_left;
        node *left_right;
        node *left_parent;

        right_t right_data;
        node *right_left;
        node *right_right;
        node *right_parent;

        void pr() const {
            std::cout << left_data << " ; " << right_data << std::endl;
        }
    };

    node fake_node;

    bool insert_left(node*);
    bool insert_right(node*);
    void remove_left(node const*);
    void remove_right(node const*);

    template<typename T>
    struct data_handler {
        data_handler() {}

        virtual ~data_handler() { }

        virtual T get_data(node *) = 0;

        virtual T const get_data(node const *) = 0;

        virtual node* get_left(node const*) = 0;

        virtual node* get_right(node const*) = 0;

        virtual node *get_parent(node const*) = 0;

        virtual void set_left(node* to, node * from) = 0;

        virtual void set_right(node * to, node * from) = 0;

        virtual void set_parent(node * to, node * from) = 0;

        virtual node const* get_fake() = 0;

        bimap const* map;
        uint8_t id;
    };

    template<typename T>
    bool insert_node_rec(node*, node*, data_handler<T>*);

    template<typename T>
    void remove_node(node const*, data_handler<T>*);

    template<typename T>
    node const* min(node const*, data_handler<T>*) const;

    template<typename T>
    node const* max(node const*, data_handler<T>*) const;

    template<typename T>
    node const* next(node const*, data_handler<T>*) const;

    template<typename T>
    node const* prev(node const*, data_handler<T>*) const;

    template<typename T>
    node const* find(node const*, data_handler<T>*, T const& key) const;

    struct left_data_handler : data_handler<left_t> {
        left_data_handler(bimap const* map) {
            this->map = map;
            id = 0;
        }

        virtual left_t get_data(node *n) {
            return n->left_data;
        }

        virtual left_t const get_data(node const* n) {
            return n->left_data;
        }

        virtual node* get_left(node const* n) {
            return n->left_left;
        }

        virtual node* get_right(node const* n) {
            return n->left_right;
        }

        virtual node* get_parent(node const *n) {
            return n->left_parent;
        }

        virtual void set_left(node *n, node *source) {
            n->left_left = source;
        }

        virtual void set_right(node *n, node *source) {
            n->left_right = source;
        }

        virtual void set_parent(node *n, node *source) {
            n->left_parent = source;
        }

        virtual node const* get_fake() {
            return &(map->fake_node);
        }
    };

    struct right_data_handler : data_handler<right_t> {
        right_data_handler(bimap const* map) {
            this->map = map;
            id = 1;
        }

        virtual right_t get_data(node *n) {
            return n->right_data;
        }

        virtual right_t const get_data(node const* n) {
            return n->right_data;
        }

        virtual node* get_left(node const* n) {
            return n->right_left;
        }

        virtual node* get_right(node const* n) {
            return n->right_right;
        }

        virtual node *get_parent(node const *n) {
            return n->right_parent;
        }

        virtual void set_left(node *n, node *source) {
            n->right_left = source;
        }

        virtual void set_right(node *n, node *source) {
            n->right_right = source;
        }

        virtual void set_parent(node *n, node *source) {
            n->right_parent = source;
        }

        virtual node const* get_fake() {
            return &(map->fake_node);
        }
    };

};

struct bimap::left_iterator {
    friend struct bimap;

    left_iterator(node const*, bimap const*);

    ~left_iterator();

    // Элемент на который сейчас ссылается итератор.
    // Разыменование итератора end_left() неопределено.
    // Разыменование невалидного итератора неопределено.
    left_t const &operator*() const;

    // Переход к следующему по величине left'у.
    // Инкремент итератора end_left() неопределен.
    // Инкремент невалидного итератора неопределен.
    left_iterator &operator++();

    left_iterator operator++(int);

    // Переход к предыдущему по величине left'у.
    // Декремент итератора begin_left() неопределен.
    // Декремент невалидного итератора неопределен.
    left_iterator &operator--();

    left_iterator operator--(int);

    // left_iterator ссылается на левый элемент некоторой пары.
    // Эта функция возвращает итератор на правый элемент той же пары.
    // end_left().flip() возращает end_right().
    // end_right().flip() возвращает end_left().
    // flip() невалидного итератора неопределен.
    right_iterator flip() const;

    bool operator==(const left_iterator) const;
    bool operator!=(const left_iterator) const;

private:
    node const* link;
    bimap const* map;
};

struct bimap::right_iterator {
    friend struct bimap;

    right_iterator(node const*, bimap const*);

    ~right_iterator();

    // Здесь всё аналогично left_iterator.
    right_t const &operator*() const;

    right_iterator &operator++();

    right_iterator operator++(int);

    right_iterator &operator--();

    right_iterator operator--(int);

    left_iterator flip() const;

    bool operator==(const right_iterator) const;
    bool operator!=(const right_iterator) const;

private:
    node const* link;
    bimap const* map;
};


#endif //BIMAP_BIMAP_H