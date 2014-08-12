/********************************************************************************
** Form generated from reading UI file 'OcrControl.ui'
**
** Created by: Qt User Interface Compiler version 5.3.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OCRCONTROL_H
#define UI_OCRCONTROL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_OcrControl
{
public:
    QVBoxLayout *verticalLayout_22;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QFrame *frame;
    QVBoxLayout *verticalLayout_20;
    QHBoxLayout *horizontalLayout_8;
    QVBoxLayout *verticalLayout_8;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_18;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QLineEdit *sessionLineEdit;
    QLabel *label_2;
    QLineEdit *raceTimeLineEdit;
    QLabel *label_20;
    QLineEdit *CountRaceTimeLineEdit;
    QPushButton *caliSessionCountDownBtn;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_12;
    QHBoxLayout *horizontalLayout_yp;
    QFrame *horseFrame;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *titleHBoxLayout;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_3;
    QLabel *label_6;
    QLabel *imageLbl;
    QVBoxLayout *verticalLayout_19;
    QLabel *versionNumLabel;
    QSpacerItem *horizontalSpacer_2;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout_13;
    QHBoxLayout *horizontalLayout_12;
    QTableWidget *fileTableWidget;
    QVBoxLayout *verticalLayout_23;
    QVBoxLayout *verticalLayout_3;
    QPushButton *loadFileBtn;
    QPushButton *appendFileBtn;
    QPushButton *delFileBtn;
    QPushButton *delFileBtn_2;
    QLineEdit *videoPosLineEdit;
    QVBoxLayout *verticalLayout_21;
    QLabel *label_33;
    QProgressBar *videoProgressBar;
    QPushButton *pauseCaliBtn;
    QPushButton *continueBtn;
    QPushButton *inputUserDataBtn;
    QPushButton *pullBackBtn;
    QPushButton *advance3MinBtn;
    QPushButton *advance1MinBtn;
    QPushButton *advance30SecBtn;
    QPushButton *advance10SecBtn;
    QSpacerItem *verticalSpacer;
    QFrame *qplFrame;
    QHBoxLayout *horizontalLayout_10;
    QHBoxLayout *horizontalLayout_9;
    QSpacerItem *horizontalSpacer;
    QVBoxLayout *verticalLayout_7;
    QLabel *label_34;
    QLabel *label_35;
    QLabel *label_14;
    QLabel *label_15;
    QLabel *label_16;
    QLabel *label_17;
    QLabel *label_18;
    QLabel *label_19;
    QVBoxLayout *verticalLayout_11;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_37;
    QLabel *userInputLabel;
    QLabel *QINLabel;
    QLabel *label_21;
    QLabel *label_22;
    QLabel *label_23;
    QLabel *label_24;
    QLabel *label_25;
    QLabel *label_26;
    QLabel *label_27;
    QLabel *label_28;
    QLabel *label_29;
    QLabel *label_30;
    QLabel *label_31;
    QLabel *label_32;
    QLabel *label_59;
    QFrame *qinFrame;
    QVBoxLayout *verticalLayout_5;
    QVBoxLayout *verticalLayout_6;
    QLabel *label_36;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_9;
    QLabel *label_10;
    QLabel *label_11;
    QLabel *label_12;
    QLabel *label_13;
    QSpacerItem *horizontalSpacer_3;
    QVBoxLayout *verticalLayout_13;
    QLabel *imageFileNameLabel;
    QLabel *adTimeLbl;
    QFrame *frame_4;
    QVBoxLayout *verticalLayout_17;
    QHBoxLayout *horizontalLayout_6;
    QHBoxLayout *horizontalLayout_5;
    QVBoxLayout *verticalLayout_16;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *connectBtn;
    QPushButton *loginBtn;
    QCheckBox *reconnectCheckBox;
    QPushButton *disconnectBtn;
    QVBoxLayout *verticalLayout_14;
    QPushButton *submitRaceTimeBtn;
    QPushButton *submitRealBtn;
    QHBoxLayout *horizontalLayout_4;
    QVBoxLayout *verticalLayout_15;
    QVBoxLayout *verticalLayout_10;
    QCheckBox *is63TAICheckBox;
    QPushButton *requestHorseInfoBtn;
    QPushButton *requestRaceIdBtn;
    QVBoxLayout *verticalLayout_9;
    QComboBox *videoTypeComboBox;
    QPushButton *startAcqBtn;
    QPushButton *stopAcqBtn;
    QTextBrowser *textBrowser;

    void setupUi(QWidget *OcrControl)
    {
        if (OcrControl->objectName().isEmpty())
            OcrControl->setObjectName(QStringLiteral("OcrControl"));
        OcrControl->resize(1051, 877);
        verticalLayout_22 = new QVBoxLayout(OcrControl);
        verticalLayout_22->setSpacing(6);
        verticalLayout_22->setContentsMargins(11, 11, 11, 11);
        verticalLayout_22->setObjectName(QStringLiteral("verticalLayout_22"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        frame = new QFrame(OcrControl);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        verticalLayout_20 = new QVBoxLayout(frame);
        verticalLayout_20->setSpacing(6);
        verticalLayout_20->setContentsMargins(11, 11, 11, 11);
        verticalLayout_20->setObjectName(QStringLiteral("verticalLayout_20"));
        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        verticalLayout_8 = new QVBoxLayout();
        verticalLayout_8->setSpacing(6);
        verticalLayout_8->setObjectName(QStringLiteral("verticalLayout_8"));
        groupBox_2 = new QGroupBox(frame);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        verticalLayout_18 = new QVBoxLayout(groupBox_2);
        verticalLayout_18->setSpacing(6);
        verticalLayout_18->setContentsMargins(11, 11, 11, 11);
        verticalLayout_18->setObjectName(QStringLiteral("verticalLayout_18"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label = new QLabel(groupBox_2);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout_2->addWidget(label);

        sessionLineEdit = new QLineEdit(groupBox_2);
        sessionLineEdit->setObjectName(QStringLiteral("sessionLineEdit"));
        sessionLineEdit->setStyleSheet(QStringLiteral("background-color: rgb(255, 130, 80);"));

        horizontalLayout_2->addWidget(sessionLineEdit);

        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_2->addWidget(label_2);

        raceTimeLineEdit = new QLineEdit(groupBox_2);
        raceTimeLineEdit->setObjectName(QStringLiteral("raceTimeLineEdit"));
        raceTimeLineEdit->setStyleSheet(QStringLiteral("background-color: rgb(255, 130, 80);"));

        horizontalLayout_2->addWidget(raceTimeLineEdit);

        label_20 = new QLabel(groupBox_2);
        label_20->setObjectName(QStringLiteral("label_20"));

        horizontalLayout_2->addWidget(label_20);

        CountRaceTimeLineEdit = new QLineEdit(groupBox_2);
        CountRaceTimeLineEdit->setObjectName(QStringLiteral("CountRaceTimeLineEdit"));
        CountRaceTimeLineEdit->setStyleSheet(QStringLiteral("background-color: rgb(255, 130, 80);"));

        horizontalLayout_2->addWidget(CountRaceTimeLineEdit);

        caliSessionCountDownBtn = new QPushButton(groupBox_2);
        caliSessionCountDownBtn->setObjectName(QStringLiteral("caliSessionCountDownBtn"));

        horizontalLayout_2->addWidget(caliSessionCountDownBtn);


        verticalLayout_18->addLayout(horizontalLayout_2);


        verticalLayout_8->addWidget(groupBox_2);

        groupBox_3 = new QGroupBox(frame);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        verticalLayout_12 = new QVBoxLayout(groupBox_3);
        verticalLayout_12->setSpacing(6);
        verticalLayout_12->setContentsMargins(11, 11, 11, 11);
        verticalLayout_12->setObjectName(QStringLiteral("verticalLayout_12"));
        horizontalLayout_yp = new QHBoxLayout();
        horizontalLayout_yp->setSpacing(6);
        horizontalLayout_yp->setObjectName(QStringLiteral("horizontalLayout_yp"));
        horseFrame = new QFrame(groupBox_3);
        horseFrame->setObjectName(QStringLiteral("horseFrame"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(horseFrame->sizePolicy().hasHeightForWidth());
        horseFrame->setSizePolicy(sizePolicy);
        horseFrame->setFrameShape(QFrame::StyledPanel);
        horseFrame->setFrameShadow(QFrame::Raised);
        verticalLayout_4 = new QVBoxLayout(horseFrame);
        verticalLayout_4->setSpacing(0);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        titleHBoxLayout = new QHBoxLayout();
        titleHBoxLayout->setSpacing(6);
        titleHBoxLayout->setObjectName(QStringLiteral("titleHBoxLayout"));
        label_4 = new QLabel(horseFrame);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setMaximumSize(QSize(16777215, 20));
        QFont font;
        font.setPointSize(14);
        font.setBold(true);
        font.setWeight(75);
        label_4->setFont(font);

        titleHBoxLayout->addWidget(label_4);

        label_5 = new QLabel(horseFrame);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setMaximumSize(QSize(16777215, 20));
        label_5->setFont(font);

        titleHBoxLayout->addWidget(label_5);

        label_3 = new QLabel(horseFrame);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setMaximumSize(QSize(16777215, 20));
        label_3->setFont(font);

        titleHBoxLayout->addWidget(label_3);

        label_6 = new QLabel(horseFrame);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setMaximumSize(QSize(16777215, 20));
        label_6->setFont(font);

        titleHBoxLayout->addWidget(label_6);

        titleHBoxLayout->setStretch(0, 1);
        titleHBoxLayout->setStretch(1, 2);
        titleHBoxLayout->setStretch(2, 1);
        titleHBoxLayout->setStretch(3, 1);

        verticalLayout_4->addLayout(titleHBoxLayout);

        verticalLayout_4->setStretch(0, 8);

        horizontalLayout_yp->addWidget(horseFrame);

        imageLbl = new QLabel(groupBox_3);
        imageLbl->setObjectName(QStringLiteral("imageLbl"));

        horizontalLayout_yp->addWidget(imageLbl);

        horizontalLayout_yp->setStretch(0, 1);
        horizontalLayout_yp->setStretch(1, 8);

        verticalLayout_12->addLayout(horizontalLayout_yp);


        verticalLayout_8->addWidget(groupBox_3);

        verticalLayout_8->setStretch(0, 1);
        verticalLayout_8->setStretch(1, 8);

        horizontalLayout_8->addLayout(verticalLayout_8);

        verticalLayout_19 = new QVBoxLayout();
        verticalLayout_19->setSpacing(6);
        verticalLayout_19->setObjectName(QStringLiteral("verticalLayout_19"));
        versionNumLabel = new QLabel(frame);
        versionNumLabel->setObjectName(QStringLiteral("versionNumLabel"));
        versionNumLabel->setStyleSheet(QStringLiteral("background-color: rgb(255, 130, 80);"));
        versionNumLabel->setLineWidth(4);
        versionNumLabel->setAlignment(Qt::AlignCenter);

        verticalLayout_19->addWidget(versionNumLabel);

        horizontalSpacer_2 = new QSpacerItem(318, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        verticalLayout_19->addItem(horizontalSpacer_2);

        groupBox = new QGroupBox(frame);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        horizontalLayout_13 = new QHBoxLayout(groupBox);
        horizontalLayout_13->setSpacing(6);
        horizontalLayout_13->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_13->setObjectName(QStringLiteral("horizontalLayout_13"));
        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setSpacing(6);
        horizontalLayout_12->setObjectName(QStringLiteral("horizontalLayout_12"));
        fileTableWidget = new QTableWidget(groupBox);
        if (fileTableWidget->columnCount() < 1)
            fileTableWidget->setColumnCount(1);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        fileTableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        fileTableWidget->setObjectName(QStringLiteral("fileTableWidget"));

        horizontalLayout_12->addWidget(fileTableWidget);

        verticalLayout_23 = new QVBoxLayout();
        verticalLayout_23->setSpacing(6);
        verticalLayout_23->setObjectName(QStringLiteral("verticalLayout_23"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        loadFileBtn = new QPushButton(groupBox);
        loadFileBtn->setObjectName(QStringLiteral("loadFileBtn"));

        verticalLayout_3->addWidget(loadFileBtn);

        appendFileBtn = new QPushButton(groupBox);
        appendFileBtn->setObjectName(QStringLiteral("appendFileBtn"));

        verticalLayout_3->addWidget(appendFileBtn);

        delFileBtn = new QPushButton(groupBox);
        delFileBtn->setObjectName(QStringLiteral("delFileBtn"));

        verticalLayout_3->addWidget(delFileBtn);

        delFileBtn_2 = new QPushButton(groupBox);
        delFileBtn_2->setObjectName(QStringLiteral("delFileBtn_2"));

        verticalLayout_3->addWidget(delFileBtn_2);

        videoPosLineEdit = new QLineEdit(groupBox);
        videoPosLineEdit->setObjectName(QStringLiteral("videoPosLineEdit"));
        videoPosLineEdit->setStyleSheet(QStringLiteral("background-color: rgb(170, 170, 127);"));

        verticalLayout_3->addWidget(videoPosLineEdit);


        verticalLayout_23->addLayout(verticalLayout_3);

        verticalLayout_21 = new QVBoxLayout();
        verticalLayout_21->setSpacing(6);
        verticalLayout_21->setObjectName(QStringLiteral("verticalLayout_21"));
        label_33 = new QLabel(groupBox);
        label_33->setObjectName(QStringLiteral("label_33"));

        verticalLayout_21->addWidget(label_33);

        videoProgressBar = new QProgressBar(groupBox);
        videoProgressBar->setObjectName(QStringLiteral("videoProgressBar"));
        videoProgressBar->setValue(24);

        verticalLayout_21->addWidget(videoProgressBar);

        verticalLayout_21->setStretch(1, 2);

        verticalLayout_23->addLayout(verticalLayout_21);

        pauseCaliBtn = new QPushButton(groupBox);
        pauseCaliBtn->setObjectName(QStringLiteral("pauseCaliBtn"));

        verticalLayout_23->addWidget(pauseCaliBtn);

        continueBtn = new QPushButton(groupBox);
        continueBtn->setObjectName(QStringLiteral("continueBtn"));

        verticalLayout_23->addWidget(continueBtn);

        inputUserDataBtn = new QPushButton(groupBox);
        inputUserDataBtn->setObjectName(QStringLiteral("inputUserDataBtn"));

        verticalLayout_23->addWidget(inputUserDataBtn);

        pullBackBtn = new QPushButton(groupBox);
        pullBackBtn->setObjectName(QStringLiteral("pullBackBtn"));

        verticalLayout_23->addWidget(pullBackBtn);

        advance3MinBtn = new QPushButton(groupBox);
        advance3MinBtn->setObjectName(QStringLiteral("advance3MinBtn"));

        verticalLayout_23->addWidget(advance3MinBtn);

        advance1MinBtn = new QPushButton(groupBox);
        advance1MinBtn->setObjectName(QStringLiteral("advance1MinBtn"));

        verticalLayout_23->addWidget(advance1MinBtn);

        advance30SecBtn = new QPushButton(groupBox);
        advance30SecBtn->setObjectName(QStringLiteral("advance30SecBtn"));

        verticalLayout_23->addWidget(advance30SecBtn);

        advance10SecBtn = new QPushButton(groupBox);
        advance10SecBtn->setObjectName(QStringLiteral("advance10SecBtn"));

        verticalLayout_23->addWidget(advance10SecBtn);

        verticalSpacer = new QSpacerItem(20, 238, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_23->addItem(verticalSpacer);

        verticalLayout_23->setStretch(0, 20);
        verticalLayout_23->setStretch(1, 1);
        verticalLayout_23->setStretch(10, 20);

        horizontalLayout_12->addLayout(verticalLayout_23);

        horizontalLayout_12->setStretch(0, 4);
        horizontalLayout_12->setStretch(1, 1);

        horizontalLayout_13->addLayout(horizontalLayout_12);


        verticalLayout_19->addWidget(groupBox);

        verticalLayout_19->setStretch(1, 1);
        verticalLayout_19->setStretch(2, 10);

        horizontalLayout_8->addLayout(verticalLayout_19);

        horizontalLayout_8->setStretch(0, 7);
        horizontalLayout_8->setStretch(1, 3);

        verticalLayout_20->addLayout(horizontalLayout_8);


        verticalLayout->addWidget(frame);


        horizontalLayout->addLayout(verticalLayout);

        horizontalLayout->setStretch(0, 5);

        verticalLayout_2->addLayout(horizontalLayout);

        qplFrame = new QFrame(OcrControl);
        qplFrame->setObjectName(QStringLiteral("qplFrame"));
        qplFrame->setFrameShape(QFrame::StyledPanel);
        qplFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout_10 = new QHBoxLayout(qplFrame);
        horizontalLayout_10->setSpacing(6);
        horizontalLayout_10->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_10->setObjectName(QStringLiteral("horizontalLayout_10"));
        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        horizontalSpacer = new QSpacerItem(67, 17, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_9->addItem(horizontalSpacer);

        verticalLayout_7 = new QVBoxLayout();
        verticalLayout_7->setSpacing(6);
        verticalLayout_7->setObjectName(QStringLiteral("verticalLayout_7"));
        label_34 = new QLabel(qplFrame);
        label_34->setObjectName(QStringLiteral("label_34"));
        label_34->setMinimumSize(QSize(0, 14));
        label_34->setMaximumSize(QSize(21, 14));

        verticalLayout_7->addWidget(label_34);

        label_35 = new QLabel(qplFrame);
        label_35->setObjectName(QStringLiteral("label_35"));
        label_35->setMinimumSize(QSize(0, 14));
        label_35->setMaximumSize(QSize(21, 14));

        verticalLayout_7->addWidget(label_35);

        label_14 = new QLabel(qplFrame);
        label_14->setObjectName(QStringLiteral("label_14"));
        label_14->setMinimumSize(QSize(0, 14));
        label_14->setMaximumSize(QSize(21, 14));

        verticalLayout_7->addWidget(label_14);

        label_15 = new QLabel(qplFrame);
        label_15->setObjectName(QStringLiteral("label_15"));
        label_15->setMinimumSize(QSize(0, 14));
        label_15->setMaximumSize(QSize(21, 14));

        verticalLayout_7->addWidget(label_15);

        label_16 = new QLabel(qplFrame);
        label_16->setObjectName(QStringLiteral("label_16"));
        label_16->setMinimumSize(QSize(0, 14));
        label_16->setMaximumSize(QSize(21, 14));

        verticalLayout_7->addWidget(label_16);

        label_17 = new QLabel(qplFrame);
        label_17->setObjectName(QStringLiteral("label_17"));
        label_17->setMinimumSize(QSize(0, 14));
        label_17->setMaximumSize(QSize(21, 14));

        verticalLayout_7->addWidget(label_17);

        label_18 = new QLabel(qplFrame);
        label_18->setObjectName(QStringLiteral("label_18"));
        label_18->setMinimumSize(QSize(0, 14));
        label_18->setMaximumSize(QSize(21, 14));

        verticalLayout_7->addWidget(label_18);

        label_19 = new QLabel(qplFrame);
        label_19->setObjectName(QStringLiteral("label_19"));
        label_19->setMinimumSize(QSize(0, 14));
        label_19->setMaximumSize(QSize(21, 14));

        verticalLayout_7->addWidget(label_19);


        horizontalLayout_9->addLayout(verticalLayout_7);

        verticalLayout_11 = new QVBoxLayout();
        verticalLayout_11->setSpacing(6);
        verticalLayout_11->setObjectName(QStringLiteral("verticalLayout_11"));
        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        label_37 = new QLabel(qplFrame);
        label_37->setObjectName(QStringLiteral("label_37"));

        horizontalLayout_7->addWidget(label_37);

        userInputLabel = new QLabel(qplFrame);
        userInputLabel->setObjectName(QStringLiteral("userInputLabel"));

        horizontalLayout_7->addWidget(userInputLabel);

        QINLabel = new QLabel(qplFrame);
        QINLabel->setObjectName(QStringLiteral("QINLabel"));
        QINLabel->setStyleSheet(QStringLiteral("background-color: rgb(144, 140, 145);"));

        horizontalLayout_7->addWidget(QINLabel);

        label_21 = new QLabel(qplFrame);
        label_21->setObjectName(QStringLiteral("label_21"));
        label_21->setMinimumSize(QSize(36, 14));
        label_21->setMaximumSize(QSize(36, 14));

        horizontalLayout_7->addWidget(label_21);

        label_22 = new QLabel(qplFrame);
        label_22->setObjectName(QStringLiteral("label_22"));
        label_22->setMinimumSize(QSize(36, 14));
        label_22->setMaximumSize(QSize(36, 14));

        horizontalLayout_7->addWidget(label_22);

        label_23 = new QLabel(qplFrame);
        label_23->setObjectName(QStringLiteral("label_23"));
        label_23->setMinimumSize(QSize(36, 14));
        label_23->setMaximumSize(QSize(36, 14));

        horizontalLayout_7->addWidget(label_23);

        label_24 = new QLabel(qplFrame);
        label_24->setObjectName(QStringLiteral("label_24"));
        label_24->setMinimumSize(QSize(36, 14));
        label_24->setMaximumSize(QSize(36, 14));

        horizontalLayout_7->addWidget(label_24);

        label_25 = new QLabel(qplFrame);
        label_25->setObjectName(QStringLiteral("label_25"));
        label_25->setMinimumSize(QSize(36, 14));
        label_25->setMaximumSize(QSize(36, 14));

        horizontalLayout_7->addWidget(label_25);

        label_26 = new QLabel(qplFrame);
        label_26->setObjectName(QStringLiteral("label_26"));
        label_26->setMinimumSize(QSize(36, 14));
        label_26->setMaximumSize(QSize(36, 14));

        horizontalLayout_7->addWidget(label_26);

        label_27 = new QLabel(qplFrame);
        label_27->setObjectName(QStringLiteral("label_27"));
        label_27->setMinimumSize(QSize(36, 14));
        label_27->setMaximumSize(QSize(36, 14));

        horizontalLayout_7->addWidget(label_27);

        label_28 = new QLabel(qplFrame);
        label_28->setObjectName(QStringLiteral("label_28"));
        label_28->setMinimumSize(QSize(36, 14));
        label_28->setMaximumSize(QSize(36, 14));

        horizontalLayout_7->addWidget(label_28);

        label_29 = new QLabel(qplFrame);
        label_29->setObjectName(QStringLiteral("label_29"));
        label_29->setMinimumSize(QSize(36, 14));
        label_29->setMaximumSize(QSize(36, 14));

        horizontalLayout_7->addWidget(label_29);

        label_30 = new QLabel(qplFrame);
        label_30->setObjectName(QStringLiteral("label_30"));
        label_30->setMinimumSize(QSize(36, 14));
        label_30->setMaximumSize(QSize(36, 14));

        horizontalLayout_7->addWidget(label_30);

        label_31 = new QLabel(qplFrame);
        label_31->setObjectName(QStringLiteral("label_31"));
        label_31->setMinimumSize(QSize(36, 14));
        label_31->setMaximumSize(QSize(36, 14));

        horizontalLayout_7->addWidget(label_31);

        label_32 = new QLabel(qplFrame);
        label_32->setObjectName(QStringLiteral("label_32"));
        label_32->setMinimumSize(QSize(36, 14));
        label_32->setMaximumSize(QSize(36, 14));

        horizontalLayout_7->addWidget(label_32);

        label_59 = new QLabel(qplFrame);
        label_59->setObjectName(QStringLiteral("label_59"));
        label_59->setMinimumSize(QSize(36, 14));
        label_59->setMaximumSize(QSize(36, 14));

        horizontalLayout_7->addWidget(label_59);


        verticalLayout_11->addLayout(horizontalLayout_7);

        qinFrame = new QFrame(qplFrame);
        qinFrame->setObjectName(QStringLiteral("qinFrame"));
        sizePolicy.setHeightForWidth(qinFrame->sizePolicy().hasHeightForWidth());
        qinFrame->setSizePolicy(sizePolicy);
        qinFrame->setMinimumSize(QSize(500, 120));
        qinFrame->setMaximumSize(QSize(500, 120));
        qinFrame->setStyleSheet(QStringLiteral(""));
        qinFrame->setFrameShape(QFrame::StyledPanel);
        qinFrame->setFrameShadow(QFrame::Raised);
        verticalLayout_5 = new QVBoxLayout(qinFrame);
        verticalLayout_5->setSpacing(0);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        verticalLayout_5->setContentsMargins(0, 0, 0, 0);

        verticalLayout_11->addWidget(qinFrame);


        horizontalLayout_9->addLayout(verticalLayout_11);

        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setSpacing(6);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        label_36 = new QLabel(qplFrame);
        label_36->setObjectName(QStringLiteral("label_36"));
        label_36->setMinimumSize(QSize(0, 14));
        label_36->setMaximumSize(QSize(21, 14));

        verticalLayout_6->addWidget(label_36);

        label_7 = new QLabel(qplFrame);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setMinimumSize(QSize(0, 14));
        label_7->setMaximumSize(QSize(21, 14));

        verticalLayout_6->addWidget(label_7);

        label_8 = new QLabel(qplFrame);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setMinimumSize(QSize(0, 14));
        label_8->setMaximumSize(QSize(21, 14));

        verticalLayout_6->addWidget(label_8);

        label_9 = new QLabel(qplFrame);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setMinimumSize(QSize(0, 14));
        label_9->setMaximumSize(QSize(21, 14));

        verticalLayout_6->addWidget(label_9);

        label_10 = new QLabel(qplFrame);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setMinimumSize(QSize(0, 14));
        label_10->setMaximumSize(QSize(21, 14));

        verticalLayout_6->addWidget(label_10);

        label_11 = new QLabel(qplFrame);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setMinimumSize(QSize(0, 14));
        label_11->setMaximumSize(QSize(21, 14));

        verticalLayout_6->addWidget(label_11);

        label_12 = new QLabel(qplFrame);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setMinimumSize(QSize(0, 14));
        label_12->setMaximumSize(QSize(21, 14));

        verticalLayout_6->addWidget(label_12);

        label_13 = new QLabel(qplFrame);
        label_13->setObjectName(QStringLiteral("label_13"));
        label_13->setMinimumSize(QSize(0, 14));
        label_13->setMaximumSize(QSize(21, 14));

        verticalLayout_6->addWidget(label_13);


        horizontalLayout_9->addLayout(verticalLayout_6);

        horizontalSpacer_3 = new QSpacerItem(44, 17, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_9->addItem(horizontalSpacer_3);

        verticalLayout_13 = new QVBoxLayout();
        verticalLayout_13->setSpacing(6);
        verticalLayout_13->setObjectName(QStringLiteral("verticalLayout_13"));
        imageFileNameLabel = new QLabel(qplFrame);
        imageFileNameLabel->setObjectName(QStringLiteral("imageFileNameLabel"));
        imageFileNameLabel->setStyleSheet(QStringLiteral("background-color: rgb(255, 130, 80);"));
        imageFileNameLabel->setAlignment(Qt::AlignCenter);

        verticalLayout_13->addWidget(imageFileNameLabel);

        adTimeLbl = new QLabel(qplFrame);
        adTimeLbl->setObjectName(QStringLiteral("adTimeLbl"));
        adTimeLbl->setStyleSheet(QStringLiteral("background-color: rgb(255, 130, 80);"));
        adTimeLbl->setAlignment(Qt::AlignCenter);

        verticalLayout_13->addWidget(adTimeLbl);


        horizontalLayout_9->addLayout(verticalLayout_13);

        horizontalLayout_9->setStretch(0, 2);
        horizontalLayout_9->setStretch(1, 1);
        horizontalLayout_9->setStretch(2, 5);
        horizontalLayout_9->setStretch(3, 5);

        horizontalLayout_10->addLayout(horizontalLayout_9);


        verticalLayout_2->addWidget(qplFrame);

        frame_4 = new QFrame(OcrControl);
        frame_4->setObjectName(QStringLiteral("frame_4"));
        frame_4->setStyleSheet(QStringLiteral(""));
        frame_4->setFrameShape(QFrame::StyledPanel);
        frame_4->setFrameShadow(QFrame::Raised);
        verticalLayout_17 = new QVBoxLayout(frame_4);
        verticalLayout_17->setSpacing(6);
        verticalLayout_17->setContentsMargins(11, 11, 11, 11);
        verticalLayout_17->setObjectName(QStringLiteral("verticalLayout_17"));
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        verticalLayout_16 = new QVBoxLayout();
        verticalLayout_16->setSpacing(6);
        verticalLayout_16->setObjectName(QStringLiteral("verticalLayout_16"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        connectBtn = new QPushButton(frame_4);
        connectBtn->setObjectName(QStringLiteral("connectBtn"));

        horizontalLayout_3->addWidget(connectBtn);

        loginBtn = new QPushButton(frame_4);
        loginBtn->setObjectName(QStringLiteral("loginBtn"));

        horizontalLayout_3->addWidget(loginBtn);


        verticalLayout_16->addLayout(horizontalLayout_3);

        reconnectCheckBox = new QCheckBox(frame_4);
        reconnectCheckBox->setObjectName(QStringLiteral("reconnectCheckBox"));

        verticalLayout_16->addWidget(reconnectCheckBox);

        disconnectBtn = new QPushButton(frame_4);
        disconnectBtn->setObjectName(QStringLiteral("disconnectBtn"));

        verticalLayout_16->addWidget(disconnectBtn);


        horizontalLayout_5->addLayout(verticalLayout_16);

        verticalLayout_14 = new QVBoxLayout();
        verticalLayout_14->setSpacing(6);
        verticalLayout_14->setObjectName(QStringLiteral("verticalLayout_14"));
        submitRaceTimeBtn = new QPushButton(frame_4);
        submitRaceTimeBtn->setObjectName(QStringLiteral("submitRaceTimeBtn"));

        verticalLayout_14->addWidget(submitRaceTimeBtn);

        submitRealBtn = new QPushButton(frame_4);
        submitRealBtn->setObjectName(QStringLiteral("submitRealBtn"));

        verticalLayout_14->addWidget(submitRealBtn);


        horizontalLayout_5->addLayout(verticalLayout_14);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        verticalLayout_15 = new QVBoxLayout();
        verticalLayout_15->setSpacing(6);
        verticalLayout_15->setObjectName(QStringLiteral("verticalLayout_15"));
        verticalLayout_10 = new QVBoxLayout();
        verticalLayout_10->setSpacing(6);
        verticalLayout_10->setObjectName(QStringLiteral("verticalLayout_10"));
        is63TAICheckBox = new QCheckBox(frame_4);
        is63TAICheckBox->setObjectName(QStringLiteral("is63TAICheckBox"));

        verticalLayout_10->addWidget(is63TAICheckBox);

        requestHorseInfoBtn = new QPushButton(frame_4);
        requestHorseInfoBtn->setObjectName(QStringLiteral("requestHorseInfoBtn"));

        verticalLayout_10->addWidget(requestHorseInfoBtn);

        requestRaceIdBtn = new QPushButton(frame_4);
        requestRaceIdBtn->setObjectName(QStringLiteral("requestRaceIdBtn"));

        verticalLayout_10->addWidget(requestRaceIdBtn);


        verticalLayout_15->addLayout(verticalLayout_10);


        horizontalLayout_4->addLayout(verticalLayout_15);

        verticalLayout_9 = new QVBoxLayout();
        verticalLayout_9->setSpacing(6);
        verticalLayout_9->setObjectName(QStringLiteral("verticalLayout_9"));
        videoTypeComboBox = new QComboBox(frame_4);
        videoTypeComboBox->setObjectName(QStringLiteral("videoTypeComboBox"));

        verticalLayout_9->addWidget(videoTypeComboBox);

        startAcqBtn = new QPushButton(frame_4);
        startAcqBtn->setObjectName(QStringLiteral("startAcqBtn"));

        verticalLayout_9->addWidget(startAcqBtn);

        stopAcqBtn = new QPushButton(frame_4);
        stopAcqBtn->setObjectName(QStringLiteral("stopAcqBtn"));

        verticalLayout_9->addWidget(stopAcqBtn);


        horizontalLayout_4->addLayout(verticalLayout_9);


        horizontalLayout_5->addLayout(horizontalLayout_4);


        horizontalLayout_6->addLayout(horizontalLayout_5);

        textBrowser = new QTextBrowser(frame_4);
        textBrowser->setObjectName(QStringLiteral("textBrowser"));

        horizontalLayout_6->addWidget(textBrowser);

        horizontalLayout_6->setStretch(0, 5);
        horizontalLayout_6->setStretch(1, 3);

        verticalLayout_17->addLayout(horizontalLayout_6);


        verticalLayout_2->addWidget(frame_4);

        verticalLayout_2->setStretch(0, 16);
        verticalLayout_2->setStretch(1, 5);
        verticalLayout_2->setStretch(2, 1);

        verticalLayout_22->addLayout(verticalLayout_2);


        retranslateUi(OcrControl);

        QMetaObject::connectSlotsByName(OcrControl);
    } // setupUi

    void retranslateUi(QWidget *OcrControl)
    {
        OcrControl->setWindowTitle(QApplication::translate("OcrControl", "OcrControl", 0));
        groupBox_2->setTitle(QString());
        label->setText(QApplication::translate("OcrControl", "\345\234\272\346\254\241\345\217\267\357\274\232", 0));
        label_2->setText(QApplication::translate("OcrControl", "\345\200\222\350\256\241\346\227\266", 0));
        label_20->setText(QApplication::translate("OcrControl", "\351\241\272\350\256\241\346\227\266", 0));
        caliSessionCountDownBtn->setText(QApplication::translate("OcrControl", "\346\240\241\346\255\243", 0));
        groupBox_3->setTitle(QString());
        label_4->setText(QApplication::translate("OcrControl", "\345\272\217\345\217\267", 0));
        label_5->setText(QApplication::translate("OcrControl", "\351\251\254\345\220\215", 0));
        label_3->setText(QApplication::translate("OcrControl", "WIN", 0));
        label_6->setText(QApplication::translate("OcrControl", "PLA", 0));
        imageLbl->setText(QApplication::translate("OcrControl", "\344\270\255\345\233\275\344\272\272", 0));
        versionNumLabel->setText(QString());
        groupBox->setTitle(QString());
        QTableWidgetItem *___qtablewidgetitem = fileTableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("OcrControl", "\346\226\207\344\273\266\345\220\215", 0));
        loadFileBtn->setText(QApplication::translate("OcrControl", "\350\275\275\345\205\245\345\216\206\345\217\262\350\247\206\351\242\221", 0));
        appendFileBtn->setText(QApplication::translate("OcrControl", "\350\277\275\345\212\240\346\226\207\344\273\266", 0));
        delFileBtn->setText(QApplication::translate("OcrControl", "\345\210\240\351\231\244\345\244\264\346\226\207\344\273\266", 0));
        delFileBtn_2->setText(QApplication::translate("OcrControl", "\350\247\206\351\242\221\350\265\267\345\247\213\345\270\247\346\225\260", 0));
        label_33->setText(QApplication::translate("OcrControl", "\350\247\206\351\242\221\345\244\204\347\220\206\350\277\233\345\272\246", 0));
        pauseCaliBtn->setText(QApplication::translate("OcrControl", "\346\232\202\345\201\234", 0));
        continueBtn->setText(QApplication::translate("OcrControl", "\347\273\247\347\273\255\350\257\206\345\210\253", 0));
        inputUserDataBtn->setText(QApplication::translate("OcrControl", "\350\276\223\345\205\245", 0));
        pullBackBtn->setText(QApplication::translate("OcrControl", "\350\277\224\345\233\236\345\277\253\350\277\233\345\211\215", 0));
        advance3MinBtn->setText(QApplication::translate("OcrControl", "\345\277\253\350\277\2333\345\210\206\351\222\237", 0));
        advance1MinBtn->setText(QApplication::translate("OcrControl", "\345\277\253\350\277\2331\345\210\206\351\222\237", 0));
        advance30SecBtn->setText(QApplication::translate("OcrControl", "\345\277\253\350\277\23330\347\247\222", 0));
        advance10SecBtn->setText(QApplication::translate("OcrControl", "\345\277\253\350\277\23310\347\247\222", 0));
        label_34->setText(QString());
        label_35->setText(QString());
        label_14->setText(QApplication::translate("OcrControl", "9", 0));
        label_15->setText(QApplication::translate("OcrControl", "10", 0));
        label_16->setText(QApplication::translate("OcrControl", "11", 0));
        label_17->setText(QApplication::translate("OcrControl", "12", 0));
        label_18->setText(QApplication::translate("OcrControl", "13", 0));
        label_19->setText(QApplication::translate("OcrControl", "14", 0));
        label_37->setText(QString());
        userInputLabel->setText(QApplication::translate("OcrControl", "Label", 0));
        QINLabel->setText(QApplication::translate("OcrControl", " Q   I   N ", 0));
        label_21->setText(QApplication::translate("OcrControl", "2", 0));
        label_22->setText(QApplication::translate("OcrControl", "3", 0));
        label_23->setText(QApplication::translate("OcrControl", "4", 0));
        label_24->setText(QApplication::translate("OcrControl", "5", 0));
        label_25->setText(QApplication::translate("OcrControl", "6", 0));
        label_26->setText(QApplication::translate("OcrControl", "7", 0));
        label_27->setText(QApplication::translate("OcrControl", "8", 0));
        label_28->setText(QApplication::translate("OcrControl", "9", 0));
        label_29->setText(QApplication::translate("OcrControl", "10", 0));
        label_30->setText(QApplication::translate("OcrControl", "11", 0));
        label_31->setText(QApplication::translate("OcrControl", "12", 0));
        label_32->setText(QApplication::translate("OcrControl", "13", 0));
        label_59->setText(QApplication::translate("OcrControl", "14", 0));
        label_36->setText(QString());
        label_7->setText(QApplication::translate("OcrControl", "1", 0));
        label_8->setText(QApplication::translate("OcrControl", "2", 0));
        label_9->setText(QApplication::translate("OcrControl", "3", 0));
        label_10->setText(QApplication::translate("OcrControl", "4", 0));
        label_11->setText(QApplication::translate("OcrControl", "5", 0));
        label_12->setText(QApplication::translate("OcrControl", "6", 0));
        label_13->setText(QApplication::translate("OcrControl", "7", 0));
        imageFileNameLabel->setText(QApplication::translate("OcrControl", "imageName", 0));
        adTimeLbl->setText(QApplication::translate("OcrControl", "ADTime", 0));
        connectBtn->setText(QApplication::translate("OcrControl", "\350\277\236\346\216\245", 0));
        loginBtn->setText(QApplication::translate("OcrControl", "\347\231\273\351\231\206", 0));
        reconnectCheckBox->setText(QApplication::translate("OcrControl", "\350\207\252\345\212\250\351\207\215\350\277\236", 0));
        disconnectBtn->setText(QApplication::translate("OcrControl", "\346\226\255\345\274\200", 0));
        submitRaceTimeBtn->setText(QApplication::translate("OcrControl", "\346\217\220\344\272\244\346\257\224\350\265\233\346\227\266\351\225\277", 0));
        submitRealBtn->setText(QApplication::translate("OcrControl", "\346\217\220\344\272\244\345\256\236\346\227\266\346\225\260\346\215\256", 0));
        is63TAICheckBox->setText(QApplication::translate("OcrControl", "63\345\217\260", 0));
        requestHorseInfoBtn->setText(QApplication::translate("OcrControl", "\350\257\267\346\261\202\351\251\254\344\277\241\346\201\257", 0));
        requestRaceIdBtn->setText(QApplication::translate("OcrControl", "\350\257\267\346\261\202RaceId", 0));
        videoTypeComboBox->clear();
        videoTypeComboBox->insertItems(0, QStringList()
         << QApplication::translate("OcrControl", "\351\246\231\346\270\25718\345\217\260D14", 0)
         << QApplication::translate("OcrControl", "\351\246\231\346\270\25718\345\217\260", 0)
         << QApplication::translate("OcrControl", "\344\272\232\346\264\262\345\217\260", 0)
         << QApplication::translate("OcrControl", "\347\233\264\346\222\255", 0)
        );
        startAcqBtn->setText(QApplication::translate("OcrControl", "\345\274\200\345\247\213\345\256\236\346\227\266\351\207\207\351\233\206", 0));
        stopAcqBtn->setText(QApplication::translate("OcrControl", "\345\201\234\346\255\242\345\256\236\346\227\266\351\207\207\351\233\206", 0));
    } // retranslateUi

};

namespace Ui {
    class OcrControl: public Ui_OcrControl {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OCRCONTROL_H
