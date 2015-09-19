#ifndef GAMEWINDOW_HPP
#define GAMEWINDOW_HPP

#include "RefinedQWidget.hpp"
#include "Tile.hpp"
#include "geometry/E2/Geometry_E2.hpp"
#include "geometry/E2/Triangle_E2.hpp"

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <cmath>

class GameWindow : public RefinedQWidget
{
public:
    typedef Geometry_E2<double> Geometry_E2d;
    typedef Geometry_E2d::Point Point;
    typedef Geometry_E2d::Triangle Triangle;

    GameWindow(QWidget * parent = 0)
        : RefinedQWidget(parent, INTERPOLATE_AND_YFLIP)
//        : RefinedQWidget(parent, INTERPOLATE_AND_DO_NOT_YFLIP)
    {
        Triangle T1(Point( 10,180), Point( 30,180), Point( 20,200));
        Triangle T2(Point(110,180), Point(130,180), Point(120,200));
        Triangle T3(Point(210,180), Point(230,180), Point(220,200));

        tiles.push_back(Tile(T1, Qt::red));
        tiles.push_back(Tile(T2, Qt::green));
        tiles.push_back(Tile(T3, Qt::blue));
    }

protected:
    virtual QRectF getUserRectF() const
    {
        return QRectF(QPointF(0.0, 0.0), QPointF(300.0, 200.0));
    }
    virtual QSize sizeHint() const { return QSize(680, 480); }

    virtual void render(QPaintDevice * device)
    {
        QPainter painter(device);
        // userToWidgetTransform is updated only when the QWidget is resized.
        painter.setTransform(userToWidgetTransform);

        typedef std::vector<Tile>::const_iterator It;
        for(It tile=tiles.begin(); tile!=tiles.end(); ++tile) {
            painter.setRenderHint(QPainter::Antialiasing, true);            
            painter.setBrush(QBrush(tile->color().lighter()));
            painter.setPen(QPen(tile->color(), 1.0));
            painter.drawPolygon(tile->polygon());
        }
    }

    std::vector<Tile> tiles;
};

#endif // GAMEWINDOW_HPP
