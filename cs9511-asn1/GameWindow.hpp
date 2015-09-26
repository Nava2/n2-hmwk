#ifndef GAMEWINDOW_HPP
#define GAMEWINDOW_HPP

#include "RefinedQWidget.hpp"

#include <QList>

#include <Composite.hpp>
#include <Tile.hpp>

#include <GeometryUtils.hpp>

class QPaintDevice;
class QKeyEvent;
class QMouseEvent;
class QPointF;
class QPolygonF;

class MainWindow;

class Tile;
class Composite;

class GameWindow : public RefinedQWidget
{
    Q_OBJECT
public:

    constexpr
    static double MAGNET_THRESHOLD = 3.5;

    constexpr
    static const float OFFSET = 1.0;

    GameWindow(QWidget * parent = NULL);

    virtual ~GameWindow() { }

public slots:

    void newShape(const int sides);

protected:
    virtual QRectF getUserRectF() const;

    virtual QSize sizeHint() const;

    virtual void render(QPaintDevice * device);

    virtual void keyPressEvent(QKeyEvent * event);

    virtual void mouseReleaseEvent(QMouseEvent * event);

signals:
    void sceneChanged();


private:

    void handleSpaceKey();
    void handleCursorKey(const Qt::Key key);
    void handleRotateKey(const bool clockwise);

    void handleMagnets();
    bool checkMagnitized(QSharedPointer<Tile> check, QSharedPointer<Tile> against);
    void magnetize(QSharedPointer<Tile> currTile, QSharedPointer<Tile> const newTile,
                   const QPointF& s, const QPointF &t,
                   const QPointF& u, const QPointF &v);

    QList<QSharedPointer<Composite> >   d_composites;
    QList<QSharedPointer<Tile> >        d_tiles;
    QSharedPointer<Tile>                d_currTile;

    friend class MainWindow;
};

inline
bool GameWindow::checkMagnitized(QSharedPointer<Tile> check, QSharedPointer<Tile> against)
{
    using namespace GeometryUtils;

    const QPolygonF& checkPoly = check->polygon();
    const QPolygonF& againstPoly = against->polygon();

    const QPointF  *s = NULL,
                   *t = NULL,
                   *u = NULL,
                   *v = NULL;
    qreal minDist = MAGNET_THRESHOLD;

    for (int i = 0; i < checkPoly.size(); ++i)
    {
        const auto& cPt = checkPoly[i];

        for (int j = 0; j < againstPoly.size(); ++j)
        {
            const auto& p1 = againstPoly[j];
            const auto& p2 = againstPoly[(j + 1) % againstPoly.size()];

            const qreal dist2Line = ptDistance(cPt, p1, p2);
            if ( dist2Line < MAGNET_THRESHOLD && dist2Line < minDist)
            {
                minDist = dist2Line;
                s = &cPt;
                u = &p1;
                v = &p2;

                const auto& cPt1 = checkPoly[(i + checkPoly.size() - 1) % checkPoly.size()];
                const auto& cPt2 = checkPoly[(i + 1) % checkPoly.size()];

                const qreal pt1dist = ptDistance(cPt1, p1, p2);
                const qreal pt2dist = ptDistance(cPt2, p1, p2);

                t = (pt1dist < pt2dist) ? &cPt1 : &cPt2;
            }
        }
    }

    for (int i = 0; i < againstPoly.size(); ++i)
    {
        const QPointF& tPt = againstPoly[i];

        for (int j = 0; j < checkPoly.size(); ++j)
        {
            const auto& p1 = checkPoly[j];
            const auto& p2 = checkPoly[(j + 1) % checkPoly.size()];

            const qreal dist2Line = ptDistance(tPt, p1, p2);
            if ( dist2Line < MAGNET_THRESHOLD && dist2Line < minDist)
            {
                minDist = dist2Line;
                s = &p1;
                t = &p2;
                u = &tPt;

                const auto& tPt1 = againstPoly[(i + againstPoly.size() - 1) % againstPoly.size()];
                const auto& tPt2 = againstPoly[(i + 1) % againstPoly.size()];

                const qreal pt1dist = ptDistance(tPt1, p1, p2);
                const qreal pt2dist = ptDistance(tPt2, p1, p2);

                v = (pt1dist < pt2dist) ? &tPt1 : &tPt1;
            }
        }
    }

    if (!!s)
    {
        Q_ASSERT(minDist < MAGNET_THRESHOLD);
        Q_ASSERT(!!s && !!t && !!u && !!v);

        this->magnetize(check, against, *s, *t, *u, *v);

        return true;
    }
    else
    {
        return false;
    }
}

#endif // GAMEWINDOW_HPP
