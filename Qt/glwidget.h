#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>

#include <QVector3D>
#include <QWheelEvent>

class Ion_cloud;

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent = 0);
    ~GLWidget();

    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void setParticles(const Ion_cloud& ic);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

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

    // bitmap for ions
    GLuint texture;
    // sets bitmap scaling
    float step;

    // zooming and rotation controls
    QPoint lastPos;
    int xRot;
    int yRot;
    int zRot;
    int displayBoxEdge;
    qreal aspect();

    static double max_zoom_distance;
    static double min_zoom_distance;

    // Representation of particle
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

    QVector<Particle> particle_vec;

};

#endif // GLWIDGET_H
