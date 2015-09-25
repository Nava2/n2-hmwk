
#include <Tile.hpp>

#include <cmath>

Tile::Tile(const uint       sides,
           const QPoint     center,
           const uint       length,
           const QColor&    color)
    : d_color(color), d_selected(false)
{
    const double theta = (2. * M_PI) / sides;
    const double radius = (1.0 * length) / sqrt(2. * (1. - cos(theta)));

    for (uint i = 0; i < sides; ++i) {
        d_polygon << QPointF(center.x() + radius * cos(theta * i),
                             center.y() + radius * sin(theta * i));
    }
}
