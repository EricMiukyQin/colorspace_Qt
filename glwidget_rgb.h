#ifndef GLWIDGET_RGB_H
#define GLWIDGET_RGB_H

#include "colortype.h"

class GLWidget_RGB : public QGLWidget
{
    Q_OBJECT

public:
    explicit GLWidget_RGB(QWidget *parent = nullptr);
    ~GLWidget_RGB();

    /*****  Functions for rotation  *****/
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);

    /********  Functions for setting private values  *******/
    void set_rgb(float R, float G, float B);
    void set_Argb(rgb_float** p2, int row, int col);
    void set_FirstDraw(bool);
    void set_DrawColorPixel(bool);
    void set_DrawColorArea(bool);

public slots:
    void setRChanged(float value);
    void setGChanged(float value);
    void setBChanged(float value);

signals:
    void RGBChanged(float Rplus, float Gplus, float Bplus);
    void HSVChanged(int value);
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
    void draw_rgbCube();
    void draw_graficarLineas();
    void drawPoint(float R, float G, float B, int lats, int longs, double r);
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
    bool isFirstDraw;
    bool isDrawPixelColor;

    // Values of selectArea mode
    rgb_float ** p;
    int row;
    int col;
    float Rplus;
    float Gplus;
    float Bplus;
    bool isDrawAreaColor;
};

#endif // GLWIDGET_RGB_H
