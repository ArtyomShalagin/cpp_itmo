#ifndef BIND_H
#define BIND_H

#include <iostream>
#include <tuple>

template<size_t N>
struct ph_t {
};

namespace ph {
    ph_t<1> _1; ph_t<2> _2; ph_t<3> _3; ph_t<4> _4; ph_t<5> _5;
    ph_t<6> _6; ph_t<7> _7; ph_t<8> _8; ph_t<9> _9; ph_t<10> _10;
}

template<size_t... N>
struct variadic_range { };

template<size_t N, size_t... Curr>
struct range_builder {
    typedef typename range_builder<N - 1, N - 1, Curr...>::range range;
};

template<size_t... Curr>
struct range_builder<0, Curr...> {
    typedef variadic_range<Curr...> range;
};

template<typename Func, typename... Args>
struct bind_t {
//    typedef std::tuple<typename std::decay<Args>::type...> args_t;
    typedef std::tuple<typename std::remove_reference<Args>::type...> args_t;
//    typedef std::tuple<Args...> args_t;

    Func func;
    args_t args;

    bind_t(Func &&func, Args &&... args) :
            func(std::forward<Func>(func)), args(std::forward<Args>(args)...) { }

    auto get_range() {
        return typename range_builder<std::tuple_size<args_t>::value>::range();
    }

    template<typename Arg, typename... ReducedArgs>
    auto &get_arg(Arg &arg, ReducedArgs &&... reduced_args) {
        return arg;
    };

    template<size_t N, typename... ReducedArgs>
    decltype(auto) get_arg(ph_t<N> &, ReducedArgs &&... reduced_args) {
        return std::get<N - 1>(std::forward_as_tuple(reduced_args...));
    };

    template<typename OtherFunc, typename... OtherArgs, typename... ReducedArgs>
    decltype(auto) get_arg(bind_t<OtherFunc, OtherArgs...> &inner_bind, ReducedArgs&... reduced_args) {
        return inner_bind(reduced_args...);
    }

    template<size_t... N, typename... ReducedArgs>
    auto call(variadic_range<N...> &&, ReducedArgs &&... reduced_args) {
//        return func(get_arg(std::get<N>(args), std::forward<ReducedArgs>(reduced_args)...)...);
        return func(get_arg(std::get<N>(args), reduced_args...)...);
    }

    template<typename... ReducedArgs>
    auto operator()(ReducedArgs &&... reduced_args) {
        return call(get_range(), std::forward<ReducedArgs>(reduced_args)...);
    }
};

template<typename Func, typename... Args>
bind_t<Func, Args...> bind(Func &&func, Args &&... args) {
    return bind_t<Func, Args...>(std::forward<Func>(func), std::forward<Args>(args)...);
};

#endif