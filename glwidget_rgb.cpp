#include <QtOpenGL>
#include "glwidget_rgb.h"

GLWidget_RGB::GLWidget_RGB(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    r = 1.0;
    g = 1.0;
    b = 1.0;
    isFirstDraw = false;
    isDrawPixelColor = false;
    isDrawAreaColor = false;
    xRot = 200.0;
    yRot = 500.0;
    zRot = 0.0;
    p = nullptr;
    row = 0;
    col = 0;
    Rplus = 0.0;
    Gplus = 0.0;
    Bplus = 0.0;
}

GLWidget_RGB::~GLWidget_RGB()
{
}

void GLWidget_RGB::set_rgb(float R, float G, float B)
{
    r = R;
    g = G;
    b = B;
    updateGL();
}

void GLWidget_RGB::set_Argb(rgb_float** p2, int r, int c)
{
    p = p2;
    row = r;
    col = c;
    Rplus = 0.0;
    Gplus = 0.0;
    Bplus = 0.0;
    updateGL();
}

void GLWidget_RGB::set_FirstDraw(bool a)
{
    isFirstDraw = a;
}

void GLWidget_RGB::set_DrawColorPixel(bool a)
{
    isDrawPixelColor = a;
}

void GLWidget_RGB::set_DrawColorArea(bool a)
{
    isDrawAreaColor = a;
}

void GLWidget_RGB::setRChanged(float value)
{
    if(isDrawAreaColor && !isFirstDraw){
        Rplus = value / 255;
        update();
    }
}

void GLWidget_RGB::setGChanged(float value)
{
    if(isDrawAreaColor && !isFirstDraw){
        Gplus = value / 255;
        update();
    }
}

void GLWidget_RGB::setBChanged(float value)
{
    if(isDrawAreaColor && !isFirstDraw){
        Bplus = value / 255;
        update();
    }
}

QSize GLWidget_RGB::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget_RGB::sizeHint() const
{
    return QSize(400, 400);
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360)
        angle -= 360 * 16;
}

void GLWidget_RGB::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        updateGL();
    }
}

void GLWidget_RGB::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        updateGL();
    }
}

void GLWidget_RGB::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        updateGL();
    }
}

void GLWidget_RGB::initializeGL()
{
    qglClearColor(Qt::gray);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
}

void GLWidget_RGB::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -10.0);
    glRotatef(GLfloat(xRot / 16.0), 1.0, 0.0, 0.0);
    glRotatef(GLfloat(yRot / 16.0), 0.0, 1.0, 0.0);
    glRotatef(GLfloat(zRot / 16.0), 0.0, 0.0, 1.0);
    draw_graficarLineas();
    if((isDrawPixelColor == false && isDrawAreaColor == false) ||
       (isDrawPixelColor == true && isDrawAreaColor == false && isFirstDraw == true) ||
       (isDrawPixelColor == false && isDrawAreaColor == true && isFirstDraw == true)){
        draw_rgbCube();
    }
    else if(isDrawPixelColor == true && isDrawAreaColor == false && isFirstDraw == false){
        draw_edgeCube();
        drawPoint(r, g, b, 10, 10, 0.05);
    }
    else if(isDrawPixelColor == false && isDrawAreaColor == true && isFirstDraw == false){
        draw_edgeCube();
        int factor = row * col / 3600;   // show  maximum 3600 points
        int i,j;
        if(row * col <= 3600 && row * col >=1){
            for(i = 0 ; i < row ; i++){
                for(j = 0 ; j < col ; j++){
                    float tr = p[i][j].r + Rplus <= 1 ? p[i][j].r + Rplus : 1;
                    tr = tr > 0 ? tr : 0;
                    float tg = p[i][j].g + Gplus <= 1 ? p[i][j].g + Gplus : 1;
                    tg = tg > 0 ? tg : 0;
                    float tb = p[i][j].b + Bplus <= 1 ? p[i][j].b + Bplus : 1;
                    tb = tb > 0 ? tb : 0;
                    drawPoint(tr, tg, tb, 3, 3, 0.02);
                }
            }
        }
        else{
            int cnt = qSqrt(factor)-qFloor(qSqrt(factor)) >= 0.5 ? qCeil(qSqrt(factor)) : qFloor(qSqrt(factor));
            for(i = 0 ; i < row ; i = i + cnt){
                for(j = 0 ; j < col ; j = j + cnt){
                    float tr = p[i][j].r + Rplus <= 1 ? p[i][j].r + Rplus : 1;
                    tr = tr > 0 ? tr : 0;
                    float tg = p[i][j].g + Gplus <= 1 ? p[i][j].g + Gplus : 1;
                    tg = tg > 0 ? tg : 0;
                    float tb = p[i][j].b + Bplus <= 1 ? p[i][j].b + Bplus : 1;
                    tb = tb > 0 ? tb : 0;
                    drawPoint(tr, tg, tb, 3, 3, 0.02);
                }
            }
        }
        emit RGBChanged(Rplus, Gplus, Bplus);
        emit HSVChanged(0);
        emit YUVChanged(0);
    }
}

void GLWidget_RGB::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifdef QT_OPENGL_ES_1
    glOrthof(-2, +2, -2, +2, 1.0, 15.0);
#else
    glOrtho(-2, +2, -2, +2, 1.0, 15.0);
#endif
    glMatrixMode(GL_MODELVIEW);
}

void GLWidget_RGB::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void GLWidget_RGB::mouseMoveEvent(QMouseEvent *event)
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

void GLWidget_RGB::draw_rgbCube()
{
    // up
    glBegin(GL_POLYGON);
    glColor3f(1.0, 1.0, 0.0);
    glVertex3f(1.0, 1.0, 1.0);
    glColor3f(1.0, 1.0, 1.0);
    glVertex3f(1.0, 1.0, -1.0);
    glColor3f(0.0, 1.0, 1.0);
    glVertex3f(-1.0, 1.0, -1.0);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(-1.0, 1.0, 1.0);
    glEnd();

    // down
    glBegin(GL_POLYGON);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(1.0, -1.0, 1.0);
    glColor3f(1.0, 0.0, 1.0);
    glVertex3f(1.0, -1.0, -1.0);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(-1.0, -1.0, -1.0);
    glColor3f(0.0, 0.0, 0.0);
    glVertex3f(-1.0, -1.0, 1.0);
    glEnd();

    // front
    glBegin(GL_POLYGON);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(1.0, -1.0, 1.0);
    glColor3f(1.0, 1.0, 0.0);
    glVertex3f(1.0, 1.0, 1.0);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(-1.0, 1.0, 1.0);
    glColor3f(0.0, 0.0, 0.0);
    glVertex3f(-1.0, -1.0, 1.0);
    glEnd();

    // behind
    glBegin(GL_POLYGON);
    glColor3f(1.0, 0.0, 1.0);
    glVertex3f(1.0, -1.0, -1.0);
    glColor3f(1.0, 1.0, 1.0);
    glVertex3f(1.0, 1.0, -1.0);
    glColor3f(0.0, 1.0, 1.0);
    glVertex3f(-1.0, 1.0, -1.0);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(-1.0, -1.0, -1.0);
    glEnd();

    // left
    glBegin(GL_POLYGON);
    glColor3f(0.0, 0.0, 0.0);
    glVertex3f(-1.0, -1.0, 1.0);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(-1.0, 1.0, 1.0);
    glColor3f(0.0, 1.0, 1.0);
    glVertex3f(-1.0, 1.0, -1.0);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(-1.0, -1.0, -1.0);
    glEnd();

    // right
    glBegin(GL_POLYGON);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(1.0, -1.0, 1.0);
    glColor3f(1.0, 1.0, 0.0);
    glVertex3f(1.0, 1.0, 1.0);
    glColor3f(1.0, 1.0, 1.0);
    glVertex3f(1.0, 1.0, -1.0);
    glColor3f(1.0, 0.0, 1.0);
    glVertex3f(1.0, -1.0, -1.0);
    glEnd();
}

void GLWidget_RGB::draw_graficarLineas()
{
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(-1.0, -1.0, 1.0);
    glVertex3f(2.0, -1.0, 1.0);

    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(-1.0, -1.0, 1.0);
    glVertex3f(-1.0, 2.0, 1.0);

    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(-1.0, -1.0, 1.0);
    glVertex3f(-1.0, -1.0, -2.0);
    glEnd();
}

void GLWidget_RGB::drawPoint(float R, float G, float B, int lats, int longs, double r)
{
    // map from RGB to coordinate
    float M1 = R*2-1;
    float M2 = G*2-1;
    float M3 = 1-B*2;

    int i, j;
    for(i = 0; i <= lats; i++){
        double lat0 = M_PI * (-0.5 + double(i - 1) / lats);
        double z0  = sin(lat0);
        double zr0 = cos(lat0);

        double lat1 = M_PI * (-0.5 + double(i) / lats);
        double z1  = sin(lat1);
        double zr1 = cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for(j = 0; j <= longs; j++){
            double lng = 2 * M_PI * double(j - 1) / longs;
            double x = cos(lng);
            double y = sin(lng);
            glColor3f(R, G, B);
            glNormal3f(float(r * x * zr0) + M1, float(r * y * zr0) + M2, float(r * z0) + M3);
            glColor3f(R, G, B);
            glVertex3f(float(r * x * zr0) + M1, float(r * y * zr0) + M2, float(r * z0) + M3);
            glColor3f(R, G, B);
            glNormal3f(float(r * x * zr1) + M1, float(r * y * zr1) + M2, float(r * z1) + M3);
            glColor3f(R, G, B);
            glVertex3f(float(r * x * zr1) + M1, float(r * y * zr1) + M2, float(r * z1) + M3);
        }
        glEnd();
    }
}

void GLWidget_RGB::draw_edgeCube(){
    // up
    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 0.0);
    glVertex3f(1.0, 1.0, 1.0);
    glVertex3f(1.0, 1.0, -1.0);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 0.0);
    glVertex3f(1.0, 1.0, -1.0);
    glVertex3f(-1.0, 1.0, -1.0);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 0.0);
    glVertex3f(-1.0, 1.0, -1.0);
    glVertex3f(-1.0, 1.0, 1.0);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 0.0);
    glVertex3f(-1.0, 1.0, 1.0);
    glVertex3f(1.0, 1.0, 1.0);

    // down
    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 0.0);
    glVertex3f(1.0, -1.0, 1.0);
    glVertex3f(1.0, -1.0, -1.0);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 0.0);
    glVertex3f(1.0, -1.0, -1.0);
    glVertex3f(-1.0, -1.0, -1.0);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 0.0);
    glVertex3f(-1.0, -1.0, -1.0);
    glVertex3f(-1.0, -1.0, 1.0);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 0.0);
    glVertex3f(-1.0, -1.0, 1.0);
    glVertex3f(1.0, -1.0, 1.0);
    glEnd();

    // front
    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 0.0);
    glVertex3f(1.0, -1.0, 1.0);
    glVertex3f(1.0, 1.0, 1.0);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 0.0);
    glVertex3f(1.0, 1.0, 1.0);
    glVertex3f(-1.0, 1.0, 1.0);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 0.0);
    glVertex3f(-1.0, 1.0, 1.0);
    glVertex3f(-1.0, -1.0, 1.0);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 0.0);
    glVertex3f(-1.0, -1.0, 1.0);
    glVertex3f(1.0, -1.0, 1.0);
    glEnd();

    // behind
    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 0.0);
    glVertex3f(1.0, -1.0, -1.0);
    glVertex3f(1.0, 1.0, -1.0);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 0.0);
    glVertex3f(1.0, 1.0, -1.0);
    glVertex3f(-1.0, 1.0, -1.0);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 0.0);
    glVertex3f(-1.0, 1.0, -1.0);
    glVertex3f(-1.0, -1.0, -1.0);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 0.0);
    glVertex3f(-1.0, -1.0, -1.0);
    glVertex3f(1.0, -1.0, -1.0);
    glEnd();
}
