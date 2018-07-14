#include "dry-comparisons.hpp"
#include <cstdio>

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
static_assert(x != none_of(1,2,4));
static_assert(!(x != none_of(1,2,3,4)));

static_assert(x < none_of(0,1,2,3));
static_assert(!(x < none_of(4,0,1,2)));

static_assert(x <= none_of(0,1,2));
static_assert(!(x <= none_of(0,1,2,3)));

static_assert(x > none_of(3,4,5));
static_assert(!(x > none_of(3,2,4,5)));

static_assert(x >= none_of(4,5,6));
static_assert(!(x >= none_of(6,4,5,3)));

int main()
{
  std::puts("cool!");
}
