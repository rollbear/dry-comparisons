#include "dry-comparisons.hpp"
#include <cstdio>
#include <sstream>
#include <iomanip>

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
constexpr auto add = [](auto v) -> decltype(v+I){ return v + I;};

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

#define REQUIRE(...) [&](){if (__VA_ARGS__) {;} else { throw #__VA_ARGS__;}}()


int main()
{
  struct test {
    const char* name;
    void (*test_func)();
  };
  test tests[] = {
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
