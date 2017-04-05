/********************************************************************************
** Form generated from reading UI file 'musicplayerform.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MUSICPLAYERFORM_H
#define UI_MUSICPLAYERFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_musicplayerform
{
public:
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton_2;
    QPushButton *pushButton;
    QPushButton *pushButton_3;
    QSlider *horizontalSlider;

    void setupUi(QWidget *musicplayerform)
    {
        if (musicplayerform->objectName().isEmpty())
            musicplayerform->setObjectName(QStringLiteral("musicplayerform"));
        musicplayerform->resize(320, 242);
        verticalLayoutWidget = new QWidget(musicplayerform);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(0, 160, 321, 80));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        pushButton_2 = new QPushButton(verticalLayoutWidget);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));

        horizontalLayout->addWidget(pushButton_2);

        pushButton = new QPushButton(verticalLayoutWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        horizontalLayout->addWidget(pushButton);

        pushButton_3 = new QPushButton(verticalLayoutWidget);
        pushButton_3->setObjectName(QStringLiteral("pushButton_3"));

        horizontalLayout->addWidget(pushButton_3);


        verticalLayout->addLayout(horizontalLayout);

        horizontalSlider = new QSlider(musicplayerform);
        horizontalSlider->setObjectName(QStringLiteral("horizontalSlider"));
        horizontalSlider->setGeometry(QRect(0, 160, 311, 22));
        horizontalSlider->setOrientation(Qt::Horizontal);

        retranslateUi(musicplayerform);

        QMetaObject::connectSlotsByName(musicplayerform);
    } // setupUi

    void retranslateUi(QWidget *musicplayerform)
    {
        musicplayerform->setWindowTitle(QApplication::translate("musicplayerform", "Form", 0));
        pushButton_2->setText(QApplication::translate("musicplayerform", "Play", 0));
        pushButton->setText(QApplication::translate("musicplayerform", "Pause", 0));
        pushButton_3->setText(QApplication::translate("musicplayerform", "Stop", 0));
    } // retranslateUi

};

namespace Ui {
    class musicplayerform: public Ui_musicplayerform {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MUSICPLAYERFORM_H
