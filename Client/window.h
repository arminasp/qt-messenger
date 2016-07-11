#ifndef WINDOW_H
#define WINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtSql>
#include <QMovie>

#include "ui_window.h"
#include "connection.h"
#include "task_manager.h"
#include "user.h"
#include "contact.h"

#define MAX_CONTACTS 3

class Window : public QMainWindow
{
    Q_OBJECT

public:
    Window(QWidget *parent = 0);

private:
    Ui::WindowClass ui;
    Connection *connection;
    User user;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    int window_width;
    QStringList contacts_found;
    bool delete_mode = false;
    QMap<QString, Contact*> chat_map;
    QQueue<Contact*> contact_queue;
    Task_manager *task_manager;
    int log_count = 0;
    QMovie *status_gif = new QMovie(":/Resources/loading.gif");

signals:
    void lock();
    void unlock();

private slots:
    void on_socket_connected();
    void on_socket_error();
    void on_send_button_clicked();
    void on_login_button_clicked();
    void on_login_signup_clicked();
    void on_signup_login_clicked();
    void on_signup_button_clicked();
    void on_add_button_clicked();
    void on_contact_button_clicked();
    void on_0_clicked();
    void on_1_clicked();
    void on_2_clicked();
    void on_3_clicked();
    void on_4_clicked();
    void on_smile_button_clicked();
    void on_smile_button_2_clicked();
    void on_smile_button_3_clicked();
    void on_smile_button_4_clicked();
    void on_smile_button_5_clicked();
    void on_smile_button_6_clicked();
    void on_smile_button_7_clicked();
    void on_login_true();
    void on_login_false();
    void on_login_duplicate();
    void on_signup_true();
    void on_signup_false();
    void on_receive_message(QByteArray, QByteArray, QString);
    void on_update_log(QString);
    void on_create_log(QByteArray, QByteArray);
    void on_contacts_itemSelectionChanged();
    void on_input_textChanged();
    void on_actionLog_out_triggered();
    void auto_scroll(int, int);
    void on_add_line_textChanged(const QString &arg1);
    void on_add_contacts(QStringList);
    void on_contact_online(QByteArray);
    void on_contact_offline(QByteArray);
    void on_someone_disconnected(QByteArray);
    void on_someone_connected(QByteArray);
    void on_delete_contact();
    void ShowContextMenu(const QPoint &pos);

private:
    void add_emoticon(QString);
    void color_notify(QByteArray);
    void keyPressEvent(QKeyEvent * e);
    void clear_layout(QLayout*);
    void format_output(QString, QString, QByteArray, QString);
    void refresh_contacts();
    void update_log_history(QByteArray, QByteArray, QString);
    void save_contact(QString);
    void load_log(QString contact);
};

#endif
