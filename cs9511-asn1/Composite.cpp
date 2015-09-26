#include "Composite.hpp"

#include <Composite.hpp>
#include <Tile.hpp>

#include <QList>
#include <QPointF>
#include <QPolygonF>

#include <QMatrix>

Composite::Composite(const QList<QSharedPointer<Tile> > &tiles)
    : Tile()
    , d_tiles(tiles)
{

}

void Composite::selected(const bool selected)
{
    for (auto& tile: d_tiles)
    {
        tile->selected(selected);
    }

    d_selected = selected;
}

const QPolygonF Composite::polygon() const
{
    QPolygonF polygon;
    for (const auto& tile: d_tiles)
    {
        polygon = polygon.united(tile->polygon());
    }

    if (polygon.size() > 3)
    {
        polygon.remove(polygon.size() - 1);
    }

    return polygon;
}


void Composite::rotate(const double deg)
{
    // Need to custom rotate around the center of the composite
    const auto polyCenter = this->center();

    QMatrix trans, rotate, back;
    trans.translate(-polyCenter.x(), -polyCenter.y());

    rotate.rotate(deg);

    back.translate(polyCenter.x(), polyCenter.y());

    for (auto& tile: d_tiles)
    {
        QPolygonF poly = trans.map(tile->d_polygon);
        poly = rotate.map(poly);
        poly = back.map(poly);

        tile->d_polygon = poly;
    }
}

void Composite::translate(const qreal tx, const qreal ty)
{
    for (auto& tile: d_tiles)
    {
        tile->translate(tx, ty);
    }
}
