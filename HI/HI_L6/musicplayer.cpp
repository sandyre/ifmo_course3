#include "musicplayer.h"

#include <QtWidgets>

MusicPlayer::MusicPlayer(QWidget *pParent) :
    QWidget(pParent, Qt::FramelessWindowHint)
{
    form = new musicplayerform(this);
    form->move(100,150);
//    form->move(this->mapToGlobal(this->rect().topLeft()));
    this->setWindowTitle("Muzec Playa");
}

void
MusicPlayer::mousePressEvent(QMouseEvent * pstEvent)
{
    if(pstEvent->button() == Qt::LeftButton)
    {
        stDragPosition = pstEvent->globalPos() - frameGeometry().topLeft();
        pstEvent->accept();
    }
}

void
MusicPlayer::mouseMoveEvent(QMouseEvent * pstEvent)
{
    if(pstEvent->buttons() & Qt::LeftButton)
    {
        this->move(pstEvent->globalPos() - stDragPosition);
        pstEvent->accept();
    }
}

void
MusicPlayer::paintEvent(QPaintEvent * pstEvent)
{
}

void
MusicPlayer::resizeEvent(QResizeEvent * pstEvent)
{
    int side = qMin(width(), height());
    QRegion maskedRegion(width() / 2 - side / 2, height() / 2 - side / 2, side,
                         side, QRegion::Ellipse);
    this->setMask(maskedRegion);
}

QSize
MusicPlayer::sizeHint() const
{
    return QSize(500, 500);
}
