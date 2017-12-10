/********************************************************************************
** Form generated from reading UI file 'networkconfig.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NETWORKCONFIG_H
#define UI_NETWORKCONFIG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QTreeWidget>

QT_BEGIN_NAMESPACE

class Ui_NetworkConfig
{
public:
    QGridLayout *gridLayout;
    QPushButton *cancelButton;
    QFrame *frame9;
    QGridLayout *gridLayout1;
    QLabel *ssidLabel;
    QLineEdit *ssidEdit;
    QLabel *authLabel;
    QComboBox *authSelect;
    QLabel *encrLabel;
    QComboBox *encrSelect;
    QLabel *pskLabel;
    QLineEdit *pskEdit;
    QLabel *eapLabel;
    QComboBox *eapSelect;
    QLabel *identityLabel;
    QLineEdit *identityEdit;
    QLabel *passwordLabel;
    QLineEdit *passwordEdit;
    QLabel *cacertLabel;
    QLineEdit *cacertEdit;
    QGroupBox *wepBox;
    QGridLayout *gridLayout2;
    QRadioButton *wep0Radio;
    QRadioButton *wep1Radio;
    QRadioButton *wep3Radio;
    QRadioButton *wep2Radio;
    QLineEdit *wep0Edit;
    QLineEdit *wep1Edit;
    QLineEdit *wep2Edit;
    QLineEdit *wep3Edit;
    QGroupBox *optionalSettingsBox;
    QGridLayout *gridLayout3;
    QLineEdit *idstrEdit;
    QSpinBox *prioritySpinBox;
    QLabel *idstrLabel;
    QLabel *priorityLabel;
    QLabel *phase2Label;
    QComboBox *phase2Select;
    QPushButton *addButton;
    QPushButton *removeButton;
    QSpacerItem *spacerItem;
    QPushButton *useWpsButton;

    void setupUi(QDialog *NetworkConfig)
    {
        if (NetworkConfig->objectName().isEmpty())
            NetworkConfig->setObjectName(QString::fromUtf8("NetworkConfig"));
        NetworkConfig->resize(410, 534);
        gridLayout = new QGridLayout(NetworkConfig);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        cancelButton = new QPushButton(NetworkConfig);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

        gridLayout->addWidget(cancelButton, 1, 3, 1, 1);

        frame9 = new QFrame(NetworkConfig);
        frame9->setObjectName(QString::fromUtf8("frame9"));
        frame9->setFrameShape(QFrame::NoFrame);
        frame9->setFrameShadow(QFrame::Plain);
        gridLayout1 = new QGridLayout(frame9);
        gridLayout1->setSpacing(6);
        gridLayout1->setContentsMargins(11, 11, 11, 11);
        gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
        ssidLabel = new QLabel(frame9);
        ssidLabel->setObjectName(QString::fromUtf8("ssidLabel"));

        gridLayout1->addWidget(ssidLabel, 0, 0, 1, 1);

        ssidEdit = new QLineEdit(frame9);
        ssidEdit->setObjectName(QString::fromUtf8("ssidEdit"));

        gridLayout1->addWidget(ssidEdit, 0, 1, 1, 1);

        authLabel = new QLabel(frame9);
        authLabel->setObjectName(QString::fromUtf8("authLabel"));

        gridLayout1->addWidget(authLabel, 1, 0, 1, 1);

        authSelect = new QComboBox(frame9);
        authSelect->setObjectName(QString::fromUtf8("authSelect"));

        gridLayout1->addWidget(authSelect, 1, 1, 1, 1);

        encrLabel = new QLabel(frame9);
        encrLabel->setObjectName(QString::fromUtf8("encrLabel"));

        gridLayout1->addWidget(encrLabel, 2, 0, 1, 1);

        encrSelect = new QComboBox(frame9);
        encrSelect->setObjectName(QString::fromUtf8("encrSelect"));

        gridLayout1->addWidget(encrSelect, 2, 1, 1, 1);

        pskLabel = new QLabel(frame9);
        pskLabel->setObjectName(QString::fromUtf8("pskLabel"));

        gridLayout1->addWidget(pskLabel, 3, 0, 1, 1);

        pskEdit = new QLineEdit(frame9);
        pskEdit->setObjectName(QString::fromUtf8("pskEdit"));
        pskEdit->setEnabled(false);
        pskEdit->setEchoMode(QLineEdit::Password);

        gridLayout1->addWidget(pskEdit, 3, 1, 1, 1);

        eapLabel = new QLabel(frame9);
        eapLabel->setObjectName(QString::fromUtf8("eapLabel"));

        gridLayout1->addWidget(eapLabel, 4, 0, 1, 1);

        eapSelect = new QComboBox(frame9);
        eapSelect->setObjectName(QString::fromUtf8("eapSelect"));
        eapSelect->setEnabled(false);

        gridLayout1->addWidget(eapSelect, 4, 1, 1, 1);

        identityLabel = new QLabel(frame9);
        identityLabel->setObjectName(QString::fromUtf8("identityLabel"));

        gridLayout1->addWidget(identityLabel, 5, 0, 1, 1);

        identityEdit = new QLineEdit(frame9);
        identityEdit->setObjectName(QString::fromUtf8("identityEdit"));
        identityEdit->setEnabled(false);

        gridLayout1->addWidget(identityEdit, 5, 1, 1, 1);

        passwordLabel = new QLabel(frame9);
        passwordLabel->setObjectName(QString::fromUtf8("passwordLabel"));

        gridLayout1->addWidget(passwordLabel, 6, 0, 1, 1);

        passwordEdit = new QLineEdit(frame9);
        passwordEdit->setObjectName(QString::fromUtf8("passwordEdit"));
        passwordEdit->setEnabled(false);
        passwordEdit->setEchoMode(QLineEdit::Password);

        gridLayout1->addWidget(passwordEdit, 6, 1, 1, 1);

        cacertLabel = new QLabel(frame9);
        cacertLabel->setObjectName(QString::fromUtf8("cacertLabel"));

        gridLayout1->addWidget(cacertLabel, 7, 0, 1, 1);

        cacertEdit = new QLineEdit(frame9);
        cacertEdit->setObjectName(QString::fromUtf8("cacertEdit"));
        cacertEdit->setEnabled(false);

        gridLayout1->addWidget(cacertEdit, 7, 1, 1, 1);

        wepBox = new QGroupBox(frame9);
        wepBox->setObjectName(QString::fromUtf8("wepBox"));
        wepBox->setEnabled(true);
        gridLayout2 = new QGridLayout(wepBox);
        gridLayout2->setSpacing(6);
        gridLayout2->setContentsMargins(11, 11, 11, 11);
        gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
        wep0Radio = new QRadioButton(wepBox);
        wep0Radio->setObjectName(QString::fromUtf8("wep0Radio"));
        wep0Radio->setEnabled(false);

        gridLayout2->addWidget(wep0Radio, 0, 0, 1, 1);

        wep1Radio = new QRadioButton(wepBox);
        wep1Radio->setObjectName(QString::fromUtf8("wep1Radio"));
        wep1Radio->setEnabled(false);

        gridLayout2->addWidget(wep1Radio, 1, 0, 1, 1);

        wep3Radio = new QRadioButton(wepBox);
        wep3Radio->setObjectName(QString::fromUtf8("wep3Radio"));
        wep3Radio->setEnabled(false);

        gridLayout2->addWidget(wep3Radio, 3, 0, 1, 1);

        wep2Radio = new QRadioButton(wepBox);
        wep2Radio->setObjectName(QString::fromUtf8("wep2Radio"));
        wep2Radio->setEnabled(false);

        gridLayout2->addWidget(wep2Radio, 2, 0, 1, 1);

        wep0Edit = new QLineEdit(wepBox);
        wep0Edit->setObjectName(QString::fromUtf8("wep0Edit"));
        wep0Edit->setEnabled(false);

        gridLayout2->addWidget(wep0Edit, 0, 1, 1, 1);

        wep1Edit = new QLineEdit(wepBox);
        wep1Edit->setObjectName(QString::fromUtf8("wep1Edit"));
        wep1Edit->setEnabled(false);

        gridLayout2->addWidget(wep1Edit, 1, 1, 1, 1);

        wep2Edit = new QLineEdit(wepBox);
        wep2Edit->setObjectName(QString::fromUtf8("wep2Edit"));
        wep2Edit->setEnabled(false);

        gridLayout2->addWidget(wep2Edit, 2, 1, 1, 1);

        wep3Edit = new QLineEdit(wepBox);
        wep3Edit->setObjectName(QString::fromUtf8("wep3Edit"));
        wep3Edit->setEnabled(false);

        gridLayout2->addWidget(wep3Edit, 3, 1, 1, 1);


        gridLayout1->addWidget(wepBox, 8, 0, 1, 2);

        optionalSettingsBox = new QGroupBox(frame9);
        optionalSettingsBox->setObjectName(QString::fromUtf8("optionalSettingsBox"));
        optionalSettingsBox->setEnabled(true);
        gridLayout3 = new QGridLayout(optionalSettingsBox);
        gridLayout3->setSpacing(6);
        gridLayout3->setContentsMargins(11, 11, 11, 11);
        gridLayout3->setObjectName(QString::fromUtf8("gridLayout3"));
        idstrEdit = new QLineEdit(optionalSettingsBox);
        idstrEdit->setObjectName(QString::fromUtf8("idstrEdit"));

        gridLayout3->addWidget(idstrEdit, 0, 1, 1, 1);

        prioritySpinBox = new QSpinBox(optionalSettingsBox);
        prioritySpinBox->setObjectName(QString::fromUtf8("prioritySpinBox"));
        prioritySpinBox->setMaximum(10000);
        prioritySpinBox->setSingleStep(10);

        gridLayout3->addWidget(prioritySpinBox, 0, 3, 1, 1);

        idstrLabel = new QLabel(optionalSettingsBox);
        idstrLabel->setObjectName(QString::fromUtf8("idstrLabel"));

        gridLayout3->addWidget(idstrLabel, 0, 0, 1, 1);

        priorityLabel = new QLabel(optionalSettingsBox);
        priorityLabel->setObjectName(QString::fromUtf8("priorityLabel"));

        gridLayout3->addWidget(priorityLabel, 0, 2, 1, 1);

        phase2Label = new QLabel(optionalSettingsBox);
        phase2Label->setObjectName(QString::fromUtf8("phase2Label"));

        gridLayout3->addWidget(phase2Label, 1, 0, 1, 1);

        phase2Select = new QComboBox(optionalSettingsBox);
        phase2Select->setObjectName(QString::fromUtf8("phase2Select"));
        phase2Select->setEnabled(false);

        gridLayout3->addWidget(phase2Select, 1, 1, 1, 1);


        gridLayout1->addWidget(optionalSettingsBox, 9, 0, 1, 2);


        gridLayout->addWidget(frame9, 0, 0, 1, 4);

        addButton = new QPushButton(NetworkConfig);
        addButton->setObjectName(QString::fromUtf8("addButton"));

        gridLayout->addWidget(addButton, 1, 2, 1, 1);

        removeButton = new QPushButton(NetworkConfig);
        removeButton->setObjectName(QString::fromUtf8("removeButton"));
        removeButton->setEnabled(false);

        gridLayout->addWidget(removeButton, 1, 3, 1, 1);

        spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(spacerItem, 1, 0, 1, 1);

        useWpsButton = new QPushButton(NetworkConfig);
        useWpsButton->setObjectName(QString::fromUtf8("useWpsButton"));
        useWpsButton->setEnabled(false);

        gridLayout->addWidget(useWpsButton, 1, 1, 1, 1);

        QWidget::setTabOrder(ssidEdit, authSelect);
        QWidget::setTabOrder(authSelect, encrSelect);
        QWidget::setTabOrder(encrSelect, pskEdit);
        QWidget::setTabOrder(pskEdit, eapSelect);
        QWidget::setTabOrder(eapSelect, identityEdit);
        QWidget::setTabOrder(identityEdit, passwordEdit);
        QWidget::setTabOrder(passwordEdit, cacertEdit);
        QWidget::setTabOrder(cacertEdit, wep0Radio);
        QWidget::setTabOrder(wep0Radio, wep0Edit);
        QWidget::setTabOrder(wep0Edit, wep1Radio);
        QWidget::setTabOrder(wep1Radio, wep1Edit);
        QWidget::setTabOrder(wep1Edit, wep2Radio);
        QWidget::setTabOrder(wep2Radio, wep2Edit);
        QWidget::setTabOrder(wep2Edit, wep3Radio);
        QWidget::setTabOrder(wep3Radio, wep3Edit);
        QWidget::setTabOrder(wep3Edit, idstrEdit);
        QWidget::setTabOrder(idstrEdit, prioritySpinBox);
        QWidget::setTabOrder(prioritySpinBox, phase2Select);
        QWidget::setTabOrder(phase2Select, addButton);
        QWidget::setTabOrder(addButton, removeButton);
        QWidget::setTabOrder(removeButton, cancelButton);

        retranslateUi(NetworkConfig);

        QMetaObject::connectSlotsByName(NetworkConfig);
    } // setupUi

    void retranslateUi(QDialog *NetworkConfig)
    {
        NetworkConfig->setWindowTitle(QApplication::translate("NetworkConfig", "NetworkConfig", 0, QApplication::UnicodeUTF8));
        cancelButton->setText(QApplication::translate("NetworkConfig", "Cancel", 0, QApplication::UnicodeUTF8));
        ssidLabel->setText(QApplication::translate("NetworkConfig", "SSID", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        ssidEdit->setToolTip(QApplication::translate("NetworkConfig", "Network name (Service Set IDentifier)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        ssidEdit->setText(QString());
        authLabel->setText(QApplication::translate("NetworkConfig", "Authentication", 0, QApplication::UnicodeUTF8));
        authSelect->clear();
        authSelect->insertItems(0, QStringList()
         << QApplication::translate("NetworkConfig", "Plaintext (open / no authentication)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("NetworkConfig", "Static WEP (no authentication)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("NetworkConfig", "Static WEP (Shared Key authentication)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("NetworkConfig", "IEEE 802.1X", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("NetworkConfig", "WPA-Personal (PSK)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("NetworkConfig", "WPA-Enterprise (EAP)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("NetworkConfig", "WPA2-Personal (PSK)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("NetworkConfig", "WPA2-Enterprise (EAP)", 0, QApplication::UnicodeUTF8)
        );
        encrLabel->setText(QApplication::translate("NetworkConfig", "Encryption", 0, QApplication::UnicodeUTF8));
        encrSelect->clear();
        encrSelect->insertItems(0, QStringList()
         << QApplication::translate("NetworkConfig", "None", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("NetworkConfig", "WEP", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("NetworkConfig", "TKIP", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("NetworkConfig", "CCMP", 0, QApplication::UnicodeUTF8)
        );
        pskLabel->setText(QApplication::translate("NetworkConfig", "PSK", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pskEdit->setToolTip(QApplication::translate("NetworkConfig", "WPA/WPA2 pre-shared key or passphrase", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        pskEdit->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
        eapLabel->setText(QApplication::translate("NetworkConfig", "EAP method", 0, QApplication::UnicodeUTF8));
        identityLabel->setText(QApplication::translate("NetworkConfig", "Identity", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        identityEdit->setToolTip(QApplication::translate("NetworkConfig", "Username/Identity for EAP methods", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        passwordLabel->setText(QApplication::translate("NetworkConfig", "Password", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        passwordEdit->setToolTip(QApplication::translate("NetworkConfig", "Password for EAP methods", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        cacertLabel->setText(QApplication::translate("NetworkConfig", "CA certificate", 0, QApplication::UnicodeUTF8));
        wepBox->setTitle(QApplication::translate("NetworkConfig", "WEP keys", 0, QApplication::UnicodeUTF8));
        wep0Radio->setText(QApplication::translate("NetworkConfig", "key 0", 0, QApplication::UnicodeUTF8));
        wep1Radio->setText(QApplication::translate("NetworkConfig", "key 1", 0, QApplication::UnicodeUTF8));
        wep3Radio->setText(QApplication::translate("NetworkConfig", "key 3", 0, QApplication::UnicodeUTF8));
        wep2Radio->setText(QApplication::translate("NetworkConfig", "key 2", 0, QApplication::UnicodeUTF8));
        optionalSettingsBox->setTitle(QApplication::translate("NetworkConfig", "Optional Settings", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        idstrEdit->setToolTip(QApplication::translate("NetworkConfig", "Network Identification String", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        prioritySpinBox->setToolTip(QApplication::translate("NetworkConfig", "Network Priority", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        idstrLabel->setText(QApplication::translate("NetworkConfig", "IDString", 0, QApplication::UnicodeUTF8));
        priorityLabel->setText(QApplication::translate("NetworkConfig", "Priority", 0, QApplication::UnicodeUTF8));
        phase2Label->setText(QApplication::translate("NetworkConfig", "Inner auth", 0, QApplication::UnicodeUTF8));
        addButton->setText(QApplication::translate("NetworkConfig", "Add", 0, QApplication::UnicodeUTF8));
        removeButton->setText(QApplication::translate("NetworkConfig", "Remove", 0, QApplication::UnicodeUTF8));
        useWpsButton->setText(QApplication::translate("NetworkConfig", "WPS", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class NetworkConfig: public Ui_NetworkConfig {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NETWORKCONFIG_H
