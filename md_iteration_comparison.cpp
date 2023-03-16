#include <spaces/optimization_hints.hpp>
#include <spaces/index_md_range.hpp>
#include <spaces/storage_md_range.hpp>
#include <spaces/index_generator.hpp>
#include <spaces/views.hpp>
#include <spaces/mdrange.hpp>
#include <spaces/for_each.hpp>
#include <spaces/on_extent.hpp>
#include <spaces/cursor.hpp>

#include <vector>

using namespace spaces;

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

void memset_2d_md_for_each(
    index_type N
  , index_type M
  , std::vector<double>& vA
    )
{
    double* __restrict__ A = vA.data();

    SPACES_ASSUME((N % 32) == 0);
    SPACES_ASSUME((M % 32) == 0);
    SPACES_ASSUME_ALIGNED(A, 32);

    for_each(cursor<2>(N, M), [=] (auto i, auto j) {
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

void memset_diagonal_2d_for_each_filter(
    index_type N
  , index_type M
  , std::vector<double>& vA // TODO: Should be a span.
    )
{
    double* __restrict__ A = vA.data();

    SPACES_ASSUME((N % 32) == 0);
    SPACES_ASSUME((M % 32) == 0);
    SPACES_ASSUME_ALIGNED(A, 32);

    for_each(
      cursor<2>(N, M)
    | std::views::filter(
        [] (auto idx) {
          auto [i, j] = idx;
          return i == j;
        })
    , [=] (auto i, auto j) {
        A[i + j * N] = 0.0;
      });
}

void memset_diagonal_2d_for_each_filter_o(
    index_type N
  , index_type M
  , std::vector<double>& vA // TODO: Should be a span.
    )
{
    double* __restrict__ A = vA.data();

    SPACES_ASSUME((N % 32) == 0);
    SPACES_ASSUME((M % 32) == 0);
    SPACES_ASSUME_ALIGNED(A, 32);

    for_each(
      cursor<2>(N, M)
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

void memset_plane_3d_md_for_each_filter_o(
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

    for_each(
      cursor<3>(N, M, O)
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
