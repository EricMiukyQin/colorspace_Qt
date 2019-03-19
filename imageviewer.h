#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QMainWindow>
#include <QImage>
#include <QSlider>
#include <QPushButton>
#include "glwidget_rgb.h"
#include "glwidget_hsv.h"
#include "glwidget_yuv.h"

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;
QT_END_NAMESPACE

class ImageViewer : public QMainWindow
{
    Q_OBJECT

public:
    ImageViewer();
    ~ImageViewer() Q_DECL_OVERRIDE;
    bool loadFile(const QString &);                                   // Function of loading file

protected:
    bool eventFilter(QObject *,QEvent *) Q_DECL_OVERRIDE;             // Event filter

private slots:
    void open();                                                      // Called when click on open file in the menu
    void saveAs();                                                    // Called when click on save file in the menu
    void copy();                                                      // Called when click on copy file in the menu
    void paste();                                                     // Called when click on paste file in the menu
    void zoomIn();                                                    // Called when click on zoom in in the menu
    void zoomOut();                                                   // Called when click on zoom out in the menu
    void normalSize();                                                // Called when click on normalSize in the menu (Show image in original size)
    void fitToWindow();                                               // Called when click on fittowindow in the menu (Show image in fit-window size)
    void selectPixel();                                               // Called when click on selectPixel in the menu
    void selectArea();                                                // Called when click on selectArea in the menu
    void setNewImage_RGB(float Rplus, float Gplus, float Bplus);      // Called when rgb sliders change values, refreshing image
    void setNewImage_HSV(double Hplus, double Splus, double Vplus);   // Called when hsv sliders change values, refreshing image
    void setNewImage_YUV(double Yplus, double Uplus, double Vplus);   // Called when yuv sliders change values, refreshing image

private:
    /********  Private functions  *********/

    void clear();                                                     // Clear all new values
    void createActions();                                             // Initialize menu
    void updateActions();                                             // Update menu
    bool saveFile(const QString &fileName);                           // Function of saving image
    void setImage(const QImage &newImage);                            // Function of showing image
    void scaleImage(double factor);                                   // Functions of changing image size
    void adjustScrollBar(QScrollBar *scrollBar, double factor);       // Functions of changing image size
    void getRGB(const QPoint &);                                      // Getting RGB in selectPixel mode
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;              // Paint choosing rectangle in selectArea mode
    void getAreaRGB(const QPoint &bp, const QPoint &ep);              // Getting RGB in selectArea mode

    // Initialization of sliders
    QSlider *createSlider_255();                                      // Create sliders for RGB
    QSlider *createSlider_360();                                      // Create slider for H
    QSlider *createSlider_1();                                        // Create sliders for S, V, Y
    QSlider *createSlider_uv();                                       // Create slider for UV
    void rstSliders();                                                // Reset all sliders to 0

    /**********  Private value  ***********/

    // Values of menu actions
    QAction *openAct;
    QAction *saveAsAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *selectPixelAct;
    QAction *selectAreaAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;
    QAction *fitToWindowAct;

    // Values of image showing
    QImage image;
    QLabel *imageLabel;
    QScrollArea *scrollArea;
    double scaleFactor;

    // Values of sliders
    QSlider *RGB_xSlider;
    QSlider *RGB_ySlider;
    QSlider *RGB_zSlider;
    QSlider *HSV_xSlider;
    QSlider *HSV_ySlider;
    QSlider *HSV_zSlider;
    QSlider *YUV_xSlider;
    QSlider *YUV_ySlider;
    QSlider *YUV_zSlider;

    // Values of buttons
    QPushButton *RGB_rstBtn;
    QPushButton *HSV_rstBtn;
    QPushButton *YUV_rstBtn;

    // Values of GLWidget
    GLWidget_RGB *gl_rgb;
    GLWidget_HSV *gl_hsv;
    GLWidget_YUV *gl_yuv;

    // Values of selectPixele mode
    QPoint coordinate;
    float r,g,b;

    // Values of selectArea mode
    bool a_isMousePress;
    QPoint a_beginPoint, a_endPoint;
    rgb_float **p2;
    int row, col;
    bool isnotRelease;
};

#endif
