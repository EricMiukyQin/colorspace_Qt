#include <QtWidgets>
#include "imageviewer.h"

/**********************  Public  ************************/

ImageViewer::ImageViewer()
   // Initialization for some private values
   /* Image */
   : imageLabel(new QLabel), scrollArea(new QScrollArea)
   , scaleFactor(1)
   /* selectPixel */
   , r(1), g(1), b(1)
   /* selectArea */
   , a_isMousePress(false)
   , p2(nullptr)
   , row(0), col(0)
   , isnotRelease(false)
{
    // CentralWidget
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    scrollArea->setVisible(false);
    setCentralWidget(scrollArea);
    createActions();
    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);

    // Sliders
    RGB_xSlider = createSlider_255();
    RGB_ySlider = createSlider_255();
    RGB_zSlider = createSlider_255();
    HSV_xSlider = createSlider_360();
    HSV_ySlider = createSlider_1();
    HSV_zSlider = createSlider_1();
    YUV_xSlider = createSlider_1();
    YUV_ySlider = createSlider_uv();
    YUV_zSlider = createSlider_uv();
    rstSliders();

    // Layouts, GLWidgetes and Buttons
    QVBoxLayout *containerRGB = new QVBoxLayout;
    QVBoxLayout *containerHSV = new QVBoxLayout;
    QVBoxLayout *containerYUV = new QVBoxLayout;

    gl_rgb = new GLWidget_RGB;
    gl_hsv = new GLWidget_HSV;
    gl_yuv = new GLWidget_YUV;

    RGB_rstBtn = new QPushButton(tr("Reset"), this);
    HSV_rstBtn = new QPushButton(tr("Reset"), this);
    YUV_rstBtn = new QPushButton(tr("Reset"), this);

    containerRGB->addWidget(gl_rgb);
    containerRGB->addWidget(RGB_xSlider);
    containerRGB->addWidget(RGB_ySlider);
    containerRGB->addWidget(RGB_zSlider);
    containerRGB->addWidget(RGB_rstBtn);
    containerHSV->addWidget(gl_hsv);
    containerHSV->addWidget(HSV_xSlider);
    containerHSV->addWidget(HSV_ySlider);
    containerHSV->addWidget(HSV_zSlider);
    containerHSV->addWidget(HSV_rstBtn);
    containerYUV->addWidget(gl_yuv);
    containerYUV->addWidget(YUV_xSlider);
    containerYUV->addWidget(YUV_ySlider);
    containerYUV->addWidget(YUV_zSlider);
    containerYUV->addWidget(YUV_rstBtn);

    QWidget* RGBw = new QWidget;
    QWidget* HSVw = new QWidget;
    QWidget* YUVw = new QWidget;
    RGBw->setLayout(containerRGB);
    HSVw->setLayout(containerHSV);
    YUVw->setLayout(containerYUV);

    // Dock
    QDockWidget *dock_rgb = new QDockWidget(tr("RGB"),this);
    dock_rgb->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable); //features
    dock_rgb->setAllowedAreas(Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea,dock_rgb);
    dock_rgb->setWidget(RGBw);

    QDockWidget *dock_hsv = new QDockWidget(tr("HSV"),this);
    dock_hsv->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable); //features
    dock_hsv->setAllowedAreas(Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea,dock_hsv);
    dock_hsv->setWidget(HSVw);

    QDockWidget *dock_yuv = new QDockWidget(tr("YUV"),this);
    dock_yuv->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable); //features
    dock_yuv->setAllowedAreas(Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea,dock_yuv);
    dock_yuv->setWidget(YUVw);

    this->tabifyDockWidget(dock_rgb, dock_hsv);
    this->tabifyDockWidget(dock_hsv, dock_yuv);
    dock_rgb->raise();

    // Install event filter
    imageLabel->installEventFilter(this);

    // Signals and slots connection
    /* Sliders change */
    connect(RGB_xSlider, &QSlider::valueChanged, gl_rgb, &GLWidget_RGB::setRChanged);
    connect(RGB_ySlider, &QSlider::valueChanged, gl_rgb, &GLWidget_RGB::setGChanged);
    connect(RGB_zSlider, &QSlider::valueChanged, gl_rgb, &GLWidget_RGB::setBChanged);
    connect(HSV_xSlider, &QSlider::valueChanged, gl_hsv, &GLWidget_HSV::setHChanged);
    connect(HSV_ySlider, &QSlider::valueChanged, gl_hsv, &GLWidget_HSV::setSChanged);
    connect(HSV_zSlider, &QSlider::valueChanged, gl_hsv, &GLWidget_HSV::setVChanged);
    connect(YUV_xSlider, &QSlider::valueChanged, gl_yuv, &GLWidget_YUV::setYChanged);
    connect(YUV_ySlider, &QSlider::valueChanged, gl_yuv, &GLWidget_YUV::setUChanged);
    connect(YUV_zSlider, &QSlider::valueChanged, gl_yuv, &GLWidget_YUV::setVChanged);

    /* One color space changes, others change too */
    connect(gl_rgb, &GLWidget_RGB::HSVChanged, HSV_xSlider, &QSlider::setValue);
    connect(gl_rgb, &GLWidget_RGB::HSVChanged, HSV_ySlider, &QSlider::setValue);
    connect(gl_rgb, &GLWidget_RGB::HSVChanged, HSV_zSlider, &QSlider::setValue);
    connect(gl_rgb, &GLWidget_RGB::YUVChanged, YUV_xSlider, &QSlider::setValue);
    connect(gl_rgb, &GLWidget_RGB::YUVChanged, YUV_ySlider, &QSlider::setValue);
    connect(gl_rgb, &GLWidget_RGB::YUVChanged, YUV_zSlider, &QSlider::setValue);
    connect(gl_hsv, &GLWidget_HSV::RGBChanged, RGB_xSlider, &QSlider::setValue);
    connect(gl_hsv, &GLWidget_HSV::RGBChanged, RGB_ySlider, &QSlider::setValue);
    connect(gl_hsv, &GLWidget_HSV::RGBChanged, RGB_zSlider, &QSlider::setValue);
    connect(gl_hsv, &GLWidget_HSV::YUVChanged, YUV_xSlider, &QSlider::setValue);
    connect(gl_hsv, &GLWidget_HSV::YUVChanged, YUV_ySlider, &QSlider::setValue);
    connect(gl_hsv, &GLWidget_HSV::YUVChanged, YUV_zSlider, &QSlider::setValue);
    connect(gl_yuv, &GLWidget_YUV::RGBChanged, RGB_xSlider, &QSlider::setValue);
    connect(gl_yuv, &GLWidget_YUV::RGBChanged, RGB_ySlider, &QSlider::setValue);
    connect(gl_yuv, &GLWidget_YUV::RGBChanged, RGB_zSlider, &QSlider::setValue);
    connect(gl_yuv, &GLWidget_YUV::HSVChanged, HSV_xSlider, &QSlider::setValue);
    connect(gl_yuv, &GLWidget_YUV::HSVChanged, HSV_ySlider, &QSlider::setValue);
    connect(gl_yuv, &GLWidget_YUV::HSVChanged, HSV_zSlider, &QSlider::setValue);

    /* Image changes */
    connect(gl_rgb, &GLWidget_RGB::RGBChanged, this, &ImageViewer::setNewImage_RGB);
    connect(gl_hsv, &GLWidget_HSV::HSVChanged, this, &ImageViewer::setNewImage_HSV);
    connect(gl_yuv, &GLWidget_YUV::YUVChanged, this, &ImageViewer::setNewImage_YUV);

    /* Reset Button clicked */
    connect(RGB_rstBtn, &QPushButton::clicked, this, &ImageViewer::rstSliders);
    connect(HSV_rstBtn, &QPushButton::clicked, this, &ImageViewer::rstSliders);
    connect(YUV_rstBtn, &QPushButton::clicked, this, &ImageViewer::rstSliders);
}

ImageViewer::~ImageViewer(){
    clear();
}

bool ImageViewer::loadFile(const QString &fileName)
{
    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();
    if (newImage.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName), reader.errorString()));
        return false;
    }
    setImage(newImage);
    setWindowFilePath(fileName);
    const QString message = tr("Opened \"%1\", %2x%3, Depth: %4")
        .arg(QDir::toNativeSeparators(fileName)).arg(image.width()).arg(image.height()).arg(image.depth());
    statusBar()->showMessage(message);
    return true;
}

/**********************  Protected  ************************/

QRect getRect(const QPoint &beginPoint, const QPoint &endPoint)
{
    int x, y, width, height;
    width = qAbs(beginPoint.x() - endPoint.x());
    height = qAbs(beginPoint.y() - endPoint.y());
    x = beginPoint.x() < endPoint.x() ? beginPoint.x() : endPoint.x();
    y = beginPoint.y() < endPoint.y() ? beginPoint.y() : endPoint.y();
    QRect selectedRect = QRect(x, y, width, height);
    if(selectedRect.width() == 0){
        selectedRect.setWidth(1);
    }
    if(selectedRect.height() == 0){
        selectedRect.setHeight(1);
    }
    return selectedRect;
}

/* Mouse EventFliter */
bool ImageViewer::eventFilter(QObject* obj, QEvent* event)
{
    // selectPixel
    if((obj == imageLabel) && (event->type() == QEvent::MouseMove) && (!image.isNull()) && (scaleFactor==1.0) && (selectPixelAct->isChecked())
            && (!selectAreaAct->isChecked())){
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        QPoint imagePoint = imageLabel->mapFromGlobal(mouseEvent->globalPos());
         if(imagePoint.x() <= image.width() && imagePoint.y() <= image.height()
             && imagePoint.x() >= 0 && imagePoint.y() >= 0){
             const QString message = tr("Position: (%1,%2)")
                 .arg(imagePoint.x()).arg(imagePoint.y());
             statusBar()->showMessage(message);
             coordinate = imagePoint;
         }
         else{
             const QString message = tr("Out of range of image");
             statusBar()->showMessage(message);
         }
         return true;
    }
    else if((obj == imageLabel) && (event->type() == QEvent::MouseButtonPress) && (!image.isNull()) && (scaleFactor==1.0) && (selectPixelAct->isChecked())
            && (!selectAreaAct->isChecked())){
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if(mouseEvent->button() == Qt::LeftButton){
            gl_rgb->set_FirstDraw(false);
            gl_hsv->set_FirstDraw(false);
            gl_yuv->set_FirstDraw(false);
            getRGB(coordinate);
            const QString message = tr("Position: (%1,%2) Color: (%3,%4,%5)")
                    .arg(coordinate.x()).arg(coordinate.y()).arg(int(r*255)).arg(int(g*255)).arg(int(b*255));
            statusBar()->showMessage(message);
            return true;
        }
        else{
            return false;
        }
    }
    // selectArea
    else if((obj == imageLabel) && (event->type() == QEvent::MouseButtonPress) && (!image.isNull()) && (scaleFactor==1.0) && (!selectPixelAct->isChecked())
            && (selectAreaAct->isChecked())){
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if(mouseEvent->button() == Qt::LeftButton){
            if(isnotRelease){
                // Release
                int i;
                for(i = 0; i < row; i++){
                    delete []p2[i];
                }
                delete []p2;
                isnotRelease = false;
            }
            a_beginPoint = imageLabel->mapFromGlobal(mouseEvent->globalPos());
            if(a_beginPoint.x() <= image.width() && a_beginPoint.y() <= image.height()
                && a_beginPoint.x() >= 0 && a_beginPoint.y() >= 0){
                a_isMousePress = true;
                return true;
            }
            else{
                a_isMousePress = false;
                imageLabel->setPixmap(QPixmap::fromImage(image));
                const QString message = tr("Out of range of image");
                statusBar()->showMessage(message);
                return false;
            }
        }
        else{
            return false;
        }
    }
    else if((obj == imageLabel) && (event->type() == QEvent::MouseMove) && (!image.isNull()) && (scaleFactor==1.0) && (!selectPixelAct->isChecked())
            && (selectAreaAct->isChecked())){
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (a_isMousePress){
            QPoint t = imageLabel->mapFromGlobal(mouseEvent->globalPos());
            int tx = t.rx() < 0 ? 0 : t.rx();
            tx = tx > image.width() ? image.width() : tx;
            int ty = t.ry() < 0 ? 0 : t.ry();
            ty = ty > image.height() ? image.height() : ty;
            a_endPoint.setX(tx);
            a_endPoint.setY(ty);
            update();
            return true;
        }
        return false;
    }
    else if((obj == imageLabel) && (event->type() == QEvent::MouseButtonRelease) && (!image.isNull()) && (scaleFactor==1.0) && (!selectPixelAct->isChecked())
            && (selectAreaAct->isChecked())){
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        QPoint t = imageLabel->mapFromGlobal(mouseEvent->globalPos());
        int tx = t.rx() < 0 ? 0 : t.rx();
        tx = tx > image.width() ? image.width() : tx;
        int ty = t.ry() < 0 ? 0 : t.ry();
        ty = ty > image.height() ? image.height() : ty;
        a_endPoint.setX(tx);
        a_endPoint.setY(ty);
        a_isMousePress = false;
        QRect R = getRect(a_beginPoint, a_endPoint);
        gl_rgb->set_FirstDraw(false);
        gl_hsv->set_FirstDraw(false);
        gl_yuv->set_FirstDraw(false);
        getAreaRGB(a_beginPoint, a_endPoint);
        rstSliders();
        const QString message = tr("Position: (%1,%2) Height: (%3) Width: (%4)")
                .arg(a_beginPoint.rx()).arg(a_beginPoint.ry()).arg(R.height()).arg(R.width());
        statusBar()->showMessage(message);
        return true;
    }
    return false;
}

/**********************  Private slots  ************************/

static void initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode)
{
    static bool firstDialog = true;

    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    QStringList mimeTypeFilters;
    const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
        ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
    foreach (const QByteArray &mimeTypeName, supportedMimeTypes)
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");
    if (acceptMode == QFileDialog::AcceptSave)
        dialog.setDefaultSuffix("jpg");
}

void ImageViewer::open()
{
    QFileDialog dialog(this, tr("Open File"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptOpen);
    while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first())) {}
}

void ImageViewer::saveAs()
{
    QFileDialog dialog(this, tr("Save File As"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptSave);
    while (dialog.exec() == QDialog::Accepted && !saveFile(dialog.selectedFiles().first())) {}
}

void ImageViewer::copy()
{
#ifndef QT_NO_CLIPBOARD
    QGuiApplication::clipboard()->setImage(image);
#endif // !QT_NO_CLIPBOARD
}

#ifndef QT_NO_CLIPBOARD
static QImage clipboardImage()
{
    if (const QMimeData *mimeData = QGuiApplication::clipboard()->mimeData()) {
        if (mimeData->hasImage()) {
            const QImage image = qvariant_cast<QImage>(mimeData->imageData());
            if (!image.isNull())
                return image;
        }
    }
    return QImage();
}
#endif // !QT_NO_CLIPBOARD

void ImageViewer::paste()
{
#ifndef QT_NO_CLIPBOARD
    const QImage newImage = clipboardImage();
    if (newImage.isNull()) {
        statusBar()->showMessage(tr("No image in clipboard"));
    } else {
        setImage(newImage);
        setWindowFilePath(QString());
        const QString message = tr("Obtained image from clipboard, %1x%2, Depth: %3")
            .arg(newImage.width()).arg(newImage.height()).arg(newImage.depth());
        statusBar()->showMessage(message);
    }
#endif // !QT_NO_CLIPBOARD
}

void ImageViewer::zoomIn()
{
    scaleImage(1.25);
}

void ImageViewer::zoomOut()
{
    scaleImage(0.8);
}

void ImageViewer::normalSize()
{
    imageLabel->adjustSize();
    scaleFactor = 1.0;
    updateActions();
}

void ImageViewer::fitToWindow()
{
    bool fitToWindow = fitToWindowAct->isChecked();
    scrollArea->setWidgetResizable(fitToWindow);
    if (!fitToWindow)
        normalSize();
    updateActions();
}

void ImageViewer::selectPixel()
{
    bool selectPixel = selectPixelAct->isChecked();
    if(selectPixel == true){
        imageLabel->setMouseTracking(true);
        centralWidget()->setMouseTracking(true);
        this->setMouseTracking(true);
        gl_rgb->set_FirstDraw(true);
        gl_hsv->set_FirstDraw(true);
        gl_yuv->set_FirstDraw(true);
        gl_rgb->set_DrawColorPixel(true);
        gl_hsv->set_DrawColorPixel(true);
        gl_yuv->set_DrawColorPixel(true);
        updateActions();
        gl_rgb->updateGL();
        gl_hsv->updateGL();
        gl_yuv->updateGL();
    }
    else{
        imageLabel->setMouseTracking(false);
        centralWidget()->setMouseTracking(false);
        this->setMouseTracking(false);
        gl_rgb->set_DrawColorPixel(false);
        gl_hsv->set_DrawColorPixel(false);
        gl_yuv->set_DrawColorPixel(false);
        updateActions();
        imageLabel->setPixmap(QPixmap::fromImage(image));
        gl_rgb->updateGL();
        gl_hsv->updateGL();
        gl_yuv->updateGL();
    }
}

void ImageViewer::selectArea()
{
    bool selectArea = selectAreaAct->isChecked();
    if(selectArea == true){
        imageLabel->setMouseTracking(true);
        centralWidget()->setMouseTracking(true);
        this->setMouseTracking(true);
        gl_rgb->set_FirstDraw(true);
        gl_hsv->set_FirstDraw(true);
        gl_yuv->set_FirstDraw(true);
        gl_rgb->set_DrawColorArea(true);
        gl_hsv->set_DrawColorArea(true);
        gl_yuv->set_DrawColorArea(true);
        rstSliders();
        updateActions();
        gl_rgb->updateGL();
        gl_hsv->updateGL();
        gl_yuv->updateGL();
    }
    else{
        imageLabel->setMouseTracking(false);
        centralWidget()->setMouseTracking(false);
        this->setMouseTracking(false);
        imageLabel->setPixmap(QPixmap::fromImage(image));
        if(isnotRelease){
            // Release
            int i;
            for(i = 0; i < row; i++){
                delete []p2[i];
            }
            delete []p2;
            isnotRelease = false;
        }
        gl_rgb->set_DrawColorArea(false);
        gl_hsv->set_DrawColorArea(false);
        gl_yuv->set_DrawColorArea(false);
        rstSliders();
        updateActions();
        gl_rgb->updateGL();
        gl_hsv->updateGL();
        gl_yuv->updateGL();
    }
}

void ImageViewer::setNewImage_RGB(float Rplus, float Gplus, float Bplus)
{
    QImage nImage = image;
    int i,j;
    for(i = 0; i < row; i++){
        for(j = 0; j < col; j++){
            float tr = p2[i][j].r + Rplus <= 1 ? p2[i][j].r + Rplus : 1;
            tr = tr > 0 ? tr : 0;
            float tg = p2[i][j].g + Gplus <= 1 ? p2[i][j].g + Gplus : 1;
            tg = tg > 0 ? tg : 0;
            float tb = p2[i][j].b + Bplus <= 1 ? p2[i][j].b + Bplus : 1;
            tb = tb > 0 ? tb : 0;
            nImage.setPixelColor(a_beginPoint.rx() + j, a_beginPoint.ry() + i, {int(tr * 255), int(tg * 255), int(tb * 255)});
        }
    }
    imageLabel->setPixmap(QPixmap::fromImage(nImage));
}

void ImageViewer::setNewImage_HSV(double Hplus, double Splus, double Vplus)
{
    QImage nImage = image;
    int i,j;
    for(i = 0; i < row; i++){
        for(j = 0; j < col; j++){
            hsv ot = rgb2hsv({double(p2[i][j].r), double(p2[i][j].g), double(p2[i][j].b)});
            hsv t;
            t.h = ot.h + Hplus <= 360 ? ot.h + Hplus : ot.h + Hplus - 360;
            t.h = t.h >= 0 ? t.h : 0;
            t.s = ot.s + Splus <= 1 ? ot.s + Splus : 1;
            t.s = t.s >= 0 ? t.s : 0;
            t.v = ot.v + Vplus <= 1 ? ot.v + Vplus : 1;
            t.v = t.v >= 0 ? t.v : 0;
            nImage.setPixelColor(a_beginPoint.rx() + j, a_beginPoint.ry() + i,
            {int(float(hsv2rgb(t).r) * 255), int(float(hsv2rgb(t).g) * 255), int(float(hsv2rgb(t).b) * 255)});
        }
    }
    imageLabel->setPixmap(QPixmap::fromImage(nImage));
}

void ImageViewer::setNewImage_YUV(double Yplus, double Uplus, double Vplus)
{
    QImage nImage = image;
    int i,j;
    for(i = 0; i < row; i++){
        for(j = 0; j < col; j++){
            yuv ot = rgb2yuv(checkrgb({int(p2[i][j].r * 255), int(p2[i][j].g * 255), int(p2[i][j].b * 255)}));
            yuv t = checkyuv({ot.y + Yplus, ot.u + Uplus, ot.v + Vplus});
            nImage.setPixelColor(a_beginPoint.rx() + j, a_beginPoint.ry() + i,
            {int(yuv2rgb(t).r * 255), int(yuv2rgb(t).g * 255), int(yuv2rgb(t).b * 255)});
        }
    }
    imageLabel->setPixmap(QPixmap::fromImage(nImage));
}

/**********************  Private  ************************/

void ImageViewer::clear()
{
    delete imageLabel;
    delete scrollArea;
    delete gl_rgb;
    delete gl_hsv;
    delete gl_yuv;
    delete RGB_xSlider;
    delete RGB_ySlider;
    delete RGB_zSlider;
    delete HSV_xSlider;
    delete HSV_ySlider;
    delete HSV_zSlider;
    delete YUV_xSlider;
    delete YUV_ySlider;
    delete YUV_zSlider;
    delete RGB_rstBtn;
    delete HSV_rstBtn;
    delete YUV_rstBtn;
}

void ImageViewer::createActions()
{
    /* File Menu */
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    openAct = fileMenu->addAction(tr("&Open..."), this, &ImageViewer::open);
    openAct->setShortcut(QKeySequence::Open);

    saveAsAct = fileMenu->addAction(tr("&Save As..."), this, &ImageViewer::saveAs);
    saveAsAct->setEnabled(false);

    fileMenu->addSeparator();

    QAction *exitAct = fileMenu->addAction(tr("E&xit"), this, &QWidget::close);
    exitAct->setShortcut(tr("Ctrl+Q"));

    /* Edit Menu */
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));

    copyAct = editMenu->addAction(tr("&Copy"), this, &ImageViewer::copy);
    copyAct->setShortcut(QKeySequence::Copy);
    copyAct->setEnabled(false);

    pasteAct = editMenu->addAction(tr("&Paste"), this, &ImageViewer::paste);
    pasteAct->setShortcut(QKeySequence::Paste);

    editMenu->addSeparator();

    selectPixelAct = editMenu->addAction(tr("&SeletPixel"), this, &ImageViewer::selectPixel);
    selectPixelAct->setEnabled(false);
    selectPixelAct->setCheckable(true);

    selectAreaAct = editMenu->addAction(tr("&SeletArea"), this, &ImageViewer::selectArea);
    selectAreaAct->setEnabled(false);
    selectAreaAct->setCheckable(true);

    /* View Menu */
    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

    zoomInAct = viewMenu->addAction(tr("Zoom &In (25%)"), this, &ImageViewer::zoomIn);
    zoomInAct->setShortcut(QKeySequence::ZoomIn);
    zoomInAct->setEnabled(false);

    zoomOutAct = viewMenu->addAction(tr("Zoom &Out (25%)"), this, &ImageViewer::zoomOut);
    zoomOutAct->setShortcut(QKeySequence::ZoomOut);
    zoomOutAct->setEnabled(false);

    normalSizeAct = viewMenu->addAction(tr("&Normal Size"), this, &ImageViewer::normalSize);
    normalSizeAct->setShortcut(tr("Ctrl+S"));
    normalSizeAct->setEnabled(false);

    viewMenu->addSeparator();

    fitToWindowAct = viewMenu->addAction(tr("&Fit to Window"), this, &ImageViewer::fitToWindow);
    fitToWindowAct->setEnabled(false);
    fitToWindowAct->setCheckable(true);
    fitToWindowAct->setShortcut(tr("Ctrl+F"));

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    /* Help Menu */
    helpMenu->addAction(tr("About &Qt"), &QApplication::aboutQt);
}

void ImageViewer::updateActions()
{
    openAct->setEnabled(!selectPixelAct->isChecked() && !selectAreaAct->isChecked());
    saveAsAct->setEnabled(!image.isNull() && !selectPixelAct->isChecked() && !selectAreaAct->isChecked());
    copyAct->setEnabled(!image.isNull() && !selectPixelAct->isChecked() && !selectAreaAct->isChecked());
    pasteAct->setEnabled(!selectPixelAct->isChecked() && !selectAreaAct->isChecked());
    selectPixelAct->setEnabled((!image.isNull()) && (!fitToWindowAct->isChecked()) && (scaleFactor == 1.0) && (!selectAreaAct->isChecked()));
    selectAreaAct->setEnabled((!image.isNull()) && (!fitToWindowAct->isChecked()) && (scaleFactor == 1.0) && (!selectPixelAct->isChecked()));
    zoomInAct->setEnabled((!fitToWindowAct->isChecked()) && (!selectPixelAct->isChecked()) && (!selectAreaAct->isChecked()));
    zoomOutAct->setEnabled(!fitToWindowAct->isChecked() && (!selectPixelAct->isChecked()) && (!selectAreaAct->isChecked()));
    normalSizeAct->setEnabled((!fitToWindowAct->isChecked()) && (!selectPixelAct->isChecked()) && (!selectAreaAct->isChecked()));
    fitToWindowAct->setEnabled(!selectPixelAct->isChecked() && (!selectAreaAct->isChecked()));
}

bool ImageViewer::saveFile(const QString &fileName)
{
    QImageWriter writer(fileName);
    if (!writer.write(image)) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot write %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName)), writer.errorString());
        return false;
    }
    const QString message = tr("Wrote \"%1\"").arg(QDir::toNativeSeparators(fileName));
    statusBar()->showMessage(message);
    return true;
}

void ImageViewer::setImage(const QImage &newImage)
{
    image = newImage;
    imageLabel->setPixmap(QPixmap::fromImage(image));
    scaleFactor = 1.0;
    scrollArea->setVisible(true);
    fitToWindowAct->setEnabled(true);
    updateActions();
    if (!fitToWindowAct->isChecked())
        imageLabel->adjustSize();
}

void ImageViewer::scaleImage(double factor)
{
    Q_ASSERT(imageLabel->pixmap());
    scaleFactor *= factor;
    imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);

    zoomInAct->setEnabled(scaleFactor < 3.0);
    zoomOutAct->setEnabled(scaleFactor > 0.333);

    updateActions();
}

void ImageViewer::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep()/2)));
}

void ImageViewer::getRGB(const QPoint &p)
{
    QRgb pixelColor_RGB = image.pixel(p);
    if(qRed(pixelColor_RGB) <= 255 && qRed(pixelColor_RGB) >= 0){
        r = float(qRed(pixelColor_RGB)) / 255;
    }
    else{
        if(qRed(pixelColor_RGB) > 255){
            r = 1;
        }
        else{
            r = 0;
        }
    }
    if(qGreen(pixelColor_RGB) <= 255 && qGreen(pixelColor_RGB) >= 0){
        g = float(qGreen(pixelColor_RGB)) / 255;
    }
    else{
        if(qGreen(pixelColor_RGB) > 255){
            g = 1;
        }
        else{
            g = 0;
        }
    }
    if(qBlue(pixelColor_RGB) <= 255 && qBlue(pixelColor_RGB) >= 0){
        b = float(qBlue(pixelColor_RGB)) / 255;
    }
    else{
        if(qBlue(pixelColor_RGB) > 255){
            b = 1;
        }
        else{
            b = 0;
        }
    }
    gl_rgb->set_rgb(r, g, b);
    gl_hsv->set_hsv(r, g, b);
    gl_yuv->set_yuv(r, g, b);
}

void ImageViewer::paintEvent(QPaintEvent *event)
{
    if (a_isMousePress == true){
        QImage tmp(image);
        QPainter painter(&tmp);
        painter.setPen(QPen(Qt::blue, 1, Qt::SolidLine, Qt::FlatCap));
        painter.drawRect(getRect(a_beginPoint, a_endPoint));
        imageLabel->setPixmap(QPixmap::fromImage(tmp));
    }
}

void ImageViewer::getAreaRGB(const QPoint &bp, const QPoint &ep)
{
    QRect t = getRect(bp, ep);
    row = t.height();
    col = t.width();
    p2 = new rgb_float*[row];
    isnotRelease = true;
    int i,j;
    for(i = 0 ; i < row ; i++){
       p2[i] = new rgb_float[col];
    }
    for(i = 0 ; i < row ; i++){
       for(j = 0 ; j < col ; j++){
           QRgb pixelColor_RGB = image.pixel({t.topLeft().rx() + j, t.topLeft().ry() + i});
           if(qRed(pixelColor_RGB) <= 255 && qRed(pixelColor_RGB) >= 0){
               p2[i][j].r = float(qRed(pixelColor_RGB)) / 255;
           }
           else{
               if(qRed(pixelColor_RGB) > 255){
                   p2[i][j].r = 1;
               }
               else{
                   p2[i][j].r = 0;
               }
           }
           if(qGreen(pixelColor_RGB) <= 255 && qGreen(pixelColor_RGB) >= 0){
               p2[i][j].g = float(qGreen(pixelColor_RGB)) / 255;
           }
           else{
               if(qGreen(pixelColor_RGB) > 255){
                   p2[i][j].g = 1;
               }
               else{
                   p2[i][j].g = 0;
               }
           }
           if(qBlue(pixelColor_RGB) <= 255 && qBlue(pixelColor_RGB) >= 0){
               p2[i][j].b = float(qBlue(pixelColor_RGB)) / 255;
           }
           else{
               if(qBlue(pixelColor_RGB) > 255){
                   p2[i][j].b = 1;
               }
               else{
                   p2[i][j].b = 0;
               }
           }
       }
    }
    gl_rgb->set_Argb(p2, row, col);
    gl_hsv->set_Argb(p2, row, col);
    gl_yuv->set_Argb(p2, row, col);
}

QSlider* ImageViewer::createSlider_255()
{
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(-255, 255);
    slider->setSingleStep(1);
    slider->setPageStep(255);
    slider->setTickInterval(10);
    slider->setTickPosition(QSlider::TicksRight);
    return slider;
}

QSlider* ImageViewer::createSlider_360()
{
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 360);
    slider->setSingleStep(1);
    slider->setPageStep(360);
    slider->setTickInterval(12);
    slider->setTickPosition(QSlider::TicksRight);
    return slider;
}

QSlider* ImageViewer::createSlider_1()
{
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(-100, 100);
    slider->setSingleStep(1);
    slider->setPageStep(100);
    slider->setTickInterval(10);
    slider->setTickPosition(QSlider::TicksRight);
    return slider;
}

QSlider* ImageViewer::createSlider_uv()
{
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(-50, 50);
    slider->setSingleStep(1);
    slider->setPageStep(50);
    slider->setTickInterval(5);
    slider->setTickPosition(QSlider::TicksRight);
    return slider;
}

void ImageViewer::rstSliders()
{
    RGB_xSlider->setValue(0);
    RGB_ySlider->setValue(0);
    RGB_zSlider->setValue(0);
    HSV_xSlider->setValue(0);
    HSV_ySlider->setValue(0);
    HSV_zSlider->setValue(0);
    YUV_xSlider->setValue(0);
    YUV_ySlider->setValue(0);
    YUV_zSlider->setValue(0);
}
