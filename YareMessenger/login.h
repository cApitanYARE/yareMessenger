#ifndef LOGIN_H
#define LOGIN_H


#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>

#include <QTcpSocket>
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

extern QString NameUser;

namespace Ui {
class LogIn;
}

class LogIn : public QWidget
{
    Q_OBJECT

public:
    explicit LogIn(QWidget *parent = nullptr);



    QLabel* login;
    QLineEdit* enter_log;

    QLabel* password;
    QLineEdit* enter_pass;

    QPushButton* closeWindow;
    QPushButton* sendData;
    QPushButton* changeField;

    QVBoxLayout *Vlayout;

    QLabel* info;



    QTcpSocket *socket;




    QSoundEffect effect;
    void playSystemSound(const QString& soundName);
    ~LogIn();

    \

private slots:
    void onServerResponse();
private:
    Ui::LogIn *ui;
};

#endif // LOGIN_H
