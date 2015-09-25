#include "Composite.hpp"

#include <Composite.hpp>
#include <Tile.hpp>

#include <QList>
#include <QPointF>
#include <QPolygonF>

Composite::Composite(const QList<Tile>& tiles)
    : d_tiles(tiles)
{

}

void Composite::selected(const bool selected)
{
    for (auto& tile: d_tiles)
    {
        tile.selected() = selected;
    }
}

const QPolygonF Composite::polygon() const
{
    QPolygonF polygon;
    for (const auto& tile: d_tiles)
    {
        polygon = polygon.united(tile.polygon());
    }

    return polygon;
}
