#ifndef GLWIDGET_HSV_H
#define GLWIDGET_HSV_H

#include <math.h>
#include "colortype.h"

class GLWidget_HSV : public QGLWidget
{
    Q_OBJECT

public:
    explicit GLWidget_HSV(QWidget *parent = nullptr);
    ~GLWidget_HSV();

    /*****  Functions for rotation  *****/
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);

    /********  Functions for setting private values  *******/
    void set_hsv(float R, float G, float B);
    void set_Argb(rgb_float** p, int row, int col);
    void set_FirstDraw(bool);
    void set_DrawColorPixel(bool);
    void set_DrawColorArea(bool);

public slots:
    void setHChanged(double value);
    void setSChanged(double value);
    void setVChanged(double value);

signals:
    void RGBChanged(int value);
    void HSVChanged(double Hplus, double Splus, double Vplus);
    void YUVChanged(int value);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    /********  Private functions  *********/

    // Painting functions
    void draw_hsvCylinder();
    void draw_graficarLineas();
    void drawPoint(hsv, float R, float G, float B,  int lats, int longs, double r);
    void draw_edgeCylinder();

    /**********  Private value  ***********/

    // Values of rotation
    int xRot;
    int yRot;
    int zRot;
    QPoint lastPos;

    // Values of selectPixel mode
    float r;
    float g;
    float b;
    hsv HSV;
    bool isFirstDraw;
    bool isDrawPixelColor;

    // Values of selectArea mode
    rgb_float ** p;
    int row;
    int col;
    double Hplus;
    double Splus;
    double Vplus;
    bool isDrawAreaColor;
};

#endif // GLWIDGET_HSV_H
