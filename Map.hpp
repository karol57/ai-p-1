#pragma once

#include "vec2d.hpp"
#include "TriMat2D.hpp"
#include <vector>
#include <ostream>

class Map
{
public:
    Map();
    std::size_t size() const noexcept { return std::size(m_cities); }

    std::vector<std::size_t> genIndexes() const noexcept;

    double travelWeight(std::size_t from, std::size_t to) const noexcept;

    void preparePheromone(const std::vector<std::size_t>& path, double pheromone);

    void updatePheromone() noexcept;

    double calcDistance(const std::vector<std::size_t>& path);

    friend std::ostream& operator<<(std::ostream& s, const Map& m);
private:

    const vec2d m_cities[7u];
    const TriMatrix<double, 7u> m_distances;
    TriMatrix<double, 7u> m_pheromone;
    TriMatrix<double, 7u> m_preparedPheromone;

    double m_pheromoneCoef = 1;
    double m_visibilityCoef = 5;
    double m_pheromoneStrength = 0.5;
};
