#include "Map.hpp"
#include <cmath>

template<size_t N>
constexpr TriMatrix<double, N> genEuclideanDistWeights(const vec2d (&verices)[N])
{
    TriMatrix<double, N> result;

    for (size_t r = 0u; r < N; ++r)
        for (size_t c = 0u; c <= r; ++c)
            result[{r, c}] = verices[r].dist2d(verices[c]);

    return result;
}

Map::Map()
    : m_cities {
          { 1,  1 }, // A
          { 2,  2 }, // B
          { 0, 0 }, // C
          { 5,  3 }, // D
          { 8, 1  }, // E
          { 7, 4 }//, // F
          //{ 4,  2 }  // G
      }
    , m_distances{ genEuclideanDistWeights(m_cities) }
    , m_pheromone{ 1.0 }
    , m_preparedPheromone{ 0.0 }

{}

std::vector<std::size_t> Map::genIndexes() const noexcept
{
    std::vector<std::size_t> result;
    result.resize(size());
    std::size_t currIdx = 0u;
    for (std::size_t& idx : result)
        idx = currIdx++;
    return result;
}

double Map::travelWeight(std::size_t from, std::size_t to) const noexcept
{
    const double visibility = 1.0 / m_distances[{from, to}];
    const double pheromone = m_pheromone[{from, to}];
    return pow(pheromone, m_pheromoneCoef) * pow(visibility, m_visibilityCoef);
}

void Map::preparePheromone(const std::vector<std::size_t>& path, double pheromone)
{
    auto it = path.cbegin();
    std::size_t lastCityIdx = *it;
    ++it;
    for (;it != path.cend(); ++it)
    {
        const std::size_t currentCityIdx = *it;
        m_preparedPheromone[{lastCityIdx, currentCityIdx}] += pheromone;
    }
    m_preparedPheromone[{lastCityIdx, path.front()}] += pheromone;
}

void Map::updatePheromone() noexcept
{
    m_pheromone *= m_pheromoneStrength;
    m_pheromone += m_preparedPheromone;
}

double Map::calcDistance(const std::vector<std::size_t>& path)
{
    if (path.size() <= 1u)
        return 0.0;

    double dist = 0.0;
    auto it = path.cbegin();

    std::size_t lastCityIdx = *it;
    ++it;
    for (;it != path.cend(); ++it)
    {
        const std::size_t currentCityIdx = *it;
        dist += m_distances[{lastCityIdx, currentCityIdx}];
        lastCityIdx = currentCityIdx;
    }
    dist += m_distances[{lastCityIdx, path.front()}];
    return dist;
}

std::ostream& operator<<(std::ostream& s, const Map& m)
{
    s << "Map:" << std::endl;
    char name = 'A';
    for (const vec2d& p : m.m_cities)
        s << name++ << p << std::endl;
    s << "==============================" << std::endl;
    s << "Distances: " << std::endl;
    s << m.m_distances << std::endl;
    s << "==============================" << std::endl;
    s << "Pheromone: " << std::endl;
    s << m.m_pheromone << std::endl;
    s << "==============================";
    return s;
}
