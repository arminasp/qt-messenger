#include "user.h"

User::User()
{
}

// contacts
void User::add_contact(QByteArray contact)
{
    current_contacts.push_back(contact);
}

void User::remove_contact(QByteArray contact)
{
    for (int i = 0; i < current_contacts.size(); i++)
    {
        if (current_contacts[i] == contact)
            current_contacts.remove(i);
    }
}

bool User::find_contact(QByteArray contact)
{
    for (int i = 0; i < current_contacts.size(); i++)
    {
        if (current_contacts[i] == contact)
            return 1;
    }

    return 0;
}

void User::clear_contacts()
{
    current_contacts.clear();
}

// setters, getters
void User::set_username(QByteArray username)
{
    this->username = username;
}

void User::set_user_id(QByteArray user_id)
{
    this->user_id = user_id;
}

void User::set_password(QByteArray password)
{
    this->password = password;
}

QByteArray User::get_username()
{
    return username;
}

QByteArray User::get_user_id()
{
    return user_id;
}

QByteArray User::get_password()
{
    return password;
}
