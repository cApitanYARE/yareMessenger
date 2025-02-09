#include "signin.h"
#include "login.h"
#include "ui_signin.h"

SignIn::SignIn(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SignIn)
{
    ui->setupUi(this);



    this->setStyleSheet("background-color:#dcdcdc;");
    this->setFixedSize(300,345);

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
                            "margin-top:10px;"
                            "}"
                            //////////////////////////////////////
                            "#EnterLine{"
                            "background-color:#cacaca;"
                            "min-height:25px;"
                            "max-height:25px;"
                            "font-size:18px;"
                            "color:#5d5b5a;"
                            "border-radius:1px;"
                            "margin:10px 10px;"
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
    //init and layout
    {
    signIn = new QLabel("Sign up");
    signIn->setObjectName("Title");
    signIn->setAlignment(Qt::AlignHCenter);

    enter_signIn = new QLineEdit();
    enter_signIn->setObjectName("EnterLine");
    enter_signIn->setPlaceholderText("enter name");

    password = new QLabel("Password");
    password->setObjectName("Title");
    password->setAlignment(Qt::AlignHCenter);

    enter_pass = new QLineEdit();
    enter_pass->setObjectName("EnterLine");
    enter_pass->setPlaceholderText("enter password");

    confirm_pass = new QLineEdit();
    confirm_pass->setObjectName("EnterLine");
    confirm_pass->setPlaceholderText("confirm password");


    info = new QLabel();
    info->setAlignment(Qt::AlignCenter);

    sendData = new QPushButton("Confirm");
    sendData->setObjectName("SendData");
    sendData->setCursor(Qt::PointingHandCursor);


    changeField = new QPushButton("Log In");
    changeField->setObjectName("changeField");
    changeField->setCursor(Qt::PointingHandCursor);


    Vlayout->addWidget(closeWindow);
    Vlayout->addWidget(signIn);
    Vlayout->addWidget(enter_signIn);
    Vlayout->addWidget(password);
    Vlayout->addWidget(enter_pass);
    Vlayout->addWidget(confirm_pass);
    Vlayout->addWidget(info);
    Vlayout->addWidget(sendData);
    Vlayout->addWidget(changeField);
    Vlayout->setSpacing(1);
    Vlayout->setAlignment(Qt::AlignTop);
    Vlayout->setAlignment(sendData, Qt::AlignHCenter);
    Vlayout->setAlignment(changeField, Qt::AlignBottom | Qt::AlignRight);

    }

    connect(closeWindow, &QPushButton::clicked, this, [this] {
        this->close();
    });
    connect(changeField, &QPushButton::clicked, this, [this] {
        this->close();
        LogIn *logInWindow = new LogIn();
        logInWindow->setWindowFlag(Qt::FramelessWindowHint);
        logInWindow->show();
    });

    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &SignIn::onServerResponse);

    connect(sendData, &QPushButton::clicked, this, [this] {
        QString username = '@' + enter_signIn->text();
        QString password = enter_pass->text();
        QString confirmPassword = confirm_pass->text();

        if (password != confirmPassword) {
            info->setStyleSheet("font-weight:bold;"
                                "font-size:15px;"
                                "background-color:grey;"
                                "color:white;"
                                );
            playSystemSound("Speech Misrecognition.wav");
            info->setText("Paswords area isn't same");
            return;
        }

        QByteArray hashedPassword = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex();

        QJsonObject json;
        json["action"] = "register";
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

    connect(enter_pass, &QLineEdit::textChanged,this,[this]{
        QString audit = enter_pass->text();
        if(audit.size() < 8){
            info->setStyleSheet("font-weight:bold;"
                                "font-size:15px;"
                                "background-color:grey;"
                                "color:white;"
                                );
            info->setText("Minimum password length - 8 characters");
        }else{
            info->setStyleSheet("background-color#dcdcdc;"
                                );
            info->setText("");
        }
    });
}

void SignIn::onServerResponse() {
    QByteArray responseData = socket->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    QJsonObject json = doc.object();

    if (json["status"].toString() == "success") {
        info->setStyleSheet("font-weight:bold;"
                            "font-size:15px;"
                            "background-color:grey;"
                            "color:white;"
                            );
        playSystemSound("Speech On.wav");
        info->setText("Success");

        this->close();
        messengerWin *MainWindow = new messengerWin(enter_signIn->text());
        MainWindow->show();

    }else if(json["status"].toString() == "UserExist"){
        info->setStyleSheet("font-weight:bold;"
                            "font-size:15px;"
                            "background-color:grey;"
                            "color:white;"
                            );
        playSystemSound("Windows Ding.wav");
        info->setText("A user with that name already exists.");
    }
    else if(json["status"].toString() == "error"){
        info->setStyleSheet("font-weight:bold;"
                           "font-size:15px;"
                           "background-color:grey;"
                           "color:white;"
                           );
        playSystemSound("Windows Ding.wav");
        info->setText("Login or Password area incorrect");
    }
}

void SignIn::playSystemSound(const QString& soundName) {
    effect.setSource(QUrl::fromLocalFile("C:/Windows/Media/" + soundName));
    effect.setVolume(0.8);
    effect.play();
}


SignIn::~SignIn()
{
    qDebug() << "SignIn destructor called";
    delete ui;
}
