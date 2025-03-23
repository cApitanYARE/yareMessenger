#include "signin.h"
#include "login.h"
#include "ui_signin.h"

SignIn::SignIn(QWidget *parent, boost::asio::io_context& context)
    : QWidget(parent),
    socket(std::make_shared<boost::asio::ip::tcp::socket>(context)),
    timer(context),
    io(context),
    ui(new Ui::SignIn)
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
    enter_signIn->setPlaceholderText("enter name, first write @");

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
    connect(changeField, &QPushButton::clicked, this, [this,&context] {
        this->close();
        LogIn *logInWindow = new LogIn(context);
        logInWindow->setWindowFlag(Qt::FramelessWindowHint);
        logInWindow->show();
    });

    timer.expires_after(boost::asio::chrono::seconds(3));
    timer.async_wait([this](const boost::system::error_code& ec) {
        if (!ec) {
            if(!socket->is_open()){
            qDebug() << "Connection timed out!";
            }
            socket->close();
        }
    });
    socket->async_connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::make_address("127.0.0.1"), 12345),[this](const boost::system::error_code& ec){
        if (!ec) {
            qDebug() << "Connected to server!\n";
            timer.cancel();
            ServerResponse();
        } else {
            qDebug() << "Connection failed: " << ec.message() << "\n";
        }
    });


    connect(sendData, &QPushButton::clicked, this, [this] {
        QString username = enter_signIn->text();
        if(!username.contains('@')){
             playSystemSound("Speech Misrecognition.wav");
            info->setText("User Name mast contains '@'");
            return;
        }
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
        jsonData = doc.toJson(QJsonDocument::Compact).toStdString();

        if (socket->is_open()) {
            boost::asio::async_write(
                *socket,
                boost::asio::buffer(jsonData),
                [&](const boost::system::error_code& ec, std::size_t bytes_transferred){
                    if (!ec) {
                        qDebug() << "Sent " << bytes_transferred << " bytes to server\n";
                        qDebug() << "Request sent successfully.";
                    } else {
                        qDebug() << "Write error: " << ec.message() << '\n';
                        socket->close();
                    }
             });
        }else{
            qDebug() << "Socket is not open. Cannot send data.";
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

void SignIn::ServerResponse(){
    try {
        if (socket && socket->is_open()) {
            boost::asio::ip::tcp::endpoint remote_ep = socket->remote_endpoint();
            qDebug() << "Connected to: " << remote_ep.address().to_string() << ":" << remote_ep.port();
        }else{
            qDebug() << "Connected no";
            return;
        }
    } catch (const std::exception& e) {
        qDebug() << "Exception: " << e.what();
        return;
    }

    boost::asio::async_read_until(
    *socket,
    bufferFromServer,
    '\n',
    [this](const boost::system::error_code& ec, std::size_t bytes_transferred){
               if(!ec){
                std::string jsonDataServer((std::istreambuf_iterator<char>(&bufferFromServer)), std::istreambuf_iterator<char>());
                bufferFromServer.consume(bytes_transferred);
                jsonDataServer.erase(std::remove(jsonDataServer.begin(), jsonDataServer.end(), '\x00'), jsonDataServer.end());

                qDebug() << "Сирі отримані дані:" << QByteArray::fromStdString(jsonDataServer);

                QByteArray jsonData = QByteArray::fromStdString(jsonDataServer);
                QJsonDocument doc = QJsonDocument::fromJson(jsonData);

                if (doc.isNull()) {
                    qDebug() << "Invalid JSON received!";
                    return;
                }
                QJsonObject json = doc.object();

            QMetaObject::invokeMethod(this, [this, json]() {
                if (json["status"].toString() == "success") {
                    info->setStyleSheet("font-weight:bold;"
                                        "font-size:15px;"
                                        "background-color:grey;"
                                        "color:white;"
                                        );
                    playSystemSound("Speech On.wav");
                    info->setText("Success");

                    this->close();
                    messengerWin *MainWindow = new messengerWin(enter_signIn->text(),io,socket);
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
            }, Qt::QueuedConnection);
            }else {
                   if (ec == boost::asio::error::eof) {
                       qDebug() << "Server closed connection.";
                   }
                qDebug() << "Error reading data: " << ec.message();
                   qDebug() <<ec.what();
                return;
            }
        }
    );
}

void SignIn::playSystemSound(const QString& soundName) {
    effect.setSource(QUrl::fromLocalFile("C:/Windows/Media/" + soundName));
    effect.setVolume(0.8);
    effect.play();
}


SignIn::~SignIn()
{
    qDebug() << "SignIn destructor called";
    socket->close();

    delete ui;
}
