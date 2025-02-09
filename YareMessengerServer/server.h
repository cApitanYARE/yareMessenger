#ifndef SERVER_H
#define SERVER_H



#include <QTcpServer>
#include <QTcpSocket>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QCryptographicHash>
#include <QJsonArray>
#include <QSqlRecord>

#include <QPixmap>

class Server : public QTcpServer{
    Q_OBJECT

public:
    Server();

    QTcpSocket *socket;
    void incomingConnection(qintptr socketDescriptor);
    ~Server();
private slots:
    void onReadyRead();
    void handleRegister(QTcpSocket *clientSocket, const QJsonObject &json);
    void handleLogin(QTcpSocket *clientSocket, const QJsonObject &request);
    void searchUser(QTcpSocket *clientSocket, const QJsonObject &request);
    void sendMessageToSomeone(QTcpSocket *clientSocket, const QJsonObject &request);
    void loadChat(QTcpSocket *clientSocket, const QJsonObject &request);

    void setToLoadAllChatsServer(QTcpSocket *clientSocket, const QJsonObject &request);
    void loadAllChats(QTcpSocket *clientSocket, const QJsonObject &request);
    void loadNewMessages(QTcpSocket *clientSocket, const QJsonObject &request);


    QString checkTableDB(QString from, QString to);

private:
    QSqlDatabase db;
};

#endif // SERVER_H
