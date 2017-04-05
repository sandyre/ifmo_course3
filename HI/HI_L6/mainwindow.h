#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

class ShapedClock : public QWidget
{
    Q_OBJECT

public:
    ShapedClock(QWidget *parent = 0);
    QSize sizeHint() const Q_DECL_OVERRIDE;

protected:
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private:
    QPoint dragPosition;
};

#endif // MAINWINDOW_H
