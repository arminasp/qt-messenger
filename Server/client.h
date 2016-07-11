#ifndef CLIENT_H
#define CLIENT_H

#include <QThread>
#include <QTcpSocket>

class Client : public QThread
{
    Q_OBJECT

public:
    explicit Client(int id, QObject *parent = 0);
    void run();

private:
    QTcpSocket *socket;
    QByteArray username;
    QByteArray destination;
    QByteArray user_id;
    bool finished = false;
    bool id_confirm = false;

signals:
    void error(QTcpSocket::SocketError socketerror);
    void receive_message(QByteArray);
    void lock();
    void unlock();

private slots:
    void receive();
    void on_disconnected();

public slots:
    void send_message(QByteArray, QByteArray, QByteArray);

public:
    QByteArray get_id();
    void set_username(const QByteArray);
    QByteArray get_username();
};

#endif
