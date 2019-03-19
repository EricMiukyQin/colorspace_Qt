#include <QtOpenGL>
#include "glwidget_hsv.h"

#define PI 3.1415926535898
#define Cos(th) float(cos(PI/180*(th)))
#define Sin(th) float(sin(PI/180*(th)))
#define DEF_D 5

GLWidget_HSV::GLWidget_HSV(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    r = 1.0;
    g = 1.0;
    b = 1.0;
    HSV = {0.0, 0.0, 1.0};
    isFirstDraw = false;
    isDrawPixelColor = false;
    isDrawAreaColor = false;
    xRot = 500.0;
    yRot = 500.0;
    zRot = 0.0;
    p = nullptr;
    row = 0;
    col = 0;
    Hplus = 0.0;
    Splus = 0.0;
    Vplus = 0.0;
}

GLWidget_HSV::~GLWidget_HSV()
{
}

void GLWidget_HSV::set_hsv(float R, float G, float B){
    r = R;
    g = G;
    b = B;
    HSV = rgb2hsv({double(R), double(G), double(B)});
    updateGL();
}

void GLWidget_HSV::set_Argb(rgb_float** p2, int r, int c)
{
    p = p2;
    row = r;
    col = c;
    Hplus = 0.0;
    Splus = 0.0;
    Vplus = 0.0;
    updateGL();
}

void GLWidget_HSV::set_FirstDraw(bool a)
{
    isFirstDraw = a;
}

void GLWidget_HSV::set_DrawColorPixel(bool a)
{
    isDrawPixelColor = a;
}

void GLWidget_HSV::set_DrawColorArea(bool a)
{
    isDrawAreaColor = a;
}

void GLWidget_HSV::setHChanged(double value)
{
    if(isDrawAreaColor && !isFirstDraw){
        Hplus = value;
        update();
    }
}

void GLWidget_HSV::setSChanged(double value)
{
    if(isDrawAreaColor && !isFirstDraw){
        Splus = value / 100;
        update();
    }
}

void GLWidget_HSV::setVChanged(double value)
{
    if(isDrawAreaColor && !isFirstDraw){
        Vplus = value / 100;
        update();
    }
}

QSize GLWidget_HSV::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget_HSV::sizeHint() const
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

void GLWidget_HSV::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        updateGL();
    }
}

void GLWidget_HSV::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        updateGL();
    }
}

void GLWidget_HSV::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        updateGL();
    }
}

void GLWidget_HSV::initializeGL()
{
    qglClearColor(Qt::gray);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
}

void GLWidget_HSV::paintGL()
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
        draw_hsvCylinder();
    }
    else if(isDrawPixelColor == true && isDrawAreaColor == false && isFirstDraw == false){
        draw_edgeCylinder();
        drawPoint(HSV, r, g, b, 10, 10, 0.05);
    }
    else if(isDrawPixelColor == false && isDrawAreaColor == true && isFirstDraw == false){
        draw_edgeCylinder();
        int factor = row * col / 3600;   // show  maximum 3600 points
        int i,j;
        if(row * col <= 3600 && row * col >=1){
            for(i = 0 ; i < row ; i++){
                for(j = 0 ; j < col ; j++){
                    hsv ot = rgb2hsv({double(p[i][j].r), double(p[i][j].g), double(p[i][j].b)});
                    hsv t;
                    t.h = ot.h + Hplus <= 360 ? ot.h + Hplus : ot.h + Hplus - 360;
                    t.h = t.h >= 0 ? t.h : 0;
                    t.s = ot.s + Splus <= 1 ? ot.s + Splus : 1;
                    t.s = t.s >= 0 ? t.s : 0;
                    t.v = ot.v + Vplus <= 1 ? ot.v + Vplus : 1;
                    t.v = t.v >= 0 ? t.v : 0;
                    drawPoint(t, float(hsv2rgb(t).r), float(hsv2rgb(t).g), float(hsv2rgb(t).b), 3, 3, 0.02);
                }
            }
        }
        else{
            int cnt = qSqrt(factor)-qFloor(qSqrt(factor)) >= 0.5 ? qCeil(qSqrt(factor)) : qFloor(qSqrt(factor));
            for(i = 0 ; i < row ; i = i + cnt){
                for(j = 0 ; j < col ; j = j + cnt){
                    hsv ot = rgb2hsv({double(p[i][j].r), double(p[i][j].g), double(p[i][j].b)});
                    hsv t;
                    t.h = ot.h + Hplus <= 360 ? ot.h + Hplus : ot.h + Hplus - 360;
                    t.h = t.h >= 0 ? t.h : 0;
                    t.s = ot.s + Splus <= 1 ? ot.s + Splus : 1;
                    t.s = t.s >= 0 ? t.s : 0;
                    t.v = ot.v + Vplus <= 1 ? ot.v + Vplus : 1;
                    t.v = t.v >= 0 ? t.v : 0;
                    drawPoint(t, float(hsv2rgb(t).r), float(hsv2rgb(t).g), float(hsv2rgb(t).b), 3, 3, 0.02);
                }
            }
        }
        emit RGBChanged(0);
        emit HSVChanged(Hplus, Splus, Vplus);
        emit YUVChanged(0);
    }
}

void GLWidget_HSV::resizeGL(int width, int height)
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

void GLWidget_HSV::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void GLWidget_HSV::mouseMoveEvent(QMouseEvent *event)
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

void GLWidget_HSV::draw_hsvCylinder()
{
    int j,k;
    /* sides */
    glBegin(GL_QUAD_STRIP);
    for(j=180;j<=540;j+=DEF_D){
        rgb_double c = hsv2rgb({double(j-180), 1.0, 1.0});
        glColor3f(float(c.r), float(c.g), float(c.b));
        glVertex3f(Cos(j),+1,-Sin(j));
        glColor3f(0.0,0.0,0.0);
        glVertex3f(Cos(j),-1,-Sin(j));
    }
    glEnd();

    /* top and botton circles */
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(1.0,1.0,1.0);
    glVertex3f(0,1,0);
    for(k=180;k<=540;k+=DEF_D){
        rgb_double c = hsv2rgb({double(k-180), 1.0, 1.0});
        glColor3f(float(c.r), float(c.g), float(c.b));
        glVertex3f(Cos(k),1,-Sin(k));
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.0,0.0,0.0);
    glVertex3f(0,-1,0);
    for(k=180;k<=540;k+=DEF_D){
        glColor3f(0.0, 0.0, 0.0);
        glVertex3f(Cos(k),-1,-Sin(k));
    }
    glEnd();
}

void GLWidget_HSV::draw_graficarLineas()
{
    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, -1.0, 0.0);
    glVertex3f(0.0, 2.0, 0.0);

    glColor3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, -1.0, 0.0);
    glVertex3f(2.0, -1.0, 0.0);

    glEnd();

}

void GLWidget_HSV::drawPoint(hsv HSV, float R, float G, float B, int lats, int longs, double r)
{
    // map from HSV to coordinate
    double M1 = 0.0, M2 = 0.0, M3 = 0.0;
    if(HSV.h>180 && HSV.h<=270){
         M1 = HSV.s * cos(PI/180*(HSV.h-180));
         M3 = -HSV.s * sin(PI/180*(HSV.h-180));
    }else if(HSV.h>270 && HSV.h<=360){
         M1 = -HSV.s * cos(PI/180*(360-HSV.h));
         M3 = -HSV.s * sin(PI/180*(360-HSV.h));
    }else if(HSV.h>=0 && HSV.h<=90){
         M1 = -HSV.s * cos(PI/180*HSV.h);
         M3 = HSV.s * sin(PI/180*HSV.h);
    }else if(HSV.h>90 && HSV.h<=180){
         M1 = HSV.s * cos(PI/180*(180-HSV.h));
         M3 = HSV.s * sin(PI/180*(180-HSV.h));
    }
    M2 = 2 * HSV.v - 1;

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

void GLWidget_HSV::draw_edgeCylinder(){
    int j,k;
    /* sides */
    for(j=180;j<=540;j+=36){
        glBegin(GL_LINES);
        glColor3f(0.0, 0.0, 0.0);
        glVertex3f(Cos(j),+1,-Sin(j));
        glColor3f(0.0, 0.0, 0.0);
        glVertex3f(Cos(j),-1,-Sin(j));
        glEnd();
    }

    /* top and botton circles */
    for(k=180;k<=539;k+=1){
        glBegin(GL_LINES);
        glColor3f(0.0, 0.0, 0.0);
        glVertex3f(Cos(k),1,-Sin(k));
        glColor3f(0.0, 0.0, 0.0);
        glVertex3f(Cos(k+1),1,-Sin(k+1));
        glEnd();

        glBegin(GL_LINES);
        glColor3f(0.0, 0.0, 0.0);
        glVertex3f(Cos(k),-1,-Sin(k));
        glColor3f(0.0, 0.0, 0.0);
        glVertex3f(Cos(k+1),-1,-Sin(k+1));
        glEnd();
    }
}
