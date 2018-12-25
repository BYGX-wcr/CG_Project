#include "glwindow.h"
#include <QDebug>

GLWindow::GLWindow(const QString& fp)
{
    create();

    m_Context = new QOpenGLContext;
    m_Context->create();
    m_Context->makeCurrent(this);

    // Initialize the widget
    initializeGL();

    // Initialize model data
    setDisplayModel(fp);
}

GLWindow::~GLWindow()
{}

void GLWindow::initializeGL()
{
    setGeometry(0, 0, 1280, 720);//设置窗口初始位置和大小
    initializeOpenGLFunctions();

    glShadeModel(GL_SMOOTH);//设置阴影平滑模式
    glClearColor(0.0, 0.0, 0.0, 0);//改变窗口的背景颜色，不过我这里貌似设置后并没有什么效果
    glClearDepth(1.0);//设置深度缓存
    glEnable(GL_DEPTH_TEST);//允许深度测试
    glDepthFunc(GL_LEQUAL);//设置深度测试类型
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);//进行透视校正
}

void GLWindow::setDisplayModel(const QString &fp)
{
    modelFile.setFileName(fp);

    if (!modelFile.isOpen())
        modelFile.open(QFile::ReadOnly | QFile::Text);

    QTextStream fin(&modelFile);

    Q_ASSERT(fin.readLine() == "OFF");
    fin >> numVertices >> numFaces >> numEdges;

    for (int i = 0; i < numVertices; ++i)
    {
        qreal x, y, z;
        fin >> x >> y >> z;
        Point3D p(x, y, z);
        vertices.push_back(p);
    }

    modelFile.close();
}

void GLWindow::paintGL()
{
    glLoadIdentity();
    glRotatef(-30.0, 0.2, 0.2, 0.0);

    if (modelFile.fileName().size() == 0)
        return;
    else if (!modelFile.isOpen())
        modelFile.open(QFile::ReadOnly | QFile::Text);

    QTextStream fin(&modelFile);

    //make the file ptr to faces section
    for (int i = 0; i < numVertices + 2; ++i)
    {
        fin.readLine();
    }

    for (int i = 0; i < numFaces; ++i)
    {
        QString faceData = fin.readLine();
        QTextStream lin(&faceData);

        //For OpenGL
        int pointNum = 0;
        lin >> pointNum;
        QVector<int> pList;
        for (int j = 0; j < pointNum; ++j)
        {
            int n;
            lin >> n;
            pList.push_back(n);
        }

        glBegin(GL_POLYGON);
        glColor3f(1.0, 0.0, 0.0);
        for (int j = 0; j < pointNum; ++j)
        {
            int n = pList[j];
            glVertex3d(vertices[n].x, vertices[n].y, vertices[n].z);
        }
        glEnd();

        glBegin(GL_LINES);
        glColor3f(0.0, 0.0, 1.0);
        for (int j = 0; j < pointNum; ++j)
        {
            glLineWidth(3.0);
            int n = pList[j];
            glVertex3d(vertices[n].x, vertices[n].y, vertices[n].z);
            n = pList[(j + 1) % pointNum];
            glVertex3d(vertices[n].x, vertices[n].y, vertices[n].z);
        }
        glEnd();
    }

    modelFile.close();
}

void GLWindow::resizeGL(int width, int height)
{
    glViewport(0, 0, static_cast<GLint>(width), static_cast<GLint>(height));
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
