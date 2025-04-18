#include "login.h"

#include "ui_login.h"

#include <QDebug>


QString NameUser;


LogIn::LogIn(boost::asio::io_context& context,QWidget *parent)
    : QWidget(parent),
    socket(std::make_shared<boost::asio::ip::tcp::socket>(context)),
    timer(context),
    io(context)
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
                            "margin:20px 10px;"
                            "}"
                            "#EnterLine:focus{"
                            "background-color:#e3e3e3;"
                            "color:red;"
                            "}"
                            ///////////////////////////////////////
                            "#EnterPass{"
                            "background-color:#cacaca;"
                            "min-height:25px;"
                            "max-height:25px;"
                            "font-size:18px;"
                            "color:#5d5b5a;"
                            "border-radius:1px;"
                            "margin:0px 10px;"
                            "}"
                            "#EnterPass:focus{"
                            "background-color:#e3e3e3;"
                            "color:red;"
                            "}"
                            ///////////////////////////////////////
                            "#SendData{"
                            "background-color:#929292;"
                            "color:white;"
                            "min-width:100px;"
                            "padding:5px;"
                            "border: 1px solid #5d5b5a;"
                            "border-radius:3px;"
                            "margin-top:10px;"
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
                            "background-color:#ff8f8f;"
                            "color:#525252;"
                            "margin:0;"
                            "padding:0;"
                            "min-width:300px;"
                            "min-height:20px;"
                            "border:none;"
                            "font-weight:bold;"
                            "letter-spacing:5px;"
                            "}"
                            "#btnClose:hover{"
                            "background-color:#ff4747;"
                            "color:white;"
                            "}"
                            );
    }

    {
    login = new QLabel("Log In");
    login->setObjectName("Title");
    login->setAlignment(Qt::AlignHCenter);

    enter_log = new QLineEdit();
    enter_log->setObjectName("EnterLine");
    enter_log->setPlaceholderText("enter name, first write @");

    //password->setObjectName("Title");
    //password->setAlignment(Qt::AlignHCenter);

    enter_pass = new QLineEdit();
    enter_pass->setObjectName("EnterPass");
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
    //Vlayout->addWidget(password);
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
        std::exit(0);
    });
    connect(changeField, &QPushButton::clicked, this, [this,&context] {
        this->close();

        SignIn* sigUpWindow = new SignIn(nullptr,context);
        sigUpWindow->setWindowFlag(Qt::FramelessWindowHint);
        sigUpWindow->show();
    });

    connectToServer();

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
        if(!username.contains('@')){
             playSystemSound("Speech Misrecognition.wav");
            info->setText("User Name mast contains '@'");
            return;
        }
        QString password = enter_pass->text();
        QByteArray hashedPassword = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex();

        QJsonObject json;
        json["action"] = "login";
        json["username"] = username;
        json["password"] = QString(hashedPassword);

        QJsonDocument doc(json);
        jsonData = doc.toJson(QJsonDocument::Compact).toStdString();

        if (!isConnected) {
            connectToServer();
            QTimer::singleShot(500, this, [this] {
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
                                isConnected = false;
                            }
                        });
                }else{
                    qDebug() << "Socket is not open. Cannot send data.";
                }
            });
        }else{
            if (socket->is_open() && isConnected) {
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
                            isConnected = false;
                        }
                    });
            }else{
                qDebug() << "Socket is not open. Cannot send data.";
            }
        }

    });
}

void LogIn::ServerResponse(){
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
                    NameUser = enter_log->text();
                    playSystemSound("Speech On.wav");
                    info->setText("Success");


                    this->close();
                    messengerWin *MainWindow = new messengerWin(NameUser,io,socket);
                    MainWindow->show();

                } else {
                    info->setStyleSheet("font-weight:bold;"
                                        "font-size:15px;"
                                        "background-color:grey;"
                                        "color:white;"
                                        );
                    playSystemSound("Windows Ding.wav");
                    info->setText("Login or Password area incorrect");
                    connectToServer();
                }
        }, Qt::QueuedConnection);
            }else {
                qDebug() << "Error reading data: " << ec.message();
                return;
            }
        }
        );
}

void LogIn::connectToServer(){
    isConnected = false;

    timer.expires_after(boost::asio::chrono::seconds(3));
    timer.async_wait([this](const boost::system::error_code& ec) {
        if (!ec && !isConnected) {
            qDebug() << "Connection timed out!";
            socket->close();
        }
    });

    socket->async_connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::make_address("127.0.0.1"), 12345),[this](const boost::system::error_code& ec){
        if (!ec) {
            qDebug() << "Connected to server!\n";
            isConnected = true;
            timer.cancel();
            ServerResponse();
        } else {
            qDebug() << "Connection failed: " << ec.message() << "\n";
        }
    });
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
