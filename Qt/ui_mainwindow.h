/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Fri Apr 27 14:09:04 2012
**      by: Qt User Interface Compiler version 4.6.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "glwidget.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *action_Save_image;
    QAction *action_Exit;
    QWidget *centralWidget;
    GLWidget *CrystalView;
    QLabel *crystal_image_label;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer;
    QPushButton *startButton;
    QPushButton *stopButton;
    QGroupBox *Trap_groupBox;
    QWidget *widget;
    QVBoxLayout *verticalLayout_3;
    QSpacerItem *verticalSpacer_2;
    QCheckBox *ImageData_checkBox;
    QPushButton *UpdateImage_button;
    QPushButton *saveImage_Button;
    QWidget *widget1;
    QGridLayout *TrapParamLayout;
    QLabel *vrf_Label;
    QDoubleSpinBox *vrf_spinBox;
    QLabel *vend_Label;
    QDoubleSpinBox *vend_spinBox;
    QMenuBar *menuBar;
    QMenu *menu_File;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(671, 737);
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        action_Save_image = new QAction(MainWindow);
        action_Save_image->setObjectName(QString::fromUtf8("action_Save_image"));
        action_Exit = new QAction(MainWindow);
        action_Exit->setObjectName(QString::fromUtf8("action_Exit"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        CrystalView = new GLWidget(centralWidget);
        CrystalView->setObjectName(QString::fromUtf8("CrystalView"));
        CrystalView->setGeometry(QRect(20, 30, 400, 300));
        crystal_image_label = new QLabel(centralWidget);
        crystal_image_label->setObjectName(QString::fromUtf8("crystal_image_label"));
        crystal_image_label->setGeometry(QRect(25, 360, 320, 320));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(crystal_image_label->sizePolicy().hasHeightForWidth());
        crystal_image_label->setSizePolicy(sizePolicy1);
        crystal_image_label->setFrameShape(QFrame::Panel);
        crystal_image_label->setFrameShadow(QFrame::Plain);
        crystal_image_label->setScaledContents(true);
        layoutWidget = new QWidget(centralWidget);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(440, 180, 160, 141));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        startButton = new QPushButton(layoutWidget);
        startButton->setObjectName(QString::fromUtf8("startButton"));

        verticalLayout->addWidget(startButton);

        stopButton = new QPushButton(layoutWidget);
        stopButton->setObjectName(QString::fromUtf8("stopButton"));

        verticalLayout->addWidget(stopButton);

        Trap_groupBox = new QGroupBox(centralWidget);
        Trap_groupBox->setObjectName(QString::fromUtf8("Trap_groupBox"));
        Trap_groupBox->setGeometry(QRect(440, 30, 211, 101));
        Trap_groupBox->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        Trap_groupBox->setCheckable(false);
        widget = new QWidget(centralWidget);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(380, 560, 147, 111));
        verticalLayout_3 = new QVBoxLayout(widget);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_2);

        ImageData_checkBox = new QCheckBox(widget);
        ImageData_checkBox->setObjectName(QString::fromUtf8("ImageData_checkBox"));

        verticalLayout_3->addWidget(ImageData_checkBox);

        UpdateImage_button = new QPushButton(widget);
        UpdateImage_button->setObjectName(QString::fromUtf8("UpdateImage_button"));

        verticalLayout_3->addWidget(UpdateImage_button);

        saveImage_Button = new QPushButton(widget);
        saveImage_Button->setObjectName(QString::fromUtf8("saveImage_Button"));
        saveImage_Button->setEnabled(false);
        saveImage_Button->setFlat(false);

        verticalLayout_3->addWidget(saveImage_Button);

        widget1 = new QWidget(centralWidget);
        widget1->setObjectName(QString::fromUtf8("widget1"));
        widget1->setGeometry(QRect(450, 61, 190, 62));
        TrapParamLayout = new QGridLayout(widget1);
        TrapParamLayout->setSpacing(6);
        TrapParamLayout->setContentsMargins(11, 11, 11, 11);
        TrapParamLayout->setObjectName(QString::fromUtf8("TrapParamLayout"));
        TrapParamLayout->setContentsMargins(0, 0, 0, 0);
        vrf_Label = new QLabel(widget1);
        vrf_Label->setObjectName(QString::fromUtf8("vrf_Label"));
        vrf_Label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        TrapParamLayout->addWidget(vrf_Label, 0, 0, 1, 1);

        vrf_spinBox = new QDoubleSpinBox(widget1);
        vrf_spinBox->setObjectName(QString::fromUtf8("vrf_spinBox"));
        vrf_spinBox->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        vrf_spinBox->setMaximum(1000);
        vrf_spinBox->setSingleStep(10);
        vrf_spinBox->setValue(200);

        TrapParamLayout->addWidget(vrf_spinBox, 0, 1, 1, 1);

        vend_Label = new QLabel(widget1);
        vend_Label->setObjectName(QString::fromUtf8("vend_Label"));
        vend_Label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        TrapParamLayout->addWidget(vend_Label, 1, 0, 1, 1);

        vend_spinBox = new QDoubleSpinBox(widget1);
        vend_spinBox->setObjectName(QString::fromUtf8("vend_spinBox"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(vend_spinBox->sizePolicy().hasHeightForWidth());
        vend_spinBox->setSizePolicy(sizePolicy2);
        vend_spinBox->setFrame(false);
        vend_spinBox->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        vend_spinBox->setMaximum(100);
        vend_spinBox->setSingleStep(1);
        vend_spinBox->setValue(1);

        TrapParamLayout->addWidget(vend_spinBox, 1, 1, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 671, 22));
        menu_File = new QMenu(menuBar);
        menu_File->setObjectName(QString::fromUtf8("menu_File"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        mainToolBar->setMovable(true);
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        statusBar->setAcceptDrops(true);
        statusBar->setAutoFillBackground(false);
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menu_File->menuAction());
        menu_File->addAction(action_Save_image);
        menu_File->addSeparator();
        menu_File->addAction(action_Exit);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        action_Save_image->setText(QApplication::translate("MainWindow", "&Save image", 0, QApplication::UnicodeUTF8));
        action_Exit->setText(QApplication::translate("MainWindow", "&Exit", 0, QApplication::UnicodeUTF8));
        crystal_image_label->setText(QString());
        startButton->setText(QApplication::translate("MainWindow", "&Start", 0, QApplication::UnicodeUTF8));
        stopButton->setText(QApplication::translate("MainWindow", "S&top", 0, QApplication::UnicodeUTF8));
        Trap_groupBox->setTitle(QApplication::translate("MainWindow", "Trap voltages", 0, QApplication::UnicodeUTF8));
        ImageData_checkBox->setText(QApplication::translate("MainWindow", "Collect image data", 0, QApplication::UnicodeUTF8));
        UpdateImage_button->setText(QApplication::translate("MainWindow", "Update image", 0, QApplication::UnicodeUTF8));
        saveImage_Button->setText(QApplication::translate("MainWindow", "Save image", 0, QApplication::UnicodeUTF8));
        vrf_Label->setText(QApplication::translate("MainWindow", "Radiofrequency", 0, QApplication::UnicodeUTF8));
        vend_Label->setText(QApplication::translate("MainWindow", "End caps", 0, QApplication::UnicodeUTF8));
        menu_File->setTitle(QApplication::translate("MainWindow", "&File", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
