#ifndef USER_H
#define USER_H

#include <QByteArray>
#include <QVector>

class User
{
public:
    User();

private:
    QByteArray username, user_id, password;
    QVector<QByteArray> current_contacts;

public:
    void add_contact(QByteArray);
    void remove_contact(QByteArray);
    bool find_contact(QByteArray);
    void clear_contacts();
    void set_user_id(QByteArray);
    void set_username(QByteArray);
    void set_password(QByteArray);
    QByteArray get_username();
    QByteArray get_user_id();
    QByteArray get_password();
};

#endif

