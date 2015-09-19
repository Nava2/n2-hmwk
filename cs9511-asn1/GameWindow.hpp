#ifndef GAMEWINDOW_HPP
#define GAMEWINDOW_HPP

#include "RefinedQWidget.hpp"
#include "Tile.hpp"
#include "geometry/E2/Geometry_E2.hpp"
#include "geometry/E2/Triangle_E2.hpp"

class QPaintDevice;
class QKeyEvent;
class QMouseEvent;

class GameWindow : public RefinedQWidget
{
public:
    typedef Geometry_E2<double> Geometry_E2d;
    typedef Geometry_E2d::Point Point;
    typedef Geometry_E2d::Triangle Triangle;

    GameWindow(QWidget * parent = 0);

protected:
    virtual QRectF getUserRectF() const;

    virtual QSize sizeHint() const;

    virtual void render(QPaintDevice * device);

    virtual void keyPressEvent(QKeyEvent * event);

    virtual void mouseReleaseEvent(QMouseEvent * event);

private:

    QList<Tile> m_tiles;
};

#endif // GAMEWINDOW_HPP
