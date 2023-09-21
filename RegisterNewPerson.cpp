#include "RegisterNewPerson.h"
#include "ui_RegisterNewPerson.h"

RegisterNewPerson::RegisterNewPerson(QWidget *parent, QMap<QString,QString>* _Result) :
    QDialog(parent), Result(_Result),
    ui(new Ui::RegisterNewPerson)
{
    QSqlQuery query;
    query.exec("SELECT * FROM People");
    ui->setupUi(this);
}


RegisterNewPerson::~RegisterNewPerson()
{
    delete ui;
}

void RegisterNewPerson::WriteInBase()
{
    QString ID;
    QString Name = ui->lineEdit_Name->text();
    QString LastName = ui->lineEdit_LastName->text();
    QString SurName =  ui->lineEdit_SurName->text();
    QString Policy = ui->lineEdit_Policy->text();
    //Проверяем заполнены ли все поля
    QStringList err;
    if (Name.isEmpty()) err << "Не заполнено Имя";
    if (LastName.isEmpty()) err << "Не заполнено Фамилия";
    if (SurName.isEmpty()) err << "Не заполнено Отчество";
    if (!err.isEmpty())
    {
        QMessageBox::warning(this, tr("Ошибка заполнения:"), tr("Не заполнены обязательные поля :(\n") + err.join("\n"));
        return;
    }
    //Создаем транзакцию
    QSqlDatabase::database().transaction();
    QSqlQuery query;

    //Проверяем уникальность полиса
    if (Policy.count())
    {
        if (!query.exec("SELECT ID FROM People WHERE "
                        "Policy = '" + Policy + "'"
                        ))
        {
            QMessageBox::warning(this, tr("Ошибка проверки уникальности полиса:"), query.lastError().text());
            QSqlDatabase::database().rollback();
            return;
        }
        else
        {
            if(query.first())
            {
                QMessageBox::warning(this, tr("Ошибка заполнения:"), tr("Такой номер полиса уже зарегистрирован в базе :("));
                QSqlDatabase::database().rollback();
                return;
            }
        }
    }

    //Проверяем есть ли такие ФИО уже в базе
    if (!query.exec("SELECT ID FROM People WHERE "
                    "LastName = '" + LastName + "' AND "
                    "Name = '" + Name + "' AND "
                    "SurName = '" + SurName + "'"
                    ))
    {
        QMessageBox::warning(this, tr("Ошибка проверки на дублирование:"), query.lastError().text());
        QSqlDatabase::database().rollback();
        return;
    }
    else
    {
        if (query.first())
        {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, tr("Обнаружен дубликат"), tr("Персона с такими данными уже есть, продолжить внесение?"));
            if (reply != QMessageBox::Yes)
            {
                QSqlDatabase::database().rollback();
                return;
            }
        }
    }

    //Получаем ID новой персоны
    if (!query.exec("SELECT MAX(ID) + 1 FROM People"))
    {
        QMessageBox::warning(this, tr("Ошибка получения нового ID:"), query.lastError().text());
        QSqlDatabase::database().rollback();
        return;
    }
    else
    {
        query.first();
        ID = query.value(0).toString();
    }
    //Подготовка и запись в базу
    query.prepare("INSERT INTO People (ID,LastName,Name,SurName,Policy) VALUES("
                + ID + ","
                "'" + LastName + "',"
                "'" + Name + "',"
                "'" + SurName + "',"
                "'" + Policy + "')"
               );
    if (!query.exec())
    {
        QMessageBox::warning(this, tr("Ошибка записи в базу!:"), query.lastError().text());
        QSqlDatabase::database().rollback();
        return;
    }
    else
    {
        if(Result != nullptr)
        {
            Result->insert("ID",ID);
            Result->insert("LastName",LastName);
            Result->insert("Name",Name);
            Result->insert("SurName",SurName);
            Result->insert("Policy",Policy);
        }
       QSqlDatabase::database().commit();
       QMessageBox::information(this, tr("Успех:"),tr("Запись выполнена!"));
       QDialog::accept();
    }
}

void RegisterNewPerson::on_pushButton_Select_clicked()
{
    WriteInBase();
}


void RegisterNewPerson::on_pushButton_Exit_clicked()
{
    QDialog::reject();
}
