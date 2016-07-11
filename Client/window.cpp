// path to common includes
//#include "/Messenger/common_includes/std.h"

#include <QKeyEvent>
#include <QScrollBar>
#include <QProcess>
#include <QSound>

// path to common includes
//#include "/Messenger/common_includes/commands.h"
#include "window.h"
#include "task_manager.h"

// error messages
QString error[] = {
                    "Username and/or password is not correct. Please try again.",
                    "Cannot connect to server. Please try again later.",
                    "Please enter username and password.",
                    "Username already taken. Please try again.",
                    "Passwords do not match. Please try again.",
                    "Please confirm password.",
                    "Username cannot contain spaces. Please try again.",
                    "User already logged in.",
                    "Error occured. Please restart the program."};

QString info[] = { "Connecting to server..." };

// stylesheets
QString button_clicked = "QPushButton:enabled"
                         "{"
                         "  background: #585879;"
                         "  min-width: 40px;"
                         "  min-height:40px;"
                         "  border-radius: 0px;"
                         "  padding: 5px;"
                         "  outline: 0px;"
                         "}";

QString button_unclicked = "QPushButton:enabled"
                           "{"
                           "    background: #1c1c37;"
                           "    min-width: 40px;"
                           "    min-height:40px;"
                           "    border-radius: 0px;"
                           "    padding: 5px;"
                           "    outline: 0px;"
                           "}"
                           "QPushButton:pressed"
                           "{"
                           "    background: #585879;"
                           "}"
                           "QPushButton:hover"
                           "{"
                           "    background: #3D3D5A;"
                           "}";

QString user_style = "background: #73739b;"
                     "border-radius: 5px;"
                     "color: black;"
                     "font:10pt \"Open Sans\";";

QString contact_style = "background: white;"
                        "border-radius: 5px;"
                        "color: black;"
                        "font:10pt \"Open Sans\";";

QString user_bubble_style = "background: white;"
                            "border-radius: 20;"
                            "color:  #73739b;"
                            "font: 20pt \"Open Sans\";";

QString contact_bubble_style = "background: #73739b;"
                               "border-radius: 20;"
                               "color: white;"
                               "font: 20pt \"Open Sans\";";

QString add_button_style = "QPushButton"
                           "{"
                           "    font: 11pt \"Open Sans Semibold\";"
                           "    background:  #73739b;"
                           "    border-radius: 0px;"
                           "    min-height:25px;"
                           "    padding: 5px;"
                           "    outline: 0 px;"
                           "    color: white;"
                           "}"
                           "QPushButton:hover"
                           "{"
                           "    background: #7a7aa1;"
                           "}"
                           "QPushButton:pressed"
                           "{"
                           "    background: #8f8fb3;"
                           "}";


Window::Window(QWidget *parent) : QMainWindow(parent)
{
    ui.setupUi(this);
    ui.stackedWidget->setCurrentWidget(ui.login);
    this->statusBar()->hide();
    this->menuBar()->hide();
    this->setMaximumSize(ui.login->minimumSize());

    // database
    db.setDatabaseName(client_database_path);

    if (!QFile::exists(client_database_path))
    {
        if (db.open())
        {
            QSqlQuery query;

            query.exec("create table Messages (first_participant text, second_participant text, message text, timestamp integer);");
            query.exec("create table Connection (ip_address text, port integer);");
            query.exec("insert into Connection (ip_address, port) values ('52.40.250.34', 8084)");
        }
    }

    if (!db.isOpen())
    {
        if (!db.open())
        {
            ui.login_error->setText(error[8]);
            ui.signup_error->setText(error[8]);
        }
    }

    // signal/slot connection
    // context menu
    ui.contacts->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui.contacts, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(ShowContextMenu(const QPoint &)));

    // scrollbars
    QScrollBar* scrollbar = ui.scrollArea->verticalScrollBar();
    QScrollBar* scrollbar_1 = ui.scrollArea_2->verticalScrollBar();
    QScrollBar* scrollbar_2 = ui.scrollArea_4->verticalScrollBar();

    connect(scrollbar, SIGNAL(rangeChanged(int, int)), this, SLOT(auto_scroll(int, int)));
    connect(scrollbar_1, SIGNAL(rangeChanged(int, int)), this, SLOT(auto_scroll(int, int)));
    connect(scrollbar_2, SIGNAL(rangeChanged(int, int)), this, SLOT(auto_scroll(int, int)));

    // connection
    connection = new Connection();
    connection->set_user(&user);

    connect(connection, SIGNAL(connected()), this, SLOT(on_socket_connected()));
    connect(connection, SIGNAL(error()), this, SLOT(on_socket_error()));

    // info message
    ui.login_error->setText(info[0]);
    ui.signup_error->setText(info[0]);

    // task manager
    task_manager = new Task_manager();
    task_manager->start();

    connect(this, SIGNAL(lock()), task_manager, SLOT(on_lock()));
    connect(this, SIGNAL(unlock()), task_manager, SLOT(on_unlock()));
    connect(connection, SIGNAL(add_task(QByteArray)), task_manager, SLOT(on_add_task(QByteArray)));
    connect(task_manager, SIGNAL(login_true()), this, SLOT(on_login_true()));
    connect(task_manager, SIGNAL(login_false()), this, SLOT(on_login_false()));
    connect(task_manager, SIGNAL(login_duplicate()), this, SLOT(on_login_duplicate()));
    connect(task_manager, SIGNAL(signup_true()), this, SLOT(on_signup_true()));
    connect(task_manager, SIGNAL(signup_false()), this, SLOT(on_signup_false()));
    connect(task_manager, SIGNAL(contact_online(QByteArray)), this, SLOT(on_contact_online(QByteArray)));
    connect(task_manager, SIGNAL(contact_offline(QByteArray)), this, SLOT(on_contact_offline(QByteArray)));
    connect(task_manager, SIGNAL(update_log(QString)), this, SLOT(on_update_log(QString)));
    connect(task_manager, SIGNAL(create_log(QByteArray, QByteArray)), this, SLOT(on_create_log(QByteArray, QByteArray)));
    connect(task_manager, SIGNAL(someone_connected(QByteArray)), this, SLOT(on_someone_connected(QByteArray)));
    connect(task_manager, SIGNAL(someone_disconnected(QByteArray)), this, SLOT(on_someone_disconnected(QByteArray)));
    connect(task_manager, SIGNAL(receive_message(QByteArray, QByteArray, QString)), this, SLOT(on_receive_message(QByteArray, QByteArray, QString)));
    connect(task_manager, SIGNAL(add_contacts(QStringList)), this, SLOT(on_add_contacts(QStringList)));
    connect(task_manager, SIGNAL(set_username(QByteArray)), connection, SLOT(on_set_username(QByteArray)));
    connect(task_manager, SIGNAL(set_user_id(QByteArray)), connection, SLOT(on_set_user_id(QByteArray)));
    connect(task_manager, SIGNAL(reconnect()), this, SLOT(on_login_button_clicked()));

    // Contact queue fill
    Contact *contact = new Contact(ui.page, ui.output, ui.contact_label, ui.contact_status);
    Contact *contact_1 = new Contact(ui.page_1, ui.output_2, ui.contact_label_2, ui.contact_status_2);
    Contact *contact_2 = new Contact(ui.page_2, ui.output_3, ui.contact_label_4, ui.contact_status_4);

    contact_queue.enqueue(contact);
    contact_queue.enqueue(contact_1);
    contact_queue.enqueue(contact_2);

    // status gif
    ui.status_label->setMovie(status_gif);
    ui.login_status_label->setMovie(status_gif);
    ui.signup_status_label->setMovie(status_gif);
    ui.status_label->hide();
    ui.login_status_label->hide();
    ui.signup_status_label->hide();
}

// connection
void Window::on_socket_connected()
{
    emit(lock());

    connection->send_message(PASSWORD);

    if ((ui.stackedWidget->currentWidget() == ui.login) || (ui.stackedWidget->currentWidget() == ui.signup))
    {
        ui.login_error->setText("");
        ui.signup_error->setText("");
    }

    if (status_gif->Running)
    {
        status_gif->stop();
        ui.status_label->hide();
        ui.login_status_label->hide();
        ui.signup_status_label->hide();
        ui.status_text_label->hide();
    }

    emit(unlock());
}

void Window::on_socket_error()
{
    emit(lock());

    if ((ui.stackedWidget->currentWidget() == ui.login) || (ui.stackedWidget->currentWidget() == ui.signup))
    {
        ui.login_error->setText(error[1]);
        ui.signup_error->setText(error[1]);
    }

    ui.status_label->show();
    ui.status_text_label->show();
    ui.login_status_label->show();
    ui.signup_status_label->show();
    status_gif->start();
    emit(unlock());
}

// login, signup
void Window::on_login_button_clicked()
{
    emit(lock());
    if (connection->get_status())
    {
        QString username_check = ui.login_username->text();
        QString password_check = ui.login_password->text();

        if ((username_check == "") || (password_check == ""))
            ui.login_error->setText(error[2]);

        else
        {
            QByteArray signal = VALIDATION;
            signal += ' ' + user.get_user_id() + ' ' + username_check + ' ' + password_check;
            connection->send_message(signal);
        }

        ui.login_username->setFocus();
    }

    emit(unlock());
}

void Window::on_signup_button_clicked()
{
    emit(lock());

    if (connection->get_status())
    {
        QString username_check = ui.signup_username->text();
        QString password_check = ui.signup_password->text();
        QString password_confirm = ui.signup_confirm->text();

        if ((username_check == "") || (password_check == ""))
            ui.signup_error->setText(error[2]);

        else if ((username_check != "") && (password_check != "") && (password_confirm == ""))
            ui.signup_error->setText(error[5]);

        else if (password_check != password_confirm)
            ui.signup_error->setText(error[4]);

        else if (username_check.contains(' '))
            ui.signup_error->setText(error[6]);

        else
        {
            QByteArray signal = SIGNUP;
            signal += ' ' + user.get_user_id() + ' ' + username_check + ' ' + password_check;
            connection->send_message(signal);
        }
    }
    emit(unlock());
}

void Window::on_login_signup_clicked()
{
    emit(lock());
    ui.stackedWidget->setCurrentWidget(ui.signup);
    emit(unlock());
}

void Window::on_signup_login_clicked()
{
    emit(lock());
    ui.stackedWidget->setCurrentWidget(ui.login);
    emit(unlock());
}

void Window::on_signup_true()
{
    emit(lock());

    ui.login_username->setText(ui.signup_username->text());
    ui.login_password->setText(ui.signup_password->text());
    on_login_button_clicked();

    emit(unlock());
}

void Window::on_signup_false()
{
    emit(lock());
    ui.signup_error->setText(error[3]);
    emit(unlock());
}

void Window::on_login_true()
{
    emit(lock());

    QSqlQuery query;

    query.exec("SELECT * FROM sqlite_master WHERE name ='" + user.get_username() + "' and type='table'");

    if (!query.next())
        query.exec("create table " + user.get_username() + " (username text primary key);");

    ui.status_text_label->hide();
    user.set_password(ui.login_password->text().toUtf8());
    ui.stackedWidget->setCurrentWidget(ui.main);
    ui.chat_windows->setCurrentWidget(ui.blank);
    this->setFixedWidth(600);
    this->setMaximumSize(QSize(16777215, 16777215));
    this->menuBar()->show();
    ui.send_button->setEnabled(false);
    ui.username_label->setText(ui.login_username->text());
    ui.input->setFocus();
    refresh_contacts();
    on_contact_button_clicked();

    emit(unlock());
}

void Window::on_login_false()
{
    emit(lock());
    ui.login_error->setText(error[0]);
    emit(unlock());
}

void Window::on_login_duplicate()
{
    emit(lock());
    ui.login_error->setText(error[7]);
    emit(unlock());
}

// chat
void Window::on_input_textChanged()
{
    emit(lock());

    if (ui.contacts->selectedItems().count() != 0)
    {
        int message_length = ui.input->toPlainText().length();

        if (message_length > 0)
        {
            ui.send_button->setEnabled(true);

            if (ui.input->toPlainText()[ui.input->toPlainText().length() - 1] == '\n')
            {
                if (message_length != 1)
                {
                    ui.input->textCursor().deletePreviousChar();
                    on_send_button_clicked();
                }

                else
                    ui.input->textCursor().deletePreviousChar();
            }
        }

        else
            ui.send_button->setEnabled(false);
    }

    int line_count = ui.input->document()->lineCount();
    int input_frame_box_size = 48;
    int input_size = 48;
    int input_frame_size = 75;
    int inc = 18;

    if (line_count == 1)
    {
        ui.input->setFixedHeight(input_size);
        ui.input_frame_box->setFixedHeight(input_frame_box_size);
        ui.input_frame->setFixedHeight(input_frame_size);
        ui.horizontalLayout_3->setContentsMargins(0, 11, 0, 0);
    }

    else if (line_count == 2)
    {
        ui.input->setFixedHeight(input_size);
        ui.input_frame_box->setFixedHeight(input_frame_box_size);
        ui.input_frame->setFixedHeight(input_frame_size);
        ui.horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
    }

    else if (line_count == 3)
    {
        ui.input->setFixedHeight(input_size + inc);
        ui.input_frame_box->setFixedHeight(input_frame_box_size + inc);
        ui.input_frame->setFixedHeight(input_frame_size + inc);
        ui.horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
    }

    else if (line_count == 4)
    {
        ui.input->setFixedHeight(input_size + 2 * inc);
        ui.input_frame_box->setFixedHeight(input_frame_box_size + 2 * inc);
        ui.input_frame->setFixedHeight(input_frame_size + 2 * inc);
        ui.horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
    }

    emit(unlock());
}

void Window::on_send_button_clicked()
{
    emit(lock());

    if (connection->get_status())
    {
        QString message = ui.input->toPlainText();
        QString new_message;
        bool is_message_correct = false;

        for (int i = 0; i < message.length(); i++)
        {
            if (message[i] > 31)
            {
                new_message += message[i];

                if (message[i] > 32)
                    is_message_correct = true;
            }
        }

        if (is_message_correct)
        {
            QByteArray username = user.get_username();
            QByteArray destination = connection->get_destination();

            ui.input->clear();
            ui.input->setFocus();
            ui.send_button->setEnabled(false);

            QByteArray signal = SEND;
            signal += ' ' + username + ' ' + destination + ' ' + new_message;

            if (connection->send_message(signal))
            {
                format_output("message", destination, username, new_message);
                update_log_history(username, destination, new_message);
            }
        }
    }
    emit(unlock());
}

void Window::on_receive_message(QByteArray sender, QByteArray destination, QString message)
{
    emit(lock());

    if (connection->get_destination() != sender)
        color_notify(sender);

    if (message.contains(PHOTO))
        format_output("photo", sender, sender, message);
    else
        format_output("message", sender, sender, message);

    /* // Sounds
    if (user.get_username() == name)
        QSound::play("sounds/outgoing.wav");

    if ((this->isMinimized()) && (user.get_username() != name))
        QSound::play("sounds/incoming.wav");
    */

    update_log_history(sender, destination, message);

    emit(unlock());
}

void Window::color_notify(QByteArray sender)
{
    for (int i = 0; i < ui.contacts->count(); i++)
    {
        if (ui.contacts->item(i)->text() == sender)
        {
            ui.contacts->item(i)->setBackgroundColor(QColor(42, 42, 76));
            ui.contacts->item(i)->setTextColor(QColor(255, 255, 255));
        }
    }
}

void Window::format_output(QString mode, QString contact, QByteArray name, QString message)
{
    QVBoxLayout *current_window = chat_map[contact]->layout;
    QString *last_message = chat_map[contact]->last_message;
    QLabel *message_label = new QLabel();
    QByteArray new_message;

    if (mode == "photo")
        message.replace(PHOTO, "");

    if (mode == "message")
    {
        int line_break_count = 0;
        int max_line_length = 30;

        for (int i = 0; i < message.length(); i++)
        {
            if (message.at(i) > 32)
            {
                new_message += message.at(i);
                line_break_count++;
            }

            if (message.at(i) == 32)
            {
                new_message += message.at(i);
                line_break_count = 0;
            }

            if (line_break_count == max_line_length)
            {
                new_message += '\n';
                line_break_count = 0;
            }
        }

        new_message.replace("&", "&amp;");
        new_message.replace("<", "&lt;");
        new_message.replace(">", "&gt;");

        new_message.replace("(smile)", "<img src=:/small_emoticons/Resources/small_emoticons/smile.png>");
        new_message.replace("(wink)", "<img src=:/small_emoticons/Resources/small_emoticons/wink.png>");
        new_message.replace("(laugh)", "<img src=:/small_emoticons/Resources/small_emoticons/laugh.png>");
        new_message.replace("(happy)", "<img src=:/small_emoticons/Resources/small_emoticons/happy.png>");
        new_message.replace("(shy)", "<img src=:/small_emoticons/Resources/small_emoticons/shy.png>");
        new_message.replace("(angry)", "<img src=:/small_emoticons/Resources/small_emoticons/angry.png>");
    }


    if (name == user.get_username())
    {
        message_label->setStyleSheet(user_style);

        QHBoxLayout *layout = new QHBoxLayout;
        QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding);

        layout->setSpacing(10);
        message_label->setMargin(10);
        message_label->setWordWrap(true);

        if (mode == "photo")
        {
            QPixmap p(message);

            message_label->setMaximumSize(300, 200);
            int w = message_label->width();
            int h = message_label->height();

            message_label->setPixmap(p.scaled(w, h, Qt::KeepAspectRatio));
        }

        else
        {
            message_label->setMaximumWidth(400);
            message_label->setText(new_message);
        }

        QLabel * bubble = new QLabel();

        if (*last_message != "user")
        {
            bubble->setAlignment(Qt::AlignCenter);
            bubble->setStyleSheet(user_bubble_style);
            QString bubble_letter(toupper(user.get_username()[0]));
            bubble->setText(bubble_letter);
        }

        bubble->setFixedSize(40, 40);

        layout->addSpacerItem(spacer);
        layout->addWidget(message_label);
        layout->addWidget(bubble);
        current_window->addLayout(layout);
        *last_message = "user";
    }

    else
    {
        message_label->setStyleSheet(contact_style);

        QHBoxLayout *layout = new QHBoxLayout;
        QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding);

        layout->setSpacing(10);
        message_label->setMargin(10);
        message_label->setWordWrap(true);

        if (mode == "photo")
        {
            QPixmap p(message);

            message_label->setMaximumSize(300, 200);
            int w = message_label->width();
            int h = message_label->height();

            message_label->setPixmap(p.scaled(w, h, Qt::KeepAspectRatio));
        }

        else
        {
            message_label->setMaximumWidth(400);
            message_label->setText(new_message);
        }

        QLabel * bubble = new QLabel();

        if (*last_message != "contact")
        {
            bubble->setAlignment(Qt::AlignCenter);
            bubble->setStyleSheet(contact_bubble_style);
            QString bubble_letter(contact.at(0).toUpper());
            bubble->setText(bubble_letter);
        }

        bubble->setFixedSize(40, 40);
        layout->addWidget(bubble);

        layout->addWidget(message_label);
        layout->addSpacerItem(spacer);
        current_window->addLayout(layout);
        *last_message = "contact";
    }
}

// emoticons
void Window::on_smile_button_clicked()
{
    emit(lock());

    if (ui.frame->width() == 0)
    {
        window_width = this->width();
        ui.frame->setMinimumWidth(180);
        this->setMinimumWidth(780);
    }

    else
    {
        ui.frame->setMinimumWidth(0);
        this->setMinimumWidth(600);
        this->resize(window_width, this->height());
    }

    emit(unlock());
}

void Window::on_smile_button_2_clicked()
{
    emit(lock());
    add_emoticon("smile");
    emit(unlock());
}

void Window::on_smile_button_3_clicked()
{
    emit(lock());
    add_emoticon("wink");
    emit(unlock());
}

void Window::on_smile_button_4_clicked()
{
    emit(lock());
    add_emoticon("laugh");
    emit(unlock());
}

void Window::on_smile_button_5_clicked()
{
    emit(lock());
    add_emoticon("happy");
    emit(unlock());
}

void Window::on_smile_button_6_clicked()
{
    emit(lock());
    add_emoticon("shy");
    emit(unlock());
}

void Window::on_smile_button_7_clicked()
{
    emit(lock());
    add_emoticon("angry");
    emit(unlock());
}

void Window::add_emoticon(QString emoticon)
{
    ui.input->moveCursor(QTextCursor::End);
    // ui.input->append("<img src=:/small_emoticons/small_emoticons/" + emoticon + ".png>");
    ui.input->insertPlainText(" (" + emoticon + ") ");
    ui.input->moveCursor(QTextCursor::End);
    ui.input->setFocus();
}

// search contact menu
void Window::on_add_button_clicked()
{
    emit(lock());

    ui.add_button->setStyleSheet(button_clicked);
    ui.contact_button->setStyleSheet(button_unclicked);
    ui.contact_widget->setCurrentWidget(ui.add_page);
    ui.add_line->clear();
    ui.add_line->setFocus();

    emit(unlock());
}

void Window::on_add_line_textChanged(const QString &arg1)
{
    emit(lock());

    if (connection->get_status())
    {
        if (ui.add_line->text().length() != 0)
        {
            QByteArray signal = ADD_CONTACT;

            signal += ' ' + user.get_username() + ' ' + ui.add_line->text();
            connection->send_message(signal);
        }

        else
            clear_layout(ui.add_layout);

    }

    emit(unlock());
}

void Window::on_add_contacts(QStringList list)
{
    emit(lock());

    clear_layout(ui.add_layout);
    contacts_found.clear();

    for (int i = 0, count = 0; i < list.size(); i++)
    {
        QByteArray contact = list.at(i).toUtf8();

        if (!user.find_contact(contact) && (contact != user.get_username()))
        {
            QPushButton * button = new QPushButton();

            button->setText(contact);
            button->setStyleSheet(add_button_style);

            contacts_found.push_back(contact);

            if (count == 0)
                connect(button, SIGNAL(clicked()), this, SLOT(on_0_clicked()));

            else if (count == 1)
                connect(button, SIGNAL(clicked()), this, SLOT(on_1_clicked()));

            else if (count == 2)
                connect(button, SIGNAL(clicked()), this, SLOT(on_2_clicked()));

            else if (count == 3)
                connect(button, SIGNAL(clicked()), this, SLOT(on_3_clicked()));

            else if (count == 4)
                connect(button, SIGNAL(clicked()), this, SLOT(on_4_clicked()));

            ui.add_layout->addWidget(button);
            count++;
        }
    }

    emit(unlock());
}

void Window::on_0_clicked()
{
    emit(lock());
    save_contact(contacts_found[0]);
    emit(unlock());
}

void Window::on_1_clicked()
{
    emit(lock());
    save_contact(contacts_found[1]);
    emit(unlock());
}

void Window::on_2_clicked()
{
    emit(lock());
    save_contact(contacts_found[2]);
    emit(unlock());
}

void Window::on_3_clicked()
{
    emit(lock());
    save_contact(contacts_found[3]);
    emit(unlock());
}

void Window::on_4_clicked()
{
    emit(lock());
    save_contact(contacts_found[4]);
    emit(unlock());
}

void Window::save_contact(QString contact)
{
    if (ui.contacts->count() < MAX_CONTACTS)
    {
        QSqlQuery query;

        user.add_contact(contact.toUtf8());
        query.exec("insert into " + user.get_username() + " (username) values ('" + contact + "');");

        refresh_contacts();
        contacts_found.removeAt(contacts_found.indexOf(contact));
        on_add_contacts(contacts_found);
    }
}

// contact menu
void Window::on_contact_button_clicked()
{
    emit(lock());

    ui.contact_button->setStyleSheet(button_clicked);
    ui.add_button->setStyleSheet(button_unclicked);
    ui.contact_widget->setCurrentWidget(ui.view_page);
    ui.input->setFocus();

    emit(unlock());
}

void Window::on_contacts_itemSelectionChanged()
{
    emit(lock());

    if (!delete_mode)
    {
        setAcceptDrops(true);

        QString selected = ui.contacts->currentItem()->text();
        QMap<QString, Contact*>::const_iterator itr = chat_map.find(selected);

        if (itr != chat_map.end())
        {
            ui.chat_windows->setCurrentWidget(chat_map[selected]->page);

            if (ui.input->toPlainText().length() != 0)
                ui.send_button->setEnabled(true);

            if (ui.contacts->currentItem()->backgroundColor() != QColor(199, 199, 216))
            {
                ui.contacts->currentItem()->setBackgroundColor(QColor(199, 199, 216));
                ui.contacts->currentItem()->setTextColor(QColor(0, 0, 0));
            }

            connection->set_destination(selected.toUtf8());
        }
    }

    else
    {
        setAcceptDrops(false);
        ui.chat_windows->setCurrentWidget(ui.blank);
        delete_mode = false;
    }

    emit(unlock());
}

void Window::load_log(QString contact)
{
    QMap<QString, Contact*>::const_iterator itr = chat_map.find(contact);

    if (itr == chat_map.end())
    {
        chat_map.insert(contact, contact_queue.dequeue());
        chat_map[contact]->name_label->setText(contact);
        chat_map[contact]->status_label->setText("Offline");

        QSqlQuery query;
        QString username = user.get_username();

        query.exec("select count(*) from messages where(first_participant = '" + username + "' and second_participant = '" + contact + "')"
            "or (first_participant = '" + contact + "' and second_participant = '" + username + "');");

        query.next();

        if (query.value(0).toInt() == 0)
        {
            QByteArray signal = LOG;

            signal += ' ' + username + ' ' + contact;

            if (!connection->send_message(signal))
                on_update_log(contact);
        }

        else
        {
            QByteArray signal = UPDATE_LOG;

            signal += ' ' + username + ' ' + contact + ' ' + query.value(0).toByteArray();

            if (!connection->send_message(signal))
                on_update_log(contact);
        }
    }
}

void Window::refresh_contacts()
{
    ui.contacts->clear();
    user.clear_contacts();

    QSqlQuery query;

    query.exec("select * from " + user.get_username() + ";");

    int username = query.record().indexOf("username");

    while (query.next())
    {
        QString contact = query.value(username).toString();

        ui.contacts->addItem(contact);
        user.add_contact(contact.toUtf8());
    }

    for (int i = 0; i < ui.contacts->count(); i++)
    {
        QListWidgetItem *item = ui.contacts->item(i);
        item->setSizeHint(QSize(item->sizeHint().width(), 40));
    }

    log_count = ui.contacts->count();

    if (log_count != 0)
        load_log(ui.contacts->item(--log_count)->text());
}

void Window::on_contact_online(QByteArray contact)
{
    emit(lock());

    QMap<QString, Contact*>::const_iterator itr = chat_map.find(contact);

    if (itr != chat_map.end())
        chat_map[contact]->status_label->setText("Online");

    if (log_count != 0)
    {
        QByteArray signal = IS_ONLINE;

        signal += ' ' + ui.contacts->item(--log_count)->text() + ' ' + user.get_username();
        connection->send_message(signal);
    }

    else
    {
        QByteArray signal = CONNECTED;

        signal += ' ' + user.get_username();
        connection->send_message(signal);
    }

    emit(unlock());
}

void Window::on_contact_offline(QByteArray contact)
{
    emit(lock());

    QMap<QString, Contact*>::const_iterator itr = chat_map.find(contact);

    if (itr != chat_map.end())
        chat_map[contact]->status_label->setText("Offline");

    if (log_count != 0)
    {
        QByteArray signal = IS_ONLINE;

        signal += ' ' + ui.contacts->item(--log_count)->text() + ' ' + user.get_username();
        connection->send_message(signal);
    }

    else
    {
        QByteArray signal = CONNECTED;

        signal += ' ' + user.get_username();
        connection->send_message(signal);
    }

    emit(unlock());
}

void Window::on_someone_disconnected(QByteArray name)
{
    emit(lock());

    QMap<QString, Contact*>::const_iterator itr = chat_map.find(name);

    if (itr != chat_map.end())
    {
        if (chat_map[name]->status_label->text() != "Offline")
            chat_map[name]->status_label->setText("Offline");
    }

    emit(unlock());
}

void Window::on_someone_connected(QByteArray name)
{
    emit(lock());

    QMap<QString, Contact*>::const_iterator itr = chat_map.find(name);

    if (itr != chat_map.end())
    {
        if (chat_map[name]->status_label->text() != "Online")
            chat_map[name]->status_label->setText("Online");
    }

    emit(unlock());
}

void Window::on_delete_contact()
{
    emit(lock());

    delete_mode = true;
    QSqlQuery query;
    QByteArray marked = ui.contacts->currentItem()->text().toUtf8();

    ui.contacts->currentItem()->setSelected(false);
    user.remove_contact(marked);
    query.exec("delete from " + user.get_username() + " where username ='" + marked + "';");

    clear_layout(chat_map[marked]->layout);
    contact_queue.enqueue(chat_map[marked]);
    chat_map.remove(marked);
    refresh_contacts();

    emit(unlock());
}

void Window::ShowContextMenu(const QPoint &pos)
{
    if (ui.contacts->selectedItems().count() != 0)
    {
        QMenu contextMenu(tr("Context menu"), this);

        QAction action1("Delete", this);
        connect(&action1, SIGNAL(triggered()), this, SLOT(on_delete_contact()));
        contextMenu.addAction(&action1);

        contextMenu.exec(QCursor::pos());
    }
}

// log
void Window::on_create_log(QByteArray contact, QByteArray log)
{
    emit(lock());

    QByteArrayList log_part = log.split('\n');
    QSqlQuery query;

    for (int i = 0; i < (log_part.size() - 1); i++)
    {
        QTextStream stream(log_part[i]);
        QString username, destination, timestamp;

        stream >> username >> destination >> timestamp;
        QString message = stream.readAll();
        message.remove(0, 1);
        query.exec("insert into messages (first_participant, second_participant, message, timestamp) values ('" + username + "', '" + destination + "', '" + message + "', '" + timestamp + "');");
    }
    emit(unlock());
    on_update_log(contact);
}

void Window::on_update_log(QString contact)
{
    emit(lock());

    QVBoxLayout *current_window = chat_map[contact]->layout;

    clear_layout(current_window);

    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    current_window->addSpacerItem(spacer);

    QSqlQuery query;
    QByteArray username = user.get_username();

    query.exec("select distinct first_participant, second_participant,"
               " message, timestamp from messages where (first_participant = '" + username + "' "
               "and second_participant = '" + contact + "') or "
               "(first_participant = '" + contact + "' and second_participant = '"
               + username + "') order by timestamp;");

    int first = query.record().indexOf("first_participant");
    int message = query.record().indexOf("message");

    while (query.next())
    {
        if (query.value(message).toString().contains(PHOTO))
            format_output("photo", contact, query.value(first).toByteArray(), query.value(message).toByteArray());
        else
            format_output("message", contact, query.value(first).toByteArray(), query.value(message).toByteArray());
    }

    ui.input->setFocus();

    if (log_count != 0)
        load_log(ui.contacts->item(--log_count)->text());

    else
    {
        log_count = ui.contacts->count();

        if (log_count != 0)
        {
            QByteArray signal = IS_ONLINE;

            signal += ' ' + ui.contacts->item(--log_count)->text() + ' ' + username;
            connection->send_message(signal);
        }
    }

    emit(unlock());
}

void Window::update_log_history(QByteArray username, QByteArray destination, QString message)
{
    QSqlQuery query;
    int timestamp = QDateTime::currentDateTime().toTime_t();

    query.exec("insert into messages (first_participant, second_participant, message, timestamp) values ('" + username + "', '" + destination + "', '" + message + "', " + QByteArray::number(timestamp) + ");");
}

// other
void Window::keyPressEvent(QKeyEvent * e)
{
    if ((e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Return))
    {

        if (ui.login->isVisible())
            on_login_button_clicked();

        else if (ui.signup->isVisible())
            on_signup_button_clicked();
    }

    else if ((e->key() == Qt::Key_Escape) || (e->key() == Qt::Key_Backspace))
    {
        if (ui.signup->isVisible())
            on_signup_login_clicked();
    }
}

void Window::auto_scroll(int min, int max)
{
    Q_UNUSED(min);

    if (ui.chat_windows->currentIndex() == 0)
        ui.scrollArea->verticalScrollBar()->setValue(max);

    else if (ui.chat_windows->currentIndex() == 1)
        ui.scrollArea_2->verticalScrollBar()->setValue(max);

    else if (ui.chat_windows->currentIndex() == 2)
        ui.scrollArea_4->verticalScrollBar()->setValue(max);
}

void Window::on_actionLog_out_triggered()
{
    QProcess::startDetached(QApplication::applicationFilePath());
    exit(12);
}

void Window::clear_layout(QLayout* layout)
{

    if (layout != NULL)
    {
        while (QLayoutItem* item = layout->takeAt(0))
        {
            if (item->layout())
            {
                clear_layout(item->layout());
                delete item->layout();
            }

            if (item->widget())
                delete item->widget();

            if (item->spacerItem())
                delete item->spacerItem();
        }
    }
}
