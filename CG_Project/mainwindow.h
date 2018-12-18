#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QLabel>
#include <QPalette>
#include <QPushButton>
#include <QAction>
#include <QSpinBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QDialog>
#include <QColorDialog>
#include "paintwidget.h"
#include "shape.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow()
    {

    }

signals:
    void changeCurrentTool(Shape::Type arg);
    void changeCurrentPenColor(QPalette arg);
    void changeCurrentBrushColor(QPalette arg);
    void rotateShapes();
    void vflipShapes();
    void hflipShapes();
    void scaleShapes();

private:
    PaintWidget *paintWidget;
    QWidget* activeWidget;

    void drawLineTriggered();
    void drawEllipseTriggered();
    void drawRectTriggered();
    void drawPolygonTriggered();

    void selectionToolTriggered();
    void clipToolTriggered();
    void rotateToolTriggered();
    void vflipToolTriggered();
    void hflipToolTriggered();
    void scaleToolTriggered();

    void openPenColorDialog();
    void openBrushColorDialog();
    void openBrushDialog();
    void judgeBrushState(bool checked);
};

#endif // MAINWINDOW_H
