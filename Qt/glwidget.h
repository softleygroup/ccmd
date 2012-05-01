#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>

#include <QVector3D>
#include <QWheelEvent>

class Ion_cloud;

struct Particle
{
    Particle()
       { color = Qt::white; color.setAlphaF(1.0); }
    Particle(const QVector3D& r)
        : pos(r) { color = Qt::white; color.setAlphaF(1.0); }
    Particle(const QColor& c)
        : color(c) {color.setAlphaF(1.0); }
    Particle(const QVector3D& r, const QColor& c)
        : pos(r), color(c) { color.setAlphaF(1.0); }

    QVector3D pos;          // Particle position
    QColor color;           // Particle colour
    void move(const QVector3D& r);
};

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent = 0);
    ~GLWidget();

    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void setParticles(const Ion_cloud& ic);

public slots:
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);
signals:
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);

private:
    void setView();
    GLuint texture;
    QVector<Particle> particle_vec;

    QPoint lastPos;
    int xRot;
    int yRot;
    int zRot;
    int displayBoxEdge;
    qreal aspect();
};

#endif // GLWIDGET_H
