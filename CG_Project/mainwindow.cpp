#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    //Initialize Main Window
    menuBar();
    QToolBar* toolBar = addToolBar("&Tools");
    QLabel *statusLabel = new QLabel(this);
    statusBar()->addWidget(statusLabel);

    //Initialize Tool Bar
    QAction *drawLineAction = new QAction(QIcon(":/icon/icon-line"), tr("&Line"), toolBar);
    drawLineAction->setStatusTip("Draw a line");
    toolBar->addAction(drawLineAction);
    connect(drawLineAction, &QAction::triggered, this, &MainWindow::drawLineTriggered);

    QAction *drawEllipseAction = new QAction(QIcon(":/icon/icon-ellipse"), tr("&Ellipse"), toolBar);
    drawEllipseAction->setStatusTip("Draw a ellipse");
    toolBar->addAction(drawEllipseAction);
    connect(drawEllipseAction, &QAction::triggered, this, &MainWindow::drawEllipseTriggered);

    QAction *drawRectAction = new QAction(QIcon(":/icon/icon-rect"), tr("&Rectangle"), toolBar);
    drawRectAction->setStatusTip("Draw a rectangle");
    toolBar->addAction(drawRectAction);
    connect(drawRectAction, &QAction::triggered, this, &MainWindow::drawRectTriggered);

    QAction *drawPloygonAction = new QAction(QIcon(":/icon/icon-polygon"), tr("&Ploygon"), toolBar);
    drawPloygonAction->setStatusTip("Draw a ploygon");
    toolBar->addAction(drawPloygonAction);
    connect(drawPloygonAction, &QAction::triggered, this, &MainWindow::drawPolygonTriggered);

    QAction *brushToolAction = new QAction(QIcon(":/icon/icon-brush-tool"), tr("&ToolSettings"), toolBar);
    brushToolAction->setStatusTip("Set options of the Pen & Brush");
    toolBar->addAction(brushToolAction);
    connect(brushToolAction, &QAction::triggered, this, &MainWindow::openBrushDialog);

    //initialize paint area
    paintWidget = new PaintWidget(this);
    QGraphicsView *view = new QGraphicsView(paintWidget, this);
    paintWidget->setSceneRect(0, 0, 1280, 720);
    this->setCentralWidget(view);
    this->setWindowTitle("Computer Graphics Project: Painter");

    connect(this, &MainWindow::changeCurrentShape, paintWidget, &PaintWidget::setCurrentShape);
    connect(this,&MainWindow::changeCurrentPenColor, paintWidget, &PaintWidget::setCurrentPenColor);
    connect(this,&MainWindow::changeCurrentBrushColor, paintWidget, &PaintWidget::setCurrentBrushColor);
}

void MainWindow::drawLineTriggered()
{
    emit changeCurrentShape(Shape::Line);
}

void MainWindow::drawEllipseTriggered()
{
    emit changeCurrentShape(Shape::Ellipse);
}

void MainWindow::drawRectTriggered()
{
    emit changeCurrentShape(Shape::Rectangle);
}

void MainWindow::drawPolygonTriggered()
{
    emit changeCurrentShape(Shape::Polygon);
}

void MainWindow::openPenColorDialog()
{
    if (activeWidget == NULL) activeWidget = this;
    QColor newColor = QColorDialog::getColor(Qt::black, activeWidget);
    QPalette pal(newColor);
    emit changeCurrentPenColor(pal);
}

void MainWindow::openBrushColorDialog()
{
    if (activeWidget == NULL) activeWidget = this;
    QColor newColor = QColorDialog::getColor(Qt::black, activeWidget);
    QPalette pal(newColor);
    emit changeCurrentBrushColor(pal);
}

void MainWindow::openBrushDialog()
{
    QDialog* penDialog = new QDialog(this);
    penDialog->setWindowTitle("Pen&Brush Settings");
    penDialog->resize(300, 400);
    activeWidget = penDialog;

    //Pen width section
    QLabel* label1 = new QLabel(penDialog);
    label1->setText(tr("Pen Width:"));
    label1->setGeometry(50, 50, label1->size().width(), label1->size().height());
    QSpinBox* intBox = new QSpinBox(penDialog);
    intBox->setGeometry(50 + label1->size().width(), 50, 100, label1->size().height());
    intBox->setValue(paintWidget->getPenWidth());
    connect(intBox, QOverload<int>::of(&QSpinBox::valueChanged), paintWidget, &PaintWidget::setPenWidth);

    //Pen color section
    QLabel* label2 = new QLabel(penDialog);
    label2->setText(tr("Pen Color:"));
    label2->setGeometry(50, label1->geometry().y() + label1->size().height() + 10, label2->size().width(), label2->size().height());
    QPushButton* penColorButton = new QPushButton(penDialog);
    {
        QPalette tempPal(paintWidget->getPenColor());
        penColorButton->setPalette(tempPal);
        penColorButton->setAutoFillBackground(true);
        penColorButton->setFlat(true);
    }
    penColorButton->setGeometry(label2->geometry().x() + label2->size().width(), label2->geometry().y(), 100, label2->size().height());
    connect(penColorButton, &QPushButton::clicked, this, &MainWindow::openPenColorDialog);
    connect(this, &MainWindow::changeCurrentPenColor, penColorButton, &QPushButton::setPalette);

    //Fill Tool Color section
    QGroupBox* fillToolBox = new QGroupBox(penDialog);
    fillToolBox->setTitle(tr("Brush Tool"));

    QRadioButton *radio1 = new QRadioButton(tr("No Brush"));
    QRadioButton *radio2 = new QRadioButton(tr("Use Brush"));
    QPushButton* brushColorButton = new QPushButton(penDialog);
    {
        QPalette tempPal(paintWidget->getBrushColor());
        brushColorButton->setPalette(tempPal);
        brushColorButton->setAutoFillBackground(true);
        brushColorButton->setFlat(true);
    }
    connect(brushColorButton, &QPushButton::clicked, this, &MainWindow::openBrushColorDialog);
    connect(this, &MainWindow::changeCurrentBrushColor, brushColorButton, &QPushButton::setPalette);
    QLabel *label3 = new QLabel(this);
    label3->setText(tr("Color:"));

    if (paintWidget->getBrushStatus())
        radio2->setChecked(true);
    else
        radio1->setChecked(true);
    connect(radio2, &QRadioButton::toggled, paintWidget, &PaintWidget::toggleBrushState);

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(radio1);
    vbox->addWidget(radio2);
    vbox->addWidget(label3);
    vbox->addWidget(brushColorButton);
    vbox->addStretch(1);
    fillToolBox->setLayout(vbox);
    fillToolBox->move(50, label2->geometry().y() + label2->height() + 10);

    penDialog->exec();
    activeWidget = this;
}
