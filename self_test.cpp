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

#define REQUIRE(...) if (__VA_ARGS__) {;} else { throw #__VA_ARGS__;}


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
        REQUIRE(s == "none_of{1,2,3}")
      }
    },
    {
      "print all_of",
      []{
        std::ostringstream os;
        os << all_of{1,2,3};
        auto s = os.str();
        REQUIRE(s == "all_of{1,2,3}")
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
