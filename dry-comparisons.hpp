#ifndef DRY_COMPARISONS_HPP_
#define DRY_COMPARISONS_HPP_

#include <utility>
#include <type_traits>
#include <tuple>
#include <functional>
#include <iostream>

namespace rollbear {

namespace internal {
template <typename ... T>
class member_print
{
public:
  template <typename ... U>
  member_print(const std::tuple<U...>& t, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<const U&>())...>* = nullptr)
    : m(t) {}
    friend
    std::ostream& operator<<(std::ostream& os, const member_print& self)
    {
      std::apply([&os](const auto& ... v) {
        int first = 1;
        ((os << &","[std::exchange(first,0)] << v),...);
      },self.m);
      return os;
    }
private:
  const std::tuple<T...>& m;
};

template <typename ... T>
member_print(const std::tuple<T...>&) -> member_print<T...>;

}
template <typename ... T>
class any_of : std::tuple<T...>
{
public:
    using std::tuple<T...>::tuple;
    template <typename U>
    constexpr auto operator==(const U& u) const
    noexcept(noexcept(((std::declval<const T&>() == u) || ...)))
    -> decltype(((std::declval<const T&>() == u) || ...))
    {
        return std::apply([&](const auto& ... a) { return ((a == u) || ...);},
                          get());
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
        return std::apply([&](const auto& ... a) { return ((a != u) && ...);},
                          get());
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
        return std::apply([&](const auto& ... a) { return ((a < u) || ...);},
                          get());
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
        return std::apply([&](const auto& ... a) { return ((a <= u) || ...);},
                          get());
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
        return std::apply([&](const auto& ... a) { return ((a > u) || ...);},
                          get());
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
        return std::apply([&](const auto& ... a) { return ((a >= u) || ...);},
                          get());

    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, any_of>{}>>
    friend constexpr auto operator<=(const U& u, const any_of& a)
    noexcept(noexcept(a >= u))
    -> decltype(a >= u)
    {
        return a >= u;
    }
    template <typename V = std::tuple<T...>,
              typename = decltype(internal::member_print(std::declval<const V&>()))>
    friend std::ostream& operator<<(std::ostream& os, const any_of& self)
    {
      return os << "any_of{" << internal::member_print{self.get()} << '}';
    }
    constexpr explicit operator bool() const
    noexcept(noexcept((std::declval<const T&>() || ...)))
    {
        return std::apply([](const auto& ... a) { return (a || ...);}, get());
    }
private:
    constexpr const std::tuple<T...>& get() const { return *this;}
};

template <typename ... T>
class none_of : std::tuple<T...>
{
public:
    using std::tuple<T...>::tuple;
    template <typename U>
    constexpr auto operator==(const U& u) const
    noexcept(noexcept(!((std::declval<const T&>() == u) || ...)))
    -> decltype(!((std::declval<const T&>() == u) || ...))
    {
        return std::apply([&](const auto& ... a) { return !((a == u) || ...);},
                          get());
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
        return std::apply([&](const auto& ... a) { return !((a != u) && ...);},
                          get());
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
        return std::apply([&](const auto& ... a) { return !((a < u) || ...);},
                          get());
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
        return std::apply([&](const auto& ... a) { return !((a <= u) || ...);},
                          get());
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
        return std::apply([&](const auto& ... a) { return !((a > u) || ...);},
                          get());
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
        return std::apply([&](const auto& ... a) { return !((a >= u) || ...);},
                          get());
    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, none_of>{}>>
    friend constexpr auto operator<=(const U& u, const none_of& a)
    noexcept(noexcept(a >= u))
    -> decltype(a >= u)
    {
        return a >= u;
    }
    template <typename V = std::tuple<T...>,
              typename = decltype(internal::member_print(std::declval<const V&>()))>
    friend std::ostream& operator<<(std::ostream& os, const none_of& self)
    {
      return os << "none_of{" << internal::member_print{self.get()} << '}';
    }
    constexpr explicit operator bool() const
    noexcept(noexcept(!(std::declval<const T&>() || ...)))
    {
        return std::apply([](const auto& ... a) { return !(a || ...);}, get());
    }
private:
    constexpr const std::tuple<T...>& get() const { return *this;}
};

template <typename ... T>
class all_of : std::tuple<T...>
{
public:
    using std::tuple<T...>::tuple;
    template <typename U>
    constexpr auto operator==(const U& u) const
    noexcept(noexcept(((std::declval<const T&>() == u) && ...)))
    -> decltype(((std::declval<const T&>() == u) && ...))
    {
        return std::apply([&](const auto& ... a) { return ((a == u) && ...);},
                          get());
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
    noexcept(noexcept(((std::declval<const T&>() == u) || ...)))
    -> decltype(((std::declval<const T&>() == u) || ...))
    {
        return std::apply([&](const auto& ... a) { return ((a != u) || ...);},
                          get());
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
        return std::apply([&](const auto& ... a) { return ((a < u) && ...);},
                          get());
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
        return std::apply([&](const auto& ... a) { return ((a <= u) && ...);},
                          get());
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
        return std::apply([&](const auto& ... a) { return ((a > u) && ...);},
                          get());
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
        return std::apply([&](const auto& ... a) { return ((a >= u) && ...);},
                          get());
    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, all_of>{}>>
    friend constexpr auto operator<=(const U& u, const all_of& a)
    noexcept(noexcept(a >= u))
    -> decltype(a >= u)
    {
        return a >= u;
    }
    template <typename V = std::tuple<T...>,
              typename = decltype(internal::member_print(std::declval<const V&>()))>
    friend std::ostream& operator<<(std::ostream& os, const all_of& self)
    {
      return os << "all_of{" << internal::member_print{self.get()} << '}';
    }
    constexpr explicit operator bool() const
    noexcept(noexcept((std::declval<const T&>() && ...)))
    {
      return std::apply([](const auto& ... a) { return (a && ...);}, get());
    }
private:
    constexpr const std::tuple<T...>& get() const { return *this;}
};


template <typename ... T>
any_of(T&& ...) -> any_of<T...>;
template <typename ... T>
none_of(T&& ...) -> none_of<T...>;
template <typename ... T>
all_of(T&& ...) -> all_of<T...>;

}

#endif
