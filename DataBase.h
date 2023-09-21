#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSql>
#include <QSqlError>
#include <QSqlDatabase>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QDebug>

class DataBase : public QObject
{
    Q_OBJECT
public:
    explicit DataBase(QObject *parent = nullptr);
    ~DataBase();
    // Подключение/отключение к базе данных
    void connectToDataBase();
    void closeDataBase();
private:
    QWidget* ParentWidget;
    // Объект базы данных, с которым будет производиться работа
    QSqlDatabase    db;
    // Открытие базы данных
    bool openDataBase();
signals:

};

#endif // DATABASE_H
