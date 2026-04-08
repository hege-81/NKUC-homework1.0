/********************************************************************************
** Form generated from reading UI file 'CGame.ui'
**
** Created by: Qt User Interface Compiler version 6.9.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CGAME_H
#define UI_CGAME_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CGameClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *CGameClass)
    {
        if (CGameClass->objectName().isEmpty())
            CGameClass->setObjectName("CGameClass");
        CGameClass->resize(600, 400);
        menuBar = new QMenuBar(CGameClass);
        menuBar->setObjectName("menuBar");
        CGameClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(CGameClass);
        mainToolBar->setObjectName("mainToolBar");
        CGameClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(CGameClass);
        centralWidget->setObjectName("centralWidget");
        CGameClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(CGameClass);
        statusBar->setObjectName("statusBar");
        CGameClass->setStatusBar(statusBar);

        retranslateUi(CGameClass);

        QMetaObject::connectSlotsByName(CGameClass);
    } // setupUi

    void retranslateUi(QMainWindow *CGameClass)
    {
        CGameClass->setWindowTitle(QCoreApplication::translate("CGameClass", "CGame", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CGameClass: public Ui_CGameClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CGAME_H
