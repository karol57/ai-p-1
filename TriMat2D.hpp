#pragma once

#include <cstdlib>
#include <algorithm>
#include <ostream>
#include <utility>

[[nodiscard]] constexpr inline std::size_t triangularNumber(std::size_t n) { return (n*n+n) / 2u; }

template<typename T, std::size_t S>
class TriMatrix
{
public:
    struct Index { std::size_t r; std::size_t c; };

    TriMatrix() noexcept = default;
    constexpr TriMatrix(T init) noexcept { std::fill_n(m_d, _elemCount, init); }

    constexpr T& operator[](Index i) noexcept { return m_d[_offset(i)]; }
    constexpr const T& operator[](Index i) const noexcept { return m_d[_offset(i)]; }

    constexpr static std::size_t size() noexcept { return S; }

    constexpr auto begin() noexcept { return std::begin(m_d); }
    constexpr auto end() noexcept { return std::end(m_d); }

    template<typename U>
    constexpr TriMatrix& operator*=(U&& v)
    {
        for (auto& val : m_d)
            val *= std::forward<U>(v);
        return * this;
    }

    constexpr TriMatrix& operator+=(const TriMatrix& v)
    {
        for (size_t i = 0u; i < _elemCount; ++i)
            m_d[i] += v.m_d[i];
        return * this;
    }
private:
    static constexpr std::size_t _elemCount = triangularNumber(S);
    static std::size_t _offset(Index i)
    {
        if (i.c > i.r)
            std::swap(i.r, i.c);

        return triangularNumber(i.r) + i.c;
    }

    friend
    std::ostream& operator<<(std::ostream& _s, const TriMatrix& m)
    {
        std::ostream* s = &_s;
        for (std::size_t i = 0u; i < S; ++i)
        {
            for (std::size_t j = 0u; j < S; ++j)
            {
                s = &(*s << m[{i, j}] << ' ');
            }
            if (i != S - 1u)
                s = &(*s << std::endl);
        }
        return *s;
    }

    T m_d[_elemCount] = {};
};