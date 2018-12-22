#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    //initialize paint area
    paintWidget = new PaintWidget(this);
    QGraphicsView *view = new QGraphicsView(paintWidget, this);
    paintWidget->setSceneRect(0, 0, 1280, 720);
    this->setCentralWidget(view);
    this->setWindowTitle("Computer Graphics Project: Painter");

    connect(this, &MainWindow::changeCurrentTool, paintWidget, &PaintWidget::setCurrentTool);
    connect(this, &MainWindow::changeCurrentPenColor, paintWidget, &PaintWidget::setCurrentPenColor);
    connect(this, &MainWindow::changeCurrentBrushColor, paintWidget, &PaintWidget::setCurrentBrushColor);
    connect(this, &MainWindow::rotateShapes, paintWidget, &PaintWidget::rotateShapes);
    connect(this, &MainWindow::vflipShapes, paintWidget, &PaintWidget::vflipShapes);
    connect(this, &MainWindow::hflipShapes, paintWidget, &PaintWidget::hflipShapes);
    connect(this, &MainWindow::scaleShapes, paintWidget, &PaintWidget::scaleShapes);

    //Initialize Main Window
    menuBar();
    QLabel *statusLabel = new QLabel(this);
    QToolBar* toolBar = NULL;
    statusBar()->addWidget(statusLabel);

    //Initialize the Draw Tool Bar
     toolBar = addToolBar(tr("&Draw Tools"));

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

    QAction *drawCurveAction = new QAction(QIcon(":/icon/icon-curve"), tr("&Curve"), toolBar);
    drawPloygonAction->setStatusTip("Draw a Curve");
    toolBar->addAction(drawCurveAction);
    connect(drawCurveAction, &QAction::triggered, this, &MainWindow::drawCurveTriggered);

    QAction *brushToolAction = new QAction(QIcon(":/icon/icon-brush-tool"), tr("&ToolSettings"), toolBar);
    brushToolAction->setStatusTip("Set options of the Pen & Brush");
    toolBar->addAction(brushToolAction);
    connect(brushToolAction, &QAction::triggered, this, &MainWindow::openBrushDialog);

    //Initialize the Edit Tool Bar
    toolBar = addToolBar(tr("&Edit Tools"));

    QAction *selectionAction = new QAction(QIcon(":/icon/icon-selection-tool"), tr("Selection Tool"), toolBar);
    selectionAction->setStatusTip("Select & Edit items");
    toolBar->addAction(selectionAction);
    connect(selectionAction, &QAction::triggered, this, &MainWindow::selectionToolTriggered);

    QAction *clipAction = new QAction(QIcon(":/icon/icon-clip-tool"), tr("Clip Tool"), toolBar);
    clipAction->setStatusTip("Clip tailorable items");
    toolBar->addAction(clipAction);
    connect(clipAction, &QAction::triggered, this, &MainWindow::clipToolTriggered);

    QAction *rotateAction = new QAction(QIcon(":/icon/icon-rotate-tool"), tr("Rotation Tool"), toolBar);
    rotateAction->setStatusTip("Rotate items");
    toolBar->addAction(rotateAction);
    connect(rotateAction, &QAction::triggered, this, &MainWindow::rotateToolTriggered);
    QSpinBox *rotateBox = new QSpinBox(toolBar);
    rotateBox->setToolTip("Rotate Angel");
    rotateBox->setStatusTip("Set the Rotate Angel");
    rotateBox->setFixedWidth(60);
    rotateBox->setMaximum(359);
    rotateBox->setMinimum(-359);
    toolBar->addWidget(rotateBox);
    connect(rotateBox, QOverload<int>::of(&QSpinBox::valueChanged), paintWidget, &PaintWidget::setRotateAngel);

    QAction *vflipAction = new QAction(QIcon(":/icon/icon-vertical-flip"), tr("Vertical Flip Tool"), toolBar);
    vflipAction->setStatusTip("Flip items vertically");
    toolBar->addAction(vflipAction);
    connect(vflipAction, &QAction::triggered, this, &MainWindow::vflipToolTriggered);

    QAction *hflipAction = new QAction(QIcon(":/icon/icon-horizontal-flip"), tr("Horizontal Flip Tool"), toolBar);
    vflipAction->setStatusTip("Flip items horizontally");
    toolBar->addAction(hflipAction);
    connect(hflipAction, &QAction::triggered, this, &MainWindow::hflipToolTriggered);

    QAction *scaleAction = new QAction(QIcon(":/icon/icon-scale-tool"), tr("Scale Tool"), toolBar);
    scaleAction->setStatusTip("Scale items");
    toolBar->addAction(scaleAction);
    connect(scaleAction, &QAction::triggered, this, &MainWindow::scaleToolTriggered);
    QSpinBox *scaleBox = new QSpinBox(toolBar);
    scaleBox->setToolTip("Scaling Factor");
    scaleBox->setStatusTip("Set the Scaling Factor");
    scaleBox->setFixedWidth(80);
    scaleBox->setMaximum(400);
    scaleBox->setMinimum(10);
    scaleBox->setSuffix(tr("%"));
    scaleBox->setSingleStep(10);
    scaleBox->setValue(100);
    toolBar->addWidget(scaleBox);
    connect(scaleBox, QOverload<int>::of(&QSpinBox::valueChanged), paintWidget, &PaintWidget::setScalingFactor);
}

void MainWindow::drawLineTriggered()
{
    emit changeCurrentTool(Shape::LineTool);
}

void MainWindow::drawEllipseTriggered()
{
    emit changeCurrentTool(Shape::EllipseTool);
}

void MainWindow::drawRectTriggered()
{
    emit changeCurrentTool(Shape::RectangleTool);
}

void MainWindow::drawPolygonTriggered()
{
    emit changeCurrentTool(Shape::PolygonTool);
}

void MainWindow::drawCurveTriggered()
{
    emit changeCurrentTool(Shape::CurveTool);
}

void MainWindow::selectionToolTriggered()
{
    emit changeCurrentTool(Shape::SelectTool);
}

void MainWindow::clipToolTriggered()
{
    emit changeCurrentTool(Shape::ClipTool);
}

void MainWindow::rotateToolTriggered()
{
    emit rotateShapes();
}

void MainWindow::vflipToolTriggered()
{
    emit vflipShapes();
}
void MainWindow::hflipToolTriggered()
{
    emit hflipShapes();
}
void MainWindow::scaleToolTriggered()
{
    emit scaleShapes();
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
