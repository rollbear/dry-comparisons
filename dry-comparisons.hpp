#ifndef DRY_COMPARISONS_HPP_
#define DRY_COMPARISONS_HPP_

#include <functional>
#include <iostream>

namespace rollbear {
namespace internal {
template <typename T, typename = void>
inline constexpr bool is_ostreamable_v = false;
template <typename T>
inline constexpr bool is_ostreamable_v<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<const T&>())>> = true;

constexpr auto tuple = [](auto&& ... ts)
{
  return [=](auto&& f) -> decltype(f(ts...)){ return f(ts...);};
};

constexpr auto or_elements = [](auto func) {
  return [=](auto... elements)
      -> decltype((func(elements) || ...))
  {
    return (func(elements) || ...);
  };
};
constexpr auto and_elements = [](auto func) {
  return [=](auto... elements)
      -> decltype((func(elements) && ...))
  {
    return (func(elements) && ...);
  };
};
constexpr auto nor_elements = [](auto func) {
  return [=](auto... elements)
      -> decltype(!(func(elements) || ...))
  {
    return !(func(elements) || ...);
  };
};
constexpr auto print_elements = [](std::ostream& os, auto name)
{
  return [&os, name](const auto&... elements)
      -> decltype(((os << elements),...))
  {
    const char* separator="";
    os << name << '(';
    ((os << std::exchange(separator,",") << elements),...);
    os << ')';
    return os;
  };
};

constexpr auto bind_rh = [](auto func, auto rh) {
  return [=](auto lh) noexcept(noexcept(func(lh,rh))) -> decltype(func(lh,rh)) { return func(lh, rh); };
};

constexpr auto equal_to = [](auto rh) { return bind_rh(std::equal_to{}, rh); };
constexpr auto less_than = [](auto rh) { return bind_rh(std::less{}, rh); };
constexpr auto less_equal = [](auto rh) { return bind_rh(std::less_equal{}, rh); };
constexpr auto greater_than = [](auto rh) { return bind_rh(std::greater{}, rh); };
constexpr auto greater_equal = [](auto rh) { return bind_rh(std::greater_equal{}, rh); };
constexpr auto as_bool = [](auto v) -> decltype(bool(v)) { return bool(v);};
template <template <typename ...> class T>
inline constexpr auto call_with = [](auto tfunc, auto name, auto&& ... ts) {
  auto tup = tuple(ts...);
  return [=](auto&& ...f) -> decltype(T(name, tfunc, tuple(tup(f)...))) { return T(name, tfunc, tuple(tup(f)...));};
};
template <typename Func, typename Tuple> class op_t {
  const char* name;
  Tuple tup;
  Func func;
  template <typename E>
  constexpr auto apply(E e) const
  noexcept(noexcept(tup(func(e))))
  -> decltype(tup(func(e)))
  { return tup(func(e)); }

public:
  constexpr op_t(const char* n, Func f, Tuple t) : name(n), tup(t), func(f) {}
  constexpr explicit operator bool() const
  noexcept(noexcept(apply(as_bool)))
  {
    return apply(as_bool);
  }
  template <typename RH>
  constexpr auto operator==(const RH &rh) const
  noexcept(noexcept(equal_to(rh)))
  -> decltype(apply(equal_to(rh)))
  {
    return apply(equal_to(rh));
  }
  template <typename LH>
  constexpr friend auto operator==(const LH &lh, const op_t &rh)
  noexcept(noexcept(rh == lh))
  -> decltype(rh == lh)
  {
    return rh == lh;
  }
  template <typename RH>
  constexpr auto operator!=(const RH &rh) const
  noexcept(noexcept(!(*this == rh)))
  -> decltype(!(*this == rh))
  {
    return !(*this == rh);
  }
  template <typename LH>
  constexpr friend auto operator!=(const LH &lh, const op_t &rh)
  noexcept(noexcept(!(rh == lh)))
  -> decltype(!(rh == lh))
  {
    return !(rh == lh);
  }
  template <typename RH>
  constexpr auto operator<(const RH &rh) const
  noexcept(noexcept(apply(less_than(rh))))
  -> decltype(apply(less_than(rh)))
  {
    return apply(less_than(rh));
  }
  template <typename LH>
  friend constexpr auto operator<(const LH &lh, const op_t& rh)
  noexcept(noexcept(rh > lh))
  -> decltype(rh > lh)
  {
    return rh > lh;
  }
  template <typename RH>
  constexpr auto operator<=(const RH &rh) const
  noexcept(noexcept(apply(less_equal(rh))))
  -> decltype(apply(less_than(rh)))
  {
    return apply(less_equal(rh));
  }
  template <typename LH>
  friend constexpr auto operator<=(const LH &lh, const op_t& rh)
  noexcept(noexcept(rh >= lh))
  -> decltype(rh >= lh)
  {
    return rh >= lh;
  }
  template <typename RH>
  constexpr auto operator>(const RH &rh) const
  noexcept(noexcept(apply(greater_than(rh))))
  -> decltype(apply(greater_than(rh)))
  {
    return apply(greater_than(rh));
  }
  template <typename LH>
  friend constexpr auto operator>(const LH &lh, const op_t& rh)
  noexcept(noexcept(rh < lh))
  -> decltype(rh < lh)
  {
    return rh < lh;
  }
  template <typename RH>
  constexpr auto operator>=(const RH &rh) const
  noexcept(noexcept(apply(greater_equal(rh))))
  -> decltype(apply(greater_equal(rh)))
  {
    return apply(greater_equal(rh));
  }
  template <typename LH>
  friend constexpr auto operator>=(const LH &lh, const op_t& rh)
  noexcept(noexcept(rh <= lh))
  -> decltype(rh <= lh)
  {
    return rh <= lh;
  }
  template <typename OS>
  friend
  auto operator<<(OS& os, const op_t& op)
  -> decltype(tup(print_elements(os, name)))
  {
    return op.tup(print_elements(os, op.name));
  }
  template <typename ... Ts>
  constexpr
  auto operator()(Ts&& ... ts) const
  noexcept(noexcept(tup(call_with<internal::op_t>(func, name, ts...))))
  -> decltype(tup(call_with<op_t>(func, name, ts...)))
  {
    return tup(call_with<op_t>(func, name, ts...));
  }
};

}
constexpr auto any_of = [](auto&&... ts) { return internal::op_t("any_of", internal::or_elements, internal::tuple(std::forward<decltype(ts)>(ts)...));};
constexpr auto all_of = [](auto&&... ts) { return internal::op_t("all_of", internal::and_elements, internal::tuple(std::forward<decltype(ts)>(ts)...));};
constexpr auto none_of = [](auto&&... ts) { return internal::op_t("none_of", internal::nor_elements, internal::tuple(std::forward<decltype(ts)>(ts)...));};

}

#endif
