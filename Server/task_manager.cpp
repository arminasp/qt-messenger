#include <QtSql>

#include "task_manager.h"
// path to common includes
//#include "\Messenger\common_includes\commands.h"

Task_manager::Task_manager()
{
}

void Task_manager::run()
{
    exec();
}

void Task_manager::on_lock()
{
    lock = true;
}

void Task_manager::on_unlock()
{
    if (receive_queue.size() != 0)
        handle_task();

    lock = false;
}

void Task_manager::disconnect()
{
    exit(0);
}

void Task_manager::on_add_task(QByteArray task)
{
    receive_queue.enqueue(task);

    if (!lock)
        handle_task();
}

void Task_manager::handle_task()
{
    QByteArray message = receive_queue.dequeue();
    QByteArray message_code;
    QTextStream stream(message);

    stream >> message_code;

    if (message[0] == SYSTEM_MESSAGE_START)
    {
        if (message_code == VALIDATION)
        {
            QByteArray signal, user_id, username, password;

            signal = VALIDATION;
            signal += ' ';
            stream >> user_id >> username >> password;

            if (!find_user(username))
            {
                if (validate_user(username, password))
                {
                    signal += GOOD;
                    signal += ' ' + username;
                    add_user(username);
                    send_message(VALIDATION, user_id, signal);
                }

                else
                {
                    signal += BAD;
                    send_message(VALIDATION, user_id, signal);
                }
            }

            else
            {
                signal += DUPLICATE;
                signal += ' ' + username;
                send_message(VALIDATION, user_id, signal);
            }
        }

        else if (message_code == SIGNUP)
        {
            QByteArray signal, user_id, username, password;

            signal = SIGNUP;
            signal += ' ';
            stream >> user_id >> username >> password;

            if (validate_signup(username, password))
            {
                signal += GOOD;
                signal += ' ' + username;
                send_message(VALIDATION, user_id, signal);
            }

            else
            {
                signal += BAD;
                send_message(VALIDATION, user_id, signal);
            }
        }

        else if (message_code == CONNECTED)
        {
            QByteArray signal, username;

            signal = CONNECTED;
            signal += ' ';
            stream >> username;

            signal += username;
            send_message(ALL, "default", signal);
        }

        else if (message_code == LOG)
        {
            QByteArray signal, username, destination;

            stream >> username >> destination;

            QByteArray log = get_log(username, destination);

            if (log != "")
            {
                signal = LOG;
                signal += ' ' + destination + ' ' + log;
                send_message(USER, username, signal);
            }
            else
            {
                signal = LOG_FINISH;
                signal += ' ' + destination;
                send_message(USER, username, signal);
            }
        }

        else if (message_code == UPDATE_LOG)
        {
            QByteArray signal, count, username, destination;
            stream >> username >> destination >> count;

            unsigned int old_count = count.toInt();
            unsigned int new_count = log_line_count(username, destination);

            if (old_count == new_count)
            {
                signal = LOG_FINISH;
                signal += ' ' + destination;
                send_message(USER, username, signal);
            }

            else
            {
                QByteArray log = get_log_part(username, destination, old_count, new_count);

                signal = LOG;
                signal += ' ' + destination + ' ' + log;
                send_message(USER, username, signal);
            }
        }

        else if (message_code == ADD_CONTACT)
        {
            QByteArray signal, username, contact;

            signal = ADD_CONTACT;
            signal += ' ';
            stream >> username >> contact;

            QByteArray contact_list = find_contact(contact);

            signal += contact_list;
            send_message(USER, username, signal);
        }


        else if (message_code == IS_ONLINE)
        {
            QByteArray signal, username, contact;

            signal = IS_ONLINE;
            signal += ' ';

            stream >> contact >> username;

            if (find_user(contact))
            {
                signal += contact + ' ';
                signal += GOOD;
                send_message(USER, username, signal);
            }

            else
            {
                signal += contact + ' ';
                signal += BAD;
                send_message(USER, username, signal);
            }

        }

        else if (message_code == EXIT)
        {
            QByteArray signal, user_id, username;

            signal = DISCONNECTED;
            signal += ' ';
            stream >> user_id >> username;

            if (find_user(username))
            {
                delete_user(username);
                signal += username;
                send_message(EXIT, user_id, signal);
                disconnect();
            }
        }
    }

    else if (message_code == SEND)
    {
        QByteArray username, destination;

        stream >> username >> destination;

        QString s = stream.readAll();
        s.remove(0, 1);
        QByteArray fixed_message = s.toUtf8();

        update_log(username, destination, fixed_message);
        send_message(USER, destination, message);
    }
}

// validation
bool Task_manager::validate_user(QByteArray username, QByteArray password)
{
    QSqlQuery query;

    query.exec("select * from users where username='" + username + "' and password='" + password + "';");

    if (query.next())
        return true;

    return false;
}

bool Task_manager::validate_signup(QByteArray username, QByteArray password)
{
    QSqlQuery query;

    if (query.exec("insert into users (username, password) values ('" + username + "', '" + password + "');"))
        return true;

    return false;
}

// log
QByteArray Task_manager::get_log(QByteArray username, QByteArray destination)
{
    QSqlQuery query;

    query.exec("select * from messages where(first_participant = '" + username + "' and second_participant = '" + destination + "')"
               "or (first_participant = '" + destination + "' and second_participant = '" + username + "') order by timestamp asc;");

    int first = query.record().indexOf("first_participant");
    int second = query.record().indexOf("second_participant");
    int message = query.record().indexOf("message");
    int timestamp = query.record().indexOf("timestamp");
    QByteArray log;

    while (query.next())
        log += query.value(first).toByteArray() + ' ' + query.value(second).toByteArray() + ' ' +
        query.value(timestamp).toByteArray() + ' ' + query.value(message).toByteArray() + '\n';

    return log;
}

bool Task_manager::update_log(QByteArray username, QByteArray destination, QByteArray message)
{
    QSqlQuery query;
    int timestamp = QDateTime::currentDateTime().toTime_t();

    if (query.exec("insert into messages (first_participant, second_participant, message, timestamp) values ('" + username + "', '" + destination + "', '" + message + "', " + QByteArray::number(timestamp) + ");"))
        return true;

    return false;
}

QByteArray Task_manager::get_log_part(QByteArray username, QByteArray destination, unsigned int old_count, unsigned int new_count)
{
    if (old_count >= new_count)
        return ERROR;

    QSqlQuery query;

    query.exec("select * from messages where (first_participant = '" + username + "' and second_participant = '" + destination + "')"
        "or (first_participant = '" + destination + "' and second_participant = '" + username + "') order by timestamp desc;");

    int first = query.record().indexOf("first_participant");
    int second = query.record().indexOf("second_participant");
    int message = query.record().indexOf("message");
    int timestamp = query.record().indexOf("timestamp");
    QByteArray log;

    for (unsigned int i = 0; i < (new_count - old_count); i++)
    {
        if (!query.next())
            break;

        log += query.value(first).toByteArray() + ' ' + query.value(second).toByteArray() + ' ' +
            query.value(timestamp).toByteArray() + ' ' + query.value(message).toByteArray() + '\n';
    }

    return log;
}

unsigned int Task_manager::log_line_count(QByteArray username, QByteArray destination)
{
    QSqlQuery query;

    query.exec("select count(*) from messages where(first_participant = '" + username + "' and second_participant = '" + destination + "')"
        "or (first_participant = '" + destination + "' and second_participant = '" + username + "');");

    query.next();

    return query.value(0).toInt();
}

// find contact
QByteArray Task_manager::find_contact(QByteArray username, int count)
{
    QSqlQuery query;

    query.exec("select * from users where username like '%" + username + "%' order by length(username)");

    int index = query.record().indexOf("username");
    QByteArray log;

    while (query.next())
    {
        log += query.value(index).toByteArray() + ' ';

        if (--count == 0)
            break;
    }

    log.remove(log.length() - 1, 1);

    return log;
}

// other
bool Task_manager::find_user(QByteArray username)
{
    QSqlQuery query;

    query.exec("select * from session where username='" + username + "';");

    if (query.next())
        return true;

    return false;
}

void Task_manager::add_user(QByteArray username)
{
    QSqlQuery query;

    query.exec("insert into session (username) values ('" + username + "');");
}

void Task_manager::delete_user(QByteArray username)
{
    QSqlQuery query;
    query.exec("delete from session where username='" + username + "';");
}
