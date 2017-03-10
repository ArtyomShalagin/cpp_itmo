// notes made while preparing to exam

// example of harmful move optimization

struct finalizer {
    const std::string& value;

    finalizer(std::string& value) : value(value) {
        cout << "constr, value = \"" << this->value << "\"" << endl;
    }

    ~finalizer() {
        cout << "destr, value  = \"" << value << "\"" << endl;
    }
};

std::string test() {
    if (false)
        return std::string("this will never happen, needed to disable NRVO");

    std::string s = "hello";
    finalizer fin(s);
    return s;
}

int main() {
    test();
}

// output will be "destr, value = "" " because the order of things happening 
// will be do return - call destructors - return from function. s will be moved 
// to result before destructor call and will be empty in destructor

// copy elision = RVO + NRVO, -fno-elide-constructors to disable it

// std::move is a function that turns its argument into an rvalue without 
// doing anything else

// void foo(int& val) can be called with lvalue arg only
// void foo(int const& val) accepts rvalue