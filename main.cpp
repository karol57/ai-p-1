#include <iostream>
#include <random>
#include <iterator>

#include "Path.hpp"
#include "Map.hpp"

int main()
{
    Map map;
    std::cout << map << std::endl;

    const std::size_t startingPoint = 'C' - 'A';
    const std::size_t iterations = 100u;
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

    bestPath.setStartingPoint(startingPoint);
    std::cout << "Best path " << bestPath << " distance: " << bestDistance << std::endl;

    return 0;
}
