# dry-comparisons

![CI](https://github.com/rollbear/dry-comparisons/workflows/CI/badge.svg)
[![codecov](https://codecov.io/gh/rollbear/dry-comparisons/branch/master/graph/badge.svg?token=XBT5Df1L3m)](https://codecov.io/gh/rollbear/dry-comparisons)

This is the source code for the `any_of`, `all_of` and `none_of` C++17 utility mentioned in the blog post
["DRY multicomparisons"](http://playfulprogramming.blogspot.com/2018/07/dry-multicomparisons.html)

Public domain.

## Usage

```Cpp
using rollbear::any_of;

class Machine
{
public:
  ...
  void func() {
    assert(state == any_of(S1, S3)); // assert(state == S1 || state == S3);
    ...
  }
private:
  enum { S0, S1, S2, S3 } state;
};
```

The types available are `any_of`, `all_of` and `none_of`.

*iff* all member types can be inserted into an ostream, an instance of `any_of`, `all_of` or
`none_of` may be printed, e.g. `std::cout << rollbear::any_of{1,3,5}`, which
will produce the output `any_of{1,3,5}`. 

*iff* it is possible to do logical and/or operations on all members, instances
of `any_of`, `all_of` and `none_of` may be used directly as boolean expressions.

* `bool(any_of{a,b,c}) == bool(a || b || c)`
* `bool(all_of{a,b,c}) == bool(a && b && c)`
* `bool(none_of{a,b,c}) == !bool(a || b || c)`

*iff* the members are callable, the resulting object is also callable.

* `any_of{std::plus{}, std::minus{}}(3,2) == 5`
* `all_of{std::plus{}, std::minus{}}(3,2) > 0`
* `none_of{std::plus{}, std::minus{}}(3,2) == 0`

with SFINAE friendly checks that the calls can be made for the values given. The
logical conditions are short cirquited, in the normal logical expression manners.

Further more, it is also possible to do relational comparisons. For example:

`assert(all_of(s1, s2) > ".")`, providing the expressions `s1` and `s2` are
greater-than comparable with a string literal.

