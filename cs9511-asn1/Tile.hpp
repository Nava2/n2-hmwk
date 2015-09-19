#ifndef TILE_HPP
#define TILE_HPP

#include "geometry/E2/Geometry_E2.hpp"
#include "geometry/E2/Triangle_E2.hpp"

#include <QColor>
#include <QDebug>
#include <QPolygonF>

class Tile
{
public:
    typedef Geometry_E2<double> Geometry_E2d;
    typedef Geometry_E2d::Point Point;
    typedef Geometry_E2d::Triangle Triangle;

    Tile(const Triangle& T,
         const QColor& color = Qt::black);

    QColor color() const;

    QPolygonF polygon() const;

    bool selected() const;

    bool& selected();

    friend
    QDebug operator<<(QDebug debug, const Tile &t);

private:
    Triangle _T;
    QColor _color;
    QPolygonF _polygon;

    bool _selected;

};

#endif // TILE_HPP
