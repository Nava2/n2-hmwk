#ifndef TILE_HPP
#define TILE_HPP

#include <QColor>
#include <QDebug>
#include <QPointF>
#include <QPolygonF>

class Tile
{
public:

    static const uint DEFAULT_LENGTH = 25;

    Tile(const uint     sides,
         const QPoint  center,
         const uint     length,
         const QColor&  color = Qt::black);

    static
    Tile triangle(const QPoint   center,
                  const QColor&  color = Qt::black,
                  const uint     length = DEFAULT_LENGTH);

    static
    Tile square(const QPoint     center,
                const QColor&    color = Qt::black,
                const uint       length = DEFAULT_LENGTH);

    static
    Tile pentagon(const QPoint   center,
                  const QColor&  color = Qt::black,
                  const uint     length = DEFAULT_LENGTH);

    static
    Tile hexagon(const QPoint    center,
                 const QColor&   color = Qt::black,
                 const uint      length = DEFAULT_LENGTH);

    QColor color() const;

    QPolygonF polygon() const;

    QPolygonF& polygon();

    bool selected() const;

    bool& selected();

    bool operator==(const Tile& other) const;

    friend
    QDebug operator<<(QDebug debug, const Tile &t);

private:
    /// Color of the tile
    QColor      d_color;

    /// Polygon that holds the drawing points.
    QPolygonF   d_polygon;

    bool        d_selected;

};

inline
Tile Tile::triangle(const QPoint    center,
                    const QColor&   color,
                    const uint      length)
{
    return Tile(3, center, length, color);
}

inline
Tile Tile::square(const QPoint  center,
                  const QColor& color,
                  const uint    length)
{
    return Tile(4, center, length, color);
}

inline
Tile Tile::pentagon(const QPoint    center,
                    const QColor&   color,
                    const uint      length)
{
    return Tile(5, center, length, color);
}

inline
Tile Tile::hexagon(const QPoint     center,
                   const QColor&    color,
                   const uint       length)
{
    return Tile(6, center, length, color);
}

inline
QColor Tile::color() const
{
    return d_color;
}

inline
QPolygonF Tile::polygon() const
{
    return d_polygon;
}

inline
QPolygonF& Tile::polygon()
{
    return d_polygon;
}

inline
bool Tile::selected() const
{
    return d_selected;
}

inline
bool& Tile::selected()
{
    return d_selected;
}

inline
bool Tile::operator ==(const Tile& other) const
{
    return this->d_selected == other.d_selected
            && this->d_color == other.d_color
            && this->d_polygon == other.d_polygon;
}

inline
QDebug operator<<(QDebug debug, const Tile& t)
{
    debug.nospace() << "Tile[0x" << hex << (size_t) &t << "]{" << "selected=" << t.d_selected << ", colour=" << t.d_color << ", polygon=" << t.d_polygon << "}";

    return debug;
}

#endif // TILE_HPP
