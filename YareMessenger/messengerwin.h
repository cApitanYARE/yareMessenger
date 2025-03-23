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
#include <QCloseEvent>

#include <optional>
#include <queue>

#include <boost/asio.hpp>

QT_BEGIN_NAMESPACE
namespace Ui {
class messengerWin;
}
QT_END_NAMESPACE

class messengerWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit messengerWin(const QString &username,boost::asio::io_context& context,std::shared_ptr<boost::asio::ip::tcp::socket> socket = nullptr,QWidget *parent = nullptr);

    std::mutex bufferMutex;

    int maxMessageId = 0;

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

    std::string jsonData;
    QString own;

    void playSystemSound(const QString& soundName);
    void insertMessageToSomeone(QString from,QString to, QString message);
    void insertMessageToChat(QString from, QString to, QString message, QVector<int> date, QString messageId);
    void createChatButton(QString name, bool isSearchUser);

    void closeEvent(QCloseEvent *event) override;

    //with socket
    void loadchat(QString from,QString to);
    void loadAllChats(QString current);
    void sendMessageToSomeone(QString from,QString to, QString message);
    void setToLoadAllChats(QString from,QString to);
    void loadNewMessages(QString from, QString to);
    void actionWithServer(boost::asio::ip::tcp::socket& socket,QJsonObject jsonObj);
    void listening();
    void connectToServer();

    ~messengerWin();

private:
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

    boost::asio::io_context& io;
    std::shared_ptr<boost::asio::ip::tcp::socket> socket;
    boost::asio::streambuf bufferFromServer;


    Ui::messengerWin *ui;
};

#endif // MESSENGERWIN_H
