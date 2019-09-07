#ifndef DRY_COMPARISONS_HPP_
#define DRY_COMPARISONS_HPP_

#include <utility>
#include <type_traits>
#include <tuple>
#include <functional>
#include <iosfwd>

namespace rollbear {

namespace internal {

template <typename, typename = void>
struct printable;
template <typename ... Ts>
struct printable<std::tuple<Ts...>, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<const Ts&>())...>>
{
    using type = void;
};
template <typename T>
using printable_t = typename printable<T>::type;

template <typename F, typename ... Args>
struct bound
{
    using RT = std::invoke_result_t<F, Args...>;

    template <typename F_, typename ... As>
    constexpr bound(F_&& f_, As&& ... as) : f(std::forward<F_>(f_)), args(std::forward<As>(as)...) {}

    constexpr operator RT() const
    noexcept(std::is_nothrow_invocable_v<const F&, const Args&...>)
    {
        return std::apply(f, args);
    }

    F f;
    std::tuple<Args...> args;
};

template <typename F, typename ... Args>
bound(F, Args...) -> bound<F, Args...>;

template <typename ... Ts>
class logical_tuple : std::tuple<Ts...>
{
    using tuple = std::tuple<Ts...>;
    constexpr const tuple& self() const { return *this; }
protected:
    using tuple::tuple;
    template <typename F>
    constexpr auto or_all(F&& f) const
    {
        return std::apply([&](const auto& ... v) { return (f(v) || ...);}, self());
    }
    template <typename F>
    constexpr auto and_all(F&& f) const
    {
        return std::apply([&](const auto& ... v) { return (f(v) && ...);}, self());
    }
    template <typename RT, typename ... Args>
    constexpr RT bind(Args&& ... args) const {
        return std::apply([&](auto&&... f) { return RT{bound<Ts, Args...>(std::forward<decltype(f)>(f), args...)...}; }, self());
    }
    template <typename Char, typename Traits>
    std::basic_ostream<Char, Traits>& print(const Char* label, std::basic_ostream<Char, Traits>& os) const
    {
        os << label << '{';
        std::apply([&os](const auto& ... v) {
            int first = 1;
            ((os << &","[std::exchange(first, 0)] << v),...);
        }, self());
        return os << '}';
    }
};
}


template <typename ... T>
class any_of : internal::logical_tuple<T...>
{
    using internal::logical_tuple<T...>::or_all;
    using internal::logical_tuple<T...>::and_all;
public:
    using internal::logical_tuple<T...>::logical_tuple;

    template <typename U>
    constexpr auto operator==(const U& u) const
    noexcept(noexcept(((std::declval<const T&>() == u) || ...)))
    -> decltype(((std::declval<const T&>() == u) || ...))
    {
        return or_all([&](auto&& v) { return v == u;});
    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, any_of>{}>>
    friend constexpr auto operator==(const U& u, const any_of& a)
    noexcept(noexcept(a == u))
    -> decltype(a == u)
    {
        return a == u;
    }
    template <typename U>
    constexpr auto operator!=(const U& u) const
    noexcept(noexcept(((std::declval<const T&>() != u) && ...)))
    -> decltype(((std::declval<const T&>() != u) && ...))
    {
        return and_all([&](auto v) { return v != u;});
    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, any_of>{}>>
    friend constexpr auto operator!=(const U& u, const any_of& a)
    noexcept(noexcept(a != u))
    -> decltype(a != u)
    {
        return a != u;
    }
    template <typename U>
    constexpr auto operator<(const U& u) const
    noexcept(noexcept(((std::declval<const T&>() < u) || ...)))
    -> decltype(((std::declval<const T&>() < u) || ...))
    {
        return or_all([&](auto&& v){ return v < u;});
    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, any_of>{}>>
    friend constexpr auto operator>(const U& u, const any_of& a)
    noexcept(noexcept(a < u))
    -> decltype(a < u)
    {
        return a < u;
    }
    template <typename U>
    constexpr auto operator<=(const U& u) const
    noexcept(noexcept(((std::declval<const T&>() <= u) || ...)))
    -> decltype(((std::declval<const T&>() <= u) || ...))
    {
        return or_all([&](auto&& v){ return v <= u;});
    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, any_of>{}>>
    friend constexpr auto operator>=(const U& u, const any_of& a)
    noexcept(noexcept(a <= u))
    -> decltype(a <= u)
    {
        return a <= u;
    }
    template <typename U>
    constexpr auto operator>(const U& u) const
    noexcept(noexcept(((std::declval<const T&>() > u) || ...)))
    -> decltype(((std::declval<const T&>() > u) || ...))
    {
        return or_all([&](auto&& v) { return v > u;});
    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, any_of>{}>>
    friend constexpr auto operator<(const U& u, const any_of& a)
    noexcept(noexcept(a > u))
    -> decltype(a > u)
    {
        return a > u;
    }
    template <typename U>
    constexpr auto operator>=(const U& u) const
    noexcept(noexcept(((std::declval<const T&>() >= u) || ...)))
    -> decltype(((std::declval<const T&>() >= u) || ...))
    {
        return or_all([&](auto&& v) { return v >= u;});
    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, any_of>{}>>
    friend constexpr auto operator<=(const U& u, const any_of& a)
    noexcept(noexcept(a >= u))
    -> decltype(a >= u)
    {
        return a >= u;
    }
    template <typename V = std::tuple<T...>, typename = internal::printable_t<V>>
    friend std::ostream& operator<<(std::ostream& os, const any_of& self)
    {
        return self.print("any_of", os);
    }
    constexpr explicit operator bool() const
    noexcept(noexcept((std::declval<const T&>() || ...)))
    {
        return or_all([](auto&& v) { return v;});
    }
    template <typename ... Ts>
    constexpr auto operator()(Ts&& ... ts) const
    noexcept(
    std::conjunction_v<std::is_nothrow_move_constructible<T>...> &&
    std::conjunction_v<std::is_nothrow_copy_constructible<Ts>...>)
    -> std::enable_if_t<std::conjunction_v<std::is_copy_constructible<Ts>...>&&
                        std::conjunction_v<std::is_invocable<T, Ts...>...>,
            any_of<internal::bound<T, Ts...>...>>
    {
        using RT = any_of<internal::bound<T, Ts...>...>;
        return this->template bind<RT>(std::forward<Ts>(ts)...);
    }
};

template <typename ... T>
class none_of : internal::logical_tuple<T...>
{
    using internal::logical_tuple<T...>::or_all;
    using internal::logical_tuple<T...>::and_all;
public:
    using internal::logical_tuple<T...>::logical_tuple;
    template <typename U>
    constexpr auto operator==(const U& u) const
    noexcept(noexcept(!((std::declval<const T&>() == u) || ...)))
    -> decltype(!((std::declval<const T&>() == u) || ...))
    {
        return !or_all([&](auto&& v) { return v == u;});
    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, none_of>{}>>
    friend constexpr auto operator==(const U& u, const none_of& a)
    noexcept(noexcept(a == u))
    -> decltype(a == u)
    {
        return a == u;
    }
    template <typename U>
    constexpr auto operator!=(const U& u) const
    noexcept(noexcept(!((std::declval<const T&>() != u) && ...)))
    -> decltype(!((std::declval<const T&>() != u) && ...))
    {
        return !and_all([&](auto && v){return v != u;});
    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, none_of>{}>>
    friend constexpr auto operator!=(const U& u, const none_of& a)
    noexcept(noexcept(a != u))
    -> decltype(a != u)
    {
        return a != u;
    }
    template <typename U>
    constexpr auto operator<(const U& u) const
    noexcept(noexcept(!((std::declval<const T&>() < u) || ...)))
    -> decltype(!((std::declval<const T&>() < u) || ...))
    {
        return !or_all([&](auto&& v){ return v < u;});
    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, none_of>{}>>
    friend constexpr auto operator>(const U& u, const none_of& a)
    noexcept(noexcept(a < u))
    -> decltype(a < u)
    {
        return a < u;
    }
    template <typename U>
    constexpr auto operator<=(const U& u) const
    noexcept(noexcept(!((std::declval<const T&>() <= u) || ...)))
    -> decltype(!((std::declval<const T&>() <= u) || ...))
    {
        return !or_all([&](auto&& v){ return v <= u;});
    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, none_of>{}>>
    friend constexpr auto operator>=(const U& u, const none_of& a)
    noexcept(noexcept(a <= u))
    -> decltype(a <= u)
    {
        return a <= u;
    }
    template <typename U>
    constexpr auto operator>(const U& u) const
    noexcept(noexcept(!((std::declval<const T&>() > u) || ...)))
    -> decltype(!((std::declval<const T&>() > u) || ...))
    {
        return !or_all([&](auto&& v) { return v > u;});
    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, none_of>{}>>
    friend constexpr auto operator<(const U& u, const none_of& a)
    noexcept(noexcept(a > u))
    -> decltype(a > u)
    {
        return a > u;
    }
    template <typename U>
    constexpr auto operator>=(const U& u) const
    noexcept(noexcept(!((std::declval<const T&>() >= u) || ...)))
    -> decltype(!((std::declval<const T&>() >= u) || ...))
    {
        return !or_all([&](auto&& v){ return v >= u;});
    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, none_of>{}>>
    friend constexpr auto operator<=(const U& u, const none_of& a)
    noexcept(noexcept(a >= u))
    -> decltype(a >= u)
    {
        return a >= u;
    }
    template <typename V = std::tuple<T...>, typename = internal::printable_t<V>>
    friend std::ostream& operator<<(std::ostream& os, const none_of& self)
    {
        return self.print("none_of", os);
    }
    constexpr explicit operator bool() const
    noexcept(noexcept(!(std::declval<const T&>() || ...)))
    {
        return !or_all([](auto&& v) { return v;});
    }

    template <typename ... Ts>
    constexpr auto operator()(Ts&& ... ts) const
    noexcept(
            std::conjunction_v<std::is_nothrow_move_constructible<T>...> &&
            std::conjunction_v<std::is_nothrow_copy_constructible<Ts>...>)
    -> std::enable_if_t<std::conjunction_v<std::is_copy_constructible<Ts>...>&&
                        std::conjunction_v<std::is_invocable<T, Ts...>...>,
            none_of<internal::bound<T, Ts...>...>>
    {
        using RT = none_of<internal::bound<T, Ts...>...>;
        return this->template bind<RT>(std::forward<Ts>(ts)...);
    }
};

template <typename ... T>
class all_of : internal::logical_tuple<T...>
{
    using internal::logical_tuple<T...>::or_all;
    using internal::logical_tuple<T...>::and_all;
public:
    using internal::logical_tuple<T...>::logical_tuple;

    template <typename U>
    constexpr auto operator==(const U& u) const
    noexcept(noexcept(((std::declval<const T&>() == u) && ...)))
    -> decltype(((std::declval<const T&>() == u) && ...))
    {
        return and_all([&](auto&& v){ return v == u;});
    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, all_of>{}>>
    friend constexpr auto operator==(const U& u, const all_of& a)
    noexcept(noexcept(a == u))
    -> decltype(a == u)
    {
        return a == u;
    }
    template <typename U>
    constexpr auto operator!=(const U& u) const
    noexcept(noexcept(((std::declval<const T&>() != u) || ...)))
    -> decltype(((std::declval<const T&>() != u) || ...))
    {
        return or_all([&](auto&& v){return v != u;});
    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, all_of>{}>>
    friend constexpr auto operator!=(const U& u, const all_of& a)
    noexcept(noexcept(a != u))
    -> decltype(a != u)
    {
        return a != u;
    }
    template <typename U>
    constexpr auto operator<(const U& u) const
    noexcept(noexcept(((std::declval<const T&>() < u) && ...)))
    -> decltype(((std::declval<const T&>() < u) && ...))
    {
        return and_all([&](auto&& v){ return v < u;});
    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, all_of>{}>>
    friend constexpr auto operator>(const U& u, const all_of& a)
    noexcept(noexcept(a < u))
    -> decltype(a < u)
    {
        return a < u;
    }
    template <typename U>
    constexpr auto operator<=(const U& u) const
    noexcept(noexcept(((std::declval<const T&>() <= u) && ...)))
    -> decltype(((std::declval<const T&>() <= u) && ...))
    {
        return and_all([&](auto&& v){ return v <= u;});
    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, all_of>{}>>
    friend constexpr auto operator>=(const U& u, const all_of& a)
    noexcept(noexcept(a <= u))
    -> decltype(a <= u)
    {
        return a <= u;
    }
    template <typename U>
    constexpr auto operator>(const U& u) const
    noexcept(noexcept(((std::declval<const T&>() > u) && ...)))
    -> decltype(((std::declval<const T&>() > u) && ...))
    {
        return and_all([&](auto&& v){ return v > u;});
    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, all_of>{}>>
    friend constexpr auto operator<(const U& u, const all_of& a)
    noexcept(noexcept(a > u))
    -> decltype(a > u)
    {
        return a > u;
    }
    template <typename U>
    constexpr auto operator>=(const U& u) const
    noexcept(noexcept(((std::declval<const T&>() >= u) && ...)))
    -> decltype(((std::declval<const T&>() >= u) && ...))
    {
        return and_all([&](auto&& v){ return v >= u;});
    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, all_of>{}>>
    friend constexpr auto operator<=(const U& u, const all_of& a)
    noexcept(noexcept(a >= u))
    -> decltype(a >= u)
    {
        return a >= u;
    }
    template <typename V = std::tuple<T...>, typename = internal::printable_t<V>>
    friend std::ostream& operator<<(std::ostream& os, const all_of& self)
    {
        return self.print("all_of", os);
    }
    constexpr explicit operator bool() const
    noexcept(noexcept((std::declval<const T&>() && ...)))
    {
        return and_all([](auto&& v) -> bool { return v;});
    }
    template <typename ... Ts>
    constexpr auto operator()(Ts&& ... ts) const
    noexcept(
            std::conjunction_v<std::is_nothrow_move_constructible<T>...> &&
            std::conjunction_v<std::is_nothrow_copy_constructible<Ts>...>)
    -> std::enable_if_t<std::conjunction_v<std::is_copy_constructible<Ts>...>&&
                        std::conjunction_v<std::is_invocable<T, Ts...>...>,
            all_of<internal::bound<T, Ts...>...>>
    {
        using RT = all_of<internal::bound<T, Ts...>...>;
        return this->template bind<RT>(std::forward<Ts>(ts)...);
    }
};


template <typename ... T>
any_of(T&& ...) -> any_of<T...>;
template <typename ... T>
none_of(T&& ...) -> none_of<T...>;
template <typename ... T>
all_of(T&& ...) -> all_of<T...>;

}

#endif
