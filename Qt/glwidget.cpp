
#include "glwidget.h"
#include <QDebug>

#include <QtOpenGL>
#include <QColor>

#include "ion_cloud.h"
#include "vector3D.h"

#include <string>
#include <QString>

// Constructor
GLWidget::GLWidget(QWidget *parent) :
     QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    xRot = 0 * 16;
    yRot = -90 * 16;
    zRot = 0 *16;
    displayBoxEdge = 12;
    connect(this,SIGNAL(xRotationChanged(int)), this, SLOT(setXRotation(int)));
    connect(this,SIGNAL(yRotationChanged(int)), this, SLOT(setYRotation(int)));
    connect(this,SIGNAL(zRotationChanged(int)), this, SLOT(setZRotation(int)));
}

// Destructor
GLWidget::~GLWidget() {
    glDeleteTextures(1, &texture);
}

void GLWidget::initializeGL()
{
    glShadeModel(GL_SMOOTH);    // Enable smooth shading
    qglClearColor(Qt::black);   // Set the clear color to a black background
    glClearDepth(1.0f);         // Depth buffer setup

    //glEnable(GL_DEPTH_TEST);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really nice perspective calculations

    // Set up texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    QPixmap texturePixmap(":/img/Particle_with_alpha_blur.png");
    texture = bindTexture( texturePixmap );

    glEnable(GL_TEXTURE_2D);

    // Set up blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_BLEND);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear screen and depth buffer
    glLoadIdentity();

    float rotate_x = xRot / 16.0;
    float rotate_y = yRot / 16.0;
    float rotate_z = -zRot / 16.0;
    float zoom = 2;

    glTranslatef(0.0, 0.0, -zoom);
    glRotatef(rotate_x, 1.0, 0.0, 0.0);
    glRotatef(rotate_y, 0.0, 1.0, 0.0);
    glRotatef(rotate_z, 0.0, 0.0, 1.0);

    // Draw particles
    float x, y, z;
    foreach (Particle particle, particle_vec)
    {
        glColor4f(particle.color.red(), particle.color.green(), particle.color.blue(),
                  particle.color.alphaF() );

        x = particle.pos.x();
        y = particle.pos.y();
        z = particle.pos.z();

        // Store matrix on matrix stack
        glPushMatrix ();
        // Generate matrix for billboarding
        glTranslatef(x, y, z);
        glRotatef (rotate_z, 0, 0, -1);
        glRotatef (rotate_y, 0, -1, 0);
        glRotatef (rotate_x, -1, 0, 0);
        glTranslatef(-x, -y, -z);

        // Use triangle strip to draw particle
        glBegin(GL_TRIANGLE_STRIP);
            glTexCoord2d(1, 1); glVertex3f(x + 0.5f, y + 0.5f, z);
            glTexCoord2d(0, 1); glVertex3f(x - 0.5f, y + 0.5f, z);
            glTexCoord2d(1, 0); glVertex3f(x + 0.5f, y - 0.5f, z);
            glTexCoord2d(0, 0); glVertex3f(x - 0.5f, y - 0.5f, z);
        glEnd();

        // Retrieve stored matrix
        glPopMatrix ();
    }
}

void GLWidget::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    // Set OpenGL viewport to cover whole widget
    glViewport(0, 0, width, height);

    // Calculate aspect ratio
    //aspect = (qreal)width / ((qreal)height?height:1);

    setView();

}

qreal GLWidget::aspect()
{
    qreal width = this->width();
    qreal height = this->height();
    return (qreal) width / ((qreal)height ? height : 1);
}

void GLWidget::setView()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    qreal aspect_ratio = aspect();
    qreal xLim, yLim, zLim;
    xLim = displayBoxEdge*aspect_ratio;
    yLim = displayBoxEdge;
    zLim = displayBoxEdge;
    glOrtho(-xLim, +xLim, -yLim, +yLim, -zLim, zLim);

    glMatrixMode(GL_MODELVIEW);
}

void Particle::move(const QVector3D& r)
{
    pos += r;
}


static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

//! [5]
void GLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}
//! [5]

void GLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot + 8 * dy);
        setYRotation(yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(xRot + 8 * dy);
        setZRotation(zRot + 8 * dx);
    }
    lastPos = event->pos();
}

void GLWidget::setParticles(const Ion_cloud& ic)
{

    particle_vec.clear();    
    int n_particles = ic.number_of_ions();

    for (int i=0; i<n_particles; ++i) {

        Vector3D ion_pos = ic.ion_position(i);
        QVector3D r(ion_pos.x, ion_pos.y, ion_pos.z);
        Particle new_particle(r);
        QString color( ic.ion_color(i).c_str() );

        if (color == "white") {
            new_particle.color = Qt::white;
        } else if (color == "green") {
            new_particle.color = Qt::green;
        } else if (color == "blue") {
            new_particle.color = Qt::blue;
        } else if (color == "red") {
            new_particle.color = Qt::red;
        } else if (color == "yellow") {
            new_particle.color = Qt::yellow;
        } else {
            new_particle.color = Qt::cyan;
        }


        particle_vec.append( new_particle );
    }

    updateGL();
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    // Work out length of wheel scroll
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;             // see QWheelEvent documentation

    // Change zoomed region size
    displayBoxEdge += numSteps;
    // Ensure minimum size of 1 and maximum size of 50
    displayBoxEdge = displayBoxEdge > 1.0 ? displayBoxEdge : 1;
    displayBoxEdge = displayBoxEdge > 50.0 ? 50.0 : displayBoxEdge;

    // Update view
    setView();
    updateGL();
}
