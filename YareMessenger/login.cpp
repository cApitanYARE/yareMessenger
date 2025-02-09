#include "login.h"

#include "ui_login.h"

#include <QDebug>

QString NameUser;

LogIn::LogIn(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LogIn)
{
    ui->setupUi(this);
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setStyleSheet("background-color:#dcdcdc;");
    this->setFixedSize(300,290);

    Vlayout = new QVBoxLayout(this);
    Vlayout->setContentsMargins(0, 0, 0, 0);

    closeWindow = new QPushButton("CLOSE");
    closeWindow->setObjectName("btnClose");

    closeWindow->setCursor(Qt::PointingHandCursor);

    //styleSheet
    {
        this->setStyleSheet("#Title{"
                            "font-size:25px;"
                            "color:#5d5b5a;"
                            "border-radius:1px;"
                            "}"
                            //////////////////////////////////////
                            "#EnterLine{"
                            "background-color:#cacaca;"
                            "min-height:25px;"
                            "max-height:25px;"
                            "font-size:18px;"
                            "color:#5d5b5a;"
                            "border-radius:1px;"
                            "margin:0px 10px;"
                            "}"
                            ///////////////////////////////////////
                            "#SendData{"
                            "background-color:#929292;"
                            "color:white;"
                            "min-width:100px;"
                            "padding:5px;"
                            "border: 1px solid #5d5b5a;"
                            "border-radius:3px;"
                            "margin-top:20px;"
                            "}"

                            "#SendData:hover{"
                            "background-color:#5d5b5a;"
                            "}"
                            ///////////////////////////////////////
                            "#changeField{"
                            "background-color:#747474;"
                            "color:white;"
                            "font-weight:bold;"
                            "min-width:50px;"
                            "min-height:25px;"
                            "border: 1px solid #77a7ff;"
                            "border-radius:6px;"
                            "margin:0px 10px;"
                            "}"
                            ///////////////////////////////////////
                            "#btnClose{"
                            "background-color:#ff6b6b;"
                            "color:white;"
                            "margin:0;"
                            "padding:0;"
                            "min-width:300px;"
                            "min-height:20px;"
                            "border: none;"
                            "font-weight:bold;"
                            "letter-spacing:5px;"
                            "}"
                            "#btnClose:hover{"
                            "background-color:#ff4747;"
                            "}"
                            );
    }

    {
    login = new QLabel("Log In");
    login->setObjectName("Title");
    login->setAlignment(Qt::AlignHCenter);

    enter_log = new QLineEdit();
    enter_log->setObjectName("EnterLine");
    enter_log->setPlaceholderText("enter name");

    password = new QLabel("Password");
    password->setObjectName("Title");
    password->setAlignment(Qt::AlignHCenter);

    enter_pass = new QLineEdit();
    enter_pass->setObjectName("EnterLine");
    enter_pass->setPlaceholderText("enter password");

    sendData = new QPushButton("Confirm");
    sendData->setObjectName("SendData");
    sendData->setCursor(Qt::PointingHandCursor);


    changeField = new QPushButton("Sign Up");
    changeField->setObjectName("changeField");
    changeField->setCursor(Qt::PointingHandCursor);


    info = new QLabel();
    info->setAlignment(Qt::AlignCenter);



    Vlayout->addWidget(closeWindow);
    Vlayout->addWidget(login);
    Vlayout->addWidget(enter_log);
    Vlayout->addWidget(password);
    Vlayout->addWidget(enter_pass);
    Vlayout->addWidget(info);
    Vlayout->addWidget(sendData);
    Vlayout->addWidget(changeField);
    Vlayout->setSpacing(8);
    Vlayout->setAlignment(Qt::AlignTop);
    Vlayout->setAlignment(sendData, Qt::AlignHCenter);
    Vlayout->setAlignment(changeField, Qt::AlignBottom | Qt::AlignRight);
    }
    connect(closeWindow, &QPushButton::clicked, this, [this] {
        this->close();
    });
    connect(changeField, &QPushButton::clicked, this, [this] {
        this->close();
        SignIn* sigUpWindow = new SignIn();
        sigUpWindow->setWindowFlag(Qt::FramelessWindowHint);
        sigUpWindow->show();
    });

    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &LogIn::onServerResponse);

    connect(sendData,&QPushButton::clicked,this, [this]{

        while(enter_log->text() == "" || enter_pass->text() == ""){
            info->setStyleSheet("font-weight:bold;"
                                "font-size:15px;"
                                "background-color:grey;"
                                "color:white;"
                                );
            playSystemSound("Speech Misrecognition.wav");

            info->setText("Login or Password area can't be empty");
            return;
        }

        QString username = enter_log->text();
        QString password = enter_pass->text();
        QByteArray hashedPassword = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex();

        QJsonObject json;
        json["action"] = "login";
        json["username"] = username;
        json["password"] = QString(hashedPassword);

        QJsonDocument doc(json);
        socket->connectToHost("127.0.0.1", 12345);
        if (socket->waitForConnected(1000)) {
            socket->write(doc.toJson());
        } else {
            QMessageBox::critical(this, "Error", "Cannot connect to server!");
        }
    });
}

void LogIn::onServerResponse() {
    QByteArray responseData = socket->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    QJsonObject json = doc.object();

    if (json["status"].toString() == "success") {
        info->setStyleSheet("font-weight:bold;"
                            "font-size:15px;"
                            "background-color:grey;"
                            "color:white;"
                            );
        NameUser = enter_log->text();
        playSystemSound("Speech On.wav");
        info->setText("Success");


        this->close();
        messengerWin *MainWindow = new messengerWin(NameUser);
        MainWindow->show();

    } else {
        info->setStyleSheet("font-weight:bold;"
                            "font-size:15px;"
                            "background-color:grey;"
                            "color:white;"
                            );
        playSystemSound("Windows Ding.wav");
        info->setText("Login or Password area incorrect");
    }
}

void LogIn::playSystemSound(const QString& soundName) {
    effect.setSource(QUrl::fromLocalFile("C:/Windows/Media/" + soundName));
    effect.setVolume(0.8);
    effect.play();
}

LogIn::~LogIn()
{
    delete ui;
}
