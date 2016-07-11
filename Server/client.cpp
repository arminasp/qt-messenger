#include "client.h"
// path to common includes
//#include "\Messenger\common_includes\commands.h"
//#include "\Messenger\common_includes\std.h"

Client::Client(int id, QObject *parent) : QThread(parent)
{
    user_id = QByteArray::number(id);
}

void Client::run()
{
    socket = new QTcpSocket();

    if (!socket->setSocketDescriptor(user_id.toInt()))
    {
        emit error(socket->error());
        return;
    }

    connect(socket, SIGNAL(readyRead()), this, SLOT(receive()), Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(on_disconnected()), Qt::DirectConnection);

    QByteArray message = ID;
    message += ' ' + user_id;
    send_message(ID, user_id, message);

    exec();
}

void Client::receive()
{
    const QByteArray message = socket->readAll();

    if (!id_confirm)
    {
        if (message == PASSWORD)
            id_confirm = true;
        else
            on_disconnected();
    }

    else
    {
        // test
        DEBUG("receiving: " + message);
        //end

        receive_message(message);
    }
}

void Client::send_message(QByteArray mode, QByteArray value, QByteArray message)
{
    if (!finished)
    {
        lock();

        // test
        DEBUG("sending: " + message);
        //end

        // send by id
        if (mode == ID)
        {
            socket->write(message);
            socket->flush();
        }

        if (mode == VALIDATION)
        {
            if (this->user_id == value)
            {
                QTextStream stream(message);
                QByteArray message_code[2];

                stream >> message_code[0] >> message_code[1];

                if (message_code[1] == GOOD)
                    stream >> this->username;

                socket->write(message);
                socket->flush();
            }
        }

        else if (mode == USER)
        {
            if (this->username == value)
            {
                socket->write(message);
                socket->flush();
            }
        }

        else if (mode == ALL)
        {
            socket->write(message);
            socket->flush();
        }
    }

    unlock();
}

void Client::on_disconnected()
{
    lock();

    QByteArray message = EXIT;
    message += ' ';

    finished = true;
    message += user_id + ' ' + this->username;
    receive_message(message);
    socket->deleteLater();

    unlock();
    exit(0);
}

// setters/getters
void Client::set_username(const QByteArray username)
{
    this->username = username;
}

QByteArray Client::get_id()
{
    return user_id;
}

QByteArray Client::get_username()
{
    return username;
}
