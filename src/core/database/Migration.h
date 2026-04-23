#pragma once
#include <QSqlDatabase>

class Migration {
public:
    static bool run(QSqlDatabase &db);
};
