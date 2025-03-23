#ifndef LOGIN_H
#define LOGIN_H


#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>

#include <QJsonDocument>
#include <QJsonObject>
#include <QCryptographicHash>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include <QSoundEffect>
#include <QMessageBox>


#include "signin.h"
#include "messengerwin.h"


namespace ioN = boost::asio;
using tcpN = ioN::ip::tcp;
using socketN = ioN::ip::tcp::socket;

extern QString NameUser;

namespace Ui {
class LogIn;
}

class LogIn : public QWidget
{
    Q_OBJECT

public:
    explicit LogIn(boost::asio::io_context& context,QWidget *parent = nullptr);

    bool isConnected = false;

    QLabel* login;
    QLineEdit* enter_log;

    QLabel* password;
    QLineEdit* enter_pass;

    QPushButton* closeWindow;
    QPushButton* sendData;
    QPushButton* changeField;

    QVBoxLayout *Vlayout;

    std::string jsonData;

    QLabel* info;
    QSoundEffect effect;
    void playSystemSound(const QString& soundName);
    ~LogIn();

private slots:
    void ServerResponse();
    void connectToServer();
private:
    //asio
    boost::asio::io_context& io;
    std::shared_ptr<boost::asio::ip::tcp::socket> socket;
    boost::asio::streambuf bufferFromServer;
    boost::asio::steady_timer timer;

    Ui::LogIn *ui;
};

#endif // LOGIN_H
