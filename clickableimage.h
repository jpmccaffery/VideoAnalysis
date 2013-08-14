#ifndef CLICKABLEIMAGE_H
#define CLICKABLEIMAGE_H

#include <QLabel>
#include <QSize>
#include <QSizePolicy>
#include <QWidget>


namespace Ui {
class ClickableImage;
}

class ProjectWidget;

class ClickableImage : public QLabel
{
    Q_OBJECT
    
public:
    explicit ClickableImage(ProjectWidget *q = 0);
    ~ClickableImage();
    void mouseReleaseEvent(QMouseEvent *eve);
//    void myResize();
    int heightForWidth(int w) const;
    void resizeEvent(QResizeEvent *e);
//    QSize minimumSizeHint();
//    QSize sizeHint() const;
    void shrink(int w, int h);
    void reset();
    void setName(QString s);
    void setIndex(int i);

private:
    Ui::ClickableImage *ui;
    double aspectRatio;
    double myWidth;
    double myHeight;
    QPixmap image;
    int ticker;
    QString fname;
    QSizePolicy policy;
    bool resized;
    QString name;
    int index;
    ProjectWidget *parent;

signals:
    void clicked();
    void clicked(int);

public slots:
    void setImage(QString src);

};

#endif // CLICKABLEIMAGE_H
