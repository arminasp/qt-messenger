#ifndef CONNECTION_H
#define CONNECTION_H

#include <QTcpSocket>

#include "user.h"

class Connection : public QTcpSocket
{
    Q_OBJECT

public:
    explicit Connection(QObject *parent = 0);
    ~Connection();
    QString server_ip = "127.0.0.1";
    int server_port = 8084;

private:
    QTcpSocket* socket = new QTcpSocket(this);
    User *user;
    bool status = false;
    QByteArray destination;

signals:
    void connected();
    void disconnected();
    void error();
    void add_task(QByteArray);

private slots:
    void receive_message();
    void on_socket_connected();
    void on_socket_error(QAbstractSocket::SocketError socketError);
    void on_socket_change(QAbstractSocket::SocketState socketState);
    void on_set_username(QByteArray);
    void on_set_user_id(QByteArray);


public:
    bool send_message(QByteArray message);
    void set_user(User *);
    bool get_status();
    void set_destination(QByteArray);
    QByteArray get_destination();
};

#endif
