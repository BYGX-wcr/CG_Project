#ifndef GLWINDOW_H
#define GLWINDOW_H

#include <QOpenGLWindow>
#include <QOpenGLContext>
#include <QFile>
#include <QTextStream>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_3_0>
#include <QtOpenGL/QtOpenGL>
#include <GL/gl.h>
#include <GL/glu.h>

class GLWindow : public QOpenGLWindow, protected QOpenGLFunctions_3_0
{
    Q_OBJECT
public:
    explicit GLWindow(const QString& fp = "");
    ~GLWindow();

protected:
    struct Point3D
    {
        qreal x;
        qreal y;
        qreal z;

        Point3D(qreal argX = 0, qreal argY = 0, qreal argZ = 0)
        {
            x = argX;
            y = argY;
            z = argZ;
        }
    };

    void paintGL();
    void initializeGL();
    void resizeGL(int width, int height);
    void setDisplayModel(const QString& fp);

private:
    QFile modelFile;
    QVector<Point3D> vertices;
    int numVertices, numFaces, numEdges;

    QOpenGLContext* m_Context;
};

#endif // GLWINDOW_H
