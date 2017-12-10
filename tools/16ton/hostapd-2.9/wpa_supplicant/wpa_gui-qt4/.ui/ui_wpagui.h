/********************************************************************************
** Form generated from reading UI file 'wpagui.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WPAGUI_H
#define UI_WPAGUI_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTabWidget>
#include <QtGui/QTextEdit>
#include <QtGui/QWidget>
#include <QtCore/QSocketNotifier>
#include <QtCore/QTimer>
#include "eventhistory.h"
#include "peers.h"
#include "scanresults.h"
#include "wpamsg.h"

QT_BEGIN_NAMESPACE

class Ui_WpaGui
{
public:
    QAction *fileEventHistoryAction;
    QAction *fileSaveConfigAction;
    QAction *fileExitAction;
    QAction *networkAddAction;
    QAction *networkEditAction;
    QAction *networkRemoveAction;
    QAction *networkEnableAllAction;
    QAction *networkDisableAllAction;
    QAction *networkRemoveAllAction;
    QAction *helpContentsAction;
    QAction *helpIndexAction;
    QAction *helpAboutAction;
    QAction *actionWPS;
    QAction *actionPeers;
    QWidget *widget;
    QGridLayout *gridLayout;
    QLabel *adapterLabel;
    QComboBox *adapterSelect;
    QLabel *networkLabel;
    QComboBox *networkSelect;
    QTabWidget *wpaguiTab;
    QWidget *statusTab;
    QGridLayout *gridLayout1;
    QFrame *frame3;
    QGridLayout *gridLayout2;
    QLabel *statusLabel;
    QLabel *lastMessageLabel;
    QLabel *authenticationLabel;
    QLabel *encryptionLabel;
    QLabel *ssidLabel;
    QLabel *bssidLabel;
    QLabel *ipAddressLabel;
    QLabel *textStatus;
    QLabel *textLastMessage;
    QLabel *textAuthentication;
    QLabel *textEncryption;
    QLabel *textSsid;
    QLabel *textBssid;
    QLabel *textIpAddress;
    QSpacerItem *spacerItem;
    QPushButton *connectButton;
    QPushButton *disconnectButton;
    QPushButton *scanButton;
    QSpacerItem *spacerItem1;
    QWidget *networkconfigTab;
    QGridLayout *gridLayout3;
    QListWidget *networkList;
    QSpacerItem *spacerItem2;
    QRadioButton *enableRadioButton;
    QPushButton *editNetworkButton;
    QPushButton *removeNetworkButton;
    QSpacerItem *spacerItem3;
    QRadioButton *disableRadioButton;
    QPushButton *addNetworkButton;
    QPushButton *scanNetworkButton;
    QWidget *wpsTab;
    QGridLayout *wpsGridLayout;
    QLabel *label_2;
    QLabel *wpsStatusText;
    QPushButton *wpsPbcButton;
    QPushButton *wpsPinButton;
    QLabel *label;
    QLineEdit *wpsPinEdit;
    QPushButton *wpsApPinButton;
    QLabel *label_3;
    QLineEdit *wpsApPinEdit;
    QTextEdit *wpsInstructions;
    QMenuBar *MenuBar;
    QMenu *fileMenu;
    QMenu *networkMenu;
    QMenu *helpMenu;

    void setupUi(QMainWindow *WpaGui)
    {
        if (WpaGui->objectName().isEmpty())
            WpaGui->setObjectName(QString::fromUtf8("WpaGui"));
        WpaGui->resize(345, 330);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/wpa_gui.svg"), QSize(), QIcon::Normal, QIcon::Off);
        WpaGui->setWindowIcon(icon);
        fileEventHistoryAction = new QAction(WpaGui);
        fileEventHistoryAction->setObjectName(QString::fromUtf8("fileEventHistoryAction"));
        fileSaveConfigAction = new QAction(WpaGui);
        fileSaveConfigAction->setObjectName(QString::fromUtf8("fileSaveConfigAction"));
        fileExitAction = new QAction(WpaGui);
        fileExitAction->setObjectName(QString::fromUtf8("fileExitAction"));
        networkAddAction = new QAction(WpaGui);
        networkAddAction->setObjectName(QString::fromUtf8("networkAddAction"));
        networkEditAction = new QAction(WpaGui);
        networkEditAction->setObjectName(QString::fromUtf8("networkEditAction"));
        networkRemoveAction = new QAction(WpaGui);
        networkRemoveAction->setObjectName(QString::fromUtf8("networkRemoveAction"));
        networkEnableAllAction = new QAction(WpaGui);
        networkEnableAllAction->setObjectName(QString::fromUtf8("networkEnableAllAction"));
        networkDisableAllAction = new QAction(WpaGui);
        networkDisableAllAction->setObjectName(QString::fromUtf8("networkDisableAllAction"));
        networkRemoveAllAction = new QAction(WpaGui);
        networkRemoveAllAction->setObjectName(QString::fromUtf8("networkRemoveAllAction"));
        helpContentsAction = new QAction(WpaGui);
        helpContentsAction->setObjectName(QString::fromUtf8("helpContentsAction"));
        helpContentsAction->setEnabled(false);
        helpIndexAction = new QAction(WpaGui);
        helpIndexAction->setObjectName(QString::fromUtf8("helpIndexAction"));
        helpIndexAction->setEnabled(false);
        helpAboutAction = new QAction(WpaGui);
        helpAboutAction->setObjectName(QString::fromUtf8("helpAboutAction"));
        actionWPS = new QAction(WpaGui);
        actionWPS->setObjectName(QString::fromUtf8("actionWPS"));
        actionWPS->setEnabled(false);
        actionPeers = new QAction(WpaGui);
        actionPeers->setObjectName(QString::fromUtf8("actionPeers"));
        widget = new QWidget(WpaGui);
        widget->setObjectName(QString::fromUtf8("widget"));
        gridLayout = new QGridLayout(widget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        adapterLabel = new QLabel(widget);
        adapterLabel->setObjectName(QString::fromUtf8("adapterLabel"));

        gridLayout->addWidget(adapterLabel, 0, 0, 1, 1);

        adapterSelect = new QComboBox(widget);
        adapterSelect->setObjectName(QString::fromUtf8("adapterSelect"));

        gridLayout->addWidget(adapterSelect, 0, 1, 1, 1);

        networkLabel = new QLabel(widget);
        networkLabel->setObjectName(QString::fromUtf8("networkLabel"));

        gridLayout->addWidget(networkLabel, 1, 0, 1, 1);

        networkSelect = new QComboBox(widget);
        networkSelect->setObjectName(QString::fromUtf8("networkSelect"));

        gridLayout->addWidget(networkSelect, 1, 1, 1, 1);

        wpaguiTab = new QTabWidget(widget);
        wpaguiTab->setObjectName(QString::fromUtf8("wpaguiTab"));
        statusTab = new QWidget();
        statusTab->setObjectName(QString::fromUtf8("statusTab"));
        gridLayout1 = new QGridLayout(statusTab);
        gridLayout1->setSpacing(6);
        gridLayout1->setContentsMargins(11, 11, 11, 11);
        gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
        frame3 = new QFrame(statusTab);
        frame3->setObjectName(QString::fromUtf8("frame3"));
        frame3->setFrameShape(QFrame::NoFrame);
        frame3->setFrameShadow(QFrame::Plain);
        gridLayout2 = new QGridLayout(frame3);
        gridLayout2->setSpacing(6);
        gridLayout2->setContentsMargins(11, 11, 11, 11);
        gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
        statusLabel = new QLabel(frame3);
        statusLabel->setObjectName(QString::fromUtf8("statusLabel"));

        gridLayout2->addWidget(statusLabel, 0, 0, 1, 1);

        lastMessageLabel = new QLabel(frame3);
        lastMessageLabel->setObjectName(QString::fromUtf8("lastMessageLabel"));

        gridLayout2->addWidget(lastMessageLabel, 1, 0, 1, 1);

        authenticationLabel = new QLabel(frame3);
        authenticationLabel->setObjectName(QString::fromUtf8("authenticationLabel"));

        gridLayout2->addWidget(authenticationLabel, 2, 0, 1, 1);

        encryptionLabel = new QLabel(frame3);
        encryptionLabel->setObjectName(QString::fromUtf8("encryptionLabel"));

        gridLayout2->addWidget(encryptionLabel, 3, 0, 1, 1);

        ssidLabel = new QLabel(frame3);
        ssidLabel->setObjectName(QString::fromUtf8("ssidLabel"));

        gridLayout2->addWidget(ssidLabel, 4, 0, 1, 1);

        bssidLabel = new QLabel(frame3);
        bssidLabel->setObjectName(QString::fromUtf8("bssidLabel"));

        gridLayout2->addWidget(bssidLabel, 5, 0, 1, 1);

        ipAddressLabel = new QLabel(frame3);
        ipAddressLabel->setObjectName(QString::fromUtf8("ipAddressLabel"));

        gridLayout2->addWidget(ipAddressLabel, 6, 0, 1, 1);

        textStatus = new QLabel(frame3);
        textStatus->setObjectName(QString::fromUtf8("textStatus"));

        gridLayout2->addWidget(textStatus, 0, 1, 1, 1);

        textLastMessage = new QLabel(frame3);
        textLastMessage->setObjectName(QString::fromUtf8("textLastMessage"));

        gridLayout2->addWidget(textLastMessage, 1, 1, 1, 3);

        textAuthentication = new QLabel(frame3);
        textAuthentication->setObjectName(QString::fromUtf8("textAuthentication"));

        gridLayout2->addWidget(textAuthentication, 2, 1, 1, 1);

        textEncryption = new QLabel(frame3);
        textEncryption->setObjectName(QString::fromUtf8("textEncryption"));

        gridLayout2->addWidget(textEncryption, 3, 1, 1, 1);

        textSsid = new QLabel(frame3);
        textSsid->setObjectName(QString::fromUtf8("textSsid"));

        gridLayout2->addWidget(textSsid, 4, 1, 1, 1);

        textBssid = new QLabel(frame3);
        textBssid->setObjectName(QString::fromUtf8("textBssid"));

        gridLayout2->addWidget(textBssid, 5, 1, 1, 1);

        textIpAddress = new QLabel(frame3);
        textIpAddress->setObjectName(QString::fromUtf8("textIpAddress"));

        gridLayout2->addWidget(textIpAddress, 6, 1, 1, 1);


        gridLayout1->addWidget(frame3, 0, 0, 1, 5);

        spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout1->addItem(spacerItem, 1, 0, 1, 1);

        connectButton = new QPushButton(statusTab);
        connectButton->setObjectName(QString::fromUtf8("connectButton"));

        gridLayout1->addWidget(connectButton, 1, 1, 1, 1);

        disconnectButton = new QPushButton(statusTab);
        disconnectButton->setObjectName(QString::fromUtf8("disconnectButton"));

        gridLayout1->addWidget(disconnectButton, 1, 2, 1, 1);

        scanButton = new QPushButton(statusTab);
        scanButton->setObjectName(QString::fromUtf8("scanButton"));

        gridLayout1->addWidget(scanButton, 1, 3, 1, 1);

        spacerItem1 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout1->addItem(spacerItem1, 1, 4, 1, 1);

        wpaguiTab->addTab(statusTab, QString());
        networkconfigTab = new QWidget();
        networkconfigTab->setObjectName(QString::fromUtf8("networkconfigTab"));
        gridLayout3 = new QGridLayout(networkconfigTab);
        gridLayout3->setSpacing(6);
        gridLayout3->setContentsMargins(11, 11, 11, 11);
        gridLayout3->setObjectName(QString::fromUtf8("gridLayout3"));
        networkList = new QListWidget(networkconfigTab);
        networkList->setObjectName(QString::fromUtf8("networkList"));
        networkList->setSelectionRectVisible(true);

        gridLayout3->addWidget(networkList, 0, 0, 1, 5);

        spacerItem2 = new QSpacerItem(20, 61, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout3->addItem(spacerItem2, 1, 0, 2, 1);

        enableRadioButton = new QRadioButton(networkconfigTab);
        enableRadioButton->setObjectName(QString::fromUtf8("enableRadioButton"));

        gridLayout3->addWidget(enableRadioButton, 1, 1, 1, 1);

        editNetworkButton = new QPushButton(networkconfigTab);
        editNetworkButton->setObjectName(QString::fromUtf8("editNetworkButton"));

        gridLayout3->addWidget(editNetworkButton, 1, 2, 1, 1);

        removeNetworkButton = new QPushButton(networkconfigTab);
        removeNetworkButton->setObjectName(QString::fromUtf8("removeNetworkButton"));

        gridLayout3->addWidget(removeNetworkButton, 1, 3, 1, 1);

        spacerItem3 = new QSpacerItem(20, 61, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout3->addItem(spacerItem3, 1, 4, 2, 1);

        disableRadioButton = new QRadioButton(networkconfigTab);
        disableRadioButton->setObjectName(QString::fromUtf8("disableRadioButton"));

        gridLayout3->addWidget(disableRadioButton, 2, 1, 1, 1);

        addNetworkButton = new QPushButton(networkconfigTab);
        addNetworkButton->setObjectName(QString::fromUtf8("addNetworkButton"));

        gridLayout3->addWidget(addNetworkButton, 2, 2, 1, 1);

        scanNetworkButton = new QPushButton(networkconfigTab);
        scanNetworkButton->setObjectName(QString::fromUtf8("scanNetworkButton"));

        gridLayout3->addWidget(scanNetworkButton, 2, 3, 1, 1);

        wpaguiTab->addTab(networkconfigTab, QString());
        wpsTab = new QWidget();
        wpsTab->setObjectName(QString::fromUtf8("wpsTab"));
        wpsGridLayout = new QGridLayout(wpsTab);
        wpsGridLayout->setSpacing(6);
        wpsGridLayout->setContentsMargins(11, 11, 11, 11);
        wpsGridLayout->setObjectName(QString::fromUtf8("wpsGridLayout"));
        label_2 = new QLabel(wpsTab);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        wpsGridLayout->addWidget(label_2, 0, 0, 1, 1);

        wpsStatusText = new QLabel(wpsTab);
        wpsStatusText->setObjectName(QString::fromUtf8("wpsStatusText"));

        wpsGridLayout->addWidget(wpsStatusText, 0, 1, 1, 3);

        wpsPbcButton = new QPushButton(wpsTab);
        wpsPbcButton->setObjectName(QString::fromUtf8("wpsPbcButton"));

        wpsGridLayout->addWidget(wpsPbcButton, 1, 0, 1, 2);

        wpsPinButton = new QPushButton(wpsTab);
        wpsPinButton->setObjectName(QString::fromUtf8("wpsPinButton"));

        wpsGridLayout->addWidget(wpsPinButton, 2, 0, 1, 2);

        label = new QLabel(wpsTab);
        label->setObjectName(QString::fromUtf8("label"));

        wpsGridLayout->addWidget(label, 2, 2, 1, 1);

        wpsPinEdit = new QLineEdit(wpsTab);
        wpsPinEdit->setObjectName(QString::fromUtf8("wpsPinEdit"));
        wpsPinEdit->setEnabled(false);
        wpsPinEdit->setReadOnly(true);

        wpsGridLayout->addWidget(wpsPinEdit, 2, 3, 1, 1);

        wpsApPinButton = new QPushButton(wpsTab);
        wpsApPinButton->setObjectName(QString::fromUtf8("wpsApPinButton"));
        wpsApPinButton->setEnabled(false);

        wpsGridLayout->addWidget(wpsApPinButton, 3, 0, 1, 2);

        label_3 = new QLabel(wpsTab);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        wpsGridLayout->addWidget(label_3, 3, 2, 1, 1);

        wpsApPinEdit = new QLineEdit(wpsTab);
        wpsApPinEdit->setObjectName(QString::fromUtf8("wpsApPinEdit"));
        wpsApPinEdit->setEnabled(false);

        wpsGridLayout->addWidget(wpsApPinEdit, 3, 3, 1, 1);

        wpsInstructions = new QTextEdit(wpsTab);
        wpsInstructions->setObjectName(QString::fromUtf8("wpsInstructions"));
        wpsInstructions->setReadOnly(true);

        wpsGridLayout->addWidget(wpsInstructions, 4, 0, 1, 4);

        wpaguiTab->addTab(wpsTab, QString());

        gridLayout->addWidget(wpaguiTab, 2, 0, 1, 2);

        WpaGui->setCentralWidget(widget);
        MenuBar = new QMenuBar(WpaGui);
        MenuBar->setObjectName(QString::fromUtf8("MenuBar"));
        MenuBar->setGeometry(QRect(0, 0, 345, 24));
        fileMenu = new QMenu(MenuBar);
        fileMenu->setObjectName(QString::fromUtf8("fileMenu"));
        networkMenu = new QMenu(MenuBar);
        networkMenu->setObjectName(QString::fromUtf8("networkMenu"));
        helpMenu = new QMenu(MenuBar);
        helpMenu->setObjectName(QString::fromUtf8("helpMenu"));
        WpaGui->setMenuBar(MenuBar);

        MenuBar->addAction(fileMenu->menuAction());
        MenuBar->addAction(networkMenu->menuAction());
        MenuBar->addAction(helpMenu->menuAction());
        fileMenu->addAction(fileEventHistoryAction);
        fileMenu->addAction(fileSaveConfigAction);
        fileMenu->addAction(actionWPS);
        fileMenu->addAction(actionPeers);
        fileMenu->addSeparator();
        fileMenu->addAction(fileExitAction);
        networkMenu->addAction(networkAddAction);
        networkMenu->addAction(networkEditAction);
        networkMenu->addAction(networkRemoveAction);
        networkMenu->addSeparator();
        networkMenu->addAction(networkEnableAllAction);
        networkMenu->addAction(networkDisableAllAction);
        networkMenu->addAction(networkRemoveAllAction);
        helpMenu->addAction(helpContentsAction);
        helpMenu->addAction(helpIndexAction);
        helpMenu->addSeparator();
        helpMenu->addAction(helpAboutAction);

        retranslateUi(WpaGui);

        wpaguiTab->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(WpaGui);
    } // setupUi

    void retranslateUi(QMainWindow *WpaGui)
    {
        WpaGui->setWindowTitle(QApplication::translate("WpaGui", "wpa_gui", 0, QApplication::UnicodeUTF8));
        fileEventHistoryAction->setText(QApplication::translate("WpaGui", "Event &History", 0, QApplication::UnicodeUTF8));
        fileSaveConfigAction->setText(QApplication::translate("WpaGui", "&Save Configuration", 0, QApplication::UnicodeUTF8));
        fileSaveConfigAction->setShortcut(QApplication::translate("WpaGui", "Ctrl+S", 0, QApplication::UnicodeUTF8));
        fileExitAction->setText(QApplication::translate("WpaGui", "E&xit", 0, QApplication::UnicodeUTF8));
        fileExitAction->setShortcut(QApplication::translate("WpaGui", "Ctrl+Q", 0, QApplication::UnicodeUTF8));
        networkAddAction->setText(QApplication::translate("WpaGui", "&Add", 0, QApplication::UnicodeUTF8));
        networkEditAction->setText(QApplication::translate("WpaGui", "&Edit", 0, QApplication::UnicodeUTF8));
        networkRemoveAction->setText(QApplication::translate("WpaGui", "&Remove", 0, QApplication::UnicodeUTF8));
        networkEnableAllAction->setText(QApplication::translate("WpaGui", "E&nable All", 0, QApplication::UnicodeUTF8));
        networkDisableAllAction->setText(QApplication::translate("WpaGui", "&Disable All", 0, QApplication::UnicodeUTF8));
        networkRemoveAllAction->setText(QApplication::translate("WpaGui", "Re&move All", 0, QApplication::UnicodeUTF8));
        helpContentsAction->setText(QApplication::translate("WpaGui", "&Contents...", 0, QApplication::UnicodeUTF8));
        helpIndexAction->setText(QApplication::translate("WpaGui", "&Index...", 0, QApplication::UnicodeUTF8));
        helpAboutAction->setText(QApplication::translate("WpaGui", "&About", 0, QApplication::UnicodeUTF8));
        actionWPS->setText(QApplication::translate("WpaGui", "&Wi-Fi Protected Setup", 0, QApplication::UnicodeUTF8));
        actionPeers->setText(QApplication::translate("WpaGui", "&Peers", 0, QApplication::UnicodeUTF8));
        adapterLabel->setText(QApplication::translate("WpaGui", "Adapter:", 0, QApplication::UnicodeUTF8));
        networkLabel->setText(QApplication::translate("WpaGui", "Network:", 0, QApplication::UnicodeUTF8));
        statusLabel->setText(QApplication::translate("WpaGui", "Status:", 0, QApplication::UnicodeUTF8));
        lastMessageLabel->setText(QApplication::translate("WpaGui", "Last message:", 0, QApplication::UnicodeUTF8));
        authenticationLabel->setText(QApplication::translate("WpaGui", "Authentication:", 0, QApplication::UnicodeUTF8));
        encryptionLabel->setText(QApplication::translate("WpaGui", "Encryption:", 0, QApplication::UnicodeUTF8));
        ssidLabel->setText(QApplication::translate("WpaGui", "SSID:", 0, QApplication::UnicodeUTF8));
        bssidLabel->setText(QApplication::translate("WpaGui", "BSSID:", 0, QApplication::UnicodeUTF8));
        ipAddressLabel->setText(QApplication::translate("WpaGui", "IP address:", 0, QApplication::UnicodeUTF8));
        textStatus->setText(QString());
        textLastMessage->setText(QString());
        textAuthentication->setText(QString());
        textEncryption->setText(QString());
        textSsid->setText(QString());
        textBssid->setText(QString());
        textIpAddress->setText(QString());
        connectButton->setText(QApplication::translate("WpaGui", "Connect", 0, QApplication::UnicodeUTF8));
        disconnectButton->setText(QApplication::translate("WpaGui", "Disconnect", 0, QApplication::UnicodeUTF8));
        scanButton->setText(QApplication::translate("WpaGui", "Scan", 0, QApplication::UnicodeUTF8));
        wpaguiTab->setTabText(wpaguiTab->indexOf(statusTab), QApplication::translate("WpaGui", "Current Status", 0, QApplication::UnicodeUTF8));
        enableRadioButton->setText(QApplication::translate("WpaGui", "Enabled", 0, QApplication::UnicodeUTF8));
        editNetworkButton->setText(QApplication::translate("WpaGui", "Edit", 0, QApplication::UnicodeUTF8));
        removeNetworkButton->setText(QApplication::translate("WpaGui", "Remove", 0, QApplication::UnicodeUTF8));
        disableRadioButton->setText(QApplication::translate("WpaGui", "Disabled", 0, QApplication::UnicodeUTF8));
        addNetworkButton->setText(QApplication::translate("WpaGui", "Add", 0, QApplication::UnicodeUTF8));
        scanNetworkButton->setText(QApplication::translate("WpaGui", "Scan", 0, QApplication::UnicodeUTF8));
        wpaguiTab->setTabText(wpaguiTab->indexOf(networkconfigTab), QApplication::translate("WpaGui", "Manage Networks", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("WpaGui", "Status:", 0, QApplication::UnicodeUTF8));
        wpsStatusText->setText(QString());
        wpsPbcButton->setText(QApplication::translate("WpaGui", "PBC - push button", 0, QApplication::UnicodeUTF8));
        wpsPinButton->setText(QApplication::translate("WpaGui", "Generate PIN", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("WpaGui", "PIN:", 0, QApplication::UnicodeUTF8));
        wpsApPinButton->setText(QApplication::translate("WpaGui", "Use AP PIN", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("WpaGui", "AP PIN:", 0, QApplication::UnicodeUTF8));
        wpaguiTab->setTabText(wpaguiTab->indexOf(wpsTab), QApplication::translate("WpaGui", "WPS", 0, QApplication::UnicodeUTF8));
        fileMenu->setTitle(QApplication::translate("WpaGui", "&File", 0, QApplication::UnicodeUTF8));
        networkMenu->setTitle(QApplication::translate("WpaGui", "&Network", 0, QApplication::UnicodeUTF8));
        helpMenu->setTitle(QApplication::translate("WpaGui", "&Help", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class WpaGui: public Ui_WpaGui {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WPAGUI_H
