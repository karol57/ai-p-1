#include <iostream>
#include <iomanip>
#include <random>
#include <array>
#include <functional>
#include <stdexcept>
#include <iterator>

#include "vec2d.hpp"
#include "TriMat2D.hpp"
template<size_t N>
constexpr TriMatrix<double, N> genDistances(const vec2d (&map)[N])
{
    TriMatrix<double, N> result;

    for (size_t r = 0u; r < N; ++r)
        for (size_t c = 0u; c <= r; ++c)
            result[{r, c}] = map[r].dist2d(map[c]);

    return result;
}

using namespace std::placeholders;

class Path : public std::vector<std::size_t>
{
public:
    friend std::ostream& operator<<(std::ostream& s, const Path& p)
    {
        static const auto transformation = [](std::size_t cityIdx) -> char { return 'A' + cityIdx; };

        s << "{ ";
        std::transform(p.cbegin(), p.cend(), std::ostream_iterator<char>(s, ", "), transformation);
        s << transformation(p.front()) << " }";
        return s;
    }
};

class Map
{
public:
    Map();
    std::size_t size() const noexcept { return std::size(m_cities); }

    std::vector<std::size_t> genIndexes() const noexcept
    {
        std::vector<std::size_t> result;
        result.resize(size());
        std::size_t currIdx = 0u;
        for (std::size_t& idx : result)
            idx = currIdx++;
        return result;
    }

    double travelWeight(std::size_t from, std::size_t to) const noexcept
    {
        const double visibility = 1.0 / m_distances[{from, to}];
        const double pheromone = m_pheromone[{from, to}];
        return pow(pheromone, m_pheromoneCoef) * pow(visibility, m_visibilityCoef);
    }

    void preparePheromone(const std::vector<std::size_t>& path, double pheromone)
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

    void updatePheromone()
    {
        m_pheromone *= m_pheromoneStrength;
        m_pheromone += m_preparedPheromone;
    }

    double calcDistance(const std::vector<std::size_t>& path)
    {
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

    friend std::ostream& operator<<(std::ostream& s, const Map& m)
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
private:

    const vec2d m_cities[7u];
    const TriMatrix<double, 7u> m_distances;
    TriMatrix<double, 7u> m_pheromone;
    TriMatrix<double, 7u> m_preparedPheromone;

    double m_pheromoneCoef = 1;
    double m_visibilityCoef = 5;
    double m_pheromoneStrength = 0.5;
};

Map::Map()
    : m_cities {
          { 1,  1 }, // A
          { 5,  8 }, // B
          { 7, 12 }, // C
          { 2,  9 }, // D
          { 7,  2 }, // E
          { 1, 12 }, // F
          { 4,  2 }  // G
      }
    , m_distances{ genDistances(m_cities) }
    , m_pheromone{ 1.0 }
    , m_preparedPheromone{ 0.0 }

{}

int main()
{
    Map map;
    std::cout << map << std::endl;

    const std::size_t iterations = 50u;
    const std::size_t ants = map.size();

    std::mt19937_64 rng{ std::random_device{}() };

    Path bestPath;
    double bestDistance = std::numeric_limits<double>::max();
    std::size_t iterationsWithoutBest = 0u;

    std::uniform_int_distribution<std::size_t> cityDist{ 0u, map.size() - 1u };
    std::vector<std::pair<Path, double>> pathes;
    pathes.resize(ants);

    for (std::size_t i = 0u; i < iterations; ++i)
    {
        std::cout << "=== Iteration " << i << " ===" << std::endl;
        for (std::size_t j = 0u; j < ants; ++j)
        {
            std::vector<std::size_t> availableCities = map.genIndexes();
            auto& [path, distance] = pathes[j];

            // Select random city for ant
            cityDist.reset();
            path.clear();
            path.push_back(cityDist(rng));
            availableCities.erase(std::next(availableCities.cbegin(), path.front()));

            while(!availableCities.empty())
            {
                std::vector<double> weights;
                double weightsSum = 0.0;
                for (std::size_t cityIdx : availableCities)
                {
                    weights.emplace_back(map.travelWeight(path.back(), cityIdx));
                    weightsSum += weights.back();
                }

                std::uniform_real_distribution<double> weightsRange{ 0.0, weightsSum };
                double selectedRnd = weightsRange(rng);
                std::size_t seletedWeight = 0u;
                weightsSum = weights.front();
                while (weightsSum < selectedRnd)
                {
                    ++seletedWeight;
                    weightsSum += weights[seletedWeight];
                }
                const size_t selectedCityIdx = availableCities[seletedWeight];
                availableCities.erase(std::next(availableCities.cbegin(), seletedWeight));
                path.push_back(selectedCityIdx);
            }

            distance = map.calcDistance(path);
        }


        bool hasBest = false;
        double distanceSum = 0.0;
        for (const auto& [path, distance] : pathes)
        {
            map.preparePheromone(path, 1.0 / distance);
            std::cout << "Path " << path << " distance: " << distance;
            if (distance < bestDistance)
            {
                bestDistance = distance;
                bestPath = path;
                std::cout << " (*)";
                hasBest = true;
            }
            std::cout << std::endl;
            distanceSum += distance;
        }
        distanceSum /= pathes.size();
        std::cout << "Avg dist: " << distanceSum << std::endl;

        if (hasBest)
            iterationsWithoutBest = 0;
        else {
            ++iterationsWithoutBest;
            if (iterationsWithoutBest > iterations / 10)
            {
                std::cout << "No better solution for " << iterationsWithoutBest << " iterations. Stopping." << std::endl;
                break;
            }
        }

        map.updatePheromone();
    }

    std::cout << "Best path " << bestPath << " distance: " << bestDistance;

    return 0;
}
