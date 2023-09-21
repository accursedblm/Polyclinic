#include "DataBase.h"

DataBase::DataBase(QObject *parent) : QObject(parent),
    ParentWidget{qobject_cast<QWidget*>(parent)}
{ }

DataBase::~DataBase()
{
    if(db.open()){
        db.close();
    }
}

//Метод для подключения к базе данных
void DataBase::connectToDataBase()
{
    //qDebug() << QDir::currentPath();
    //Перед подключением к базе данных производим проверку на её существование.
    //В зависимости от результата производим открытие базы данных или её восстановление
    /*!QFile(QDir::currentPath() + QDir::separator() + ".." + QDir::separator() + "Polyclinic"
              + QDir::separator() + "res" + QDir::separator() + "PolyClinic.mdb").exists()*/
    if(!QFile(QDir::currentPath() + QDir::separator() + "res" + QDir::separator() + "PolyClinic.mdb").exists())
    {
        QMessageBox::warning(ParentWidget,
                             tr("Ошибка запроса заполнения таблицы:"),
                             tr("База данных не найдена :("));
    }
    else
    {
        this->openDataBase();
    }
}


// Метод для открытия базы данных
bool DataBase::openDataBase()
{
    // База данных открывается по заданному пути
    // и имени базы данных, если она существует
    db = QSqlDatabase::addDatabase("QODBC");
    //db.setHostName(DATABASE_HOSTNAME);
    /*    db.setDatabaseName("DRIVER={Microsoft Access Driver (*.mdb, *.accdb)};FIL={MS Access};DBQ=" +
                       QDir::currentPath() + QDir::separator() + ".." + QDir::separator() +
                       "Polyclinic" + QDir::separator() + "res" + QDir::separator() +
                       "PolyClinic.mdb" ";Uid=Admin;Pwd=123;");*/
    db.setDatabaseName("DRIVER={Microsoft Access Driver (*.mdb, *.accdb)};FIL={MS Access};DBQ=" +
                       QDir::currentPath() + QDir::separator() + "res" + QDir::separator() +
                       "PolyClinic.mdb" ";Uid=Admin;Pwd=123;");
    //FIL={MS Access}
    if(db.open()){
        return true;
    } else {
        QMessageBox::warning(ParentWidget, tr("Ошибка подключение к базе данных:"), db.lastError().text());
        return false;
    }
}

// Методы закрытия базы данных
void DataBase::closeDataBase()
{
    db.close();
}
