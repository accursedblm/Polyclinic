#include "RegisterNewDoctor.h"
#include "ui_RegisterNewDoctor.h"

RegisterNewDoctor::RegisterNewDoctor(QWidget *parent, QMap<QString,QString>* _Result) :
    QDialog(parent), Result(_Result),
    ui(new Ui::RegisterNewDoctor)
{
    QSqlQuery query;
    query.exec("SELECT * FROM People");
    ui->setupUi(this);
}


RegisterNewDoctor::~RegisterNewDoctor()
{
    delete ui;
}

void RegisterNewDoctor::WriteInBase()
{
    QString ID;
    QString Name = ui->lineEdit_Name->text();
    QString LastName = ui->lineEdit_LastName->text();
    QString SurName =  ui->lineEdit_SurName->text();
    QString Info =  ui->textEdit->toHtml();

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

    //Проверяем есть ли такие ФИО уже в базе
    if (!query.exec("SELECT ID FROM Doctors WHERE "
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
            reply = QMessageBox::question(this, tr("Обнаружен дубликат"), tr("Врач с такими данными уже есть, продолжить внесение?:"));
            if (reply != QMessageBox::Yes)
            {
                QSqlDatabase::database().rollback();
                return;
            }
        }
    }

    //Получаем ID нового врача
    if (!query.exec("SELECT MAX(ID) + 1 FROM Doctors"))
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
    query.prepare("INSERT INTO Doctors (ID,LastName,Name,SurName,Status,Info) VALUES("
                "'" + ID + "',"
                "'" + LastName + "',"
                "'" + Name + "',"
                "'" + SurName + "',"
                "TRUE,"
                " :Info)"
               );
    query.bindValue(":Info",Info);
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
        }
        QSqlDatabase::database().commit();
        QMessageBox::information(this, tr("Успех:"),tr("Запись выполнена!"));
        QDialog::accept();
    }
}

void RegisterNewDoctor::on_pushButton_Select_clicked()
{
    WriteInBase();
}


void RegisterNewDoctor::on_pushButton_Exit_clicked()
{
    QDialog::reject();
}
