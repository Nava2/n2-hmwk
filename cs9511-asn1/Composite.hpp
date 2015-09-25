#ifndef COMPOSITE_H
#define COMPOSITE_H

#include <QList>

#include <Tile.hpp>

class QPolygonF;

class Composite final
{
public:
    Composite(const QList<Tile>& tiles = QList<Tile>());

    virtual ~Composite() { }

    const QList<Tile> tiles() const;

    QList<Tile>& tiles();

    bool selected() const;
    void selected(const bool selected);

    const QPolygonF polygon() const;

    bool containsTile(const Tile &tile) const;

private:
    QList<Tile> d_tiles;
    bool        d_selected;
};

inline
QList<Tile>& Composite::tiles()
{
    return d_tiles;
}

inline
const QList<Tile> Composite::tiles() const
{
    return d_tiles;
}

inline
bool Composite::selected() const
{
    return d_selected;
}

inline
bool Composite::containsTile(const Tile& tile) const
{
    return d_tiles.contains(tile);
}


#endif // COMPOSITE_H
