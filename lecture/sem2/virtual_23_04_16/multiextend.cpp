#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
 
using namespace std;
 
const int MAXN = 120;
typedef long long LL;
 
struct game_object {
protected:
    int a = 4;
public:
    int getA() {
        return a;
    }
    void subscribe(game_object_observer*); 
    void unsubscribe(game_object_obserber*);
};
 
struct player : game_object {
};

struct game_object_observer {
    void on_moved(); // если были protected, а в классе наследование было не private, то можно будет приводить, но нельзя вызывать методы извне
    void on_died();
};

struct game_object_tracker : game_object_observer {
    void track(game_object* target);
    void untrack();
private:
    game_object* tracked;
};
 
struct mob : game_object, private game_object_tracker { // наследование может быть private, это значит, что никто не может приводить к gameoo,
                                                        // кроме самого этого класса. Можно не писать private, если методы в game_object_tracker были protected
    int getA() {
        return a + 2;
    }
private:
    void track(game_object* obj) {
        assert(!tracked)
        obj -> subscribe(this); // тут this кастуется
        tracked = obj;
    }
    void untrack() {
        tracked -> unsubscribe(this);
        tracked = nullptr;
    }
private:
    game_object* tracked;
};

// еще примеры
struct base {
private: // только из этого класса
    int a;
protected: // из этого класса + в наследниках только от this
    int b;

    void f(base* other) {
        a = 5;
        other -> a = 6;
    }
};

struct derived : x {
    void f(derived* other) {
        b = 5; // ok
        other -> b = 6; // err
    }
};

// больше примеров богу примеров
struct vault {
protected:
    int secret;
};

struct x : valut {
    int get_secret(valut* v) {
        return v -> secret; // это так не работает, ради этого сделаны такие странные ограничения на protected
    }
};

 
int main() {
    mob m = mob();
    game_object g = game_object();
    cout << m.getA() << g.getA();
}