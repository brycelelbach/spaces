#include <spaces/optimization_hints.hpp>
#include <spaces/index_md_range.hpp>
#include <spaces/storage_md_range.hpp>
#include <spaces/index_generator.hpp>

using spaces::index_type;

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
    SPACES_DEMAND_VECTORIZATION
    for (auto&& e: mdrange<I>(space, (OuterTuple&&)outer)) {
      invoke_o(
        [&] (auto&& t) {
          __mdfor<I - 1>((MDSpace&&)space, f, std::forward<decltype(t)>(t));
        }, std::forward<decltype(e)>(e));
    }
  } else {
    SPACES_DEMAND_VECTORIZATION
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
    SPACES_ASSUME((N % 32) == 0);
    SPACES_ASSUME((M % 32) == 0);
    SPACES_ASSUME_ALIGNED(A, 32);

    SPACES_DEMAND_VECTORIZATION
    for (index_type j = 0; j != M; ++j)
        SPACES_DEMAND_VECTORIZATION
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

    SPACES_ASSUME((N % 32) == 0);
    SPACES_ASSUME((M % 32) == 0);
    SPACES_ASSUME_ALIGNED(A, 32);

    SPACES_DEMAND_VECTORIZATION
    for (auto pos : spaces::index_2d_range(N, M))
        A[pos[0] + pos[1] * N] = 0.0;
}

void memset_2d_index_forward_iterators(
    index_type N
  , index_type M
  , std::vector<double>& vA
    )
{
    double* __restrict__ A = vA.data();

    SPACES_ASSUME((N % 32) == 0);
    SPACES_ASSUME((M % 32) == 0);
    SPACES_ASSUME_ALIGNED(A, 32);

    auto&& r   = spaces::index_2d_range(N, M);
    auto first = r.begin();
    auto last  = r.end();

    SPACES_DEMAND_VECTORIZATION
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

    SPACES_ASSUME((N % 32) == 0);
    SPACES_ASSUME((M % 32) == 0);
    SPACES_ASSUME_ALIGNED(A, 32);

    auto&& r   = spaces::index_2d_range(N, M);
    auto first = r.begin();
    auto last  = r.end();

    index_type dist = last - first;
    SPACES_DEMAND_VECTORIZATION
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

    SPACES_ASSUME((N % 32) == 0);
    SPACES_ASSUME((M % 32) == 0);
    SPACES_ASSUME_ALIGNED(A, 32);

    auto&& r   = spaces::index_2d_range(N, M);
    auto first = r.begin();
    auto last  = r.end();

    index_type dist = last - first;
    SPACES_DEMAND_VECTORIZATION
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

    SPACES_ASSUME((N % 32) == 0);
    SPACES_ASSUME((M % 32) == 0);
    SPACES_ASSUME_ALIGNED(A, 32);

    auto&& r   = spaces::storage_2d_range(N, M);
    auto first = r.begin();
    auto last  = r.end();

    SPACES_DEMAND_VECTORIZATION
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

    SPACES_ASSUME((N % 32) == 0);
    SPACES_ASSUME((M % 32) == 0);
    SPACES_ASSUME_ALIGNED(A, 32);

    SPACES_DEMAND_VECTORIZATION
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

    SPACES_ASSUME((N % 32) == 0);
    SPACES_ASSUME((M % 32) == 0);
    SPACES_ASSUME_ALIGNED(A, 32);

    SPACES_DEMAND_VECTORIZATION
    for (auto pos : spaces::generate_indices(N, M))
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

    SPACES_ASSUME((N % 32) == 0);
    SPACES_ASSUME((M % 32) == 0);
    SPACES_ASSUME_ALIGNED(A, 32);

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
    SPACES_ASSUME((N % 32) == 0);
    SPACES_ASSUME((M % 32) == 0);
    SPACES_ASSUME_ALIGNED(A, 32);

    SPACES_DEMAND_VECTORIZATION
    for (index_type j = 0; j != M; ++j)
        SPACES_DEMAND_VECTORIZATION
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

    SPACES_ASSUME((N % 32) == 0);
    SPACES_ASSUME((M % 32) == 0);
    SPACES_ASSUME_ALIGNED(A, 32);

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

    SPACES_ASSUME((N % 32) == 0);
    SPACES_ASSUME((M % 32) == 0);
    SPACES_ASSUME_ALIGNED(A, 32);

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
    SPACES_ASSUME((N % 32) == 0);
    SPACES_ASSUME((M % 32) == 0);
    SPACES_ASSUME_ALIGNED(A, 32);

    SPACES_DEMAND_VECTORIZATION
    for (index_type k = 0; k != O; ++k)
        SPACES_DEMAND_VECTORIZATION
        for (index_type j = 0; j != M; ++j)
            SPACES_DEMAND_VECTORIZATION
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

    SPACES_ASSUME((N % 32) == 0);
    SPACES_ASSUME((M % 32) == 0);
    SPACES_ASSUME((O % 32) == 0);
    SPACES_ASSUME_ALIGNED(A, 32);

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
