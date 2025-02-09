#include "messengerwin.h"
#include "./ui_messengerwin.h"

#include "login.h"

QString searchUser;

QString sendFrom;
QString sendTo;
QString message;
QString photo;

QSet<QString> addedUsers;
QString current_yaerDayMount;


QSet<QString> allChats;

QSet<QString> addedDates;


QString  insert_from;
QString  insert_to;
QString  insert_message;
QVector<int>  insert_date;

QString lastMessageText;

messengerWin::messengerWin(const QString &username,QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::messengerWin)
    , own(username)
{
    ui->setupUi(this);
    {
        this->setStyleSheet("background-color:#ccdfda;");
        this->setFixedSize(850,525);


        centralWidget = new QWidget(this);
        centralWidget->setFixedSize(850,525);
        centralWidget->setStyleSheet("padding:0;"
                                     "margin:0;");

        mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->setSpacing(0);
        mainLayout->setContentsMargins(0,0,0,0);

        Hlayout = new QHBoxLayout();
        layoutUpperWidget = new QVBoxLayout();

        logOutLabelLayout = new QHBoxLayout();
        logOutLabelLayout->setSpacing(0);
        logOutLabelLayout->setContentsMargins(0,0,0,0);

        upperLayoutLabel = new QLabel(own,this);
        upperLayoutLabel->setStyleSheet("padding:5px;"
                                    "background-color:#809f96;"
                                     "color:white;"
                                    "font-weight: bold;"
                                      "font-size:15px;");
        logOutLabelLayout->addWidget(upperLayoutLabel);

        logOut = new QPushButton("Log out");
        logOut->setCursor(Qt::PointingHandCursor);
        logOut->setObjectName("logOut");
        logOut->setStyleSheet("#logOut{"
                                "max-width:60px;"
                                "min-height:25px;"
                                "background-color:#375249;"
                                "color:white;"
                                "}"
                              "#logOut:hover{"
                                "background-color:#809f96;"
                                "}");
        logOutLabelLayout->addWidget(logOut);

        layoutUpperWidget->addLayout(logOutLabelLayout);



        layoutSearch = new QVBoxLayout(this);
        search = new QLineEdit(this);
        search->setStyleSheet("min-width:200px;"
                              "min-height:25px;"
                              "font-size:15px;"
                              "padding:0;"
                              "margin:0;"
                              "color: #375249;"
                              "background-color:#d7e6e1;"
                              "border-radius:1px;"
                              "border:1px solid white;");
        search->setPlaceholderText("Start with '@'");

        startSearch = new QPushButton("Search");
        startSearch->setObjectName("startSearch");
        startSearch->setStyleSheet("#startSearch{"
                                   "min-width:180px;"
                                   "max-width:180px;"
                                   "min-height:25px;"
                                   "font-size:15px;"
                                   "padding:0;"
                                   "margin:3px 10px 0px 10px;"
                                   "background-color:#b8d3cb;"
                                   "color:#5b786f;"
                                   "border-radius:3px;"
                                   "border:1px solid white;"
                                   "}"
                                   "#startSearch:hover{"
                                   "background-color:#375249;"
                                   "color:#e1ece9;"
                                   "}");
        startSearch->setCursor(Qt::PointingHandCursor);

        stateAction = new QLabel("Chats");
        stateAction->setStyleSheet("min-width:200px;"
                                   "min-height:25px;"
                                   "font-size:15px;"
                                   "padding:3px;"
                                   "margin:0;"
                                   "background-color:#375249;"
                                   "color: #a4c6bc;");
        stateAction->setAlignment(Qt::AlignCenter);


        chatsWidget = new QWidget(this);
        chatsWidget->setStyleSheet("background-color:#243e35;");
        chatsWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        chatsWidgetLayout = new QVBoxLayout(chatsWidget);
        chatsWidgetLayout->setAlignment(Qt::AlignTop);
        chatsWidgetLayout->setContentsMargins(0, 0, 0, 0);
        chatsWidgetLayout->setSpacing(0);

        chatsWidgetContainer = new QWidget();
        chatsWidgetContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        chatsWidgetContainer->setStyleSheet("background-color:#6d8c82;");

        chatsWidgetLabelLayout = new QVBoxLayout(chatsWidgetContainer);
        chatsWidgetLabelLayout->setAlignment(Qt::AlignTop);
        chatsWidgetLabelLayout->setContentsMargins(0, 0, 0, 0);
        chatsWidgetLabel = new QLabel(chatsWidgetContainer);
        chatsWidgetLabel->setAlignment(Qt::AlignCenter);
        chatsWidgetLabelLayout->addWidget(chatsWidgetLabel);
        chatsWidgetLabel->setStyleSheet("background-color:#6d8c82;");

        chatsSearchContainer = new QWidget();
        chatsSearchContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        chatsSearchContainer->setStyleSheet("background-color:#809f96;");

        chatsSearchLabelLayout = new QVBoxLayout(chatsSearchContainer);
        chatsSearchLabelLayout->setAlignment(Qt::AlignTop);
        chatsSearchLabelLayout->setContentsMargins(0, 0, 0, 0);
        chatsSearchLabel = new QLabel(chatsSearchContainer);
        chatsSearchLabel->setAlignment(Qt::AlignCenter);
        chatsSearchLabelLayout->addWidget(chatsSearchLabel);
        chatsSearchLabel->setStyleSheet("background-color:#809f96;");


        stackedActionWidget = new QStackedWidget(chatsWidget);
        stackedActionWidget->addWidget(chatsWidgetContainer);
        stackedActionWidget->addWidget(chatsSearchContainer);
        stackedActionWidget->setCurrentWidget(chatsWidgetContainer);

        chatsWidgetLayout->addWidget(stackedActionWidget);
        chatsWidgetLayout->setSpacing(3);
        chatsWidget->setLayout(chatsWidgetLayout);

        layoutSearch->addWidget(search);
        layoutSearch->addWidget(startSearch);
        layoutSearch->addWidget(stateAction);
        layoutSearch->addWidget(chatsWidget,1);
        layoutSearch->setAlignment(Qt::AlignTop);
        Hlayout->addLayout(layoutSearch);


        stackedWidget = new QStackedWidget(this);
        stackedWidget->setFixedSize(650, 500);


        centerLabel = new QLabel("Select the chat to start",stackedWidget);
        centerLabel->setStyleSheet("font-size:25px; color:#5b786f; background-color:#ccdfda;");
        centerLabel->setAlignment(Qt::AlignCenter);

        stackedWidget->addWidget(centerLabel);
        stackedWidget->setCurrentWidget(centerLabel);


        Hlayout->addWidget(stackedWidget);
        Hlayout->setSpacing(0);
        Hlayout->setContentsMargins(0, 0, 0, 0);


        mainLayout->addLayout(layoutUpperWidget);
        mainLayout->addLayout(Hlayout);

        centralWidget->setLayout(mainLayout);
        this->setCentralWidget(centralWidget);

    }



    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, [this]() {
        qDebug() << "Connected to server";
        if (!own.isEmpty()) {
            qDebug() << "Calling loadAllChats with own =" << own;

            if (socket->state() == QTcpSocket::ConnectedState) {
                loadAllChats(own);
            } else {
                qDebug() << "Socket is not connected yet!";
            }
        } else {
            qDebug() << "Variable 'own' is empty, skipping loadAllChats";
        }
    });
    socket->connectToHost("127.0.0.1", 12345);
    if (!socket->waitForConnected(2000)) {
        qDebug() << "Failed to connect to server!";
    }

    connect(startSearch,&QPushButton::clicked,this,[this]{
        QString username = search->text();

        QJsonObject json;
        json["action"] = "searchUser";
        json["username"] = username;

        QJsonDocument doc(json);
        socket->connectToHost("127.0.0.1", 12345);
        if (socket->waitForConnected(1000)) {
            socket->write(doc.toJson());
        } else {
            stateAction->setText("Cannot find the user!");
        }
    });

    connect(socket, &QTcpSocket::readyRead, this, [this] {
        QByteArray responseData = socket->readAll();  // Отримуємо дані від сервера
        QJsonDocument doc = QJsonDocument::fromJson(responseData);  // Перетворюємо у JSON
        if (doc.isNull()) {
            qDebug() << "Error: Invalid JSON received!";
            return;
        }
        QJsonObject json = doc.object();  // Отримуємо об'єкт JSON
        searchUser = json["user"].toString();
        // Перевірка статусу відповіді
        if (json["status"].toString() == "success") {
            if (!searchUser.isEmpty() && !addedUsers.contains(searchUser)){
                createChatButton(searchUser,true);
                addedUsers.insert(searchUser);
            }
        }else if(json["status"].toString() == "message"){

            sendFrom = json["from"].toString();
            sendTo = json["to"].toString();
            message = json["message"].toString();

            setToLoadAllChats(sendFrom, sendTo);
            loadAllChats(own);
        }else if (json["status"].toString() == "infoAboutChat") {
            QJsonArray messagesArray = json["data"].toArray();

            for (const auto& msg : messagesArray) {
                QJsonObject messageObject = msg.toObject();

                QString from = messageObject["from"].toString();
                QString to = messageObject["to"].toString();
                QString message = messageObject["message"].toString();
                QString messageId = messageObject["id"].toString();  // ID повідомлення
                QString time = messageObject["timestamp"].toString();

                QDateTime dateTime = QDateTime::fromString(time, Qt::ISODate);
                QVector<int> date = {dateTime.date().year(), dateTime.date().month(), dateTime.date().day(),
                                     dateTime.time().hour(), dateTime.time().minute()};

                insertMessageToChat(from, to, message, date, messageId);
            }
        }
        else if(json["status"].toString() == "loadAllChats"){
            if (json.contains("currentChats")) {
                QJsonArray currentChatsArray = json["currentChats"].toArray();
                int sizeChats = currentChatsArray.size();
                if (sizeChats == 0) {
                    qDebug() << "No chats available";
                }
                for (int i = 0; i < sizeChats; ++i) {
                    QString chatName = currentChatsArray[i].toString();

                    qDebug() << "Chat: " << chatName;

                    if(!allChats.contains(chatName)) {
                        createChatButton(chatName,false);
                        allChats.insert(chatName);
                    }
                }
            } else {
                qDebug() << "No currentChats key found in response!";
            }
        }
        else if(json["status"].toString() == "newMessages"){
            QJsonArray messages = json["messages"].toArray();
            int maxMessageId = lastReceivedMessageId.toInt();
            for (const QJsonValue &messageValue : messages) {
                QJsonObject message = messageValue.toObject();

                QString from = message["from"].toString();
                QString to = message["to"].toString();
                QString messageText = message["message"].toString();
                QString time = message["timestamp"].toString();
                int messageId = message["id"].toInt();

                if (messageId > maxMessageId) {
                    maxMessageId = messageId;
                }


                QDateTime dateTime = QDateTime::fromString(time, Qt::ISODate);
                QVector<int> date = {dateTime.date().year(), dateTime.date().month(), dateTime.date().day(),
                                     dateTime.time().hour(), dateTime.time().minute()};

                insertMessageToChat(from, to, messageText, date, QString::number(messageId));
            }
            lastReceivedMessageId = QString::number(maxMessageId);
        }
        else {
            stateAction->setText("User doesn't  exist!");

        }
    });

    connect(search,&QLineEdit::textChanged,this,[this]{
        QLayoutItem *item;
        while ((item = chatsSearchLabelLayout->takeAt(0)) != nullptr) {
            if (QWidget *widget = item->widget()) {
                widget->deleteLater();
            }
            delete item;
        }

        stateAction->setText("Search");

        if(search->text()==""){
            stateAction->setText("Chats");
        }

        if(stateAction->text() == "Chats"){
            stackedActionWidget->setCurrentWidget(chatsWidgetContainer);
        }else if (stateAction->text() == "Search"){
            stackedActionWidget->setCurrentWidget(chatsSearchContainer);
        }
    });

    connect(logOut,&QPushButton::clicked,this,[this]{
        LogIn *login = new LogIn();
        login->show();
        this->close();
    });

    QString currentChatName = upperNameLabel->text();
    QTimer *messageUpdateTimer = new QTimer(this);
    connect(messageUpdateTimer, &QTimer::timeout, this, [this,currentChatName]() {
        QString updatedChatName = upperNameLabel->text();
        if (!updatedChatName.isEmpty()) {
            loadNewMessages(own, updatedChatName);
        }

        QTimer::singleShot(500, this, [this]() {
            loadAllChats(own);
        });

    });
    messageUpdateTimer->start(1000);

}

///? mb delete
void messengerWin::insertMessageToSomeone(QString from,QString to, QString message){
     qDebug() << "insertMessageToSomeone";
    QDateTime currentDateTime = QDateTime::currentDateTime();

    int year = currentDateTime.date().year();
    int month = currentDateTime.date().month();
    int day = currentDateTime.date().day();

    int hour = currentDateTime.time().hour();
    int minute = currentDateTime.time().minute();

    QHBoxLayout *messageLayout = new QHBoxLayout(this);
    QString hourAndMinutes = QString::number(hour) + ":" + QString::number(minute);
    QLabel *messageLabel = new QLabel(message.leftJustified(5, ' ') + " | " + hourAndMinutes + " |");
    QLabel *likeAIcon = new QLabel();

    QString yaerDayMount = QString::number(day) +'-'+  QString::number(month) +'-'+ QString::number(year);

    if (chatwindow->property("current_yaerDayMount").toString() != yaerDayMount) {
        chatwindow->setProperty("current_yaerDayMount", yaerDayMount);

        QLabel *yaerDayMountLabel = new QLabel(yaerDayMount);
        yaerDayMountLabel->setStyleSheet("background-color:#a4c6bc; color:white;");
        yaerDayMountLabel->setAlignment(Qt::AlignCenter);

        QHBoxLayout *Layout_yaerDayMountLabel = new QHBoxLayout();
        Layout_yaerDayMountLabel->addWidget(yaerDayMountLabel);

        yaerDayMountLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        chatLayout->addLayout(Layout_yaerDayMountLabel);
    }




    if(own == from){
        likeAIcon->setText("Me");
        likeAIcon->setStyleSheet("padding:1px;"
                                 "color:black;"
                                 "background-color:white;"
                                 "border-radius:10px;"
                                 "border: 1px solid black;"
                                 "max-width: 25px;"
                                 "height: 15px;");

        messageLayout->addWidget(likeAIcon);
        messageLabel->setStyleSheet("padding:5px;"
                                    "background-color:#122b22;"
                                    "color:white;"
                                    "max-height:20px;"
                                    "border-radius:3px;"
                                    "margin: 0;");
    }else{
        likeAIcon->setText(from[0].toUpper());
        likeAIcon->setStyleSheet("padding:3px;"
                                 "color:black;"
                                 "background-color:white;"
                                 "border-radius:10px;"
                                 "border: 1px solid black;"
                                 "width: 15px;"
                                 "height: 15px;");
        messageLayout->addWidget(likeAIcon);
        messageLabel->setStyleSheet("padding:5px;"
                                    "background-color:#6d8c82;"
                                    "color:white;"
                                    "max-height:20px;"
                                    "border-radius:3px;"
                                    "margin: 0;");
        messageLayout->addWidget(likeAIcon);
    }
    messageLayout->setContentsMargins(0, 0, 0, 0);
    messageLayout->setSpacing(5);
    messageLayout->setAlignment(likeAIcon, Qt::AlignLeft);
    messageLayout->setAlignment(Qt::AlignLeft);

    messageLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    messageLabel->adjustSize();

    messageLayout->addWidget(messageLabel, 0, Qt::AlignLeft);
    chatLayout->addLayout(messageLayout);

    QTimer::singleShot(100, this, [this]() {
        scrollArea->verticalScrollBar()->setValue(scrollArea->verticalScrollBar()->maximum());
    });
}
//?

void messengerWin::insertMessageToChat(QString from, QString to, QString message, QVector<int> date, QString messageId) {

    QHBoxLayout *messageLayout = new QHBoxLayout();
    QString hourAndMinutes = QString::number(date[3]) + ":" + QString::number(date[4]);
    QLabel *messageLabel = new QLabel(message + " | " + hourAndMinutes);
    QLabel *likeAIcon = new QLabel();

    QString yaerDayMount = QString::number(date[2]) +'-'+  QString::number(date[1]) +'-'+ QString::number(date[0]);

    if (chatwindow->property("current_yaerDayMount").toString() != yaerDayMount) {
        chatwindow->setProperty("current_yaerDayMount", yaerDayMount);

        QLabel *yaerDayMountLabel = new QLabel(yaerDayMount);
        yaerDayMountLabel->setStyleSheet("background-color:#122b22; color:white;");
        yaerDayMountLabel->setAlignment(Qt::AlignCenter);

        QHBoxLayout *Layout_yaerDayMountLabel = new QHBoxLayout();
        Layout_yaerDayMountLabel->addWidget(yaerDayMountLabel);

        yaerDayMountLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        chatLayout->addLayout(Layout_yaerDayMountLabel);
    }


    if (own == from) {
        likeAIcon->setText("Me");
        likeAIcon->setStyleSheet("padding:1px; color:black; background-color:white; border-radius:10px; border: 1px solid black; max-width: 25px; height: 15px;");
        messageLayout->addWidget(likeAIcon);
        messageLabel->setStyleSheet("padding:5px; background-color:#122b22; color:white; max-height:20px; border-radius:3px; margin: 0;");
    } else {
        likeAIcon->setText(from[0].toUpper());
        likeAIcon->setStyleSheet("padding:3px; color:black; background-color:white; border-radius:10px; border: 1px solid black; width: 15px; height: 15px;");
        messageLayout->addWidget(likeAIcon);
        messageLabel->setStyleSheet("padding:5px; background-color:#6d8c82; color:white; max-height:20px; border-radius:3px; margin: 0;");
    }

    messageLayout->setContentsMargins(0, 0, 0, 0);
    messageLayout->setSpacing(5);
    messageLayout->setAlignment(likeAIcon, Qt::AlignLeft);
    messageLayout->setAlignment(Qt::AlignLeft);

    messageLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    messageLabel->adjustSize();

    messageLayout->addWidget(messageLabel, 0, Qt::AlignLeft);
    chatLayout->addLayout(messageLayout);

    // Прокрутка до низу, якщо користувач не скролить вгору
    QScrollBar *scrollBar = scrollArea->verticalScrollBar();
    if (scrollBar->value() == scrollBar->maximum()) {
        QTimer::singleShot(50, this, [this]() {
            scrollArea->verticalScrollBar()->setValue(scrollArea->verticalScrollBar()->maximum());
        });
    }
}


void messengerWin::loadAllChats(QString current){
        QJsonObject json;
        json["action"] = "loadAllChats";
        json["current"] = current;

        QJsonDocument doc(json);
        QByteArray jsonData = doc.toJson();

        qDebug() << "Checking socket state...";
        if (!socket->isOpen()) {
        qDebug() << "Socket is not open, attempting to reconnect...";
        socket->connectToHost("127.0.0.1", 12345);

        if (!socket->waitForConnected(2000)) {
            qDebug() << "Failed to reconnect!";
            return;
        }
    }

        qDebug() << "Sending request to server...";
        socket->write(jsonData);
        socket->flush();  // Додаємо flush, щоб точно відправити пакет

        qDebug() << "Request sent successfully.";
}
void messengerWin::loadchat(QString from,QString to){

    QJsonObject json;
    json["action"] = "loadchat";
    json["from"] = from;
    json["to"] = to;

    QJsonDocument doc(json);
    QByteArray jsonData = doc.toJson();

    if (!socket->isOpen()) {
        socket->connectToHost("127.0.0.1", 12345);
    }

    socket->write(jsonData);
}
void messengerWin::sendMessageToSomeone(QString from,QString to, QString message){
    QJsonObject json;
    json["action"] = "sendMessage";
    json["from"] = from;
    json["to"] = to;
    json["message"] = message;

    QJsonDocument doc(json);
    QByteArray jsonData = doc.toJson();

    qDebug() << "Prepared JSON: " << jsonData;

    if (!socket->isOpen()) {
        socket->connectToHost("127.0.0.1", 12345);
    }

    socket->write(jsonData);
}
void messengerWin::setToLoadAllChats(QString from,QString to){
    QJsonObject json;

    json["action"] = "setToLoadAllChats";
    json["current"] = from;
    json["search"] = to;

    QJsonDocument doc(json);
    QByteArray jsonData = doc.toJson();



    if (!socket->isOpen()) {
        qDebug() << "Socket is not open, attempting to reconnect...";
        socket->connectToHost("127.0.0.1", 12345);
    }else{
        qDebug() << "Socket is  open";
    }
    socket->write(jsonData);
    socket->flush();

}

void messengerWin::loadNewMessages(QString from, QString to) {
    QJsonObject json;
    json["action"] = "loadNewMessages";
    json["from"] = from;
    json["to"] = to;
    json["lastMessageId"] = lastReceivedMessageId.toInt();

    QJsonDocument doc(json);
    QByteArray jsonData = doc.toJson();

    if (!socket->isOpen()) {
        socket->connectToHost("127.0.0.1", 12345);
    }

    socket->write(jsonData);
}

void messengerWin::createChatButton(QString name, bool isSearchUser) {
    qDebug() << "Створення кнопки чату для: " << name;

    QPushButton* chatButton = new QPushButton(this);
    chatButton->setObjectName("chatButton");
    chatButton->setStyleSheet(
        "#chatButton {"
        "    min-height: 35px;"
        "    max-width: 180px;"
        "    font-size: 15px;"
        "    padding: 0 0 0 10px;"
        "    margin: 0 10px 0 10px;"
        "    background-color: #122b22;"
        "    text-align: left;"
        "    color: #a4c6bc;"
        "    border-radius: 5px;"
        "}"
        "#chatButton:hover {"
        "    background-color: #49655c;"
        "    color: #e1ece9;"
        "}"
        );
    chatButton->setText(name);
    chatButton->setCursor(Qt::PointingHandCursor);

    if (isSearchUser) {
         chatsSearchLabelLayout->addWidget(chatButton);
    }
    else chatsWidgetLabelLayout->addWidget(chatButton);


    connect(chatButton, &QPushButton::clicked, this, [this, name] {
        playSystemSound("ding.wav");

        if (currentChat && currentChat->parent()) {
            currentChat->hide();
        }

        QWidget* existingChat = nullptr;
        for (int i = 0; i < stackedWidget->count(); ++i) {
            QWidget* widget = stackedWidget->widget(i);
            QLabel* chatLabel = widget->findChild<QLabel*>("upperNameLabel");
            if (chatLabel && chatLabel->text() == name) {
                existingChat = widget;
                break;
            }
        }

        if (existingChat) {
            existingChat->show();
            stackedWidget->setCurrentWidget(existingChat);
            currentChat = existingChat;
        }
        else {
            chatwindow = new QWidget();
            if (!chatwindow) {
                qCritical() << "can't create chatwindow!";
                return;
            }
            chatwindow->setStyleSheet("background-color:#243e35;");

            upperNameLabel->setText(name);
            upperNameLabel->setStyleSheet(
                "background-color:#b8d3cb;"
                "min-height: 20px;"
                "font-size:18px;"
                "padding: 3px;"
                "color:#122b22;"
                "border-radius:10px;"
                );
            upperNameLabel->setAlignment(Qt::AlignCenter);
            upperNameLabel->setObjectName("upperNameLabel");

            scrollArea = new QScrollArea(chatwindow);
            scrollArea->setWidgetResizable(true);
            scrollArea->setStyleSheet("background-color:#a4c6bc;border-radius:10px;");

            scrollWidget = new QWidget();
            scrollWidget->setStyleSheet("background-color:#a4c6bc;border-radius:10px;");
            chatLayout = new QVBoxLayout(scrollWidget);
            chatLayout->setAlignment(Qt::AlignBottom | Qt::AlignLeft);
            scrollArea->setWidget(scrollWidget);

            sendMessage = new QLineEdit(chatwindow);
            sendMessage->setStyleSheet(
                "text-align: left; background-color:#6d8c82; color: white;"
                "border-radius:20px; min-height: 25px; max-width: 550px; border-radius:6px;"
                );
            sendMessage->setPlaceholderText("Write to " + name +"...");

            BtnSendMessage = new QPushButton("Send", chatwindow);
            BtnSendMessage->setObjectName("BtnSendMessage");
            BtnSendMessage->setStyleSheet("#BtnSendMessage"
                                          "{"
                                          "background-color:#49655c; "
                                          "min-height: 25px; "
                                          "color:#92b3a9;"
                                          "border-radius:5px;"
                                          "}"
                                          "#BtnSendMessage:hover{"
                                          "background-color:#6d8c82;"
                                          "color:#ebf2f0;"
                                          "}");
            BtnSendMessage->setCursor(Qt::PointingHandCursor);
            BtnSendMessage->setEnabled(false);

            HbottomSend = new QHBoxLayout();
            HbottomSend->addWidget(sendMessage);
            HbottomSend->addWidget(BtnSendMessage);

            vlayoutChat = new QVBoxLayout();
            vlayoutChat->addWidget(upperNameLabel);
            vlayoutChat->addWidget(scrollArea);
            vlayoutChat->addLayout(HbottomSend);
            vlayoutChat->setAlignment(Qt::AlignBottom | Qt::AlignLeft);

            chatwindow->setLayout(vlayoutChat);
            stackedWidget->addWidget(chatwindow);
            stackedWidget->setCurrentWidget(chatwindow);
            currentChat = chatwindow;

            connect(sendMessage, &QLineEdit::textChanged, this, [this] {
                BtnSendMessage->setEnabled(!sendMessage->text().trimmed().isEmpty());
            });

            connect(BtnSendMessage, &QPushButton::clicked, this, [this, name] {
                QString message = sendMessage->text().trimmed();
                if (!message.isEmpty()) {
                    if (own.isEmpty() || name.isEmpty()) {
                        qCritical() << "incorrect data sendMessageToSomeone!";
                        return;
                    }
                    sendMessageToSomeone(own, name, message);
                    sendMessage->clear();
                }
            });

        }
    });

}

void messengerWin::playSystemSound(const QString& soundName) {
    effect.setSource(QUrl::fromLocalFile("C:/Windows/Media/" + soundName));
    effect.setVolume(0.8);
    effect.play();
}


messengerWin::~messengerWin()
{
    delete ui;
}
