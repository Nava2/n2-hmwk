#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSignalMapper>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    virtual void keyPressEvent(QKeyEvent * event);

    virtual void mouseReleaseEvent(QMouseEvent * event);


private:
    Ui::MainWindow *ui;

    QSignalMapper *d_newBtnMapper;
};

#endif // MAINWINDOW_H
