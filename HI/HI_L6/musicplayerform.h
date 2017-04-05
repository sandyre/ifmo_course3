#ifndef MUSICPLAYERFORM_H
#define MUSICPLAYERFORM_H

#include <QWidget>

namespace Ui {
class musicplayerform;
}

class musicplayerform : public QWidget
{
    Q_OBJECT

public:
    explicit musicplayerform(QWidget *parent = 0);
    ~musicplayerform();

private:
    Ui::musicplayerform *ui;
};

#endif // MUSICPLAYERFORM_H
