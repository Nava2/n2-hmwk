
#include <GameWindow.hpp>
#include <Composite.hpp>
#include <Tile.hpp>

#include <QWidget>
#include <QPainter>

#include <QMouseEvent>
#include <QKeyEvent>

#include <QList>

#include <QDebug>

#include <QMatrix>

#include <cmath>

GameWindow::GameWindow(QWidget * parent)
    : RefinedQWidget(parent, INTERPOLATE_AND_YFLIP)
    , d_currTile(NULL)
{
    d_tiles.push_back(Tile::triangle(QPoint( 20, 190), Qt::red));
    d_tiles.push_back(Tile::triangle(QPoint(120, 190), Qt::green));
    d_tiles.push_back(Tile::triangle(QPoint(220, 190), Qt::blue));

    QObject::connect(this, &GameWindow::sceneChanged, this, &GameWindow::handleMagnets);
}

QRectF GameWindow::getUserRectF() const
{
    return QRectF(QPointF(0.0, 0.0), QPointF(300.0, 200.0));
}

QSize GameWindow::sizeHint() const
{
    return QSize(680, 480);
}

void GameWindow::render(QPaintDevice* device)
{
    QPainter painter(device);
    // userToWidgetTransform is updated only when the QWidget is resized.
    painter.setTransform(userToWidgetTransform);

    for (const Tile& tile : d_tiles)
    {
        painter.setRenderHint(QPainter::Antialiasing, true);

        if (tile.selected())
        {
            painter.setBrush(QBrush(tile.color().lighter()));
            painter.setPen(QPen(tile.color().darker(), 1.0));
        }
        else
        {
            painter.setBrush(QBrush(tile.color()));
            painter.setPen(QPen(tile.color().lighter(), 1.0));
        }
        painter.drawPolygon(tile.polygon());
    }
}

void GameWindow::mouseReleaseEvent(QMouseEvent* event)
{
    qDebug() << "mouseReleaseEvent =" << event->pos();

    if (event->button() == Qt::LeftButton)
    {
        qDebug() << "Left button";

        event->accept();
        const auto& pt = event->pos();
        const auto cvtPt = widgetToUserTransform.map(pt);

        bool selectionChanged = false;
        for (auto& tile : d_tiles)
        {
            bool contained = tile.polygon().containsPoint(cvtPt, Qt::OddEvenFill);
            selectionChanged |= contained ^ tile.selected();
            tile.selected() = contained;

            if (tile.selected())
            {
                d_currTile = &tile;
            }
        }

        if (selectionChanged)
        {
            sceneChanged();
        }
    }
    else
    {
        RefinedQWidget::mouseReleaseEvent(event);
    }
}

void GameWindow::handleSpaceKey()
{
    bool found = false;
    for (int i = 0; !found && i < d_tiles.size(); ++i)
    {
        auto& tile = d_tiles[i];
        if (tile.selected())
        {
           tile.selected() = false;

           int index = (i + 1) % d_tiles.size();
           d_tiles[index].selected() = true;
           d_currTile = &d_tiles[index];
           found = true;
        }
    }

    if (!found)
    {
        d_tiles[0].selected() = true;
        d_currTile = &d_tiles[0];
    }

    update();
}

static const float OFFSET = 1;

void GameWindow::handleCursorKey(const Qt::Key key)
{
    Q_ASSERT(NULL != d_currTile);

    auto& poly = d_currTile->polygon();
    switch (key)
    {
    case Qt::Key_Up:
    {
        poly.translate(0, +OFFSET);
    } break;

    case Qt::Key_Down:
    {
        poly.translate(0, -OFFSET);
    } break;

    case Qt::Key_Left:
    {
        poly.translate(-OFFSET, 0);
    } break;

    case Qt::Key_Right:
    {
        poly.translate(+OFFSET, 0);
    } break;

    default:
    {
        qFatal("Unknown key passed 0x%X", key);
    }
    } // end switch

    sceneChanged();
}

static const double OFFSET_ANGLE = 30.0;

void GameWindow::handleRotateKey(const bool clockwise)
{
    Q_ASSERT(NULL != d_currTile);

    qDebug() << "Rotating" << *d_currTile << (clockwise ? "clockwise" : "counter-clockwise");

    auto& poly = d_currTile->polygon();
    QMatrix rotate;

    // translate, then rotate
    const auto polyCenter = poly.boundingRect().center();
    rotate.translate(polyCenter.x(), polyCenter.y());
    rotate.rotate((clockwise ? -1 : 1) * OFFSET_ANGLE);
    rotate.translate(-1. * polyCenter.x(), -1. * polyCenter.y());

    poly = rotate.map(poly);

    sceneChanged();
}

void GameWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Space:
    {
        event->accept();

        handleSpaceKey();
    } break;

    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_Left:
    case Qt::Key_Right:
    {
        if (!!d_currTile)
        {
            event->accept();

            handleCursorKey(static_cast<Qt::Key>(event->key()));
        }
        else
        {
            qDebug() << "Cursor key pressed, but no Tile selected.";
        }
    } break;

    case Qt::Key_Z:
    case Qt::Key_X:
    {
        if (!!d_currTile)
        {
            event->accept();

            handleRotateKey(event->key() == Qt::Key_X);
        }
        else
        {
            qDebug() << "Rotate key pressed, but no Tile selected.";
        }
    } break;

    default:
    {
        // no-op
    }
    } // end switch

    RefinedQWidget::keyReleaseEvent(event);
}

namespace {
    inline
    qreal distSqr(const QPointF& p1, const QPointF& p2)
    {
        const QPointF pt = p2 - p1;
        return pt.x() * pt.x() + pt.y() * pt.y();
    }

    inline
    qreal dist(const QPointF& p1, const QPointF& p2)
    {
        return sqrt(distSqr(p1, p2));
    }

    inline
    qreal dot(const QPointF& p1, const QPointF& p2)
    {
        return p1.x() * p2.x() + p1.y() * p2.y();
    }

    inline
    double angleBtwn(const QPointF& u1, const QPointF& u2,
                     const QPointF& v1, const QPointF& v2)
    {
        const QPointF u = u2 - u1;
        const QPointF v = v2 - v1;

        const qreal algDot = dot(u, v);
        const qreal uMag = dist(u2, u1);
        const qreal vMag = dist(v2, v1);

        return std::acos(algDot / (uMag * vMag));
    }

    inline
    qreal ptDistance(const QPointF& p, const QPointF& v, const QPointF& w)
    {
        const qreal lenSqr = distSqr(v, w);

        if (lenSqr == 0)
        {   // The same point, thus points are coincident
            return dist(p, v);
        }

        const qreal t = dot(p - v, w - v) / lenSqr;
        if (t < 0.0)
        {
            return dist(p, v);
        }
        else if (t > 1.0)
        {
            return dist(p, w);
        }

        // calculate the projection
        const QPointF proj = v + t * (w - v);

        return dist(p, proj);
    }

    inline
    double slope(const QPointF& u, const QPointF& v)
    {
        return (v.y() - u.y()) / (v.x() - u.x());
    }
}

static double MAGNET_THRESHOLD = 5.0;

void GameWindow::magnetize(Tile* const currTile, Tile* const newTile,
                           const QPointF& s, const QPointF* t,
                           const QPointF& u, const QPointF* v)
{
    Q_ASSERT(!!t ^ !!v); // one or the other, but not both

    const QPolygonF& currPoly   = currTile->polygon();
    const QPolygonF& refPoly    = newTile->polygon();

    if (!t)
    {   // we have two reference points, only one current tile, find the correct tile
        for (int i = 0; i < currPoly.size(); ++i)
        {
            if (s == currPoly[i])
            {
                const auto& tPt1 = currPoly[(currPoly.size() + i - 1) % currPoly.size()];
                const auto& tPt2 = currPoly[(i + 1) % currPoly.size()];

                const double alpha1 = angleBtwn(tPt1, s, u, *v);
                const double alpha2 = angleBtwn(tPt2, s, u, *v);

                t = (alpha1 < alpha2) ? &tPt1 : &tPt2;

                break;
            }
        }
    }

    if (!v)
    {   // we have one reference points, two for the current tile, find the correct tile
        for (int i = 0; i < refPoly.size(); ++i)
        {
            if (u == refPoly[i])
            {
                const auto& tPt1 = refPoly[(refPoly.size() + i - 1) % refPoly.size()];
                const auto& tPt2 = refPoly[(i + 1) % refPoly.size()];

                const double alpha1 = angleBtwn(s, *t, u, tPt1);
                const double alpha2 = angleBtwn(s, *t, u, tPt2);

                v = (alpha1 < alpha2) ? &tPt1 : &tPt2;

                break;
            }
        }
    }

    if (   ( s == u  && *t == *v )
        || ( s == *v || *t ==  u ) )
    {
        qDebug() << "Found coincident lines.";
        return;
    }

    const double gamma = angleBtwn(s, *t, u, *v);

    const QPointF* pS, *pT;
    if (    (s.x() < t->x() && u.x() > v->x())
         || (s.x() > t->x() && u.x() < v->x()))
    {   // check that s->t is in the same direction as u->v
        pS = &s;
        pT = t;
    }
    else
    {
        pS = t;
        pT = &s;
    }

    QMatrix rotate;

    const auto polyCenter = currPoly.boundingRect().center();
    const double rotateAngle = std::round((180. * gamma / M_PI) / 30) * 30;
    rotate.translate(polyCenter.x(), polyCenter.y());
    rotate.rotate( rotateAngle );
    rotate.translate(-1. * polyCenter.x(), -1. * polyCenter.y());

    const auto rotatedPoly = currTile->polygon() = rotate.map(currPoly);

    const QPointF* rPt = NULL;

    const double m_uv = slope(u, *v);

    for (int i = 0; i < rotatedPoly.size(); ++i)
    {
        auto pt1 = rotatedPoly[i];
        auto pt2 = rotatedPoly[(i + 1) % rotatedPoly.size()];

        if (std::fabs(slope(pt1, pt2) - m_uv) < 0.001)
        {
            rPt = &pt1;

            break;
        }
        else if (std::fabs(slope(pt2, pt1) - m_uv) < 0.001)
        {
            rPt = &pt2;

            break;
        }
    }

    Q_ASSERT( !!rPt );

    QMatrix trans;
    trans.translate(u.x() - rPt->x(), u.y() - rPt->y());

    currTile->polygon() = trans.map(rotatedPoly);

    qDebug() << "rotateAngle =" << rotateAngle;

}

void GameWindow::handleMagnets()
{
    GameWindow* that = qobject_cast<GameWindow*>(sender());

    Tile* currTile = that->d_currTile;

    if (!!currTile)
    {
        const auto& currPoly = currTile->polygon();

        for (auto& tile : that->d_tiles)
        {
            if (currTile != &tile)
            {
                const auto& tilePoly = tile.polygon();

                for (const auto& cPt : currPoly)
                {
                    for (int j = 0; j < tilePoly.size(); ++j)
                    {
                        const auto& p1 = tilePoly[j];
                        const auto& p2 = tilePoly[(j + 1) % tilePoly.size()];

                        const qreal dist2Line = ptDistance(cPt, p1, p2);
                        if ( dist2Line < MAGNET_THRESHOLD)
                        {
                            that->magnetize(currTile, &tile, cPt, NULL, p1, &p2);
                        }
                    }
                }

                for (const auto& tPt: tilePoly)
                {
                    for (int j = 0; j < currPoly.size(); ++j)
                    {
                        const auto& p1 = currPoly[j];
                        const auto& p2 = currPoly[(j + 1) % currPoly.size()];

                        const qreal dist2Line = ptDistance(tPt, p1, p2);
                        if ( dist2Line < MAGNET_THRESHOLD)
                        {
                            that->magnetize(currTile, &tile, p1, &p2, tPt, NULL);
                        }
                    }
                }

            }
        }
    }

    update();
}



