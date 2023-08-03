#include "dry-comparisons.hpp"
#include <cstdio>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <memory>

template <template <typename ...> class, typename ...>
  struct is_detected_ : std::false_type {};

template <template <typename ...> class D, typename ... A>
  struct is_detected_<D, std::void_t<D<A...>>, A...> : std::true_type {};

template <template <typename ...> class D, typename ... A>
using is_detected = typename is_detected_<D, void, A...>::type;

template <template <typename ...> class D, typename ... A>
  constexpr bool is_detected_v = is_detected<D, A...>::value;

using rollbear::all_of;
using rollbear::any_of;
using rollbear::none_of;

constexpr int x = 3;
constexpr const char* nullstr = nullptr;

template <int I>
static constexpr auto add = [](auto v) -> decltype(v+I){ return v + I;};

static_assert(x == any_of(1,3,5));
static_assert(!(x == any_of(1,2,5)));
static_assert(x != any_of(1,2,5));
static_assert(!(x != any_of(1,3,5)));

static_assert(x < any_of(5,3,2,4));
static_assert(!(x < any_of(0,1,2,3)));

static_assert(x <= any_of(6,5,3,4));
static_assert(!(x <= any_of(0,1,2)));

static_assert(x > any_of(0,1,2));
static_assert(!(x > any_of(4,3,5,6)));

static_assert(x >= any_of(0,3,1,2));
static_assert(!(x >= any_of(4,5,6)));

static_assert(any_of{add<1>, add<2>, add<3>}(-2) == 0);
static_assert(!(rollbear::any_of{add<1>, add<2>, add<3>}(-2) == 3));


static_assert(x == all_of(3,3,3));
static_assert(!(x == all_of(3,2,3)));
static_assert(x != all_of(3,2,3));
static_assert(!(x != all_of(3,3,3)));

static_assert(x < all_of(4,5,6));
static_assert(!(x < all_of(3,4,5,6)));

static_assert(x <= all_of(4,5,6,3));
static_assert(!(x <= all_of(4,5,6,2)));

static_assert(x > all_of(0,1,2));
static_assert(!(x > all_of(3,0,1,2)));

static_assert(x >= all_of(0,3,1,2));
static_assert(!(x >= all_of(0,4,1,2)));

static_assert(!(rollbear::all_of{add<1>, add<2>, add<3>}(-3) < 0));
static_assert(all_of{add<1>, add<2>, add<3>}(-3) <= 3);


static_assert(x == none_of(1,2,4));
static_assert(!(x == none_of(1,3,4)));
static_assert(x != none_of(1,2,4,3));
static_assert(!(x != none_of(1,2,4)));

static_assert(x < none_of(0,1,2,3));
static_assert(!(x < none_of(4,0,1,2)));

static_assert(x <= none_of(0,1,2));
static_assert(!(x <= none_of(0,1,2,3)));

static_assert(x > none_of(3,4,5));
static_assert(!(x > none_of(3,2,4,5)));

static_assert(x >= none_of(4,5,6));
static_assert(!(x >= none_of(6,4,5,3)));

static_assert(!(rollbear::none_of{add<1>, add<2>, add<3>}(-2) == 0));
static_assert(none_of{add<1>, add<2>, add<3>}(-2) == 3);


template <typename T>
struct any_of_eq {
  template<typename ... Args>
  using type = decltype(std::declval<T>() == any_of{std::declval<Args>()...});
};
template <typename T>
using print_result_type = decltype(std::declval<std::ostream&>() << std::declval<const T&>());

static_assert(x == any_of(3));
static_assert(!is_detected_v<any_of_eq<int>::type, char*,int>);
static_assert(is_detected_v<any_of_eq<int>::type, int,int>);

struct nonprintable {};

static_assert(std::is_constructible_v<any_of<nonprintable>, nonprintable>);
static_assert(std::is_constructible_v<none_of<nonprintable>, nonprintable>);
static_assert(std::is_constructible_v<all_of<nonprintable>, nonprintable>);

static_assert(is_detected_v<print_result_type, any_of<int,int>>);
static_assert(!is_detected_v<print_result_type, any_of<int, nonprintable>>);
static_assert(is_detected_v<print_result_type, none_of<int,int>>);
static_assert(!is_detected_v<print_result_type, none_of<int, nonprintable>>);
static_assert(is_detected_v<print_result_type, all_of<int,int>>);
static_assert(!is_detected_v<print_result_type, all_of<int, nonprintable>>);

template <typename F, typename ... Ts>
constexpr bool is_callable_v = is_detected_v<std::invoke_result_t, F, Ts...>;

static_assert(!is_callable_v<decltype(all_of{add<1>,add<2>}), int,int>);
static_assert(!is_callable_v<decltype(all_of{add<1>,add<2>}), void*>);
static_assert(!is_callable_v<decltype(all_of{add<1>,add<2>})>);

constexpr auto gt = [](auto v) { return [=](auto u){ return u > v;};};
constexpr auto aborts = [](auto) constexpr -> bool { abort(); return false;};
static_assert(!all_of{gt(3), aborts}(2), "all_of short cirquits calls");
//static_assert(all_of{gt(1), aborts}(2), "all_of short cirquits calls");
//static_assert(!any_of{gt(3), aborts}(2), "any_of short cirquits calls");
static_assert(any_of{gt(1), aborts}(2), "any_of short cirquits calls");
//static_assert(none_of{gt(3), aborts}(2), "none_of short cirquits calls");
static_assert(!none_of{gt(1), aborts}(2), "none_of short cirquits calls");

constexpr auto not_null = [](auto&& v) { return v != nullptr;};

static_assert(!is_callable_v<decltype(all_of{not_null}), std::unique_ptr<int>>, "non-copyable rvalues are disallowed");
static_assert(!is_callable_v<decltype(any_of{not_null}), std::unique_ptr<int>>, "non-copyable rvalues are disallowed");
static_assert(!is_callable_v<decltype(none_of{not_null}), std::unique_ptr<int>>, "non-copyable rvalues are disallowed");

template <typename T>
struct throwing
{
    constexpr throwing(T t_) noexcept(false) : t{t_} {}
    constexpr throwing(const throwing& orig) noexcept(false) : t{orig.t} {}
    constexpr operator T() const noexcept(false) { return t;}
    T t;
};

constexpr bool throwing_tuple = !std::is_nothrow_constructible_v<std::tuple<int>, int>;

constexpr auto identity = [](auto v) noexcept  { return v;};
constexpr auto throwing_identity = [](auto v) noexcept(false) { return v;};

constexpr auto all_of_ident = all_of{identity,identity};
constexpr auto all_of_ident_throw = all_of{identity, throwing_identity};
static_assert(throwing_tuple || std::is_nothrow_constructible_v<all_of<int,int>, int, int>);
static_assert(throwing_tuple || noexcept(all_of{1,2}));
static_assert(!noexcept(all_of{1,throwing(3)}));
static_assert(std::is_nothrow_invocable_v<std::equal_to<>, all_of<int,int>, int>);
static_assert(!std::is_nothrow_invocable_v<std::equal_to<>, all_of<int,throwing<int>>, int>);
static_assert(std::is_nothrow_invocable_v<std::not_equal_to<>, all_of<int,int>, int>);
static_assert(!std::is_nothrow_invocable_v<std::not_equal_to<>, all_of<int,throwing<int>>, int>);
static_assert(std::is_nothrow_invocable_v<std::less<>, all_of<int,int>, int>);
static_assert(!std::is_nothrow_invocable_v<std::less<>, all_of<int,throwing<int>>, int>);
static_assert(std::is_nothrow_invocable_v<std::less_equal<>, all_of<int,int>, int>);
static_assert(!std::is_nothrow_invocable_v<std::less_equal<>, all_of<int,throwing<int>>, int>);
static_assert(std::is_nothrow_invocable_v<std::greater<>, all_of<int,int>, int>);
static_assert(!std::is_nothrow_invocable_v<std::greater<>, all_of<int,throwing<int>>, int>);
static_assert(std::is_nothrow_invocable_v<std::greater_equal<>, all_of<int,int>, int>);
static_assert(!std::is_nothrow_invocable_v<std::greater_equal<>, all_of<int,throwing<int>>, int>);
static_assert(std::is_invocable_v<std::logical_and<>, all_of<int,int>, bool>);
static_assert(!std::is_nothrow_invocable_v<std::logical_and<>, all_of<int,throwing<int>>, bool>);
static_assert(std::is_invocable_v<std::logical_and<>, all_of<int,throwing<int>>, bool>);
static_assert(noexcept(all_of_ident(3)));
static_assert(noexcept(all_of_ident(3) > 0));
static_assert(noexcept(all_of_ident_throw(3)));
#if defined(__clang__) || defined(__GNUC__) && __GNUC__ >= 9
static_assert(!noexcept(all_of_ident_throw(3) > 0));
#endif

constexpr auto none_of_ident = none_of{identity,identity};
constexpr auto none_of_ident_throw = none_of{identity, throwing_identity};
static_assert(throwing_tuple || std::is_nothrow_constructible_v<none_of<int,int>, int, int>);
static_assert(throwing_tuple || noexcept(none_of{1,2}));
static_assert(!noexcept(none_of{1,throwing(3)}));
static_assert(std::is_nothrow_invocable_v<std::equal_to<>, none_of<int,int>, int>);
static_assert(!std::is_nothrow_invocable_v<std::equal_to<>, none_of<int,throwing<int>>, int>);
static_assert(std::is_nothrow_invocable_v<std::not_equal_to<>, none_of<int,int>, int>);
static_assert(!std::is_nothrow_invocable_v<std::not_equal_to<>, none_of<int,throwing<int>>, int>);
static_assert(std::is_nothrow_invocable_v<std::less<>, none_of<int,int>, int>);
static_assert(!std::is_nothrow_invocable_v<std::less<>, none_of<int,throwing<int>>, int>);
static_assert(std::is_nothrow_invocable_v<std::less_equal<>, none_of<int,int>, int>);
static_assert(!std::is_nothrow_invocable_v<std::less_equal<>, none_of<int,throwing<int>>, int>);
static_assert(std::is_nothrow_invocable_v<std::greater<>, none_of<int,int>, int>);
static_assert(!std::is_nothrow_invocable_v<std::greater<>, none_of<int,throwing<int>>, int>);
static_assert(std::is_nothrow_invocable_v<std::greater_equal<>, none_of<int,int>, int>);
static_assert(!std::is_nothrow_invocable_v<std::greater_equal<>, none_of<int,throwing<int>>, int>);
static_assert(std::is_invocable_v<std::logical_and<>, none_of<int,int>, bool>);
static_assert(!std::is_nothrow_invocable_v<std::logical_and<>, none_of<int,throwing<int>>, bool>);
static_assert(std::is_invocable_v<std::logical_and<>, none_of<int,throwing<int>>, bool>);
static_assert(noexcept(none_of_ident(3)));
static_assert(noexcept(none_of_ident(3) > 0));
static_assert(noexcept(none_of_ident_throw(3)));
#if defined(__clang__) || defined(__GNUC__) && __GNUC__ >= 9
static_assert(!noexcept(none_of_ident_throw(3) > 0));
#endif

constexpr auto any_of_ident = any_of{identity,identity};
constexpr auto any_of_ident_throw = any_of{identity, throwing_identity};
static_assert(throwing_tuple || std::is_nothrow_constructible_v<any_of<int,int>, int, int>);
static_assert(throwing_tuple || noexcept(any_of{1,2}));
static_assert(!noexcept(any_of{1,throwing(3)}));
static_assert(std::is_nothrow_invocable_v<std::equal_to<>, any_of<int,int>, int>);
static_assert(!std::is_nothrow_invocable_v<std::equal_to<>, any_of<int,throwing<int>>, int>);
static_assert(std::is_nothrow_invocable_v<std::not_equal_to<>, any_of<int,int>, int>);
static_assert(!std::is_nothrow_invocable_v<std::not_equal_to<>, any_of<int,throwing<int>>, int>);
static_assert(std::is_nothrow_invocable_v<std::less<>, any_of<int,int>, int>);
static_assert(!std::is_nothrow_invocable_v<std::less<>, any_of<int,throwing<int>>, int>);
static_assert(std::is_nothrow_invocable_v<std::less_equal<>, any_of<int,int>, int>);
static_assert(!std::is_nothrow_invocable_v<std::less_equal<>, any_of<int,throwing<int>>, int>);
static_assert(std::is_nothrow_invocable_v<std::greater<>, any_of<int,int>, int>);
static_assert(!std::is_nothrow_invocable_v<std::greater<>, any_of<int,throwing<int>>, int>);
static_assert(std::is_nothrow_invocable_v<std::greater_equal<>, any_of<int,int>, int>);
static_assert(!std::is_nothrow_invocable_v<std::greater_equal<>, any_of<int,throwing<int>>, int>);
static_assert(std::is_invocable_v<std::logical_and<>, any_of<int,int>, bool>);
static_assert(!std::is_nothrow_invocable_v<std::logical_and<>, any_of<int,throwing<int>>, bool>);
static_assert(std::is_invocable_v<std::logical_and<>, any_of<int,throwing<int>>, bool>);
static_assert(noexcept(any_of_ident(3)));
static_assert(noexcept(any_of_ident(3) > 0));
static_assert(noexcept(any_of_ident_throw(3)));
#if defined(__clang__) || defined(__GNUC__) && __GNUC__ >= 9
static_assert(!noexcept(any_of_ident_throw(3) > 0));
#endif

#define REQUIRE(...) [&](){if (__VA_ARGS__) {;} else { throw #__VA_ARGS__;}}()


int zero = 0;

const auto negate = [](auto x) { return -x;};
const auto next = [](auto x) { return ++x;};

int main()
{
  struct test {
    const char* name;
    void (*test_func)();
  };
  test tests[] = {
    {
        "== any_of",
        []{
            REQUIRE(zero == any_of(0,1,2));
            REQUIRE(zero == any_of(1,0,3));
            REQUIRE(zero == any_of(1,2,0));
            REQUIRE(!(zero == any_of(1,2,3)));
            REQUIRE(any_of(0,2,3) == zero);
            REQUIRE(any_of(1,0,3) == zero);
            REQUIRE(any_of(1,2,0) == zero);
            REQUIRE(!(any_of(1,2,3) == zero));
        }
    },
    {
        "== all_of",
        []{
            REQUIRE(!(zero == all_of(1,2,3)));
            REQUIRE(!(zero == all_of(0,1,2)));
            REQUIRE(!(zero == all_of(0,1,0)));
            REQUIRE(zero == all_of(0,0,0));
            REQUIRE(zero == all_of(0,0));
            REQUIRE(zero == all_of(0));
            REQUIRE(!(all_of(1,2,3) == zero));
            REQUIRE(!(all_of(0,1,2) == zero));
            REQUIRE(!(all_of(0,1,0) == zero));
            REQUIRE(all_of(0,0,0) == zero);
            REQUIRE(all_of(0,0) == zero);
            REQUIRE(all_of(0) == zero);
        }
    },
    {
      "== none_of",
      []{
          REQUIRE(zero == none_of(1,2,3));
          REQUIRE(zero == none_of(1,2));
          REQUIRE(zero == none_of(1));
          REQUIRE(!(zero == none_of(0,1,2)));
          REQUIRE(!(zero == none_of(1,2,0)));
          REQUIRE(!(zero == none_of(1,0,2)));
          REQUIRE(!(zero == none_of(0,1)));
          REQUIRE(!(zero == none_of(1,0)));
          REQUIRE(!(zero == none_of(0)));
          REQUIRE(zero == none_of(1));
      }
    },
    {
      "!= any_of",
      []{
          REQUIRE(zero != any_of(1,2,3));
          REQUIRE(!(zero != any_of(0,1,2)));
          REQUIRE(!(zero != any_of(1,2,0)));
          REQUIRE(!(zero != any_of(1,0,2)));
          REQUIRE(!(zero != any_of(0,1)));
          REQUIRE(!(zero != any_of(1,0)));
          REQUIRE(!(zero != any_of(0)));
          REQUIRE(zero != any_of(1));
          REQUIRE(any_of(1,2,3) != zero);
          REQUIRE(!(any_of(0,1,2) != zero));
          REQUIRE(!(any_of(1,2,0) != zero));
          REQUIRE(!(any_of(1,0,2) != zero));
          REQUIRE(!(any_of(0,1) != zero));
          REQUIRE(!(any_of(1,0) != zero));
          REQUIRE(!(any_of(0) != zero));
          REQUIRE(any_of(1) != zero);
      }
    },
    {
        "!= all_of",
        [] {
            REQUIRE(zero != all_of(1, 2, 3));
            REQUIRE(zero != all_of(0,1,2));
            REQUIRE(zero != all_of(1,0,2));
            REQUIRE(zero != all_of(1,2,0));
            REQUIRE(zero != all_of(1,2));
            REQUIRE(zero != all_of(0,1));
            REQUIRE(zero != all_of(1,0));
            REQUIRE(zero != all_of(1));
            REQUIRE(!(zero != all_of(0)));
            REQUIRE(!(zero != all_of(0,0)));
            REQUIRE(!(zero != all_of(0,0,0)));
            REQUIRE(all_of(1, 2, 3) != zero);
            REQUIRE(all_of(0,1,2) != zero);
            REQUIRE(all_of(1,0,2) != zero);
            REQUIRE(all_of(1,2,0) != zero);
            REQUIRE(all_of(1,2) != zero);
            REQUIRE(all_of(0,1) != zero);
            REQUIRE(all_of(1,0) != zero);
            REQUIRE(all_of(1) != zero);
            REQUIRE(!(all_of(0) != zero));
            REQUIRE(!(all_of(0,0) != zero));
            REQUIRE(!(all_of(0,0,0) != zero));
        }
    },
    {
        "!= none_of",
        []{
            REQUIRE(!(zero != none_of(1,2,3)));
            REQUIRE(zero != none_of(0,1,2));
            REQUIRE(zero != none_of(1,0,2));
            REQUIRE(zero != none_of(1,2,0));
            REQUIRE(!(zero != none_of(1,2)));
            REQUIRE(zero != none_of(0,1));
            REQUIRE(zero != none_of(1,0));
            REQUIRE(zero != none_of(0));
            REQUIRE(!(zero != none_of(1)));
            REQUIRE(!(none_of(1,2,3) != zero));
            REQUIRE(none_of(0,1,2) != zero);
            REQUIRE(none_of(1,0,2) != zero);
            REQUIRE(none_of(1,2,0) != zero);
            REQUIRE(!(none_of(1,2) != zero));
            REQUIRE(none_of(0,1) != zero);
            REQUIRE(none_of(1,0) != zero);
            REQUIRE(none_of(0) != zero);
            REQUIRE(!(none_of(1) != zero));
        }
    },
    {
        "< any_of",
        []{
            REQUIRE(zero < any_of(0,0,1));
            REQUIRE(zero < any_of(0,1,0));
            REQUIRE(zero < any_of(1,0,0));
            REQUIRE(!(zero < any_of(0,0,0)));
            REQUIRE(zero < any_of(0,1));
            REQUIRE(zero < any_of(1,0));
            REQUIRE(!(zero < any_of(0,0)));
            REQUIRE(zero < any_of(1));
            REQUIRE(!(zero < any_of(0)));
            REQUIRE(any_of(0,0,1) > zero);
            REQUIRE(any_of(0,1,0) > zero);
            REQUIRE(any_of(1,0,0) > zero);
            REQUIRE(!(any_of(0,0,0) > zero));
            REQUIRE(any_of(0,1) > zero);
            REQUIRE(any_of(1,0) > zero);
            REQUIRE(!(any_of(0,0) > zero));
            REQUIRE(any_of(1) > zero);
            REQUIRE(!(any_of(0) > zero));
        }
    },
    {
        "< all_of",
        []{
            REQUIRE(zero < all_of(1,2,3));
            REQUIRE(!(zero < all_of(0,1,2)));
            REQUIRE(!(zero < all_of(1,0,2)));
            REQUIRE(!(zero < all_of(1,2,0)));
            REQUIRE(zero < all_of(1,2));
            REQUIRE(!(zero < all_of(0,1)));
            REQUIRE(!(zero < all_of(1,0)));
            REQUIRE(zero < all_of(1));
            REQUIRE(!(zero < all_of(0)));
            REQUIRE(all_of(1,2,3) > zero);
            REQUIRE(!(all_of(0,1,2) > zero));
            REQUIRE(!(all_of(1,0,2) > zero));
            REQUIRE(!(all_of(1,2,0) > zero));
            REQUIRE(all_of(1,2) > zero);
            REQUIRE(!(all_of(0,1) > zero));
            REQUIRE(!(all_of(1,0) > zero));
            REQUIRE(all_of(1) > zero);
            REQUIRE(!(all_of(0) > zero));
        }
    },
    {
      "< none_of",
      []{
          REQUIRE(zero < none_of(0,0,0));
          REQUIRE(!(zero < none_of(1,0,0)));
          REQUIRE(!(zero < none_of(0,1,0)));
          REQUIRE(!(zero < none_of(0,0,1)));
          REQUIRE(zero < none_of(0,0));
          REQUIRE(!(zero < none_of(1,0)));
          REQUIRE(!(zero < none_of(0,1)));
          REQUIRE(zero < none_of(0));
          REQUIRE(!(zero < none_of(1)));
          REQUIRE(none_of(0,0,0) > zero);
          REQUIRE(!(none_of(1,0,0) > zero));
          REQUIRE(!(none_of(0,1,0) > zero));
          REQUIRE(!(none_of(0,0,1) > zero));
          REQUIRE(none_of(0,0) > zero);
          REQUIRE(!(none_of(1,0) > zero));
          REQUIRE(!(none_of(0,1) > zero));
          REQUIRE(none_of(0) > zero);
          REQUIRE(!(none_of(1) > zero));
      }
    },
    {
        "<= any_of",
        []{
            REQUIRE(!(zero <= any_of(-1,-1,-1)));
            REQUIRE(zero <= any_of(0,-1,-1));
            REQUIRE(zero <= any_of(-1,0,-1));
            REQUIRE(zero <= any_of(-1,-1,0));
            REQUIRE(zero <= any_of(0,-1));
            REQUIRE(zero <= any_of(-1,0));
            REQUIRE(!(zero <= any_of(-1,-1)));
            REQUIRE(zero <= any_of(0));
            REQUIRE(!(zero <= any_of(-1)));
            REQUIRE(!(any_of(-1,-1,-1) >= zero));
            REQUIRE(any_of(0,-1,-1) >= zero);
            REQUIRE(any_of(-1,0,-1) >= zero);
            REQUIRE(any_of(-1,-1,0) >= zero);
            REQUIRE(any_of(0,-1) >= zero);
            REQUIRE(any_of(-1,0) >= zero);
            REQUIRE(!(any_of(-1,-1) >= zero));
            REQUIRE(any_of(0) >= zero);
            REQUIRE(!(any_of(-1) >= zero));
        }
    },
    {
        "<= all_of",
        []{
            REQUIRE(zero <= all_of(0,0,0));
            REQUIRE(!(zero <= all_of(0,-1,-1)));
            REQUIRE(!(zero <= all_of(-1,0,-1)));
            REQUIRE(!(zero <= all_of(-1,-1,0)));
            REQUIRE(zero <= all_of(0,0));
            REQUIRE(!(zero <= all_of(0,-1)));
            REQUIRE(!(zero <= all_of(-1,0)));
            REQUIRE(zero <= all_of(0));
            REQUIRE(!(zero <= all_of(-1)));
            REQUIRE(all_of(0,0,0) >= zero);
            REQUIRE(!(all_of(0,-1,-1) >= zero));
            REQUIRE(!(all_of(-1,0,-1) >= zero));
            REQUIRE(!(all_of(-1,-1,0) >= zero));
            REQUIRE(all_of(0,0) >= zero);
            REQUIRE(!(all_of(0,-1) >= zero));
            REQUIRE(!(all_of(-1,0) >= zero));
            REQUIRE(all_of(0) >= zero);
            REQUIRE(!(all_of(-1) >= zero));
        }
    },
    {
        "<= none_of",
        []{
            REQUIRE(zero <= none_of(-1,-1,-1));
            REQUIRE(!(zero <= none_of(0,-1,-1)));
            REQUIRE(!(zero <= none_of(-1,0,-1)));
            REQUIRE(!(zero <= none_of(-1,-1,0)));
            REQUIRE(zero <= none_of(-1,-1));
            REQUIRE(!(zero <= none_of(-1,0)));
            REQUIRE(!(zero <= none_of(0,-1)));
            REQUIRE(zero <= none_of(-1));
            REQUIRE(!(zero <= none_of(0)));
            REQUIRE(none_of(-1,-1,-1) >= zero);
            REQUIRE(!(none_of(0,-1,-1) >= zero));
            REQUIRE(!(none_of(-1,0,-1) >= zero));
            REQUIRE(!(none_of(-1,-1,0) >= zero));
            REQUIRE(none_of(-1,-1) >= zero);
            REQUIRE(!(none_of(-1,0) >= zero));
            REQUIRE(!(none_of(0,-1) >= zero));
            REQUIRE(none_of(-1) >= zero);
            REQUIRE(!(none_of(0) >= zero));
        }
    },
    ///
    {
        "> any_of",
        []{
            REQUIRE(zero > any_of(0,0,-1));
            REQUIRE(zero > any_of(0,-1,0));
            REQUIRE(zero > any_of(-1,0,0));
            REQUIRE(!(zero > any_of(0,0,0)));
            REQUIRE(zero > any_of(0,-1));
            REQUIRE(zero > any_of(-1,0));
            REQUIRE(!(zero > any_of(0,0)));
            REQUIRE(zero > any_of(-1));
            REQUIRE(!(zero > any_of(0)));
            REQUIRE(any_of(0,0,-1) < zero);
            REQUIRE(any_of(0,-1,0) < zero);
            REQUIRE(any_of(-1,0,0) < zero);
            REQUIRE(!(any_of(0,0,0) < zero));
            REQUIRE(any_of(0,-1) < zero);
            REQUIRE(any_of(-1,0) < zero);
            REQUIRE(!(any_of(0,0) < zero));
            REQUIRE(any_of(-1) < zero);
            REQUIRE(!(any_of(0) < zero));
        }
    },
    {
        "> all_of",
        []{
            REQUIRE(zero > all_of(-1,-2,-3));
            REQUIRE(!(zero > all_of(0,-1,-2)));
            REQUIRE(!(zero > all_of(-1,0,-2)));
            REQUIRE(!(zero > all_of(-1,-2,0)));
            REQUIRE(zero > all_of(-1,-2));
            REQUIRE(!(zero > all_of(0,-1)));
            REQUIRE(!(zero > all_of(-1,0)));
            REQUIRE(zero > all_of(-1));
            REQUIRE(!(zero > all_of(0)));
            REQUIRE(all_of(-1,-2,-3) < zero);
            REQUIRE(!(all_of(0,-1,-2) < zero));
            REQUIRE(!(all_of(-1,0,-2) < zero));
            REQUIRE(!(all_of(-1,-2,0) < zero));
            REQUIRE(all_of(-1,-2) < zero);
            REQUIRE(!(all_of(0,-1) < zero));
            REQUIRE(!(all_of(-1,0) < zero));
            REQUIRE(all_of(-1) < zero);
            REQUIRE(!(all_of(0) < zero));
        }
    },
    {
        "> none_of",
        []{
            REQUIRE(zero > none_of(0,0,0));
            REQUIRE(!(zero > none_of(-1,0,0)));
            REQUIRE(!(zero > none_of(0,-1,0)));
            REQUIRE(!(zero > none_of(0,0,-1)));
            REQUIRE(zero > none_of(0,0));
            REQUIRE(!(zero > none_of(-1,0)));
            REQUIRE(!(zero > none_of(0,-1)));
            REQUIRE(zero > none_of(0));
            REQUIRE(!(zero > none_of(-1)));
            REQUIRE(none_of(0,0,0) < zero);
            REQUIRE(!(none_of(-1,0,0) < zero));
            REQUIRE(!(none_of(0,-1,0) < zero));
            REQUIRE(!(none_of(0,0,-1) < zero));
            REQUIRE(none_of(0,0) < zero);
            REQUIRE(!(none_of(-1,0) < zero));
            REQUIRE(!(none_of(0,-1) < zero));
            REQUIRE(none_of(0) < zero);
            REQUIRE(!(none_of(-1) < zero));
        }
    },
    {
        ">= any_of",
        []{
            REQUIRE(!(zero >= any_of(1,1,1)));
            REQUIRE(zero >= any_of(0,1,1));
            REQUIRE(zero >= any_of(1,0,1));
            REQUIRE(zero >= any_of(1,1,0));
            REQUIRE(zero >= any_of(0,1));
            REQUIRE(zero >= any_of(1,0));
            REQUIRE(!(zero >= any_of(1,1)));
            REQUIRE(zero >= any_of(0));
            REQUIRE(!(zero >= any_of(1)));
            REQUIRE(!(any_of(1,1,1) <= zero));
            REQUIRE(any_of(0,1,1) <= zero);
            REQUIRE(any_of(1,0,1) <= zero);
            REQUIRE(any_of(1,1,0) <= zero);
            REQUIRE(any_of(0,1) <= zero);
            REQUIRE(any_of(1,0) <= zero);
            REQUIRE(!(any_of(1,1) <= zero));
            REQUIRE(any_of(0) <= zero);
            REQUIRE(!(any_of(1) <= zero));
        }
    },
    {
        ">= all_of",
        []{
            REQUIRE(zero >= all_of(0,0,0));
            REQUIRE(!(zero >= all_of(0,1,1)));
            REQUIRE(!(zero >= all_of(1,0,1)));
            REQUIRE(!(zero >= all_of(1,1,0)));
            REQUIRE(zero >= all_of(0,0));
            REQUIRE(!(zero >= all_of(0,1)));
            REQUIRE(!(zero >= all_of(1,0)));
            REQUIRE(zero >= all_of(0));
            REQUIRE(!(zero >= all_of(1)));
            REQUIRE(all_of(0,0,0) <= zero);
            REQUIRE(!(all_of(0,1,1) <= zero));
            REQUIRE(!(all_of(1,0,1) <= zero));
            REQUIRE(!(all_of(1,1,0) <= zero));
            REQUIRE(all_of(0,0) <= zero);
            REQUIRE(!(all_of(0,1) <= zero));
            REQUIRE(!(all_of(1,0) <= zero));
            REQUIRE(all_of(0) <= zero);
            REQUIRE(!(all_of(1) <= zero));
        }
    },
    {
        ">= none_of",
        []{
            REQUIRE(zero >= none_of(1,1,1));
            REQUIRE(!(zero >= none_of(0,1,1)));
            REQUIRE(!(zero >= none_of(1,0,1)));
            REQUIRE(!(zero >= none_of(1,1,0)));
            REQUIRE(zero >= none_of(1,1));
            REQUIRE(!(zero >= none_of(1,0)));
            REQUIRE(!(zero >= none_of(0,1)));
            REQUIRE(zero >= none_of(1));
            REQUIRE(!(zero >= none_of(0)));
            REQUIRE(none_of(1,1,1) <= zero);
            REQUIRE(!(none_of(0,1,1) <= zero));
            REQUIRE(!(none_of(1,0,1) <= zero));
            REQUIRE(!(none_of(1,1,0) <= zero));
            REQUIRE(none_of(1,1) <= zero);
            REQUIRE(!(none_of(1,0) <= zero));
            REQUIRE(!(none_of(0,1) <= zero));
            REQUIRE(none_of(1) <= zero);
            REQUIRE(!(none_of(0) <= zero));
        }
    },
    {
        "any_of(...)",
        []{
            REQUIRE(any_of(negate, next)(1) < 0);
            REQUIRE(!(any_of(negate, next)(zero) < 0));
        }
    },
    {
        "all_of(...)",
        []{
            REQUIRE(all_of(negate, next)(zero) >= 0);
            REQUIRE(all_of(negate, next)(-1) >= 0);
        }
    },
    {
        "none_of(...)",
        []{
            REQUIRE(none_of(negate, next)(zero) < 0);
            REQUIRE(none_of(negate, next)(-1) < 0);
        }
    },
    {
      "print any_of",
      []{
        std::ostringstream os;
        os << any_of{1,3,5};
        auto s = os.str();
        REQUIRE(s == "any_of{1,3,5}");
      }
    },
    {
      "print none_of",
      []{
        std::ostringstream os;
        os << none_of{1,2,3};
        auto s = os.str();
        REQUIRE(s == "none_of{1,2,3}");
      }
    },
    {
      "print all_of",
      []{
        std::ostringstream os;
        os << all_of{1,2,3};
        auto s = os.str();
        REQUIRE(s == "all_of{1,2,3}");
      }
    },
    {
      "all_of is true if all members are true",
      []{
        int v;
        auto b = bool(all_of{true, 1, &v});
        REQUIRE(b);
      }
    },
    {
      "all_of is false with nullptr",
      []{
        auto b = bool(all_of{true, 1, nullstr});
        REQUIRE(!b);
      }
    },
    {
      "all_of is false with int(0)",
      [] {
        int v = 0;
        auto b = bool(all_of{true, v, &v});
        REQUIRE(!b);
      }
    },
    {
      "all_of isfalse with false member",
      [] {
        int v = 1;
        auto b = bool(all_of(false, v, &v));
        REQUIRE(!b);
      }
    },
    {
      "none_of is true if all members are false",
      []{
        int v = 0;
        auto b = bool(none_of{false, v, nullstr});
        REQUIRE(b);
      }
    },
    {
      "none_of is false if a member is a non-nullptr",
      []{
        int v = 0;
        auto b = bool(none_of{false, v, &v});
        REQUIRE(!b);
      }
    },
    {
      "none_of is false if a member is a non-zero int",
      []{
        int v = 1;
        auto b = bool(none_of{false, v, nullstr});
        REQUIRE(!b);
      }
    },
    {
      "none_of is false if a member true",
      []{
        int v = 0;
        auto b = bool(none_of{true, v, nullstr});
        REQUIRE(!b);
      }
    },
    {
      "any_of is false all members are false",
      []{
        int v = 0;
        auto b = bool(any_of{false, v, nullstr});
        REQUIRE(!b);
      }
    },
    {
      "any_of is true if a member is a non-nullptr",
      []{
        int v = 0;
        auto b = bool(any_of{false, v, &v});
        REQUIRE(b);
      }
    },
    {
      "any_of is true if a member is a non-zero int",
      []{
        int v = 1;
        auto b = bool(any_of{false, v, nullstr});
        REQUIRE(b);
      }
    },
    {
      "any_of is true if a member is true",
      []{
        int v = 0;
        auto b = bool(any_of{true, v, nullstr});
        REQUIRE(b);
      }
    },
    {
        "binding an lvalue reference keeps a reference",
        []{
            auto p = std::make_shared<int>(3);
            auto cond = all_of{[](auto&& v) -> bool{ return v.get();}}(p);
            REQUIRE(p.use_count() == 1U);
            REQUIRE(cond);
        }
    },
    {
        "binding an rvalue reference copies an instance",
        []{
            auto cond = all_of{[](auto&& v) { return v.use_count() == 1;}}(std::make_shared<int>(3));
            REQUIRE(cond);
        }

    }

  };
  int failures = 0;
  for (auto& t : tests)
  {
    try {
      std::cout << std::setw(60) << std::left << t.name << ' ';
      t.test_func();
      std::cout << "pass\n";
    }
    catch (const char* m)
    {
      ++failures;
      std::cout << "failed: " << m << '\n';
    }
  }
  if (!failures)
  {
    std::puts("\ncool!");
  }
  else
  {
    std::puts("\nbummer!");
  }
  return failures;
}
