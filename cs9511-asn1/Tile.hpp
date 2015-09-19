
#include "geometry/E2/Geometry_E2.hpp"
#include "geometry/E2/Triangle_E2.hpp"

#include <QColor>
#include <QPolygonF>

class Tile
{
public:
    typedef Geometry_E2<double> Geometry_E2d;
    typedef Geometry_E2d::Point Point;
    typedef Geometry_E2d::Triangle Triangle;

    Tile(const Triangle& T,
         const QColor& color = Qt::black)
        : _T(T), _color(color)
    {
        _polygon << QPointF(T.P0().x(), T.P0().y())
                 << QPointF(T.P1().x(), T.P1().y())
                 << QPointF(T.P2().x(), T.P2().y());
    }
    QColor color() const { return _color; }
    QPolygonF polygon() const { return _polygon; }
private:
    Triangle _T;
    QColor _color;
    QPolygonF _polygon;
};
