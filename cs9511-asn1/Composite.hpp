#ifndef COMPOSITE_H
#define COMPOSITE_H

#include <QList>

#include <Tile.hpp>

class QPolygonF;

class Composite final : public Tile
{
public:
    Composite(const QList<QSharedPointer<Tile> >& tiles = QList<QSharedPointer<Tile>>());

    virtual ~Composite() { }

    const QList<QSharedPointer<Tile> > tiles() const;

    QList<QSharedPointer<Tile> >& tiles();

    virtual bool selected() const override;
    void selected(const bool selected) override;

    const QPolygonF polygon() const override;

    void rotate(const double deg) override;
    void translate(const qreal tx, const qreal ty) override;

    bool containsTile(const QSharedPointer<Tile> &tile) const;

private:
    QList<QSharedPointer<Tile>> d_tiles;
};

inline
QList<QSharedPointer<Tile> > &Composite::tiles()
{
    return d_tiles;
}

inline
const QList<QSharedPointer<Tile> > Composite::tiles() const
{
    return d_tiles;
}

inline
bool Composite::selected() const
{
    return d_selected;
}

inline
bool Composite::containsTile(const QSharedPointer<Tile>& tile) const
{
    return d_tiles.contains(tile);
}


#endif // COMPOSITE_H
