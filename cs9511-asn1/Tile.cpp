
#include <Tile.hpp>

#include <QMatrix>

#include <cmath>

Tile::Tile(const uint       sides,
           const QPoint     center,
           const uint       length,
           const QColor&    color)
    : d_color(color)
    , d_selected(false)
{
    const double theta = (2. * M_PI) / sides;
    const double radius = (1.0 * length) / sqrt(2. * (1. - cos(theta)));

    for (uint i = 0; i < sides; ++i) {
        d_polygon << QPointF(center.x() + radius * cos(theta * i),
                             center.y() + radius * sin(theta * i));
    }
}

Tile::Tile()
    : d_color(Qt::black)
    , d_selected(false)
{

}

void Tile::rotate(const double deg)
{

    const auto polyCenter = d_polygon.boundingRect().center();

    QMatrix trans, rotate, back;
    trans.translate(-polyCenter.x(), -polyCenter.y());

    QPolygonF poly = trans.map(this->d_polygon);

    rotate.rotate(deg);
    poly = rotate.map(poly);

    back.translate(polyCenter.x(), polyCenter.y());
    poly = back.map(poly);

    this->d_polygon = poly;
}

void Tile::translate(const qreal tx, const qreal ty)
{
    QMatrix transMat;
    transMat.translate(tx, ty);

    this->d_polygon = transMat.map(this->d_polygon);
}
