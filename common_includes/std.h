#ifndef STD_H
#define STD_H

#include <QDebug>

#define server_database_path "database.db"
#define client_database_path "database.db"
#define max_contact_search 5

#define DEBUG_MODE
#define OUTPUT_MODE

#ifdef OUTPUT_MODE
#define OUTPUT(x) (qDebug() << "::" << x)
#else
#define OUTPUT(x)
#endif

#ifdef DEBUG_MODE
#define DEBUG(x) (qDebug() << "-->" << x)
#else
#define DEBUG(x)
#endif

#endif
