#ifndef GAMEWINDOW_HPP
#define GAMEWINDOW_HPP

#include "RefinedQWidget.hpp"

#include <QList>

#include <Composite.hpp>
#include <Tile.hpp>

class QPaintDevice;
class QKeyEvent;
class QMouseEvent;
class QPointF;
class QPolygonF;

class Tile;
class Composite;

class GameWindow : public RefinedQWidget
{
    Q_OBJECT
public:

    GameWindow(QWidget * parent = NULL);

    virtual ~GameWindow() { }

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
    void magnetize(Tile* const currTile, Tile* const newTile,
                   const QPointF& s, const QPointF* t,
                   const QPointF& u, const QPointF* v);

    QList<Composite>    d_composites;
    QList<Tile>         d_tiles;
    Tile*               d_currTile;
};

#endif // GAMEWINDOW_HPP
