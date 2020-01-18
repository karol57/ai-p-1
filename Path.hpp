#pragma once

#include <vector>
#include <ostream>

class Path : public std::vector<std::size_t>
{
public:
    void setStartingPoint(std::size_t cityIdx);
    friend std::ostream& operator<<(std::ostream& s, const Path& p);
};
