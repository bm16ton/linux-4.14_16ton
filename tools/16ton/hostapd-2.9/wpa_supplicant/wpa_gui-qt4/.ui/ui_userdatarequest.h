/********************************************************************************
** Form generated from reading UI file 'userdatarequest.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_USERDATAREQUEST_H
#define UI_USERDATAREQUEST_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_UserDataRequest
{
public:
    QVBoxLayout *vboxLayout;
    QLabel *queryInfo;
    QHBoxLayout *hboxLayout;
    QLabel *queryField;
    QLineEdit *queryEdit;
    QHBoxLayout *hboxLayout1;
    QSpacerItem *spacer4;
    QPushButton *buttonOk;
    QPushButton *buttonCancel;

    void setupUi(QDialog *UserDataRequest)
    {
        if (UserDataRequest->objectName().isEmpty())
            UserDataRequest->setObjectName(QString::fromUtf8("UserDataRequest"));
        UserDataRequest->resize(216, 103);
        UserDataRequest->setSizeGripEnabled(true);
        vboxLayout = new QVBoxLayout(UserDataRequest);
        vboxLayout->setSpacing(6);
        vboxLayout->setContentsMargins(11, 11, 11, 11);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        queryInfo = new QLabel(UserDataRequest);
        queryInfo->setObjectName(QString::fromUtf8("queryInfo"));

        vboxLayout->addWidget(queryInfo);

        hboxLayout = new QHBoxLayout();
        hboxLayout->setSpacing(6);
        hboxLayout->setContentsMargins(0, 0, 0, 0);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        queryField = new QLabel(UserDataRequest);
        queryField->setObjectName(QString::fromUtf8("queryField"));

        hboxLayout->addWidget(queryField);

        queryEdit = new QLineEdit(UserDataRequest);
        queryEdit->setObjectName(QString::fromUtf8("queryEdit"));
        queryEdit->setEnabled(true);
        queryEdit->setEchoMode(QLineEdit::Password);

        hboxLayout->addWidget(queryEdit);


        vboxLayout->addLayout(hboxLayout);

        hboxLayout1 = new QHBoxLayout();
        hboxLayout1->setSpacing(6);
        hboxLayout1->setContentsMargins(0, 0, 0, 0);
        hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
        spacer4 = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout1->addItem(spacer4);

        buttonOk = new QPushButton(UserDataRequest);
        buttonOk->setObjectName(QString::fromUtf8("buttonOk"));
        buttonOk->setAutoDefault(true);
        buttonOk->setDefault(true);

        hboxLayout1->addWidget(buttonOk);

        buttonCancel = new QPushButton(UserDataRequest);
        buttonCancel->setObjectName(QString::fromUtf8("buttonCancel"));
        buttonCancel->setAutoDefault(true);

        hboxLayout1->addWidget(buttonCancel);


        vboxLayout->addLayout(hboxLayout1);


        retranslateUi(UserDataRequest);

        QMetaObject::connectSlotsByName(UserDataRequest);
    } // setupUi

    void retranslateUi(QDialog *UserDataRequest)
    {
        UserDataRequest->setWindowTitle(QApplication::translate("UserDataRequest", "Authentication credentials required", 0, QApplication::UnicodeUTF8));
        queryInfo->setText(QString());
        queryField->setText(QString());
        buttonOk->setText(QApplication::translate("UserDataRequest", "&OK", 0, QApplication::UnicodeUTF8));
        buttonOk->setShortcut(QString());
        buttonCancel->setText(QApplication::translate("UserDataRequest", "&Cancel", 0, QApplication::UnicodeUTF8));
        buttonCancel->setShortcut(QString());
    } // retranslateUi

};

namespace Ui {
    class UserDataRequest: public Ui_UserDataRequest {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_USERDATAREQUEST_H
