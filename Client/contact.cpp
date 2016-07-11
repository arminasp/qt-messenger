#include "contact.h"

Contact::Contact(QWidget *page, QVBoxLayout *layout, QLabel *name_label, QLabel *status_label)
{
    this->page = page;
    this->layout = layout;
    this->name_label = name_label;
    this->status_label = status_label;
}

Contact::~Contact()
{
}
