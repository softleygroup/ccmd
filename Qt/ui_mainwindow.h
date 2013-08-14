/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Wed May 23 15:39:28 2012
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
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QTreeView>
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
    QLabel *label;
    QLabel *label_2;
    QFrame *line;
    QGroupBox *groupBox;
    QWidget *widget;
    QGridLayout *gridLayout;
    QLabel *label_4;
    QDoubleSpinBox *w0_doubleSpinBox;
    QLabel *label_3;
    QDoubleSpinBox *z0_doubleSpinBox;
    QLabel *label_5;
    QDoubleSpinBox *distpixels_doubleSpinBox;
    QWidget *widget1;
    QVBoxLayout *verticalLayout;
    QTreeView *trap_treeView;
    QHBoxLayout *horizontalLayout_3;
    QCheckBox *ImageData_checkBox;
    QSpacerItem *horizontalSpacer;
    QPushButton *startButton;
    QPushButton *stopButton;
    QWidget *widget2;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *UpdateImage_button;
    QPushButton *ResetHistogram_button;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *saveImage_Button;
    QMenuBar *menuBar;
    QMenu *menu_File;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(797, 582);
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
        CrystalView->setGeometry(QRect(20, 30, 400, 320));
        crystal_image_label = new QLabel(centralWidget);
        crystal_image_label->setObjectName(QString::fromUtf8("crystal_image_label"));
        crystal_image_label->setGeometry(QRect(460, 30, 320, 320));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(crystal_image_label->sizePolicy().hasHeightForWidth());
        crystal_image_label->setSizePolicy(sizePolicy1);
        crystal_image_label->setFrameShape(QFrame::Panel);
        crystal_image_label->setFrameShadow(QFrame::Plain);
        crystal_image_label->setScaledContents(true);
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 10, 71, 16));
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(460, 10, 81, 16));
        line = new QFrame(centralWidget);
        line->setObjectName(QString::fromUtf8("line"));
        line->setGeometry(QRect(430, 10, 20, 511));
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);
        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(460, 360, 321, 121));
        widget = new QWidget(groupBox);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(10, 21, 301, 97));
        gridLayout = new QGridLayout(widget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label_4 = new QLabel(widget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 0, 0, 1, 1);

        w0_doubleSpinBox = new QDoubleSpinBox(widget);
        w0_doubleSpinBox->setObjectName(QString::fromUtf8("w0_doubleSpinBox"));
        w0_doubleSpinBox->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        w0_doubleSpinBox->setDecimals(1);
        w0_doubleSpinBox->setValue(5);

        gridLayout->addWidget(w0_doubleSpinBox, 0, 1, 1, 1);

        label_3 = new QLabel(widget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 1, 0, 1, 1);

        z0_doubleSpinBox = new QDoubleSpinBox(widget);
        z0_doubleSpinBox->setObjectName(QString::fromUtf8("z0_doubleSpinBox"));
        z0_doubleSpinBox->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        z0_doubleSpinBox->setDecimals(1);
        z0_doubleSpinBox->setMaximum(200);
        z0_doubleSpinBox->setSingleStep(5);
        z0_doubleSpinBox->setValue(50);

        gridLayout->addWidget(z0_doubleSpinBox, 1, 1, 1, 1);

        label_5 = new QLabel(widget);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout->addWidget(label_5, 2, 0, 1, 1);

        distpixels_doubleSpinBox = new QDoubleSpinBox(widget);
        distpixels_doubleSpinBox->setObjectName(QString::fromUtf8("distpixels_doubleSpinBox"));
        distpixels_doubleSpinBox->setEnabled(false);
        distpixels_doubleSpinBox->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        distpixels_doubleSpinBox->setDecimals(3);
        distpixels_doubleSpinBox->setMinimum(0.1);
        distpixels_doubleSpinBox->setMaximum(10);
        distpixels_doubleSpinBox->setSingleStep(0.2);
        distpixels_doubleSpinBox->setValue(1);

        gridLayout->addWidget(distpixels_doubleSpinBox, 2, 1, 1, 1);

        w0_doubleSpinBox->raise();
        label_3->raise();
        label_4->raise();
        z0_doubleSpinBox->raise();
        line->raise();
        distpixels_doubleSpinBox->raise();
        label_5->raise();
        widget1 = new QWidget(centralWidget);
        widget1->setObjectName(QString::fromUtf8("widget1"));
        widget1->setGeometry(QRect(20, 370, 401, 161));
        verticalLayout = new QVBoxLayout(widget1);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        trap_treeView = new QTreeView(widget1);
        trap_treeView->setObjectName(QString::fromUtf8("trap_treeView"));

        verticalLayout->addWidget(trap_treeView);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        ImageData_checkBox = new QCheckBox(widget1);
        ImageData_checkBox->setObjectName(QString::fromUtf8("ImageData_checkBox"));

        horizontalLayout_3->addWidget(ImageData_checkBox);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);

        startButton = new QPushButton(widget1);
        startButton->setObjectName(QString::fromUtf8("startButton"));

        horizontalLayout_3->addWidget(startButton);

        stopButton = new QPushButton(widget1);
        stopButton->setObjectName(QString::fromUtf8("stopButton"));

        horizontalLayout_3->addWidget(stopButton);


        verticalLayout->addLayout(horizontalLayout_3);

        widget2 = new QWidget(centralWidget);
        widget2->setObjectName(QString::fromUtf8("widget2"));
        widget2->setGeometry(QRect(460, 500, 321, 32));
        horizontalLayout_4 = new QHBoxLayout(widget2);
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        UpdateImage_button = new QPushButton(widget2);
        UpdateImage_button->setObjectName(QString::fromUtf8("UpdateImage_button"));

        horizontalLayout_4->addWidget(UpdateImage_button);

        ResetHistogram_button = new QPushButton(widget2);
        ResetHistogram_button->setObjectName(QString::fromUtf8("ResetHistogram_button"));

        horizontalLayout_4->addWidget(ResetHistogram_button);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_2);

        saveImage_Button = new QPushButton(widget2);
        saveImage_Button->setObjectName(QString::fromUtf8("saveImage_Button"));
        saveImage_Button->setEnabled(false);
        saveImage_Button->setFlat(false);

        horizontalLayout_4->addWidget(saveImage_Button);

        MainWindow->setCentralWidget(centralWidget);
        CrystalView->raise();
        crystal_image_label->raise();
        trap_treeView->raise();
        label->raise();
        label_2->raise();
        saveImage_Button->raise();
        line->raise();
        saveImage_Button->raise();
        groupBox->raise();
        saveImage_Button->raise();
        UpdateImage_button->raise();
        ResetHistogram_button->raise();
        UpdateImage_button->raise();
        UpdateImage_button->raise();
        ResetHistogram_button->raise();
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 797, 22));
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
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "CCMD", 0, QApplication::UnicodeUTF8));
        action_Save_image->setText(QApplication::translate("MainWindow", "&Save image", 0, QApplication::UnicodeUTF8));
        action_Exit->setText(QApplication::translate("MainWindow", "&Exit", 0, QApplication::UnicodeUTF8));
        crystal_image_label->setText(QString());
        label->setText(QApplication::translate("MainWindow", "Simulation", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "CCD image", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("MainWindow", "Microscope image parameters", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWindow", "Instrument blur (pixels)", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindow", "Depth of field (pixels)", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MainWindow", "Pixels to microns", 0, QApplication::UnicodeUTF8));
        ImageData_checkBox->setText(QApplication::translate("MainWindow", "Collect image data", 0, QApplication::UnicodeUTF8));
        startButton->setText(QApplication::translate("MainWindow", "&Start", 0, QApplication::UnicodeUTF8));
        stopButton->setText(QApplication::translate("MainWindow", "S&top", 0, QApplication::UnicodeUTF8));
        UpdateImage_button->setText(QApplication::translate("MainWindow", "Update ", 0, QApplication::UnicodeUTF8));
        ResetHistogram_button->setText(QApplication::translate("MainWindow", "Reset ", 0, QApplication::UnicodeUTF8));
        saveImage_Button->setText(QApplication::translate("MainWindow", "Save image", 0, QApplication::UnicodeUTF8));
        menu_File->setTitle(QApplication::translate("MainWindow", "&File", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
