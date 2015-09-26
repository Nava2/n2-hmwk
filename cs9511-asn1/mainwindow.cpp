#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
  , d_newBtnMapper(new QSignalMapper(this))
{
    ui->setupUi(this);

    ui->gameWindow->setFocus();

    d_newBtnMapper->setMapping(ui->triangle_btn, 3);
    QObject::connect(ui->triangle_btn, SIGNAL(clicked()), d_newBtnMapper, SLOT(map()));

    d_newBtnMapper->setMapping(ui->square_btn, 4);
    QObject::connect(ui->square_btn, SIGNAL(clicked()), d_newBtnMapper, SLOT(map()));

    d_newBtnMapper->setMapping(ui->pentagon_btn, 5);
    QObject::connect(ui->pentagon_btn, SIGNAL(clicked()), d_newBtnMapper, SLOT(map()));

    QObject::connect(d_newBtnMapper, SIGNAL(mapped(int)), ui->gameWindow, SLOT(newShape(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent * event)
{
//    ui->gameWindow->keyPressEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent * event)
{
//    ui->gameWindow->mouseReleaseEvent(event);
}
