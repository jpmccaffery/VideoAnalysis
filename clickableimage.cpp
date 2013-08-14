#include "clickableimage.h"
#include "ui_clickableimage.h"
#include "QDebug"
#include "QResizeEvent"
#include "projectwidget.h"

ClickableImage::ClickableImage(ProjectWidget *q) :
    QLabel(q)
//    ui(new Ui::ClickableImage)
{
//  ui->setupUi(this);

    parent = q;

    policy = QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    policy.setHeightForWidth(true);
    setSizePolicy(policy);
    setAlignment(Qt::AlignCenter);

    resized = false;

    ticker = 0;
    myWidth = 640;
    myHeight = 480;
    aspectRatio = myWidth / myHeight;

    index = -1;

    sizePolicy().setHeightForWidth(false);
    if(parent){
        qDebug() << "pParent exists";
        fname = "./" + parent->getProjectName() + "/thumbs/baseThumb.png";
    }else{

        qDebug() << "pParent does not exist";
        fname = "";
    }
    setImage(fname);

}

ClickableImage::~ClickableImage()
{
//    delete ui;
}

void ClickableImage::mouseReleaseEvent(QMouseEvent *eve){

    qDebug() << name;
    if(index < 0){
        emit clicked();
    }else{
        emit clicked(index);
    }
}

void ClickableImage::setImage(QString src){

//    qDebug() << src;
    image = QPixmap(src);
    fname = src;
    myWidth = image.width();
    myHeight = image.height();
    aspectRatio = myHeight/myWidth;


    image = QPixmap(fname);

    if(height() > width() * aspectRatio){

        image = image.scaled(width()-20, (width() * aspectRatio) - 20);
        setPixmap(image);
    }
    else{

        image = image.scaled((height() / aspectRatio) - 20, height()-20);
        setPixmap(image);
    }

    setFrameShadow(QFrame::Raised);
    setFrameShape(QFrame::Box);
    setLineWidth(3);

    resized = true;

    updateGeometry();
}

/*

void ClickableImage::myResize(){


    aspectRatio = myWidth / myHeight;


    if(myWidth > maximumWidth()){

        setMaximumHeight((((double) maximumWidth()) / aspectRatio));
    }

}

*/

int ClickableImage::heightForWidth(int w) const{

    return w;
}

/*

QSize ClickableImage::sizeHint() const{

    qDebug() << "Checking hint";
    qDebug() << parent()->objectName();
    qDebug() << width();
    qDebug() << height();
    qDebug() << image.width();
    qDebug() << image.height();

    int w = image.width();

    return QSize(w,heightForWidth(w));
}

*/

void ClickableImage::resizeEvent(QResizeEvent *e){

    ticker++;

    int w = e->size().width();
    int h = e->size().height();

    image = QPixmap(fname);

    if(h > w * aspectRatio){

        image = image.scaled(w-20, (w * aspectRatio) - 20);
        setPixmap(image);
    }
    else{

        image = image.scaled((h / aspectRatio) - 20, h-20);
        setPixmap(image);
    }

}

void ClickableImage::shrink(int w, int h){

    /*

    image = QPixmap(fname);
    image = image.scaled(w-7, h-7, Qt::KeepAspectRatio);
    setPixmap(image);

    */
}

void ClickableImage::reset(){


    if(parent){
        fname = "./" + parent->getProjectName() + "/thumbs/baseThumb.png";
    }
    shrink(width(), height());
}

void ClickableImage::setName(QString s){

    name = s;
}

void ClickableImage::setIndex(int i){

    index = i;
}

