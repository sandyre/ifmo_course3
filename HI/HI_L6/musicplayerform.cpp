#include "musicplayerform.h"
#include "ui_musicplayerform.h"

musicplayerform::musicplayerform(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::musicplayerform)
{
    ui->setupUi(this);
}

musicplayerform::~musicplayerform()
{
    delete ui;
}
