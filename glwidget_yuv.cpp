#include <QtOpenGL>
#include "glwidget_yuv.h"

GLWidget_YUV::GLWidget_YUV(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    r = 1.0;
    g = 1.0;
    b = 1.0;
    YUV = rgb2yuv(checkrgb({int(r * 255), int(g * 255), int(b * 255)}));
    isFirstDraw = false;
    isDrawPixelColor = false;
    isDrawAreaColor = false;
    xRot = 200.0;
    yRot = -500.0;
    zRot = 0.0;
    p = nullptr;
    row = 0;
    col = 0;
    Yplus = 0.0;
    Uplus = 0.0;
    Vplus = 0.0;
}

GLWidget_YUV::~GLWidget_YUV()
{
}

void GLWidget_YUV::set_yuv(float R, float G, float B){
    r = R;
    g = G;
    b = B;
    YUV = rgb2yuv(checkrgb({int(R * 255), int(G * 255), int(B * 255)}));
    updateGL();
}

void GLWidget_YUV::set_Argb(rgb_float** p2, int r, int c)
{
    p = p2;
    row = r;
    col = c;
    Yplus = 0.0;
    Uplus = 0.0;
    Vplus = 0.0;
    updateGL();
}

void GLWidget_YUV::set_FirstDraw(bool a)
{
    isFirstDraw = a;
}

void GLWidget_YUV::set_DrawColorPixel(bool a)
{
    isDrawPixelColor = a;
}

void GLWidget_YUV::set_DrawColorArea(bool a)
{
    isDrawAreaColor = a;
}

void GLWidget_YUV::setYChanged(double value)
{
    if(isDrawAreaColor && !isFirstDraw){
        Yplus = value / 100;
        update();
    }
}

void GLWidget_YUV::setUChanged(double value)
{
    if(isDrawAreaColor && !isFirstDraw){
        Uplus = value / 50;
        update();
    }
}

void GLWidget_YUV::setVChanged(double value)
{
    if(isDrawAreaColor && !isFirstDraw){
        Vplus = value / 50;
        update();
    }
}

QSize GLWidget_YUV::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget_YUV::sizeHint() const
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

void GLWidget_YUV::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        updateGL();
    }
}

void GLWidget_YUV::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        updateGL();
    }
}

void GLWidget_YUV::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        updateGL();
    }
}

void GLWidget_YUV::initializeGL()
{
    qglClearColor(Qt::gray);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
}

void GLWidget_YUV::paintGL()
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
        drawPoint(YUV, r, g, b, 10, 10, 0.05);
    }
    else if(isDrawPixelColor == false && isDrawAreaColor == true && isFirstDraw == false){
        draw_edgeCube();
        int factor = row * col / 3600;   // show  maximum 3600 points
        int i,j;
        if(row * col <= 3600 && row * col >=1){
            for(i = 0 ; i < row ; i++){
                for(j = 0 ; j < col ; j++){
                    yuv ot = rgb2yuv(checkrgb({int(p[i][j].r * 255), int(p[i][j].g * 255), int(p[i][j].b * 255)}));
                    yuv t = checkyuv({ot.y + Yplus, ot.u + Uplus, ot.v + Vplus});
                    drawPoint(t, yuv2rgb(t).r, yuv2rgb(t).g, yuv2rgb(t).b, 3, 3, 0.02);
                }
            }
        }
        else{
            int cnt = qSqrt(factor)-qFloor(qSqrt(factor)) >= 0.5 ? qCeil(qSqrt(factor)) : qFloor(qSqrt(factor));
            for(i = 0 ; i < row ; i = i + cnt){
                for(j = 0 ; j < col ; j = j + cnt){
                    yuv ot = rgb2yuv(checkrgb({int(p[i][j].r * 255), int(p[i][j].g * 255), int(p[i][j].b * 255)}));
                    yuv t = checkyuv({ot.y + Yplus, ot.u + Uplus, ot.v + Vplus});
                    drawPoint(t, yuv2rgb(t).r, yuv2rgb(t).g, yuv2rgb(t).b, 3, 3, 0.02);
                }
            }
        }
        emit RGBChanged(0);
        emit HSVChanged(0);
        emit YUVChanged(Yplus, Uplus, Vplus);
    }
}

void GLWidget_YUV::resizeGL(int width, int height)
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

void GLWidget_YUV::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void GLWidget_YUV::mouseMoveEvent(QMouseEvent *event)
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

void GLWidget_YUV::draw_rgbCube()
{
    //color
    rgb_float rgb1 = {yuv2rgb({0.0,0.5,0.5}).r, yuv2rgb({0.0,0.5,0.5}).g, yuv2rgb({0.0,0.5,0.5}).b};
    rgb_float rgb2 = {yuv2rgb({1.0,0.5,0.5}).r, yuv2rgb({1.0,0.5,0.5}).g, yuv2rgb({1.0,0.5,0.5}).b};
    rgb_float rgb3 = {yuv2rgb({1.0,-0.5,0.5}).r, yuv2rgb({1.0,-0.5,0.5}).g, yuv2rgb({1.0,-0.5,0.5}).b};
    rgb_float rgb4 = {yuv2rgb({0.0,-0.5,0.5}).r, yuv2rgb({0.0,-0.5,0.5}).g, yuv2rgb({0.0,-0.5,0.5}).b};

    rgb_float rgba = {yuv2rgb({0.0,0.5,-0.5}).r, yuv2rgb({0.0,0.5,-0.5}).g, yuv2rgb({0.0,0.5,-0.5}).b};
    rgb_float rgbb = {yuv2rgb({1.0,0.5,-0.5}).r, yuv2rgb({1.0,0.5,-0.5}).g, yuv2rgb({1.0,0.5,-0.5}).b};
    rgb_float rgbc = {yuv2rgb({1.0,-0.5,-0.5}).r, yuv2rgb({1.0,-0.5,-0.5}).g, yuv2rgb({1.0,-0.5,-0.5}).b};
    rgb_float rgbd = {yuv2rgb({0.0,-0.5,-0.5}).r, yuv2rgb({0.0,-0.5,-0.5}).g, yuv2rgb({0.0,-0.5,-0.5}).b};

    // up
    rgb_float rgbUp = {yuv2rgb({0.5,0.0,0.5}).r, yuv2rgb({0.5,0.0,0.5}).g, yuv2rgb({0.5,0.0,0.5}).b};
    rgb_float rgbUp1 = {yuv2rgb({0.5,0.5,0.5}).r, yuv2rgb({0.0,0.5,0.5}).g, yuv2rgb({0.0,0.5,0.5}).b};
    rgb_float rgbUp2 = {yuv2rgb({1.0,0.0,0.5}).r, yuv2rgb({1.0,0.0,0.5}).g, yuv2rgb({1.0,0.0,0.5}).b};
    rgb_float rgbUp3 = {yuv2rgb({0.5,-0.5,0.5}).r, yuv2rgb({1.0,-0.5,0.5}).g, yuv2rgb({1.0,-0.5,0.5}).b};
    rgb_float rgbUp4 = {yuv2rgb({0.0,0.0,0.5}).r, yuv2rgb({0.0,0.0,0.5}).g, yuv2rgb({0.0,0.0,0.5}).b};

    glBegin(GL_POLYGON);
    glColor3f(rgb1.r, rgb1.g, rgb1.b);        // 1
    glVertex3f(1.0, 1.0, 1.0);
    glColor3f(rgbUp1.r, rgbUp1.g, rgbUp1.b);  // Up1
    glVertex3f(1.0, 1.0, 0.0);
    glColor3f(rgbUp.r, rgbUp.g, rgbUp.b);     // Up
    glVertex3f(0.0, 1.0, 0.0);
    glColor3f(rgbUp4.r, rgbUp4.g, rgbUp4.b);  // Up4
    glVertex3f(0.0, 1.0, 1.0);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(rgbUp1.r, rgbUp1.g, rgbUp1.b);  // Up1
    glVertex3f(1.0, 1.0, 0.0);
    glColor3f(rgb2.r, rgb2.g, rgb2.b);        // 2
    glVertex3f(1.0, 1.0, -1.0);
    glColor3f(rgbUp2.r, rgbUp2.g, rgbUp2.b);  // Up2
    glVertex3f(0.0, 1.0, -1.0);
    glColor3f(rgbUp.r, rgbUp.g, rgbUp.b);     // Up
    glVertex3f(0.0, 1.0, 0.0);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(rgbUp.r, rgbUp.g, rgbUp.b);     // Up
    glVertex3f(0.0, 1.0, 0.0);
    glColor3f(rgbUp2.r, rgbUp2.g, rgbUp2.b);  // Up2
    glVertex3f(0.0, 1.0, -1.0);
    glColor3f(rgb3.r, rgb3.g, rgb3.b);        // 3
    glVertex3f(-1.0, 1.0, -1.0);
    glColor3f(rgbUp3.r, rgbUp3.g, rgbUp3.b);  // Up3
    glVertex3f(-1.0, 1.0, 0.0);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(rgbUp4.r, rgbUp4.g, rgbUp4.b);  // Up4
    glVertex3f(0.0, 1.0, 1.0);
    glColor3f(rgbUp.r, rgbUp.g, rgbUp.b);     // Up
    glVertex3f(0.0, 1.0, 0.0);
    glColor3f(rgbUp3.r, rgbUp3.g, rgbUp3.b);  // Up3
    glVertex3f(-1.0, 1.0, 0.0);
    glColor3f(rgb4.r, rgb4.g, rgb4.b);        // 4
    glVertex3f(-1.0, 1.0, 1.0);
    glEnd();

    // down
    rgb_float rgbDown = {yuv2rgb({0.5,0.0,-0.5}).r, yuv2rgb({0.5,0.0,-0.5}).g, yuv2rgb({0.5,0.0,-0.5}).b};
    rgb_float rgbDown1 = {yuv2rgb({0.5,0.5,-0.5}).r, yuv2rgb({0.0,0.5,-0.5}).g, yuv2rgb({0.0,0.5,-0.5}).b};
    rgb_float rgbDown2 = {yuv2rgb({1.0,0.0,-0.5}).r, yuv2rgb({1.0,0.0,-0.5}).g, yuv2rgb({1.0,0.0,-0.5}).b};
    rgb_float rgbDown3 = {yuv2rgb({0.5,-0.5,-0.5}).r, yuv2rgb({1.0,-0.5,-0.5}).g, yuv2rgb({1.0,-0.5,-0.5}).b};
    rgb_float rgbDown4 = {yuv2rgb({0.0,0.0,-0.5}).r, yuv2rgb({0.0,0.0,-0.5}).g, yuv2rgb({0.0,0.0,-0.5}).b};

    glBegin(GL_POLYGON);
    glColor3f(rgba.r, rgba.g, rgba.b);              // a
    glVertex3f(1.0, -1.0, 1.0);
    glColor3f(rgbDown1.r, rgbDown1.g, rgbDown1.b);  // Down1
    glVertex3f(1.0, -1.0, 0.0);
    glColor3f(rgbDown.r, rgbDown.g, rgbDown.b);     // Down
    glVertex3f(0.0, -1.0, 0.0);
    glColor3f(rgbDown4.r, rgbDown4.g, rgbDown4.b);  // Down4
    glVertex3f(0.0, -1.0, 1.0);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(rgbDown1.r, rgbDown1.g, rgbDown1.b);  // Down1
    glVertex3f(1.0, -1.0, 0.0);
    glColor3f(rgbb.r, rgbb.g, rgbb.b);              // b
    glVertex3f(1.0, -1.0, -1.0);
    glColor3f(rgbDown2.r, rgbDown2.g, rgbDown2.b);  // Down2
    glVertex3f(0.0, -1.0, -1.0);
    glColor3f(rgbDown.r, rgbDown.g, rgbDown.b);     // Down
    glVertex3f(0.0, -1.0, 0.0);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(rgbDown.r, rgbDown.g, rgbDown.b);     // Down
    glVertex3f(0.0, -1.0, 0.0);
    glColor3f(rgbDown2.r, rgbDown2.g, rgbDown2.b);  // Down2
    glVertex3f(0.0, -1.0, -1.0);
    glColor3f(rgbc.r, rgbc.g, rgbc.b);              // c
    glVertex3f(-1.0, -1.0, -1.0);
    glColor3f(rgbDown3.r, rgbDown3.g, rgbDown3.b);  // Down3
    glVertex3f(-1.0, -1.0, 0.0);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(rgbDown4.r, rgbDown4.g, rgbDown4.b);  // Down4
    glVertex3f(0.0, -1.0, 1.0);
    glColor3f(rgbDown.r, rgbDown.g, rgbDown.b);     // Down
    glVertex3f(0.0, -1.0, 0.0);
    glColor3f(rgbDown3.r, rgbDown3.g, rgbDown3.b);  // Down3
    glVertex3f(-1.0, -1.0, 0.0);
    glColor3f(rgbd.r, rgbd.g, rgbd.b);              // d
    glVertex3f(-1.0, -1.0, 1.0);
    glEnd();

    // front
    rgb_float rgbFront = {yuv2rgb({0.0,0.0,0.0}).r, yuv2rgb({0.0,0.0,0.0}).g, yuv2rgb({0.0,0.0,0.0}).b};
    rgb_float rgbFront1 = {yuv2rgb({0.0,0.5,0.0}).r, yuv2rgb({0.0,0.5,0.0}).g, yuv2rgb({0.0,0.5,0.0}).b};
    rgb_float rgbFront2 = {yuv2rgb({0.0,0.0,0.5}).r, yuv2rgb({0.0,0.0,0.5}).g, yuv2rgb({0.0,0.0,0.5}).b};
    rgb_float rgbFront3 = {yuv2rgb({0.0,-0.5,0.0}).r, yuv2rgb({0.0,-0.5,0.0}).g, yuv2rgb({0.0,-0.5,0.0}).b};
    rgb_float rgbFront4 = {yuv2rgb({0.0,0.0,-0.5}).r, yuv2rgb({0.0,0.0,-0.5}).g, yuv2rgb({0.0,0.0,-0.5}).b};

    glBegin(GL_POLYGON);
    glColor3f(rgba.r, rgba.g, rgba.b);                 // a
    glVertex3f(1.0, -1.0, 1.0);
    glColor3f(rgbFront1.r, rgbFront1.g, rgbFront1.b);  // Front1
    glVertex3f(1.0, 0.0, 1.0);
    glColor3f(rgbFront.r, rgbFront.g, rgbFront.b);     // Front
    glVertex3f(0.0, 0.0, 1.0);
    glColor3f(rgbFront4.r, rgbFront4.g, rgbFront4.b);  // Front4
    glVertex3f(0.0, -1.0, 1.0);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(rgbFront1.r, rgbFront1.g, rgbFront1.b);  // Front1
    glVertex3f(1.0, 0.0, 1.0);
    glColor3f(rgb1.r, rgb1.g, rgb1.b);                 // 1
    glVertex3f(1.0, 1.0, 1.0);
    glColor3f(rgbFront2.r, rgbFront2.g, rgbFront2.b);  // Front2
    glVertex3f(0.0, 1.0, 1.0);
    glColor3f(rgbFront.r, rgbFront.g, rgbFront.b);     // Front
    glVertex3f(0.0, 0.0, 1.0);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(rgbFront.r, rgbFront.g, rgbFront.b);     // Front
    glVertex3f(0.0, 0.0, 1.0);
    glColor3f(rgbFront2.r, rgbFront2.g, rgbFront2.b);  // Front2
    glVertex3f(0.0, 1.0, 1.0);
    glColor3f(rgb4.r, rgb4.g, rgb4.b);                 // 4
    glVertex3f(-1.0, 1.0, 1.0);
    glColor3f(rgbFront3.r, rgbFront3.g, rgbFront3.b);  // Front3
    glVertex3f(-1.0, 0.0, 1.0);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(rgbFront4.r, rgbFront4.g, rgbFront4.b);  // Front4
    glVertex3f(0.0, -1.0, 1.0);
    glColor3f(rgbFront.r, rgbFront.g, rgbFront.b);     // Front
    glVertex3f(0.0, 0.0, 1.0);
    glColor3f(rgbFront3.r, rgbFront3.g, rgbFront3.b);  // Front3
    glVertex3f(-1.0, 0.0, 1.0);
    glColor3f(rgbd.r, rgbd.g, rgbd.b);                 // d
    glVertex3f(-1.0, -1.0, 1.0);
    glEnd();

    // behind
    rgb_float rgbBehind = {yuv2rgb({1.0,0.0,0.0}).r, yuv2rgb({1.0,0.0,0.0}).g, yuv2rgb({1.0,0.0,0.0}).b};
    rgb_float rgbBehind1 = {yuv2rgb({1.0,0.5,0.0}).r, yuv2rgb({1.0,0.5,0.0}).g, yuv2rgb({1.0,0.5,0.0}).b};
    rgb_float rgbBehind2 = {yuv2rgb({1.0,0.0,0.5}).r, yuv2rgb({1.0,0.0,0.5}).g, yuv2rgb({1.0,0.0,0.5}).b};
    rgb_float rgbBehind3 = {yuv2rgb({1.0,-0.5,0.0}).r, yuv2rgb({1.0,-0.5,0.0}).g, yuv2rgb({1.0,-0.5,0.0}).b};
    rgb_float rgbBehind4 = {yuv2rgb({1.0,0.0,-0.5}).r, yuv2rgb({1.0,0.0,-0.5}).g, yuv2rgb({1.0,0.0,-0.5}).b};

    glBegin(GL_POLYGON);
    glColor3f(rgbb.r, rgbb.g, rgbb.b);                    // b
    glVertex3f(1.0, -1.0, -1.0);
    glColor3f(rgbBehind1.r, rgbBehind1.g, rgbBehind1.b);  // Behind1
    glVertex3f(1.0, 0.0, -1.0);
    glColor3f(rgbBehind.r, rgbBehind.g, rgbBehind.b);     // Behind
    glVertex3f(0.0, 0.0, -1.0);
    glColor3f(rgbBehind4.r, rgbBehind4.g, rgbBehind4.b);  // Behind4
    glVertex3f(0.0, -1.0, -1.0);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(rgbBehind1.r, rgbBehind1.g, rgbBehind1.b);  // Behind1
    glVertex3f(1.0, 0.0, -1.0);
    glColor3f(rgb2.r, rgb2.g, rgb2.b);                    // 2
    glVertex3f(1.0, 1.0, -1.0);
    glColor3f(rgbBehind2.r, rgbBehind2.g, rgbBehind2.b);  // Behind2
    glVertex3f(0.0, 1.0, -1.0);
    glColor3f(rgbBehind.r, rgbBehind.g, rgbBehind.b);     // Behind
    glVertex3f(0.0, 0.0, -1.0);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(rgbBehind.r, rgbBehind.g, rgbBehind.b);     // Behind
    glVertex3f(0.0, 0.0, -1.0);
    glColor3f(rgbBehind2.r, rgbBehind2.g, rgbBehind2.b);  // Behind2
    glVertex3f(0.0, 1.0, -1.0);
    glColor3f(rgb3.r, rgb3.g, rgb3.b);                    // 3
    glVertex3f(-1.0, 1.0, -1.0);
    glColor3f(rgbBehind3.r, rgbBehind3.g, rgbBehind3.b);  // Behind3
    glVertex3f(-1.0, 0.0, -1.0);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(rgbBehind4.r, rgbBehind4.g, rgbBehind4.b);  // Behind4
    glVertex3f(0.0, -1.0, -1.0);
    glColor3f(rgbBehind.r, rgbBehind.g, rgbBehind.b);     // Behind
    glVertex3f(0.0, 0.0, -1.0);
    glColor3f(rgbBehind3.r, rgbBehind3.g, rgbBehind3.b);  // Behind3
    glVertex3f(-1.0, 0.0, -1.0);
    glColor3f(rgbc.r, rgbc.g, rgbc.b);                    // c
    glVertex3f(-1.0, -1.0, -1.0);
    glEnd();

    // left
    rgb_float rgbLeft = {yuv2rgb({0.5,-0.5,0.0}).r, yuv2rgb({0.5,-0.5,0.0}).g, yuv2rgb({0.5,-0.5,0.0}).b};
    rgb_float rgbLeft1 = {yuv2rgb({0.0,-0.5,0.0}).r, yuv2rgb({0.0,-0.5,0.0}).g, yuv2rgb({0.0,-0.5,0.0}).b};
    rgb_float rgbLeft2 = {yuv2rgb({0.5,-0.5,0.5}).r, yuv2rgb({0.5,-0.5,0.5}).g, yuv2rgb({0.5,-0.5,0.5}).b};
    rgb_float rgbLeft3 = {yuv2rgb({1.0,-0.5,0.0}).r, yuv2rgb({1.0,-0.5,0.0}).g, yuv2rgb({1.0,-0.5,0.0}).b};
    rgb_float rgbLeft4 = {yuv2rgb({0.5,-0.5,-0.5}).r, yuv2rgb({1.0,-0.5,-0.5}).g, yuv2rgb({1.0,-0.5,-0.5}).b};

    glBegin(GL_POLYGON);
    glColor3f(rgbd.r, rgbd.g, rgbd.b);              // d
    glVertex3f(-1.0, -1.0, 1.0);
    glColor3f(rgbLeft1.r, rgbLeft1.g, rgbLeft1.b);  // Left1
    glVertex3f(-1.0, 0.0, 1.0);
    glColor3f(rgbLeft.r, rgbLeft.g, rgbLeft.b);     // Left
    glVertex3f(-1.0, 0.0, 0.0);
    glColor3f(rgbLeft4.r, rgbLeft4.g, rgbLeft4.b);  // Left4
    glVertex3f(-1.0, -1.0, 0.0);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(rgbLeft1.r, rgbLeft1.g, rgbLeft1.b);  // Left1
    glVertex3f(-1.0, 0.0, 1.0);
    glColor3f(rgb4.r, rgb4.g, rgb4.b);              // 4
    glVertex3f(-1.0, 1.0, 1.0);
    glColor3f(rgbLeft2.r, rgbLeft2.g, rgbLeft2.b);  // Left2
    glVertex3f(-1.0, 1.0, 0.0);
    glColor3f(rgbLeft.r, rgbLeft.g, rgbLeft.b);     // Left
    glVertex3f(-1.0, 0.0, 0.0);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(rgbLeft.r, rgbLeft.g, rgbLeft.b);     // Left
    glVertex3f(-1.0, 0.0, 0.0);
    glColor3f(rgbLeft2.r, rgbLeft2.g, rgbLeft2.b);  // Left2
    glVertex3f(-1.0, 1.0, 0.0);
    glColor3f(rgb3.r, rgb3.g, rgb3.b);              // 3
    glVertex3f(-1.0, 1.0, -1.0);
    glColor3f(rgbLeft3.r, rgbLeft3.g, rgbLeft3.b);  // Left3
    glVertex3f(-1.0, 0.0, -1.0);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(rgbLeft4.r, rgbLeft4.g, rgbLeft4.b);  // Left4
    glVertex3f(-1.0, -1.0, 0.0);
    glColor3f(rgbLeft.r, rgbLeft.g, rgbLeft.b);     // Left
    glVertex3f(-1.0, 0.0, 0.0);
    glColor3f(rgbLeft3.r, rgbLeft3.g, rgbLeft3.b);  // Left3
    glVertex3f(-1.0, 0.0, -1.0);
    glColor3f(rgbc.r, rgbc.g, rgbc.b);              // c
    glVertex3f(-1.0, -1.0, -1.0);
    glEnd();

    // right
    rgb_float rgbRight = {yuv2rgb({0.5,0.5,0.0}).r, yuv2rgb({0.5,0.5,0.0}).g, yuv2rgb({0.5,0.5,0.0}).b};
    rgb_float rgbRight1 = {yuv2rgb({0.0,0.5,0.0}).r, yuv2rgb({0.0,0.5,0.0}).g, yuv2rgb({0.0,0.5,0.0}).b};
    rgb_float rgbRight2 = {yuv2rgb({0.5,0.5,0.5}).r, yuv2rgb({0.5,0.5,0.5}).g, yuv2rgb({0.5,0.5,0.5}).b};
    rgb_float rgbRight3 = {yuv2rgb({1.0,0.5,0.0}).r, yuv2rgb({1.0,0.5,0.0}).g, yuv2rgb({1.0,0.5,0.0}).b};
    rgb_float rgbRight4 = {yuv2rgb({0.5,0.5,-0.5}).r, yuv2rgb({1.0,0.5,-0.5}).g, yuv2rgb({1.0,0.5,-0.5}).b};

    glBegin(GL_POLYGON);
    glColor3f(rgba.r, rgba.g, rgba.b);                 // a
    glVertex3f(1.0, -1.0, 1.0);
    glColor3f(rgbRight1.r, rgbRight1.g, rgbRight1.b);  // Right1
    glVertex3f(1.0, 0.0, 1.0);
    glColor3f(rgbRight.r, rgbRight.g, rgbRight.b);     // Right
    glVertex3f(1.0, 0.0, 0.0);
    glColor3f(rgbRight4.r, rgbRight4.g, rgbRight4.b);  // Right4
    glVertex3f(1.0, -1.0, 0.0);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(rgbRight1.r, rgbRight1.g, rgbRight1.b);  // Right1
    glVertex3f(1.0, 0.0, 1.0);
    glColor3f(rgb1.r, rgb1.g, rgb1.b);                 // 1
    glVertex3f(1.0, 1.0, 1.0);
    glColor3f(rgbRight2.r, rgbRight2.g, rgbRight2.b);  // Right2
    glVertex3f(1.0, 1.0, 0.0);
    glColor3f(rgbRight.r, rgbRight.g, rgbRight.b);     // Right
    glVertex3f(1.0, 0.0, 0.0);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(rgbRight.r, rgbRight.g, rgbRight.b);     // Right
    glVertex3f(1.0, 0.0, 0.0);
    glColor3f(rgbRight2.r, rgbRight2.g, rgbRight2.b);  // Right2
    glVertex3f(1.0, 1.0, 0.0);
    glColor3f(rgb2.r, rgb2.g, rgb2.b);                 // 2
    glVertex3f(1.0, 1.0, -1.0);
    glColor3f(rgbRight3.r, rgbRight3.g, rgbRight3.b);  // Right3
    glVertex3f(1.0, 0.0, -1.0);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(rgbRight4.r, rgbRight4.g, rgbRight4.b);  // Right4
    glVertex3f(1.0, -1.0, 0.0);
    glColor3f(rgbRight.r, rgbRight.g, rgbRight.b);     // Right
    glVertex3f(1.0, 0.0, 0.0);
    glColor3f(rgbRight3.r, rgbRight3.g, rgbRight3.b);  // Right3
    glVertex3f(1.0, 0.0, -1.0);
    glColor3f(rgbb.r, rgbb.g, rgbb.b);                 // b
    glVertex3f(1.0, -1.0, -1.0);
    glEnd();
}

void GLWidget_YUV::draw_graficarLineas()
{
    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, -2.0);

    glColor3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 1.0);
    glVertex3f(2.0, 0.0, 1.0);

    glColor3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 2.0, 1.0);
    glEnd();

}

void GLWidget_YUV::drawPoint(yuv YUV, float R, float G, float B, int lats, int longs, double r)
{
    // map from YUV to coordinate
    double M1 = YUV.u * 2;
    double M2 = YUV.v * 2;
    double M3 = 1 - 2 * YUV.y;

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
            glNormal3f(float(r * x * zr0 + M1), float(r * y * zr0 + M2), float(r * z0 + M3));
            glColor3f(R, G, B);
            glVertex3f(float(r * x * zr0 + M1), float(r * y * zr0 + M2), float(r * z0 + M3));
            glColor3f(R, G, B);
            glNormal3f(float(r * x * zr1 + M1), float(r * y * zr1 + M2), float(r * z1 + M3));
            glColor3f(R, G, B);
            glVertex3f(float(r * x * zr1 + M1), float(r * y * zr1 + M2), float(r * z1 + M3));
        }
        glEnd();
    }
}

void GLWidget_YUV::draw_edgeCube(){
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
