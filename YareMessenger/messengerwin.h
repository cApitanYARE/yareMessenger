#ifndef MESSENGERWIN_H
#define MESSENGERWIN_H

#include <QMainWindow>
#include <QTextBrowser>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QLabel>
#include <QStackedWidget>
#include <QPushButton>
#include <QMessageBox>
#include <QQueue>

#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCryptographicHash>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QScrollArea>
#include <QJsonArray>
#include <QDateTime>
#include <QScrollBar>
#include <QTimer>
#include <QPixmap>

#include <QFileDialog>
#include <QFile>
#include <QBuffer>

#include <QSoundEffect>



QT_BEGIN_NAMESPACE
namespace Ui {
class messengerWin;
}
QT_END_NAMESPACE

class messengerWin : public QMainWindow
{
    Q_OBJECT

public:

    explicit messengerWin(const QString &username,QWidget *parent = nullptr);

    QVBoxLayout * mainLayout;

    QHBoxLayout *HbottomSend;
    QLineEdit *sendMessage;
    QPushButton * BtnSendMessage;
    QVBoxLayout *vlayoutChat;
    QVBoxLayout *chatLayout;
    QLabel *upperNameLabel = new QLabel();
    QWidget *scrollWidget= nullptr;
    QWidget *chatwindow;
    QWidget *currentChat = nullptr;
    QScrollArea *scrollArea;




    QWidget *centralWidget;

    QVBoxLayout *layoutSearch;

    QHBoxLayout *selectWidgetMediaLayout;

    QGroupBox * groupBox;
    QHBoxLayout *Hlayout;

    QLabel *centerLabel;

    QLineEdit *search;
    QPushButton *startSearch;

    QStackedWidget *stackedWidget;
    QStackedWidget *stackedActionWidget;

    QTcpSocket *socket;



    QLabel *chatWithSomeone;


    QLabel *stateAction;

    QWidget *chatsWidget;
    QVBoxLayout *chatsWidgetLayout;
    QWidget* chatsWidgetContainer;
    QLabel *chatsWidgetLabel;
    QVBoxLayout *chatsWidgetLabelLayout;
    QWidget* chatsSearchContainer;
    QLabel *chatsSearchLabel;
    QVBoxLayout *chatsSearchLabelLayout;



    QVBoxLayout *layoutUpperWidget;
    QHBoxLayout *logOutLabelLayout;
    QLabel *upperLayoutLabel;
    QPushButton *logOut;


    QSoundEffect effect;


    QString own;


    QString lastReceivedMessageId = "";

    void playSystemSound(const QString& soundName);
    void sendMessageToSomeone(QString from,QString to, QString message);
    void insertMessageToSomeone(QString from,QString to, QString message);
    void loadchat(QString from,QString to);
    void insertMessageToChat(QString from, QString to, QString message, QVector<int> date, QString messageId);
    void loadAllChats(QString current);

    void createChatButton(QString name, bool isSearchUser);
    void loadNewMessages(QString from, QString to);


    void setToLoadAllChats(QString from,QString to);



    ~messengerWin();

private:
    Ui::messengerWin *ui;
};
#endif // MESSENGERWIN_H
