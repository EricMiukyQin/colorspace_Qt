#ifndef GLWIDGET_YUV_H
#define GLWIDGET_YUV_H

#include "colortype.h"

class GLWidget_YUV : public QGLWidget
{
    Q_OBJECT

public:
    explicit GLWidget_YUV(QWidget *parent = nullptr);
    ~GLWidget_YUV();

    /*****  Functions for rotation  *****/
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);

    /********  Functions for setting private values  *******/
    void set_yuv(float R, float G, float B);
    void set_Argb(rgb_float** p, int row, int col);
    void set_FirstDraw(bool);
    void set_DrawColorPixel(bool);
    void set_DrawColorArea(bool);

public slots:
    void setYChanged(double value);
    void setUChanged(double value);
    void setVChanged(double value);

signals:
    void RGBChanged(int value);
    void HSVChanged(int value);
    void YUVChanged(double Yplus, double Uplus, double Vplus);

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
    void draw_rgbCube();
    void draw_graficarLineas();
    void drawPoint(yuv, float, float, float,  int lats, int longs, double r);
    void draw_edgeCube();

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
    yuv YUV;
    bool isFirstDraw;
    bool isDrawPixelColor;

    // Values of selectArea mode
    rgb_float ** p;
    int row;
    int col;
    double Yplus;
    double Uplus;
    double Vplus;
    bool isDrawAreaColor;
};

#endif // GLWIDGET_YUV_H
