#ifndef CONTACT_H
#define CONTACT_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>

class Contact
{
public:
    Contact(QWidget *, QVBoxLayout *, QLabel *, QLabel *);
    ~Contact();

public:
    QWidget *page;
    QVBoxLayout *layout;
    QLabel *name_label;
    QLabel *status_label;
    QString *last_message = new QString("");
};

#endif

