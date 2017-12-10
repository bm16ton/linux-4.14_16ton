/********************************************************************************
** Form generated from reading UI file 'peers.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PEERS_H
#define UI_PEERS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QListView>

QT_BEGIN_NAMESPACE

class Ui_Peers
{
public:
    QGridLayout *gridLayout;
    QListView *peers;

    void setupUi(QDialog *Peers)
    {
        if (Peers->objectName().isEmpty())
            Peers->setObjectName(QString::fromUtf8("Peers"));
        Peers->resize(400, 300);
        gridLayout = new QGridLayout(Peers);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        peers = new QListView(Peers);
        peers->setObjectName(QString::fromUtf8("peers"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(peers->sizePolicy().hasHeightForWidth());
        peers->setSizePolicy(sizePolicy);
        peers->setMouseTracking(true);
        peers->setEditTriggers(QAbstractItemView::NoEditTriggers);
        peers->setViewMode(QListView::IconMode);

        gridLayout->addWidget(peers, 0, 0, 1, 1);


        retranslateUi(Peers);

        QMetaObject::connectSlotsByName(Peers);
    } // setupUi

    void retranslateUi(QDialog *Peers)
    {
        Peers->setWindowTitle(QApplication::translate("Peers", "Peers", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Peers: public Ui_Peers {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PEERS_H
