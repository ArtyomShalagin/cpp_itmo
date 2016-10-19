Виртульные функции -- если в предке есть виртуальная функция, то 
при наследовании и после этого касте к предку будет вызываться 
все равно новый метод

статический и динамический типы
статический -- тип переменной, который написан в коде
derived x;
base2* vv = &x;
статический тип у vv -- указатель на base2, а динамический -- указатель на derived

void* можно рассматривать, как самый базовый тип, можно всегда привести к нему и назад


 
struct base {
    int a, b, c;
};

struct derived : base {
    int d, e;
};

void test1() {
    derived d;
    base b = d;
    b.f();
    тут на самом деле base b((base&) d);
}

void test2() { // test2 и test3 делают одно и то же
    derived d;
    base& b = d;
    // b.f()''
}

void test3() {
    derived d;
    base* b = &d;
    // b -> f();®
}


Еще примеров:

struct base1 {
    int a;
    int b;
};

struct base2 {
    int c;
    int d;
};

struct derived : base1, base2 {

};

int main() {
    derived x;
    x.a = 1;
    x.b = 2;
    x.c = 3;
    x.d = 4;

    void* v = &x;
    derived* vv = (derived*) v; // если поменять эти строки местами, не будет работать
    cout << vv -> a << vv -> b << vv -> c << vv -> d << endl;
}

_____________________

struct base {
    virtual ~base() { // деструктор

    }

    virtual void print() = 0; // то же самое, что abstract в жабе
};

struct derived : base {
    derived(std::string const& msg)
        : msg(msg); // вызов конструктора стринга (this.msg = msg) 
    {}

    void print() {
        std::cout << msg << endl;
    }

private:
    std::string msg;
};

struct derived2 : derived {
    ~derived() {
        
    }
};

int main() {
    base* b = new derived("hello");
    b -> print();
    delete b; 
    Вызывает деструктор не того, чего надо, но работает, когда короткие строки, потому что 
    сделано так, что стринг внутри себя хранит не только 
    указатель, но и небольшой буфер, поэтому при вызове деструктора при короткой строке объект 
    удаляется нормально, а если нет, то будет утечка памяти, потому что у того стрига память, 
    занятая под ссылку, не освободится

    Если есть базовый класс, который на самом деле наследник, то для корректного удаления
    надо, чтобы у базового класса был виртуальный деструктор
}

**************

struct base {
    virtual ~base() {
        cout << "base\n";
    }

    base() {
        cout << "base\n";
    }
};

struct derived : base {
    virtual ~derived() {
        cout << "derived\n";
    }

    derived() {
        cout << "derived\n";
    }
};

struct derived2 : derived {
    virtual ~derived2() {
        cout << "derived2\n";
    }

    derived2() {
        cout << "derived2\n";
    }
};

int main() {
    // delete (base*) new derived2();
    // вызывает последовательно деструкторы

    cout << endl;
    derived2 d2;
}

*************

МНОЖЕСТВЕННОЕ НАСЛЕДОВАНИЕ

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
    void on_moved(); 
    Если были protected, а в классе наследование было не private, то можно будет приводить, 
    но нельзя вызывать методы извне
    void on_died();
};

struct game_object_tracker : game_object_observer {
    void track(game_object* target);
    void untrack();
private:
    game_object* tracked;
};
 
struct mob : game_object, private game_object_tracker { 
Наследование может быть private, это значит, что никто не может приводить к gameoo,
кроме самого этого класса. Можно не писать private, если методы в 
game_object_tracker были protected
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
        return v -> secret; 
        Это так не работает, ради этого сделаны такие странные ограничения на protected
    }
};

 
int main() {
    mob m = mob();
    game_object g = game_object();
    cout << m.getA() << g.getA();
}

______________________
 Странноватая хрень

 int main() {
    float y = 1.5f;
    long i = * (long *) &y; // лонг из битовой записи флота
    cout << i << endl;
}























































