#ifndef SERVER_H
#define SERVER_H



#include <QTimer>
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

#include <boost/asio.hpp>
#include <memory>

namespace ioN = boost::asio;
using tcpN = ioN::ip::tcp;
using socketN = ioN::ip::tcp::socket;


class Server : public QObject {
    Q_OBJECT

public:
    Server(ioN::io_context& io_context,short port);

    int carrentMessageId = 0;

    void async_accept();

    QString chunk;
    QString responseString;


    QJsonArray SendLargeDataArray;
    QJsonObject DataString;
    //QJsonDocument DataDoc;

    ~Server();
private slots:
    void ReadyRead(std::shared_ptr<boost::asio::ip::tcp::socket> socket,QString data);
    void handleRegister(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const QJsonObject &json);
    void handleLogin(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const QJsonObject &request);
    void searchUser(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const QJsonObject &request);
    void sendMessageToSomeone(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const QJsonObject &request);
    void loadChat(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const QJsonObject &request);

    void setToLoadAllChatsServer(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const QJsonObject &request);
    void loadAllChats(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const QJsonObject &request);
    void loadNewMessages(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const QJsonObject &request);



    QString checkTableDB(QString from, QString to);

private:
    ioN::io_context& io_context;
    tcpN::acceptor acceptor;
    QSqlDatabase db;
};

#endif // SERVER_H
