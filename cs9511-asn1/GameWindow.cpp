
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
    d_tiles.push_back(Tile::triangle(QPoint( 70, 190), Qt::green));
    d_tiles.push_back(Tile::square(  QPoint(120, 190), Qt::blue));

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

    for (const auto& tile : d_tiles)
    {
        painter.setRenderHint(QPainter::Antialiasing, true);

        if (tile->selected())
        {
            painter.setBrush(QBrush(tile->color().lighter()));
            painter.setPen(QPen(tile->color().darker(), 1.0));
        }
        else
        {
            painter.setBrush(QBrush(tile->color()));
            painter.setPen(QPen(tile->color().lighter(), 1.0));
        }

        painter.drawPolygon(tile->polygon());
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

        QSet<QSharedPointer<Tile> > tilesInComposites;

        // check composites
        for (auto& comp: d_composites) {
            for (auto& tile: comp->tiles())
            {
                tilesInComposites.insert(tile);
            }

            bool contained = comp->polygon().containsPoint(cvtPt, Qt::OddEvenFill);
            selectionChanged |= contained ^ comp->selected();
            comp->selected(contained);

            if (comp->selected())
            {
                d_currTile = comp;
            }
        }

        for (auto& tile : d_tiles)
        {
            if (tilesInComposites.contains(tile))
            {
                continue;
            }

            bool contained = tile->polygon().containsPoint(cvtPt, Qt::OddEvenFill);
            selectionChanged |= contained ^ tile->selected();
            tile->selected(contained);

            if (tile->selected())
            {
                d_currTile = tile;
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

    QSet<QSharedPointer<Tile> > tilesInComposites;

    QList<QSharedPointer<Tile> > tiles;

    // check composites
    for (auto& comp : d_composites) {
        for (auto& tile: comp->tiles())
        {
            tilesInComposites.insert(tile);
        }

        tiles.append(comp);
    }

    for (auto& tile: d_tiles)
    {
        if (!tilesInComposites.contains(tile))
        {
            tiles.append(tile);
        }
    }

    for (int i = 0; !found && i < tiles.size(); ++i)
    {
        auto& tile = tiles[i];
        if (tile->selected())
        {
           tile->selected(false);

           int index = (i + 1) % tiles.size();
           tiles[index]->selected(true);
           d_currTile = tiles[index];
           found = true;
        }
    }

    if (!found)
    {
        tiles[0]->selected(true);
        d_currTile = tiles[0];
    }

    update();
}

void GameWindow::handleCursorKey(const Qt::Key key)
{
    Q_ASSERT(NULL != d_currTile);

    switch (key)
    {
    case Qt::Key_Up:
    {
        d_currTile->translate(0, +OFFSET);
    } break;

    case Qt::Key_Down:
    {
        d_currTile->translate(0, -OFFSET);
    } break;

    case Qt::Key_Left:
    {
        d_currTile->translate(-OFFSET, 0);
    } break;

    case Qt::Key_Right:
    {
        d_currTile->translate(+OFFSET, 0);
    } break;

    default:
    {
        qFatal("Unknown key passed 0x%X", key);
    }
    } // end switch

    sceneChanged();
}


void GameWindow::handleRotateKey(const bool clockwise)
{
    Q_ASSERT(NULL != d_currTile);

    qDebug() << "Rotating" << *d_currTile << (clockwise ? "clockwise" : "counter-clockwise");

    d_currTile->rotate((clockwise ? -1 : 1) * Tile::OFFSET_ANGLE);

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

void GameWindow::magnetize(QSharedPointer<Tile> currTile,
                           const QSharedPointer<Tile> newTile,
                           const QPointF& s, const QPointF& t,
                           const QPointF& u, const QPointF& v)
{
    using namespace GeometryUtils;

    if (   ( s == u && t == v )
        || ( s == v && t == u ) )
    {
        qDebug() << "Found coincident lines.";
        return;
    }

    const double gamma = angleBtwn(s, t, u, v);
    double gamma_deg =  (180. * gamma / M_PI);

//    const QPointF inter = intersection(s, *t, u, *v);

    currTile->rotate(gamma_deg);

    qDebug() << "Rotating" << gamma_deg << "deg";


    QPointF rPt1, rPt2;

    findParallelLine(currTile->polygon(), u, v, &rPt1, &rPt2);

    if (rPt1.isNull() || rPt2.isNull())
    {
        currTile->rotate(-2. * gamma_deg);
        findParallelLine(currTile->polygon(), u, v, &rPt1, &rPt2);
    }

    Q_ASSERT( !rPt1.isNull() && !rPt2.isNull() );

    currTile->translate(u.x() - rPt1.x(), u.y() - rPt1.y());

    const QPolygonF intersect = currTile->polygon().intersected(newTile->polygon());
    if (!intersect.isEmpty())
    {
        currTile->rotate(180);

        findParallelLine(currTile->polygon(), u, v, &rPt1, &rPt2);

        currTile->translate(u.x() - rPt1.x(), u.y() - rPt1.y());
    }

//    const auto intersect = currTile->polygon().intersected(newTile->polygon());

//    Q_ASSERT( intersect.isEmpty() );

    // Now merge them
    auto newComp = newTile.dynamicCast<Composite>();
    auto currComp = currTile.dynamicCast<Composite>();
    if (!!newComp && !currComp)
    {   // it is a composite
        newComp->tiles().append(currTile);
        newComp->selected(true);

        this->d_currTile = newComp;
    }
    else if (!newComp && !!currComp)
    {
        currComp->tiles().append(newTile);
        currComp->selected(true);

        this->d_currTile = currComp;
    }
    else if (!!newComp && !!currComp)
    {
        qDebug() << "Merging compisites.";

        QSharedPointer<Composite> composite(new Composite(newComp->tiles()));
        for (auto tile: currComp->tiles())
        {
            composite->tiles().append(tile);
        }

        this->d_composites.removeAll(newComp);
        this->d_composites.removeAll(currComp);

        this->d_composites.append(composite);
        this->d_currTile = composite;
    }
    else
    {
        Q_ASSERT(!newComp && !currComp);

        QSharedPointer<Composite> mergedComp(new Composite({ newTile, currTile }));
        mergedComp->selected(true);
        this->d_composites.append(mergedComp);
        this->d_currTile = mergedComp;
    }
}

void GameWindow::handleMagnets()
{
    if (!!d_currTile)
    {
        QSet<QSharedPointer<Tile> > tilesInComposites;

        bool found = false;
        for (auto& comp: this->d_composites)
        {
            for (const auto& tile: comp->tiles())
            {
                tilesInComposites.insert(tile);
            }

            if (d_currTile != comp)
            {
                if (this->checkMagnitized(d_currTile, comp))
                {
                    found = true;

                    break;
                }
            }
        }

        if (!found)
        {
            for (auto& tile : this->d_tiles)
            {
                if (d_currTile != tile && !tilesInComposites.contains(tile))
                {
                    if (this->checkMagnitized(d_currTile, tile))
                    {
                        found = true;

                        break;
                    }
                }
            }
        }
    }

    update();
}

void GameWindow::newShape(const int sides)
{
    srand(time(NULL));

    auto newTile = Tile::shape(QPoint(50, 50),
                               sides,
                               QColor(qrand() % 255, qrand() % 255, qrand() % 255));
    this->d_tiles.append(newTile);

    sceneChanged();
}


