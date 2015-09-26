#ifndef TILE_HPP
#define TILE_HPP

#include <QColor>
#include <QDebug>
#include <QPointF>
#include <QPolygonF>
#include <QSharedPointer>

#include <GeometryUtils.hpp>

// Forward
class Composite;

class Tile
{
public:

    static const uint   DEFAULT_LENGTH  = 25;

    static constexpr
    const double OFFSET_ANGLE    = 30.0;

    Tile(const uint     sides,
         const QPoint  center,
         const uint     length,
         const QColor&  color = Qt::black);

    static
    QSharedPointer<Tile> shape(const QPoint center,
                               const uint sides,
                               const QColor& color  = Qt::black,
                               const uint length    = DEFAULT_LENGTH);

    static
    QSharedPointer<Tile> triangle(const QPoint   center,
                                  const QColor&  color = Qt::black,
                                  const uint     length = DEFAULT_LENGTH);

    static
     QSharedPointer<Tile> square(const QPoint     center,
                                 const QColor&    color = Qt::black,
                                 const uint       length = DEFAULT_LENGTH);

    static
     QSharedPointer<Tile> pentagon(const QPoint   center,
                                   const QColor&  color = Qt::black,
                                   const uint     length = DEFAULT_LENGTH);

    static
     QSharedPointer<Tile> hexagon(const QPoint    center,
                                  const QColor&   color = Qt::black,
                                  const uint      length = DEFAULT_LENGTH);

    virtual ~Tile() { }

    virtual QColor color() const;

    virtual const QPolygonF polygon() const;

    const QPointF center() const;
    double area() const;

    virtual bool selected() const;

    virtual void selected(const bool selected);

    virtual void rotate(const double deg);
    virtual void translate(const qreal tx, const qreal ty);

    bool operator==(const Tile& other) const;

    friend
    QDebug operator<<(QDebug debug, const Tile &t);

protected:
    Tile();

    /// Color of the tile
    QColor      d_color;

    /// Polygon that holds the drawing points.
    QPolygonF   d_polygon;

    bool        d_selected;

    // It's 8:02 PM, this is bad, I know it's bad, I'm sorry.
    friend class Composite;

};

inline
QSharedPointer<Tile> Tile::shape(const QPoint    center,
                                 const uint      sides,
                                 const QColor&   color,
                                 const uint      length)
{
    return QSharedPointer<Tile>(new Tile(sides, center, length, color));
}

inline
QSharedPointer<Tile> Tile::triangle(const QPoint    center,
                                    const QColor&   color,
                                    const uint      length)
{
    return Tile::shape(center, 3, color, length);
}

inline
QSharedPointer<Tile> Tile::square(const QPoint  center,
                                  const QColor& color,
                                  const uint    length)
{
    return Tile::shape(center, 4, color, length);
}

inline
QSharedPointer<Tile> Tile::pentagon(const QPoint    center,
                                    const QColor&   color,
                                    const uint      length)
{
    return Tile::shape(center, 5, color, length);
}

inline
QSharedPointer<Tile> Tile::hexagon(const QPoint     center,
                                   const QColor&    color,
                                   const uint       length)
{
    return Tile::shape(center, 6, color, length);
}

inline
QColor Tile::color() const
{
    return d_color;
}

inline
const QPolygonF Tile::polygon() const
{
    return d_polygon;
}

inline
bool Tile::selected() const
{
    return d_selected;
}

inline
void Tile::selected(const bool selected)
{
    d_selected = selected;
}

inline
bool Tile::operator ==(const Tile& other) const
{
    return this->d_selected == other.d_selected
            && this->d_color == other.d_color
            && this->d_polygon == other.d_polygon;
}

inline
const QPointF Tile::center() const
{
    const auto poly = this->polygon();
    QPointF center;

    GeometryUtils::polygonCentroid(poly, &center, NULL);

    return center;
}

inline
double Tile::area() const
{
    const auto poly = this->polygon();
    double area;

    GeometryUtils::polygonCentroid(poly, NULL, &area);

    return area;
}

inline
QDebug operator<<(QDebug debug, const Tile& t)
{
    debug.nospace() << "Tile[0x" << hex << (size_t) &t << "]{" << "selected=" << t.d_selected << ", colour=" << t.d_color << ", polygon=" << t.d_polygon << "}";

    return debug;
}

inline uint qHash(const Tile &key)
{
    uint hash = qHash(key.color().name()) ^ (key.selected() ? 1 : 0 );
    for (const auto& pt: key.polygon())
    {
        hash += 31 * (qHash(pt.x()) ^ qHash(pt.y()));
    }

    return hash;
}

#endif // TILE_HPP
