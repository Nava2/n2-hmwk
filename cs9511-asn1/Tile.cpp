
#include <Tile.hpp>

Tile::Tile(const Triangle &T, const QColor &color)
    : _T(T), _color(color), _selected(false)
{
    _polygon << QPointF(T.P0().x(), T.P0().y())
             << QPointF(T.P1().x(), T.P1().y())
             << QPointF(T.P2().x(), T.P2().y());
}

QColor Tile::color() const
{
    return _color;
}

QPolygonF Tile::polygon() const
{
    return _polygon;
}

bool Tile::selected() const
{
    return _selected;
}

bool& Tile::selected()
{
    return _selected;
}

QDebug operator<<(QDebug debug, const Tile& t)
{
    debug << "Tile{" << "selected=" << t._selected << "colour=" << t._color << "polygon=" << t._polygon << "}";

    return debug;
}
