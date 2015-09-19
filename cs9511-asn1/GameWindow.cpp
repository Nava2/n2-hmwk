
#include <GameWindow.hpp>

#include <QWidget>
#include <QPainter>

#include <QMouseEvent>
#include <QKeyEvent>

#include <QList>

#include <QDebug>

GameWindow::GameWindow(QWidget * parent)
    : RefinedQWidget(parent, INTERPOLATE_AND_YFLIP)
{
    Triangle T1(Point( 10,180), Point( 30,180), Point( 20,200));
    Triangle T2(Point(110,180), Point(130,180), Point(120,200));
    Triangle T3(Point(210,180), Point(230,180), Point(220,200));

    m_tiles.push_back(Tile(T1, Qt::red));
    m_tiles.push_back(Tile(T2, Qt::green));
    m_tiles.push_back(Tile(T3, Qt::blue));
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

    for (const Tile& tile : m_tiles)
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
        for (auto& tile : m_tiles)
        {
            bool contained = tile.polygon().containsPoint(cvtPt, Qt::OddEvenFill);
            selectionChanged |= contained ^ tile.selected();
            tile.selected() = contained;
        }

        if (selectionChanged)
        {
            this->update();
        }
    }
    else
    {
        RefinedQWidget::mouseReleaseEvent(event);
    }
}

void GameWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Space:
    {
        event->accept();

        bool found = false;
        for (int i = 0; !found && i < m_tiles.size(); ++i)
        {
            auto& tile = m_tiles[i];
            if (tile.selected())
            {
               tile.selected() = false;
               m_tiles[(i + 1) % m_tiles.size()].selected() = true;
               found = true;
            }
        }

        if (!found)
        {
            m_tiles[0].selected() = true;
        }

        update();
    } break;

    default:
    {
        // no-op
    }
    } // end switch

    RefinedQWidget::keyReleaseEvent(event);
}



