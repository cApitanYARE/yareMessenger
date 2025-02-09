#include "server.h"

Server::Server(){
    db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("DRIVER={SQL Server};SERVER=DESKTOP-HOKI7E3\\SQLEXPRESS;DATABASE=master;Trusted_Connection=yes;");

    if (db.open()) {
        qDebug() << "Connected to SQL Server";

        QSqlQuery query(db);
        query.exec("SELECT name FROM sys.databases WHERE name = 'YareMessanger'");

        if (!query.next()) {
            qDebug() << "Database does not exist, creating 'YareMessanger'...";
            query.exec("CREATE DATABASE YareMessanger");
        } else {
            qDebug() << "Database 'YareMessanger' exists.";
        }

        db.setDatabaseName("DRIVER={SQL Server};SERVER=DESKTOP-HOKI7E3\\SQLEXPRESS;DATABASE=YareMessanger;Trusted_Connection=yes;");

        if (db.open()) {
            qDebug() << "Connected to 'YareMessanger' database";
            QString createTableUserDb =
                "IF NOT EXISTS (SELECT * FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_NAME = 'usersDB') "
                "BEGIN "
                "CREATE TABLE usersDB ("
                "id INT IDENTITY(1,1) PRIMARY KEY, "
                "username VARCHAR(50) UNIQUE NOT NULL, "
                "password VARCHAR(255) NOT NULL, "
                "created_at DATETIME DEFAULT GETDATE()"
                ") "
                "END";
            if (!query.exec(createTableUserDb)) {
                qDebug() << "userDb table creation failed:" << query.lastError().text();
            } else {
                qDebug() << "userDb table created or already exists.";
            }

            if (listen(QHostAddress::Any, 12345)) {
                qDebug() << "Server is running on port 12345";
            } else {
                qDebug() << "Error: Unable to start the server!";
            }
        } else {
            qDebug() << "Database error:" << db.lastError().text();
        }
    } else {
        qDebug() << "Database connection failed:" << db.lastError().text();
    }
}

void Server::incomingConnection(qintptr socketDescriptor) {
    QTcpSocket *clientSocket = new QTcpSocket;
    if (clientSocket->setSocketDescriptor(socketDescriptor)) {
        connect(clientSocket, &QTcpSocket::readyRead, this, &Server::onReadyRead);
        connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
        qDebug() << "New client connected, descriptor:" << socketDescriptor;
    } else {
        delete clientSocket;
    }
}

void Server::onReadyRead() {
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    if (!clientSocket)
        return;
    QByteArray requestData = clientSocket->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(requestData);
    QJsonObject request = doc.object();

    if (!doc.isNull()) {
        QString action = request["action"].toString();
        qDebug() << action;
        if (action == "register") {
            handleRegister(clientSocket, request);
        } else if (action == "login") {
            handleLogin(clientSocket, request);
        } else if (action == "searchUser") {
            searchUser(clientSocket, request);
        } else if (action == "sendMessage") {
            sendMessageToSomeone(clientSocket, request);
        } else if (action == "loadchat") {
            loadChat(clientSocket, request);
        } else if (action == "loadAllChats") {
            loadAllChats(clientSocket, request);
        } else if (action == "setToLoadAllChats") {
            setToLoadAllChatsServer(clientSocket, request);
        }else if(action == "loadNewMessages"){
            loadNewMessages(clientSocket, request);
        }
        else {
            qDebug() << "Unknown action received: " << action;
        }
    } else {
        qDebug() << "Received invalid JSON!";
    }

}

void Server::handleRegister(QTcpSocket *clientSocket, const QJsonObject &request) {
    QString username = request["username"].toString();
    QString password = request["password"].toString();

    qDebug()<< username;
    qDebug()<< password;

    QSqlQuery query;
    QJsonObject response;

    query.prepare("SELECT username FROM usersDB WHERE username = :username");
    query.bindValue(":username", username);
    query.exec();


    if(query.next()){
        response["status"] = "UserExist";
        QJsonDocument responseDoc(response);
        clientSocket->write(responseDoc.toJson());
        clientSocket->flush();
        return;
    }

    query.prepare("INSERT INTO usersDB (username, password) VALUES (:username, :password)");
    query.bindValue(":username", username);
    query.bindValue(":password", password);

    if (query.exec()) {
        response["status"] = "success";
        response["message"] = "User registered successfully!";
    } else {
        response["status"] = "error";
        response["message"] = query.lastError().text();
    }

    QJsonDocument responseDoc(response);
    clientSocket->write(responseDoc.toJson());
    clientSocket->flush();
}
void Server::handleLogin(QTcpSocket *clientSocket, const QJsonObject &request) {
    QString username = request["username"].toString();
    QString password = request["password"].toString();

    QSqlQuery query;

    query.prepare("SELECT password FROM usersDB WHERE username = :username");
    query.bindValue(":username", username);

    QJsonObject response;
    if (query.exec() && query.next()) {
        QString storedHash = query.value(0).toString();

        if (storedHash == password) {
            response["status"] = "success";
            response["message"] = "User enter successfully!";
        }else{
            response["status"] = "error";
            response["message"] = query.lastError().text();
        }
    } else {
        response["status"] = "error";
        response["message"] = query.lastError().text();
    }

    QJsonDocument responseDoc(response);
    clientSocket->write(responseDoc.toJson());
    clientSocket->flush();
}


void Server::searchUser(QTcpSocket *clientSocket, const QJsonObject &request) {
    QString username = request["username"].toString();
    qDebug() << "Searching for user: " << username;  // Логування для дебагу

    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM usersDB WHERE username = :username");
    query.bindValue(":username", username);

    QJsonObject response;


    if (query.exec()) {
        if (query.next()) {
            int count = query.value(0).toInt();
            if (count > 0) {
                response["status"] = "success";
                response["message"] = "User found!";
                response["user"] = username;
            } else {
                response["status"] = "error";
                response["message"] = "User does not exist!";
            }
        } else {
            response["status"] = "error";
            response["message"] = "Error executing query.";
        }
    } else {
        response["status"] = "error";
        response["message"] = query.lastError().text();
    }

    QJsonDocument responseDoc(response);
    clientSocket->write(responseDoc.toJson());
    clientSocket->flush();
}

void Server::sendMessageToSomeone(QTcpSocket *clientSocket, const QJsonObject &request) {
    QString from = request["from"].toString();
    QString to = request["to"].toString();
    QString message = request["message"].toString();

    QString TableName = from + to;
    QString TableExist = checkTableDB(from,to);

    QSqlQuery query;

    query.prepare("SELECT * FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_NAME = :tableName");
    query.bindValue(":tableName", TableExist);
    if(TableExist=="false"){
        if (!query.next()) {

            QString createTableQuery = QString(
                                           "CREATE TABLE [%1] ("
                                           "[id] INT IDENTITY(1,1) PRIMARY KEY,"
                                           "[from] VARCHAR(100),"
                                           "[to] VARCHAR(100),"
                                           "[message] VARCHAR(MAX),"
                                           "[timestamp] DATETIME DEFAULT CURRENT_TIMESTAMP"
                                           ");").arg(TableName);

            query.prepare(createTableQuery);
            query.exec();


            query.prepare(createTableQuery);
            query.exec();
            TableExist = TableName;
        }
    }

    QString insertQuery = QString("INSERT INTO [%1] ([from], [to], [message]) VALUES ('%2', '%3', '%4');")
                              .arg(TableExist)
                              .arg(from)
                              .arg(to)
                              .arg(message);

    query.prepare(insertQuery);

    QJsonObject response;

    if (query.exec()) {
        response["from"] = from;
        response["to"] = to;
        response["message"] = message;
        response["status"] = "message";
    } else {
        response["status"] = "error";
        response["message"] = query.lastError().text();
    }

    QJsonDocument responseDoc(response);
    clientSocket->write(responseDoc.toJson());
    clientSocket->flush();
}

void Server::loadChat(QTcpSocket *clientSocket, const QJsonObject &request){
    QString from = request["from"].toString();
    QString to = request["to"].toString();

    QString TableExist = checkTableDB(from,to);

    if(TableExist == "false"){
        QJsonObject response;
        response["status"] = "error";
        response["message"] = "until don't exist caht";

        QJsonDocument responseDoc(response);
        clientSocket->write(responseDoc.toJson());
        clientSocket->flush();
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT * FROM " + TableExist);

    if (!query.exec()) {
        qDebug() << "SQL error: " << query.lastError().text();

        QJsonObject response;
        response["status"] = "error";
        response["message"] = "Database query failed.";

        QJsonDocument responseDoc(response);
        clientSocket->write(responseDoc.toJson());
        clientSocket->flush();
        return;
    }

    QJsonArray rows;

    while (query.next()) {
        QJsonObject row;
        for (int i = 0; i < query.record().count(); ++i) {
            row[query.record().fieldName(i)] = query.value(i).toString();
        }
        rows.append(row);
    }

    if (rows.isEmpty()) {
        qDebug() << "No messages found in table: " << TableExist;
    }

    QJsonObject response;
    response["status"] = "infoAboutChat";
    response["data"] = rows;


    QJsonDocument responseDoc(response);
    clientSocket->write(responseDoc.toJson());
    clientSocket->flush();
}

void Server::setToLoadAllChatsServer(QTcpSocket *clientSocket, const QJsonObject &request){
    QSqlQuery query;

    QString current = request["current"].toString();
    QString search = request["search"].toString();


    QString currentQuery = QString("SELECT name FROM sys.tables WHERE name = '%1';").arg(current);
    query.prepare(currentQuery);
    if (!query.exec()) {
        qDebug() << "error audit table:" << query.lastError().text();
        return;
    }


    if (!query.next()) {
        currentQuery = QString("CREATE TABLE [%1] ("
                               "[id] INT IDENTITY(1,1) PRIMARY KEY,"
                               "[saved] VARCHAR(100)"
                               ");").arg(current);
        query.prepare(currentQuery);
        if (!query.exec()) {
            qDebug() << "error create db:" << query.lastError().text();
            return;
        }
    }


    currentQuery = QString("SELECT COUNT(*) FROM [%1] WHERE [saved] = :saved;").arg(current);
    query.prepare(currentQuery);
    query.bindValue(":saved", search);
    if (!query.exec()) {
        qDebug() << "error insert:" << query.lastError().text();
        return;
    }

    query.next();
    int count = query.value(0).toInt();
    if (count > 0) {
        qDebug() << "already exist!";
        return;
    }

    currentQuery = QString("INSERT INTO [%1] ([saved]) VALUES (:saved);").arg(current);
    query.prepare(currentQuery);
    query.bindValue(":saved", search);
    if (!query.exec()) {
        qDebug() << "bad insert:" << query.lastError().text();
    } else {
        qDebug() << "insert good!";
    }


    /////////////////////
    currentQuery = QString("SELECT name FROM sys.tables WHERE name = '%1';").arg(search);
    query.prepare(currentQuery);
    if (!query.exec()) {
        qDebug() << "error audit table:" << query.lastError().text();
        return;
    }


    if (!query.next()) {
        currentQuery = QString("CREATE TABLE [%1] ("
                               "[id] INT IDENTITY(1,1) PRIMARY KEY,"
                               "[saved] VARCHAR(100)"
                               ");").arg(search);
        query.prepare(currentQuery);
        if (!query.exec()) {
            qDebug() << "error create db:" << query.lastError().text();
            return;
        }
    }


    currentQuery = QString("SELECT COUNT(*) FROM [%1] WHERE [saved] = :saved;").arg(search);
    query.prepare(currentQuery);
    query.bindValue(":saved", current);
    if (!query.exec()) {
        qDebug() << "error insert:" << query.lastError().text();
        return;
    }

    query.next();
    int count1 = query.value(0).toInt();
    if (count > 0) {
        qDebug() << "already exist!";
        return;
    }

    currentQuery = QString("INSERT INTO [%1] ([saved]) VALUES (:saved);").arg(search);
    query.prepare(currentQuery);
    query.bindValue(":saved", current);
    if (!query.exec()) {
        qDebug() << "bad insert:" << query.lastError().text();
    } else {
        qDebug() << "insert good!";
    }
}
void Server::loadAllChats(QTcpSocket *clientSocket, const QJsonObject &request){
    QSqlQuery query;
    QString current = request["current"].toString();

    // Перевірка, чи існує таблиця
    QString currentQuery = QString("SELECT name FROM sys.tables WHERE name = ?;");
    query.prepare(currentQuery);
    query.addBindValue(current);

    if (!query.exec()) {
        qDebug() << "Error checking table existence:" << query.lastError().text();
        return;
    }

    // Якщо таблиця не існує, створюємо її
    if (!query.next()) {
        currentQuery = QString("CREATE TABLE [%1] ("
                               "[id] INT IDENTITY(1,1) PRIMARY KEY,"
                               "[saved] VARCHAR(100)"
                               ");").arg(current);
        if (!query.exec(currentQuery)) {
            qDebug() << "error with create:" << query.lastError().text();
            return;
        }
    }
    // Виконуємо вибірку даних
    currentQuery = QString("SELECT saved FROM [%1];").arg(current);
    if (!query.exec(currentQuery)) {
        qDebug() << "error with taken from:" << query.lastError().text();
        return;
    }

    QJsonArray rows;
    while (query.next()) {
        rows.append(query.value("saved").toString());
    }

    QJsonObject response;
    response["status"] = "loadAllChats";
    response["currentChats"] = rows;

     qDebug() << "Sending response to client:" << response["currentChats"];

    QJsonDocument responseDoc(response);
    clientSocket->write(responseDoc.toJson());
    clientSocket->flush();

}

void Server::loadNewMessages(QTcpSocket *clientSocket, const QJsonObject &request) {
    QString from = request["from"].toString();
    QString to = request["to"].toString();
    int lastMessageId = request["lastMessageId"].toInt();

    QString TableExist = checkTableDB(from, to);
    qDebug() << "Table name from checkTableDB:" << TableExist;

    if (TableExist.isEmpty()) {
        qDebug() << "Error: Table name is empty!";
        return;
    }

    QSqlQuery query;
    QString currentQuery = QString(
                               "SELECT id, [from], [to], [message], [timestamp] "
                               "FROM [%1] "
                               "WHERE (([from] = :from AND [to] = :to) OR ([from] = :to AND [to] = :from)) "
                               "AND id > :lastMessageId "
                               "ORDER BY [timestamp] ASC;").arg(TableExist);

    qDebug() << "Generated Query: " << currentQuery;

    query.prepare(currentQuery);
    query.bindValue(":from", from);
    query.bindValue(":to", to);
    query.bindValue(":lastMessageId", lastMessageId);

    if (!query.exec()) {
        qDebug() << "Error loading new messages:" << query.lastError().text();
        return;
    }

    QJsonArray messagesArray;
    while (query.next()) {
        QJsonObject messageObject;
        messageObject["id"] = query.value("id").toInt();
        messageObject["from"] = query.value("from").toString();
        messageObject["to"] = query.value("to").toString();
        messageObject["message"] = query.value("message").toString();
        messageObject["timestamp"] = query.value("timestamp").toString();

        messagesArray.append(messageObject);
    }

    QJsonObject response;
    response["status"] = "newMessages";
    response["messages"] = messagesArray;

    qDebug() << "Sending new messages response to client:" << response["messages"];

    QJsonDocument responseDoc(response);
    clientSocket->write(responseDoc.toJson());
    clientSocket->flush();


}


QString Server::checkTableDB(QString from, QString to){
    QSqlQuery query;
    QString table1 = from + to;
    QString table2 = to + from;
    query.prepare("SELECT COUNT(*) FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_NAME = :tableName");

    query.bindValue(":tableName", table1);
    qDebug() << table1;
    if (query.exec() && query.next() && query.value(0).toInt() > 0) {
        return table1;
    }

    qDebug() << table2;
    query.bindValue(":tableName", table2);
    if (query.exec() && query.next() && query.value(0).toInt() > 0) {
        return table2;
    }

    return "false";
}

Server::~Server() {
    if (db.isOpen()) {
        db.close();
        qDebug() << "Database connection closed.";
    }
}
