#ifndef MUSICPLAYERWIDGET_H
#define MUSICPLAYERWIDGET_H

#include "musicplayerform.h"

#include <QWidget>

class MusicPlayer : public QWidget
{
    Q_OBJECT

public:
    MusicPlayer(QWidget * pParent = nullptr);
    QSize sizeHint() const Q_DECL_OVERRIDE;

protected:
    void mouseMoveEvent(QMouseEvent*) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent*) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent*) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent*) Q_DECL_OVERRIDE;
private:
    QPoint stDragPosition;
    musicplayerform * form;
};

#endif // MUSICPLAYERWIDGET_H
