/********************************************************************************
** Form generated from reading UI file 'scanresults.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCANRESULTS_H
#define UI_SCANRESULTS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ScanResults
{
public:
    QVBoxLayout *vboxLayout;
    QTreeWidget *scanResultsWidget;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;
    QPushButton *scanButton;
    QPushButton *closeButton;

    void setupUi(QDialog *ScanResults)
    {
        if (ScanResults->objectName().isEmpty())
            ScanResults->setObjectName(QString::fromUtf8("ScanResults"));
        ScanResults->resize(452, 244);
        vboxLayout = new QVBoxLayout(ScanResults);
        vboxLayout->setSpacing(6);
        vboxLayout->setContentsMargins(11, 11, 11, 11);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        scanResultsWidget = new QTreeWidget(ScanResults);
        scanResultsWidget->setObjectName(QString::fromUtf8("scanResultsWidget"));
        scanResultsWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        scanResultsWidget->setUniformRowHeights(true);
        scanResultsWidget->setSortingEnabled(true);
        scanResultsWidget->setColumnCount(5);

        vboxLayout->addWidget(scanResultsWidget);

        hboxLayout = new QHBoxLayout();
        hboxLayout->setSpacing(6);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem);

        scanButton = new QPushButton(ScanResults);
        scanButton->setObjectName(QString::fromUtf8("scanButton"));

        hboxLayout->addWidget(scanButton);

        closeButton = new QPushButton(ScanResults);
        closeButton->setObjectName(QString::fromUtf8("closeButton"));

        hboxLayout->addWidget(closeButton);


        vboxLayout->addLayout(hboxLayout);


        retranslateUi(ScanResults);

        QMetaObject::connectSlotsByName(ScanResults);
    } // setupUi

    void retranslateUi(QDialog *ScanResults)
    {
        ScanResults->setWindowTitle(QApplication::translate("ScanResults", "Scan results", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = scanResultsWidget->headerItem();
        ___qtreewidgetitem->setText(4, QApplication::translate("ScanResults", "flags", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(3, QApplication::translate("ScanResults", "signal", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(2, QApplication::translate("ScanResults", "frequency", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(1, QApplication::translate("ScanResults", "BSSID", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(0, QApplication::translate("ScanResults", "SSID", 0, QApplication::UnicodeUTF8));
        scanButton->setText(QApplication::translate("ScanResults", "Scan", 0, QApplication::UnicodeUTF8));
        closeButton->setText(QApplication::translate("ScanResults", "Close", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ScanResults: public Ui_ScanResults {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCANRESULTS_H
