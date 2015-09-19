#ifndef REFINEDQWIDGET_H
#define REFINEDQWIDGET_H

#include <QSize>
#include <QRectF>
#include <QWidget>
#include <QPrinter>
#include <QKeyEvent>
#include <QTransform>
#include <QMessageBox>
#include <QSvgGenerator>

enum YFlip
{
    INTERPOLATE_AND_YFLIP,
    INTERPOLATE_AND_DO_NOT_YFLIP,
};

//----------------RefinedQWidget_Begin----------------
// tag::RefinedQWidget[]
// tag::RefinedQWidget[]
// RefinedQWidget is responsible for centering the userRectF.
class RefinedQWidget : public QWidget
{
public:
    RefinedQWidget(QWidget * parent = 0,
                   YFlip _yflip = INTERPOLATE_AND_YFLIP)
        : QWidget(parent), yflip(_yflip)
    {}
protected:
    YFlip yflip;
    QTransform userToWidgetTransform;
    QTransform widgetToUserTransform;

    virtual QSize sizeHint() const { return QSize(200, 200); }

    // Classes derived from RefinedQWidget must define the user window..
    virtual QRectF getUserRectF()const=0;

    virtual void paintEvent(QPaintEvent * /*event*/)
    {
        render(this);
    }
    // Classes derived from RefinedQWidget must also define what to render.
    virtual void render(QPaintDevice * device)=0;

    virtual void resizeEvent(QResizeEvent *)
    {
        userToWidgetTransform = getUserToWidgetTransform(this);
        widgetToUserTransform = userToWidgetTransform.inverted();
    }

    QTransform getUserToWidgetTransform(QPaintDevice * device)
    {
        const QRectF device_window(0, 0, device->width(), device->height());

        QRectF w2 = get_centered_window(getUserRectF(), device_window);

        QTransform T;
        switch(yflip) {
        case INTERPOLATE_AND_YFLIP:
            T = interpolate_and_yflip(w2, device_window);
            break;
        case INTERPOLATE_AND_DO_NOT_YFLIP:
            T = interpolate_and_do_not_yflip(w2, device_window);
            break;
        }
        return T;
    }
    QTransform interpolate_and_yflip(const QRectF & from, const QRectF & to)
    {
        QTransform T;
        T.translate( to.bottomLeft().x(), to.bottomLeft().y() ); // third: translate from origin
        T.scale(to.width() / from.width(), - to.height() / from.height() ); // second: scale
        T.translate( - from.bottomLeft().x(), - from.topLeft().y() ); // first: translate to origin
        return T;
    }
    QTransform interpolate_and_do_not_yflip(const QRectF & from, const QRectF & to)
    {
        QTransform T;
        T.translate( to.bottomLeft().x(), to.bottomLeft().y() ); // third: translate from origin
        T.scale(to.width() / from.width(), to.height() / from.height() ); // second: scale
        T.translate( - from.bottomLeft().x(), - from.bottomLeft().y() ); // first: translate to origin
        return T;
    }
    QRectF get_centered_window(const QRectF & w /*input*/, const QRectF & d /*reference*/)
    {
        const qreal w_ar = w.width()/w.height();
        const qreal d_ar = d.width()/d.height();

        if(d_ar > w_ar) {
            qreal new_w_width = w.height() * d_ar;
            return QRectF( w.topLeft().x() - (new_w_width - w.width()) / 2.0 , w.topLeft().y(),
                           new_w_width, w.height() );
        }
        else {
            qreal new_w_height = w.width() / d_ar;
            return QRectF( w.topLeft().x(), w.topLeft().y() - (new_w_height - w.height()) / 2.0,
                           w.width(), new_w_height );
        }
    }

    virtual void keyPressEvent(QKeyEvent * event)
    {
        QPrinter printer;
        QSvgGenerator svgGenerator;
        switch(event->key()) {
        case Qt::Key_P:
            printer.setOutputFormat(QPrinter::PdfFormat);
            printer.setOutputFileName("GameWindow.pdf");
            userToWidgetTransform = getUserToWidgetTransform(&printer);
            render(&printer);
            userToWidgetTransform = getUserToWidgetTransform(this);
            QMessageBox::information(this, "Print", "Document printed", QMessageBox::Ok);
            break;
        case Qt::Key_S:
            svgGenerator.setFileName("GameWindow.svg");
            userToWidgetTransform = getUserToWidgetTransform(&svgGenerator);
            render(&svgGenerator);
            userToWidgetTransform = getUserToWidgetTransform(this);
            QMessageBox::information(this, "SVG", "Document saved to SVG file", QMessageBox::Ok);
            break;
        default:
            QWidget::keyPressEvent(event);
        }
    }
};
// end::RefinedQWidget[]

#endif // REFINEDQWIDGET_H
