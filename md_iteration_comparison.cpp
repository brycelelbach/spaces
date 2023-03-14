// "Portable" Bryce-to-vectorizer communication facilities.
// I never leave home without 'em!

// BOOST_DETAIL_PP_STRINGIZE(expr) - Return expr as a string literal.
#define BOOST_DETAIL_PP_STRINGIZE_(expr) #expr
#define BOOST_DETAIL_PP_STRINGIZE(expr) BOOST_DETAIL_PP_STRINGIZE_(expr)

// BOOST_PP_PRAGMA(args) - Emits a pragma.
#define BOOST_PRAGMA(args) _Pragma(BOOST_DETAIL_PP_STRINGIZE(args))

// BOOST_DEMAND_VECTORIZATION - Insist that the compiler disregard loop-carried
// dependency analysis and cost modelling and vectorize the loop directly
// following the macro. Using this incorrectly can silently cause bogus codegen
// that blows up in unexpected ways. Usage:
//
// BOOST_DEMAND_VECTORIZATION for (/* ... */) { /* ... */ }
//
// NOTE: Unlike Clang and Intel, GCC doesn't have a stronger hint than ivdep,
// so this is the best we can do. It is not clear if this overrides GCC's cost
// modeling.
#if   defined(__INTEL_COMPILER)
    #define BOOST_DEMAND_VECTORIZATION                                         \
        BOOST_PRAGMA(simd)                                                     \
        /**/
#elif defined(__clang__)
    #define BOOST_DEMAND_VECTORIZATION                                         \
        BOOST_PRAGMA(clang loop vectorize(enable) interleave(enable))          \
        /**/
#else
    #define BOOST_DEMAND_VECTORIZATION                                         \
        BOOST_PRAGMA(GCC ivdep)                                                \
        /**/
#endif

// BOOST_PREVENT_VECTORIZATION - Tell the compiler to not vectorize a loop.
// Usage:
//
// BOOST_PREVENT_VECTORIZATION for (/* ... */) { /* ... */ }
//
// NOTE: Unlike Clang and Intel, GCC doesn't seem to have a way to do this.
#if   defined(__INTEL_COMPILER)
    #define BOOST_PREVENT_VECTORIZATION                                        \
        BOOST_PRAGMA(novector)                                                 \
        /**/
#elif defined(__clang__)
    #define BOOST_PREVENT_VECTORIZATION                                        \
        BOOST_PRAGMA(clang loop vectorize(disable) interleave(disable))        \
        /**/
#else
    #define BOOST_PREVENT_VECTORIZATION                                        \
                                                                               \
        /**/
#endif

// Sometimes it is nice to check that our brash and bold claims are, in fact,
// correct. Defining BOOST_CHECK_ASSUMPTIONS does that (e.g. assumption will be
// asserted before they are assumed).
#if defined(BOOST_CHECK_ASSUMPTIONS)
    #include <cassert>
    #include <stdint>
    #define BOOST_ASSERT_ASSUMPTION(expr) assert(expr)
#else
    #define BOOST_ASSERT_ASSUMPTION(expr)
#endif

// BOOST_ASSUME(expr) - Tell the compiler to assume that expr is true.
// Useful for telling the compiler that the trip count for a loop is division
// by a unrolling/vectorizing-friendly number:
//
//   BOOST_ASSUME((N % 32) == 0); for (auto i = 0; i != N; ++i) /* ... */
//
// BOOST_ASSUME_ALIGNED(ptr, align) - Tell the compiler to
// assume that ptr is aligned to align bytes. ptr must be an lvalue non-const
// pointer.
//
// NOTE: These used to have ridiculous exponential-in-number-of-uses
// compile-time costs with Clang/LLVM. For example, a 10k line project with
// ~100 BOOST_ASSUME/BOOST_ASSUME_ALIGNED usages would take ~20
// seconds to build with ICPC and ~5-10 minutes with Clang/LLVM. I believe the
// issue has now been fixed, but you'll run into it with older versions.
//
// NOTE: To the best of my knowledge, ICPC's __assume_aligned() is an
// assumption about the first argument, while Clang/GCC's
// __builtin_assume_aligned() is an assumption about the return value of the
// intrinsic.
#if   defined(__INTEL_COMPILER)
    #define BOOST_ASSUME(expr)                                                 \
        BOOST_ASSERT_ASSUMPTION(expr)                                          \
        __assume(expr)                                                         \
        /**/
    #define BOOST_ASSUME_ALIGNED(ptr, align)                                   \
        BOOST_ASSERT_ASSUMPTION(0 == (std::uintptr_t(ptr) % alignment))        \
        __assume_aligned(ptr, align)                                           \
        /**/
#elif defined(__clang__)
    #define BOOST_ASSUME(expr)                                                 \
        BOOST_ASSERT_ASSUMPTION(expr)                                          \
        __builtin_assume(expr)                                                 \
        /**/
    #define BOOST_ASSUME_ALIGNED(ptr, align)                                   \
        BOOST_ASSERT_ASSUMPTION(0 == (std::uintptr_t(ptr) % alignment))        \
        {                                                                      \
            ptr = reinterpret_cast<decltype(ptr)>(                             \
                __builtin_assume_aligned(ptr, align)                           \
            );                                                                 \
        }                                                                      \
        /**/
#else // GCC
    #define BOOST_ASSUME(expr)                                                 \
        BOOST_ASSERT_ASSUMPTION(expr)                                          \
        do { if (!(expr)) __builtin_unreachable(); } while (0)                 \
        /**/
    #define BOOST_ASSUME_ALIGNED(ptr, align)                                   \
        BOOST_ASSERT_ASSUMPTION(0 == (std::uintptr_t(ptr) % alignment))        \
        {                                                                      \
            ptr = reinterpret_cast<decltype(ptr)>(                             \
                __builtin_assume_aligned(ptr, align)                           \
            );                                                                 \
        }                                                                      \
        /**/
#endif

///////////////////////////////////////////////////////////////////////////////////

#include <array>
#include <cstddef>

using index_type = std::ptrdiff_t; // Signed 4 life.

template <std::size_t N>
struct position
{
    constexpr position() noexcept : idxs{{}} {}

    template <typename... Indices>
    constexpr explicit position(Indices... idxs_) noexcept
      : idxs{{static_cast<index_type>(idxs_)...}}
    {
        static_assert(
            sizeof...(Indices) == N
          , "Insufficient index parameters passed to constructor."
        );
    }

    constexpr position(position const&)            noexcept = default;
    constexpr position(position&&)                 noexcept = default;
    constexpr position& operator=(position const&) noexcept = default;
    constexpr position& operator=(position&&)      noexcept = default;

    constexpr index_type& operator[](index_type i) noexcept
    {
        return idxs[i];
    }
    constexpr index_type const& operator[](index_type i) const noexcept
    {
        return idxs[i];
    }

    std::array<index_type, N> idxs;
};

struct dimension
{
    constexpr dimension() noexcept : extent(0), idx(0) {}

    constexpr dimension(index_type extent_, index_type idx_) noexcept
      : extent(extent_), idx(idx_)
    {}

    constexpr dimension(dimension const&) noexcept = default;
    constexpr dimension(dimension&&)      noexcept = default;

    index_type const extent;
    index_type       idx;
};

///////////////////////////////////////////////////////////////////////////////////

struct index_2d_iterator
{
    struct sentinel
    {
        constexpr sentinel(index_type nj_) noexcept : nj(nj_) {}

        index_type nj;
    };

    constexpr index_2d_iterator(dimension i_, dimension j_) noexcept
      : i(i_), j(j_)
    {}

    constexpr index_2d_iterator& operator++() noexcept
    {
        BOOST_ASSUME(i.idx    >= 0);
        BOOST_ASSUME(i.extent >  0);

        ++i.idx;                // Inner loop iteration-expression.

        if (i.extent == i.idx)  // Inner loop condition.
        {
            ++j.idx;            // Outer loop increment.
            i.idx = 0;          // Inner loop init-statement.
        }

        return *this;
    }

    // NOTE: ICPC requires this when we use an iterator-sentinel range.
    friend constexpr index_type operator-(
        index_2d_iterator const& l
      , index_2d_iterator const& r
        ) noexcept
    {
        BOOST_ASSUME(l.i.idx    >= 0);
        BOOST_ASSUME(r.i.idx    >= 0);
        BOOST_ASSUME(l.j.idx    >= 0);
        BOOST_ASSUME(r.j.idx    >= 0);
        BOOST_ASSUME(l.i.extent >  0);
        BOOST_ASSUME(l.j.extent >  0);

        return (l.j.idx - r.j.idx) * (l.i.extent) - (l.i.idx - r.i.idx);
    }

    // NOTE: ICPC requires this when we use an iterator-sentinel range.
    friend constexpr index_type operator-(
        index_2d_iterator const& l
      , sentinel r
        ) noexcept
    {
        BOOST_ASSUME(l.i.idx    >= 0);
        BOOST_ASSUME(l.j.idx    >= 0);
        BOOST_ASSUME(l.i.extent >  0);
        BOOST_ASSUME(r.nj       >  0);

        return (l.j.idx - r.nj) * (l.i.extent) - (l.i.idx - l.i.extent);
    }
    friend constexpr index_type operator-(
        sentinel r
      , index_2d_iterator const& l
        ) noexcept
    {
        BOOST_ASSUME(l.i.idx    >= 0);
        BOOST_ASSUME(l.j.idx    >= 0);
        BOOST_ASSUME(l.i.extent >  0);
        BOOST_ASSUME(r.nj       >  0);

        return (r.nj - l.j.idx) * (l.i.extent) - (l.i.extent - l.i.idx);
    }

    friend constexpr index_2d_iterator operator+(
        index_2d_iterator it
      , index_type d
        ) noexcept
    {
        return index_2d_iterator(
          dimension(it.i.extent, it.i.idx + it.i.extent % d)
        , dimension(it.j.extent, it.j.idx + d / it.i.extent)
        );
    }

    // NOTE: ICPC requires this when we use an iterator-sentinel range.
    constexpr index_2d_iterator& operator+=(
        index_type d
        ) noexcept
    {
        i.idx += i.extent % d;
        j.idx += j.idx + d / i.extent;
        return *this;
    }

    constexpr position<2> operator[](
        index_type d
        ) noexcept
    {
        return *(*this + d);
    }

    constexpr position<2> operator*() const noexcept
    {
        return position<2>(i.idx, j.idx);
    }

    friend constexpr bool
    operator==(index_2d_iterator const& l, index_2d_iterator const& r) noexcept
    {
        BOOST_ASSUME(l.i.idx    >= 0);
        BOOST_ASSUME(r.i.idx    >= 0);
        BOOST_ASSUME(l.j.idx    >= 0);
        BOOST_ASSUME(r.j.idx    >= 0);
        BOOST_ASSUME(l.i.extent >  0);
        BOOST_ASSUME(r.i.extent >  0);
        BOOST_ASSUME(l.j.extent >  0);
        BOOST_ASSUME(r.j.extent >  0);

        return l.i.extent == r.i.extent && l.j.extent == r.j.extent
            && l.i.idx    == r.i.idx    && l.j.idx    == r.j.idx;
    }
    friend constexpr bool operator!=(
        index_2d_iterator const& l
      , index_2d_iterator const& r
        ) noexcept
    {
        return !(l == r);
    }

    friend constexpr bool operator==(
        index_2d_iterator const& l
      , sentinel r
        ) noexcept
    {
        BOOST_ASSUME(l.j.idx >= 0);
        BOOST_ASSUME(r.nj    >  0);

        return l.j.idx == r.nj;
    }
    friend constexpr bool operator!=(
        index_2d_iterator const& l
      , sentinel r
        ) noexcept
    {
        return !(l == r);
    }

  private:
    dimension i;
    dimension j;
};

constexpr index_2d_iterator index_2d_iterator_begin(
    index_type ni
  , index_type nj
    ) noexcept
{
    return index_2d_iterator(dimension(ni, 0), dimension(nj, 0));
}

constexpr index_2d_iterator index_2d_iterator_end(
    index_type ni
  , index_type nj
    ) noexcept
{
    return index_2d_iterator(dimension(ni, 0), dimension(nj, nj));
}

struct index_2d_iterator_sentinel_range
{
    constexpr index_2d_iterator_sentinel_range(
        index_type ni
      , index_type nj
        ) noexcept
      : first(index_2d_iterator_begin(ni, nj)), last(nj)
    {}

    constexpr index_2d_iterator begin() const noexcept { return first; }

    constexpr index_2d_iterator::sentinel end() const noexcept { return last; }

  private:
    index_2d_iterator first;
    index_2d_iterator::sentinel last;
};

///////////////////////////////////////////////////////////////////////////////////

struct storage_2d_iterator
{
  private:
    index_type location;
    std::array<index_type, 2> extents;

  public:
    storage_2d_iterator(
      index_type location_
    , std::array<index_type, 2> extents_
    )
      : location(location_), extents{extents_}
    {}

    auto& operator++()
    {
        ++location;
        return *this;
    }

    auto operator*() const
    {
        return std::array{location % extents[1], location / extents[1]};
    }

    bool operator!=(storage_2d_iterator const& other)
    {
        return location != other.location
            || extents[0] != other.extents[0]
            || extents[1] != other.extents[1];
    }
};

struct storage_2d_range {
    storage_2d_range(
        index_type ni
      , index_type nj
        )
      : first(0, std::array<index_type, 2>{ni, nj})
      , last(ni * nj, std::array<index_type, 2>{ni, nj})
    {}

    auto begin() const { return first; }

     auto end() const { return last; }

  private:
    storage_2d_iterator first;
    storage_2d_iterator last;
};

///////////////////////////////////////////////////////////////////////////////////

#if !(defined(__INTEL_LLVM_COMPILER) || defined(__INTEL_COMPILER))

#include <coroutine>

template <std::size_t N>
struct index_generator
{
    static_assert(N != 0, "N must be greater than 0.");

    struct promise_type
    {
        using return_type = index_generator;

        position<N> pos;

        constexpr std::suspend_always yield_value(position<N> pos_)
            noexcept
        {
            pos = pos_;
            return {};
        }

        constexpr std::suspend_always initial_suspend() const
            noexcept
        {
            return {};
        }

        constexpr std::suspend_always final_suspend() const
            noexcept
        {
            return {};
        }

        index_generator get_return_object() noexcept
        {
            return index_generator(this);
        }

        constexpr void return_void() noexcept {}
        constexpr void unhandled_exception() noexcept {}
    };

    struct iterator
    {
        std::coroutine_handle<promise_type> coro;
        bool done;

        constexpr iterator(
            std::coroutine_handle<promise_type> coro_
          , bool done_
            )
          : coro(coro_), done(done_)
        {}

        iterator& operator++()
        {
            coro.resume();
            done = coro.done();
            return *this;
        }

        position<N> operator*() const
        {
            return coro.promise().pos;
        }

        constexpr bool operator==(iterator const& rhs) const noexcept
        {
            return done == rhs.done;
        }
        constexpr bool operator!=(iterator const& rhs) const noexcept
        {
            return !(*this == rhs);
        }
    };

    iterator begin()
    {
        p.resume();
        return iterator(p, p.done());
    }

    constexpr iterator end()
    {
        return iterator(p, true);
    }

    constexpr index_generator(index_generator&& rhs) noexcept
      : p(rhs.p)
    {
        rhs.p = nullptr;
    }

    ~index_generator()
    {
        if (p) p.destroy();
    }

  private:
    explicit index_generator(promise_type* p) noexcept
      : p(std::coroutine_handle<promise_type>::from_promise(*p))
    {}

    std::coroutine_handle<promise_type> p;
};

inline index_generator<1> generate_indices(
    position<2> ni
    ) noexcept
{
    index_type const nilo = ni[0];
    index_type const nihi = ni[1];
    BOOST_ASSUME(nilo >= 0);
    BOOST_ASSUME(nihi >  nilo);
    for (index_type i = nilo; i != nihi; ++i)
        co_yield position<1>(i);
}

inline index_generator<1> generate_indices(
    index_type ni
    ) noexcept
{
    return generate_indices(position<2>{0, ni});
}

inline index_generator<2> generate_indices(
    position<2> ni
  , position<2> nj
    ) noexcept
{
    index_type const nilo = ni[0];
    index_type const nihi = ni[1];
    index_type const njlo = nj[0];
    index_type const njhi = nj[1];
    BOOST_ASSUME(njlo >= 0);
    BOOST_ASSUME(nilo >= 0);
    BOOST_ASSUME(njhi >  njlo);
    BOOST_ASSUME(nihi >  nilo);
    for (index_type j = njlo; j != njhi; ++j)
        for (index_type i = nilo; i != nihi; ++i)
            co_yield position<2>(i, j);
}

inline index_generator<2> generate_indices(
    index_type ni
  , index_type nj
    ) noexcept
{
    return generate_indices(position<2>{0, ni}, position<2>{0, nj});
}

#endif

///////////////////////////////////////////////////////////////////////////////////

#include <functional>
#include <tuple>
#include <optional>
#include <array>
#include <ranges>

template <std::ptrdiff_t I, typename Tuple, typename T, typename Op>
auto __tuple_reduce(Tuple&& tuple, T&& init, Op&& op) {
  if constexpr (I > 0) {
    return std::invoke(op,
                       std::get<I>(tuple),
                       __tuple_reduce<I - 1>((Tuple&&)tuple, (T&&)init, (Op&&)op));
  } else {
    return std::invoke((Op&&)op,
                       (T&&)init,
                       std::get<I>((Tuple&&)tuple));
  }
}

template <typename Tuple, typename T, typename Op>
auto tuple_reduce(Tuple&& tuple, T&& init, Op&& op) {
  return __tuple_reduce<std::tuple_size_v<Tuple> - 1>((Tuple&&)tuple, (T&&)init, (Op&&)op);
}

template <auto I>
using constant = std::integral_constant<decltype(I), I>;

template <typename T, template <typename...> class Primary>
struct is_specialization_of : std::false_type {};

template <template <typename...> class Primary, typename... Args>
struct is_specialization_of<Primary<Args...>, Primary> : std::true_type {};

template <typename T, template <typename...> class Primary>
concept specialization_of = is_specialization_of<std::remove_cvref_t<T>, Primary>::value;

template <typename T>
struct __add_optional { using type = std::optional<T>; };

template <typename T>
struct __add_optional<std::optional<T>> { using type = std::optional<T>; };

template <typename T>
using add_optional = __add_optional<T>::type;

template <typename T>
struct __remove_optional { using type = T; };

template <typename T>
struct __remove_optional<std::optional<T>> { using type = T; };

template <typename T>
using remove_optional = __remove_optional<T>::type;

///////////////////////////////////////////////////////////////////////////////////

inline constexpr auto invoke_o = [] <typename F, typename T> (F&& f, T&& t)
                                 -> std::conditional_t<
                                      std::same_as<std::invoke_result_t<F&&, remove_optional<T>>, void>,
                                      std::nullopt_t,
                                      add_optional<std::invoke_result_t<F&&, remove_optional<T>>>
                                    > {
  if constexpr (std::same_as<std::invoke_result_t<F&&, remove_optional<T>>, void>) {
    if constexpr (specialization_of<T, std::optional>) {
      if (t.has_value()) std::invoke((F&&)f, ((T&&)t).value());
    } else {
      std::invoke((F&&)f, (T&&)t);
    }
    return std::nullopt;
  } else {
    if constexpr (specialization_of<T, std::optional>) {
      if (t.has_value()) return std::invoke((F&&)f, ((T&&)t).value());
      else return std::nullopt;
    } else {
      return std::invoke((F&&)f, (T&&)t);
    }
  }
};

inline constexpr auto transform_o = [] <typename F> (F&& f) {
  return std::views::transform(invoke_o);
};

inline constexpr auto filter_o = [] <typename F> (F&& f) {
  return std::views::transform(
    [&] <typename T> (T&& t) -> add_optional<T> {
      if constexpr (specialization_of<T, std::optional>) {
        if (t.has_value() && std::invoke((F&&)f, t.value())) return ((T&&)t).value();
        else return std::nullopt;
      } else {
        if (std::invoke((F&&)f, t)) return (T&&)t;
        else return std::nullopt;
      }
    }
  );
};

///////////////////////////////////////////////////////////////////////////////////

template <std::ptrdiff_t N, typename MDSpace, typename OuterTuple>
constexpr auto mdrange(MDSpace&& space, OuterTuple&& outer);

template <typename MDSpace>
struct mdrank_t : std::rank<MDSpace> {};

template <typename MDSpace>
inline constexpr std::ptrdiff_t mdrank = mdrank_t<std::remove_cvref_t<MDSpace>>::value;

template <std::ptrdiff_t I, typename MDSpace, typename UnaryFunction, typename OuterTuple>
constexpr void __mdfor(MDSpace&& space, UnaryFunction&& f, OuterTuple&& outer) {
  if constexpr (I > 0) {
    BOOST_DEMAND_VECTORIZATION
    for (auto&& e: mdrange<I>(space, (OuterTuple&&)outer)) {
      invoke_o(
        [&] (auto&& t) {
          __mdfor<I - 1>((MDSpace&&)space, f, std::forward<decltype(t)>(t));
        }, std::forward<decltype(e)>(e));
    }
  } else {
    BOOST_DEMAND_VECTORIZATION
    for (auto&& e: mdrange<I>((MDSpace&&)space, (OuterTuple&&)outer)) {
      invoke_o(
        [&] (auto&& t) {
          std::apply((UnaryFunction&&)f, std::forward<decltype(t)>(t));
        }, std::forward<decltype(e)>(e));
    }
  }
}

template <typename MDSpace, typename UnaryFunction>
constexpr void mdfor(MDSpace&& space, UnaryFunction&& f) {
  if constexpr (mdrank<MDSpace> > 0)
    __mdfor<mdrank<MDSpace> - 1>((MDSpace&&)space, (UnaryFunction&&)f, std::tuple<>{});
}

///////////////////////////////////////////////////////////////////////////////////

template <typename Space, std::ptrdiff_t I, typename Factory>
struct __mdspace_binder {
  static_assert(I < mdrank<Space>);

private:
  Space underlying;
  Factory factory;

public:
  template <typename USpace, typename UFactory>
  constexpr __mdspace_binder(USpace&& underlying_, UFactory&& factory_)
    : underlying((USpace&&)underlying_), factory((UFactory&&)factory) {}

  constexpr __mdspace_binder(__mdspace_binder const& other)
    : underlying(other.underlying), factory(other.factory) {}
  constexpr __mdspace_binder(__mdspace_binder&& other)
    : underlying(std::move(other.underlying)), factory(std::move(other.factory)) {}

  template <std::ptrdiff_t J, typename USpace, typename OuterTuple>
    requires(std::convertible_to<USpace, __mdspace_binder>)
  friend constexpr auto mdrange(USpace&& space, OuterTuple&& outer) {
    static_assert(J < mdrank<USpace>);
    if constexpr (I == J) {
      return std::invoke(std::forward<decltype(space.factory)>(space.factory),
                         mdrange<I>(std::forward<decltype(space.underlying)>(space.underlying),
                                    (OuterTuple&&)outer));
    } else {
      return mdrange<I>(std::forward<decltype(space.underlying)>(space.underlying),
                        (OuterTuple&&)outer);
    }
  }
};

template <typename Space, std::ptrdiff_t I, typename Factory>
struct mdrank_t<__mdspace_binder<Space, I, Factory>> : mdrank_t<Space> {};

template <typename Space, typename Factory>
constexpr auto __mdspace_bind(Space&& space, Factory&& factory) {
  using T = __mdspace_binder<
    std::remove_cvref_t<Space>, 0, std::remove_cvref_t<Factory>
  >;
  return T((Space&&)space, (Factory&&)factory);
}

template <typename Index, typename Factory>
struct __on_extent_factory {
private:
  static constexpr Index index{};
  Factory underlying;

public:
  template <typename UFactory>
  explicit constexpr __on_extent_factory(UFactory&& underlying_)
    : underlying(underlying_) {}

  constexpr __on_extent_factory(__on_extent_factory const&) = default;
  constexpr __on_extent_factory(__on_extent_factory&&) = default;

  template <typename Space, typename UFactory>
    requires(specialization_of<UFactory, __on_extent_factory>)
  friend constexpr auto __mdspace_bind(Space&& space, UFactory&& factory) {
    using T = __mdspace_binder<
      std::remove_cvref_t<Space>, UFactory::index, decltype(factory.underlying)
    >;
    return T((Space&&)space, ((UFactory&&)factory).underlying);
  }
};

template <std::ptrdiff_t I, typename Space, typename Factory>
constexpr auto on_extent(Space&& space, Factory&& factory) {
  using T = __mdspace_binder<
    std::remove_cvref_t<Space>, I, std::remove_cvref_t<Factory>
  >;
  return T((Space&&)space, (Factory&&)factory);
}

template <std::ptrdiff_t I, typename Factory>
constexpr auto on_extent(Factory&& factory) {
  using T = __on_extent_factory<constant<I>, std::remove_cvref_t<Factory>>;
  return T((Factory&&)factory);
}

///////////////////////////////////////////////////////////////////////////////////

// TODO: Make this a proper implementation of `std::extents`.
template <std::ptrdiff_t N>
struct extents {
private:
  std::array<std::ptrdiff_t, N> data;

public:
  template <typename... Ts>
    requires(std::convertible_to<Ts, std::ptrdiff_t> && ...)
  explicit constexpr extents(Ts&&... ts) : data{(Ts&&)ts...} {
    static_assert(sizeof...(Ts) == N);
  }

  constexpr extents(extents const& other) : data(other.data) {}
  constexpr extents(extents&& other) : data(std::move(other.data)) {}

  template <typename OuterTuple>
  struct extent_range;

  template <typename... Outer>
  struct extent_range<std::tuple<Outer...>> {
    struct iterator : std::forward_iterator_tag {
      using value_type = std::tuple<std::ptrdiff_t, Outer...>;
      using difference_type = std::ptrdiff_t;

    private:
      value_type idx;

    public:
      template <typename OuterTuple>
      constexpr iterator(std::ptrdiff_t ext, OuterTuple&& outer)
        : idx(std::tuple_cat(std::make_tuple(ext), (OuterTuple&&)outer))
      {}

      constexpr iterator() = default;

      constexpr iterator(iterator const& other) : idx(other.idx) {}
      constexpr iterator(iterator&& other) : idx(std::move(other.idx)) {}

      constexpr iterator& operator=(iterator const& other) {
        idx = other.idx;
        return *this;
      }
      constexpr iterator& operator=(iterator&& other)  {
        idx = std::move(other.idx);
        return *this;
      }

      constexpr iterator& operator++() {
        ++std::get<0>(idx);
        return *this;
      }

      constexpr iterator operator++(int) {
        iterator tmp(*this);
        ++(*this);
        return tmp;
      }

      constexpr iterator operator+(difference_type n) const {
        iterator tmp(*this);
        std::get<0>(tmp.idx) += n;
        return tmp;
      }

      constexpr auto operator*() { return idx; }
      constexpr auto operator*() const { return idx; }

      constexpr bool operator==(iterator const& it) const {
        return idx == it.idx;
      }
      constexpr bool operator!=(iterator const& it) const {
        return idx != it.idx;
      }
    };

  private:
    iterator first, last;

  public:
    template <typename OuterTuple>
    constexpr extent_range(std::ptrdiff_t ext, OuterTuple&& outer)
      : first(0, outer), last(ext, outer)
    {}

    constexpr extent_range() = default;

    constexpr extent_range(extent_range const& other)
      : first(other.first), last(other.last) {}
    constexpr extent_range(extent_range&& other)
      : first(std::move(other.first)), last(std::move(other.last)) {}

    constexpr extent_range& operator=(extent_range const& other) {
      first = other.first;
      last = other.last;
      return *this;
    }
    constexpr extent_range& operator=(extent_range&& other) {
      first = std::move(other.first);
      last = std::move(other.last);
      return *this;
    }

    constexpr auto begin() const {
      return first;
    }

    constexpr auto end() const {
      return last;
    }

    constexpr auto size() const {
      return tuple_reduce(*last, 1, std::multiplies{});
    }
  };

  static_assert(std::ranges::forward_range<extent_range<std::tuple<>>>);

  template <std::ptrdiff_t I, typename OuterTuple>
  friend constexpr auto mdrange(extents space, OuterTuple&& outer) {
    static_assert(I < N);
    using T = extent_range<std::remove_cvref_t<OuterTuple>>;
    return T(space.data[I], (OuterTuple&&)outer);
  }

  template <typename Factory>
  friend constexpr auto operator|(extents space, Factory&& factory) {
    return __mdspace_bind(space, (Factory&&)factory);
  }
};

template <std::ptrdiff_t M>
struct mdrank_t<extents<M>> : std::integral_constant<std::ptrdiff_t, M> {};

///////////////////////////////////////////////////////////////////////////////////

#include <vector>
#include <iterator>

void memset_2d_reference(
    index_type           N
  , index_type           M
  , double* __restrict__ A
    ) noexcept
{
    BOOST_ASSUME((N % 32) == 0);
    BOOST_ASSUME((M % 32) == 0);
    BOOST_ASSUME_ALIGNED(A, 32);

    BOOST_DEMAND_VECTORIZATION
    for (index_type j = 0; j != M; ++j)
        BOOST_DEMAND_VECTORIZATION
        for (index_type i = 0; i != N; ++i)
            A[i + j * N] = 0.0;
}

void memset_2d_index_forward_range_based_for_loop(
    index_type N
  , index_type M
  , std::vector<double>& vA
    )
{
    double* __restrict__ A = vA.data();

    BOOST_ASSUME((N % 32) == 0);
    BOOST_ASSUME((M % 32) == 0);
    BOOST_ASSUME_ALIGNED(A, 32);

    BOOST_DEMAND_VECTORIZATION
    for (auto pos : index_2d_iterator_sentinel_range(N, M))
        A[pos[0] + pos[1] * N] = 0.0;
}

void memset_2d_index_forward_iterators(
    index_type N
  , index_type M
  , std::vector<double>& vA
    )
{
    double* __restrict__ A = vA.data();

    BOOST_ASSUME((N % 32) == 0);
    BOOST_ASSUME((M % 32) == 0);
    BOOST_ASSUME_ALIGNED(A, 32);

    auto&& r   = index_2d_iterator_sentinel_range(N, M);
    auto first = r.begin();
    auto last  = r.end();

    BOOST_DEMAND_VECTORIZATION
    for (; first != last; ++first)
    {
        auto pos = *first;
        A[pos[0] + pos[1] * N] = 0.0;
    }
}

void memset_2d_index_random_access_iterators(
    index_type N
  , index_type M
  , std::vector<double>& vA
    )
{
    double* __restrict__ A = vA.data();

    BOOST_ASSUME((N % 32) == 0);
    BOOST_ASSUME((M % 32) == 0);
    BOOST_ASSUME_ALIGNED(A, 32);

    auto&& r   = index_2d_iterator_sentinel_range(N, M);
    auto first = r.begin();
    auto last  = r.end();

    index_type dist = last - first;
    BOOST_DEMAND_VECTORIZATION
    for (index_type d = 0; d < dist; ++d)
    {
        auto pos = first[d];
        A[pos[0] + pos[1] * N] = 0.0;
    }
}

void memset_2d_index_known_distance_iterators(
    index_type N
  , index_type M
  , std::vector<double>& vA
    )
{
    double* __restrict__ A = vA.data();

    BOOST_ASSUME((N % 32) == 0);
    BOOST_ASSUME((M % 32) == 0);
    BOOST_ASSUME_ALIGNED(A, 32);

    auto&& r   = index_2d_iterator_sentinel_range(N, M);
    auto first = r.begin();
    auto last  = r.end();

    index_type dist = last - first;
    BOOST_DEMAND_VECTORIZATION
    for (index_type d = 0; d < dist; ++d, ++first)
    {
        auto pos = *first;
        A[pos[0] + pos[1] * N] = 0.0;
    }
}

void memset_2d_storage_iterator(
    index_type N
  , index_type M
  , std::vector<double>& vA
    )
{
    double* __restrict__ A = vA.data();

    BOOST_ASSUME((N % 32) == 0);
    BOOST_ASSUME((M % 32) == 0);
    BOOST_ASSUME_ALIGNED(A, 32);

    auto&& r   = storage_2d_range(N, M);
    auto first = r.begin();
    auto last  = r.end();

    BOOST_DEMAND_VECTORIZATION
    for (; first != last; ++first)
    {
        auto pos = *first;
        A[pos[0] + pos[1] * N] = 0.0;
    }
}

#if 0 //!defined(__INTEL_COMPILER)
void memset_2d_cartesian_product_iota(
    index_type N
  , index_type M
  , std::vector<double>& vA
    )
{
    double* __restrict__ A = vA.data();

    BOOST_ASSUME((N % 32) == 0);
    BOOST_ASSUME((M % 32) == 0);
    BOOST_ASSUME_ALIGNED(A, 32);

    BOOST_DEMAND_VECTORIZATION
    for (auto [i, j] : ranges::views::cartesian_product(
                         std::views::iota(0, N)
                       , std::views::iota(0, M)
                       ))
        A[i + j * N] = 0.0;
}
#endif

#if !(defined(__INTEL_LLVM_COMPILER) || defined(__INTEL_COMPILER))
void memset_2d_index_generator(
    index_type N
  , index_type M
  , std::vector<double>& vA
    )
{
    double* __restrict__ A = vA.data();

    BOOST_ASSUME((N % 32) == 0);
    BOOST_ASSUME((M % 32) == 0);
    BOOST_ASSUME_ALIGNED(A, 32);

    BOOST_DEMAND_VECTORIZATION
    for (auto pos : generate_indices(N, M))
        A[pos[0] + pos[1] * N] = 0.0;
}
#endif

void memset_2d_mdfor(
    index_type N
  , index_type M
  , std::vector<double>& vA
    )
{
    double* __restrict__ A = vA.data();

    BOOST_ASSUME((N % 32) == 0);
    BOOST_ASSUME((M % 32) == 0);
    BOOST_ASSUME_ALIGNED(A, 32);

    mdfor(extents<2>(N, M), [=] (auto i, auto j) {
      A[i + j * N] = 0.0;
    });
}

void memset_diagonal_2d_reference(
    index_type           N
  , index_type           M
  , double* __restrict__ A
    ) noexcept
{
    BOOST_ASSUME((N % 32) == 0);
    BOOST_ASSUME((M % 32) == 0);
    BOOST_ASSUME_ALIGNED(A, 32);

    BOOST_DEMAND_VECTORIZATION
    for (index_type j = 0; j != M; ++j)
        BOOST_DEMAND_VECTORIZATION
        for (index_type i = 0; i != N; ++i)
            if (i == j) A[i + j * N] = 0.0;
}

void memset_diagonal_2d_mdfor_filter(
    index_type N
  , index_type M
  , std::vector<double>& vA // TODO: Should be a span.
    )
{
    double* __restrict__ A = vA.data();

    BOOST_ASSUME((N % 32) == 0);
    BOOST_ASSUME((M % 32) == 0);
    BOOST_ASSUME_ALIGNED(A, 32);

    mdfor(
      extents<2>(N, M)
    | std::views::filter(
        [] (auto idx) {
          auto [i, j] = idx;
          return i == j;
        })
    , [=] (auto i, auto j) {
        A[i + j * N] = 0.0;
      });
}

void memset_diagonal_2d_mdfor_filter_o(
    index_type N
  , index_type M
  , std::vector<double>& vA // TODO: Should be a span.
    )
{
    double* __restrict__ A = vA.data();

    BOOST_ASSUME((N % 32) == 0);
    BOOST_ASSUME((M % 32) == 0);
    BOOST_ASSUME_ALIGNED(A, 32);

    mdfor(
      extents<2>(N, M)
    | filter_o(
        [] (auto idx) {
          auto [i, j] = idx;
          return i == j;
        })
    , [=] (auto i, auto j) {
        A[i + j * N] = 0.0;
      });
}

void memset_diagonal_3d_reference(
    index_type           N
  , index_type           M
  , index_type O
  , double* __restrict__ A
    ) noexcept
{
    BOOST_ASSUME((N % 32) == 0);
    BOOST_ASSUME((M % 32) == 0);
    BOOST_ASSUME_ALIGNED(A, 32);

    BOOST_DEMAND_VECTORIZATION
    for (index_type k = 0; k != O; ++k)
        BOOST_DEMAND_VECTORIZATION
        for (index_type j = 0; j != M; ++j)
            BOOST_DEMAND_VECTORIZATION
            for (index_type i = 0; i != N; ++i)
                if (i == j) A[i + j * N + k * N * M] = 0.0;
}

void memset_plane_3d_mdfor_filter_o(
    index_type N
  , index_type M
  , index_type O
  , std::vector<double>& vA // TODO: Should be a span.
    )
{
    double* __restrict__ A = vA.data();

    BOOST_ASSUME((N % 32) == 0);
    BOOST_ASSUME((M % 32) == 0);
    BOOST_ASSUME((O % 32) == 0);
    BOOST_ASSUME_ALIGNED(A, 32);

    mdfor(
      extents<3>(N, M, O)
    | on_extent<1>(filter_o(
        [] (auto idx) {
          auto [i, j] = idx;
          return i == j;
        }))
    , [=] (auto i, auto j, auto k) {
        A[i + j * N + k * N * M] = 0.0;
      });
}

int main() {}
