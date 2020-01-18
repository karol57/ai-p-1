#include "Path.hpp"
#include <algorithm>
#include <iterator>
#include <stdexcept>

void Path::setStartingPoint(std::size_t cityIdx)
{
    const auto it = std::find(begin(), end(), cityIdx);
    if (it == end())
        throw std::runtime_error("Path::setStartingPoint: invalid city index.");
    std::rotate(begin(), it, end());
}

std::ostream& operator<<(std::ostream& s, const Path& p)
{
    static const auto transformation = [](std::size_t cityIdx) -> char { return 'A' + cityIdx; };

    s << "{ ";
    std::transform(p.cbegin(), p.cend(), std::ostream_iterator<char>(s, ", "), transformation);
    s << transformation(p.front()) << " }";
    return s;
}
