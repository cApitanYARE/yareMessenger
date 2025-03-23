#include "server.h"

Server::Server(boost::asio::io_context& io_context,short port)
    : io_context(io_context),
    acceptor(io_context, tcpN::endpoint(tcpN::v4(), port)){
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
/*
            if (listen(QHostAddress::Any, 12345)) {
                qDebug() << "Server is running on port 12345";
            } else {
                qDebug() << "Error: Unable to start the server!";
            }
*/
        } else {
            qDebug() << "Database error:" << db.lastError().text();
        }
    } else {
        qDebug() << "Database connection failed:" << db.lastError().text();
    }
}


void Server::ReadyRead(std::shared_ptr<boost::asio::ip::tcp::socket> socket,QString data){
    qDebug() << "ReadyRead";
    QJsonDocument docFromString = QJsonDocument::fromJson(data.toUtf8());
    QJsonObject jsonFromString = docFromString.object();
    QJsonObject request = docFromString.object();

    if (!docFromString.isNull()) {
        QString action = request["action"].toString();
        qDebug() << action;
        if (action == "register") {
            handleRegister(socket, request);
        } else if (action == "login") {
            handleLogin(socket, request);
        } else if (action == "searchUser") {
            qDebug()<<"searchUser";
            searchUser(socket, request);
        } else if (action == "sendMessage") {
            sendMessageToSomeone(socket, request);
        } else if (action == "loadchat") {
            loadChat(socket, request);
        } else if (action == "loadAllChats") {
            loadAllChats(socket, request);
        } else if (action == "setToLoadAllChats") {
            setToLoadAllChatsServer(socket, request);
        }else if(action == "loadNewMessages"){
            loadNewMessages(socket, request);
        }
        else {
            qDebug() << "Unknown action received: " << action;
        }
    } else {
        qDebug() << "Received invalid JSON!";
    }
}

void Server::handleRegister(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const QJsonObject &request) {
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
        responseString = responseDoc.toJson();
        try{
            boost::asio::async_write(*socket, boost::asio::buffer(responseString),
                                     [this,socket](const boost::system::error_code& ec, std::size_t /*bytes_transferred*/) {
                                         if (!ec) {
                                             // НЕ закривайте сокет одразу! Очікуйте нових запитів або обробляйте їх далі.
                                             async_accept();
                                         } else {
                                             qDebug() << "Error sending response: " << QString::fromStdString(ec.message());
                                             socket->close();
                                         }
                                     }
                                     );
        }
        catch(const std::exception& e) {
            qDebug() << "Exception: " << e.what();
        }
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
    responseString = responseDoc.toJson();
    //clientSocket->write(responseDoc.toJson());
    //clientSocket->flush();
    qDebug() << responseString;
    if (!socket->is_open()) {
        qDebug() << "Socket is closed!";
        return;
    }else  qDebug() << "Socket isn't closed!";
    try{
    boost::asio::async_write(*socket, boost::asio::buffer(responseString),
        [this,socket](const boost::system::error_code& ec, std::size_t bytes_transferred) {
            if (!ec) {
                qDebug() << "Response sent successfully.";
                                    // НЕ закривайте сокет одразу! Очікуйте нових запитів або обробляйте їх далі.
                qDebug() << responseString;
                async_accept();
            } else {
                qDebug() << "Error sending response: " << QString::fromStdString(ec.message());
                socket->close();
            }
        }
    );
    }
    catch(const std::exception& e) {
        qDebug() << "Exception: " << e.what();
    }
}

void Server::handleLogin(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const QJsonObject &request) {
    QString username = request["username"].toString();
    QString password = request["password"].toString();

    qDebug()<<username;
    qDebug()<<password;

    QSqlQuery query;
    QJsonObject response;

    query.prepare("SELECT password FROM usersDB WHERE username = :username");
    query.bindValue(":username", username);

    if (query.exec()){
        if (query.next()){
            QString storedHash = query.value(0).toString();
            qDebug() << "Stored hash: " << storedHash;
            if (storedHash == password) {
                response["status"] = "success";
                response["message"] = "User entered successfully!";
            } else {
                response["status"] = "error";
                response["message"] = "Incorrect password!";
            }
        } else {
            response["status"] = "error";
            response["message"] = "User not found!";
        }
    } else {
        response["status"] = "error";
        response["message"] = "Database query error: " + query.lastError().text();
    }

    QJsonDocument responseDoc(response);
    responseString = responseDoc.toJson();
    try{
        boost::asio::async_write(*socket, boost::asio::buffer(responseString),
                                 [this,socket](const boost::system::error_code& ec, std::size_t bytes_transferred) {
                                     if (!ec) {
                                         qDebug() << "Response sent successfully.";
                                             // НЕ закривайте сокет одразу! Очікуйте нових запитів або обробляйте їх далі.
                                         qDebug() << responseString;
                                         async_accept();
                                     } else {
                                         qDebug() << "Error sending response: " << QString::fromStdString(ec.message());
                                         socket->close();
                                     }
                                 }
                                 );
    }
    catch(const std::exception& e) {
        qDebug() << "Exception: " << e.what();
    }
}

void Server::searchUser(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const QJsonObject &request) {
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
    responseString = responseDoc.toJson();
    try{
        boost::asio::async_write(*socket, boost::asio::buffer(responseString),
                                 [this,socket](const boost::system::error_code& ec, std::size_t bytes_transferred) {
                                     if (!ec) {
                                         qDebug() << "Response sent successfully.";
                                             // НЕ закривайте сокет одразу! Очікуйте нових запитів або обробляйте їх далі.
                                         qDebug() << responseString;
                                         async_accept();
                                     } else {
                                         qDebug() << "Error sending response: " << QString::fromStdString(ec.message());
                                         socket->close();
                                     }
                                 }
                                 );
    }
    catch(const std::exception& e) {
        qDebug() << "Exception: " << e.what();
    }
}

void Server::sendMessageToSomeone(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const QJsonObject &request) {
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
    responseString = responseDoc.toJson();
    try{
        boost::asio::async_write(*socket, boost::asio::buffer(responseString),
                                 [this,socket](const boost::system::error_code& ec, std::size_t bytes_transferred) {
                                     if (!ec) {
                                         qDebug() << "Response sent successfully.";
                                             // НЕ закривайте сокет одразу! Очікуйте нових запитів або обробляйте їх далі.
                                         qDebug() << responseString;
                                         async_accept();
                                     } else {
                                         qDebug() << "Error sending response: " << QString::fromStdString(ec.message());
                                         socket->close();
                                     }
                                 }
                                 );
    }
    catch(const std::exception& e) {
        qDebug() << "Exception: " << e.what();
    }
}

void Server::loadChat(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const QJsonObject &request){
    carrentMessageId = 0;
    QString from = request["from"].toString();
    QString to = request["to"].toString();
    QString TableExist = checkTableDB(from,to);

    if(TableExist == "false"){
        QJsonObject response;
        response["status"] = "error";
        response["message"] = "until don't exist caht";

        QJsonDocument responseDoc(response);

        QString responseString = responseDoc.toJson();
        boost::asio::async_write(*socket, boost::asio::buffer(responseString));
    }

    QSqlQuery query;
    query.prepare("SELECT * FROM [dbo].[" + TableExist + +"]");

    if (!query.exec()) {
        qDebug() << "SQL error: " << query.lastError().text();

        QJsonObject response;
        response["status"] = "error";
        response["message"] = "Database query failed.";

        QJsonDocument responseDoc(response);

        responseString = responseDoc.toJson();
        try{
            boost::asio::async_write(*socket, boost::asio::buffer(responseString),
                                     [this,socket](const boost::system::error_code& ec, std::size_t bytes_transferred) {
                                         if (!ec) {
                                             qDebug() << "Response sent successfully.";
                                             qDebug() << responseString;
                                             async_accept();
                                         } else {
                                             qDebug() << "Error sending response: " << QString::fromStdString(ec.message());
                                             socket->close();
                                         }
                                     }
                                     );
        }
        catch(const std::exception& e) {
            qDebug() << "Exception: " << e.what();
        }
        return;
    }

    //SendLargeDataArray;

    while (query.next()) {
        //DataString;
        for (int i = 0; i < query.record().count(); ++i) {
            DataString[query.record().fieldName(i)] = query.value(i).toString();
            if(query.record().fieldName(i) == "id") if (query.value(i).toInt() > carrentMessageId) carrentMessageId = query.value(i).toInt();
        }
        DataString["status"] = "infoAboutChat";
        QJsonDocument DataDoc(DataString);
        chunk = DataDoc.toJson();

        boost::asio::async_write(*socket, boost::asio::buffer(chunk),
                                 [this,socket](const boost::system::error_code& ec, std::size_t) {
                                     if (ec) {
                                         qDebug() << "Error sending chunk: " << QString::fromStdString(ec.message());
                                         socket->close();
                                     } else {
                                         qDebug() << "Chunk sent: " << chunk;
                                     }
                                 });
    }
    qDebug() << "carrentMessageId" << carrentMessageId;

}

void Server::setToLoadAllChatsServer(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const QJsonObject &request){
    qDebug() << "!!!!!!!!!!!!!!!!!!!!!!!!!!setToLoadAllChatsServer!!!!!!!!!!!!!!!!!!!";
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

void Server::loadAllChats(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const QJsonObject &request){
    qDebug() << "loadAllChats";
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

     qDebug() << "Sending response to client:" << response["currentChats"].toString();

    QJsonDocument responseDoc(response);

    responseString = responseDoc.toJson();
    try{
        boost::asio::async_write(*socket, boost::asio::buffer(responseString),
                                 [this,socket](const boost::system::error_code& ec, std::size_t bytes_transferred) {
                                     if (!ec) {
                                         qDebug() << "Response sent successfully.";
                                             // НЕ закривайте сокет одразу! Очікуйте нових запитів або обробляйте їх далі.
                                         qDebug() << responseString;
                                         async_accept();
                                     } else {
                                         qDebug() << "Error sending response: " << QString::fromStdString(ec.message());
                                         socket->close();
                                     }
                                 }
                                 );
    }
    catch(const std::exception& e) {
        qDebug() << "Exception: " << e.what();
    }
}

void Server::loadNewMessages(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const QJsonObject &request) {
    qDebug() << "loadNewMessages";
    QString from = request["from"].toString();
    QString to = request["to"].toString();
    int maxIdMessage = request["lastMessageId"].toInt();
    qDebug() << "======================"<<maxIdMessage<<"===========FOR:"<<from<<"========";


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
                               "AND id > :maxIdMessage "
                               "ORDER BY [timestamp] ASC;").arg(TableExist);

    query.prepare(currentQuery);
    query.bindValue(":from", from);
    query.bindValue(":to", to);
    query.bindValue(":maxIdMessage", maxIdMessage);
    carrentMessageId = maxIdMessage;
    if (!query.exec()) {
        qDebug() << "Error loading new messages:" << query.lastError().text();
        return;
    }
    while (query.next()) {
        //DataString;
        for (int i = 0; i < query.record().count(); ++i) {
            DataString[query.record().fieldName(i)] = query.value(i).toString();
            if(query.record().fieldName(i) == "id") if (query.value(i).toInt() > carrentMessageId) carrentMessageId = query.value(i).toInt();
        }
        DataString["status"] = "newMessages";

        QJsonDocument DataDoc(DataString);
        chunk = DataDoc.toJson();

        boost::asio::async_write(*socket, boost::asio::buffer(chunk),
                                 [this,socket,query](const boost::system::error_code& ec, std::size_t) {
                                     if (ec) {
                                         qDebug() << "Error sending chunk: " << QString::fromStdString(ec.message());
                                         socket->close();
                                     } else {
                                         //lastMessageId = query.value("id").toInt();

                                         qDebug() << "Chunk sent: " << chunk;

                                     }
                                 });
    }
}

void Server::async_accept(){
    auto socket = std::make_shared<boost::asio::ip::tcp::socket>(io_context);
    qDebug() << "wait async_accept";
    qDebug() << acceptor.is_open();
    acceptor.async_accept(
        *socket,
        [this, socket](const boost::system::error_code& ec){
            if(ec){
                qDebug() << "Помилка прийому з'єднання: " << ec.message();
                return;
            }else{
                qDebug() << "Connect successful";
                auto data = std::make_shared<std::array<char, 1024>>();
                auto client_socket = socket;
                socket->async_read_some(
                    boost::asio::buffer(*data),
                    [this,client_socket,data](boost::system::error_code ec, std::size_t length) {
                        if (ec) {
                            qDebug() << "Error get data: " << ec.message();
                            return;
                        }
                        qDebug() << "data received: " << QString::fromStdString(std::string(data->data(), length));
                        QString strData(data->data());
                        ReadyRead(client_socket,strData);
                    });
            }
            async_accept();
        }
    );
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
