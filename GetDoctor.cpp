#include "GetDoctor.h"
#include "ui_GetDoctor.h"

GetDoctor::GetDoctor(QWidget *parent, QMap<QString,QString>* _Result) :
    QDialog(parent), Result(_Result),
    ui(new Ui::GetDoctor)
{
    QSqlQuery query;
    query.exec("SELECT * FROM Doctors");
    ui->setupUi(this);
}


GetDoctor::~GetDoctor()
{
    delete ui;
}

//Заполение листа
void GetDoctor::FillTW(QSqlQuery _query)
{
    ui->listWidget->clear();
    while(_query.next())
    {
        QListWidgetItem* i = new QListWidgetItem(ui->listWidget);
        i->setText(QString("%1 \t %2 \t %3 %4")
               .arg(
                   _query.value("LastName").toString().simplified(),
                   _query.value("Name").toString().simplified(),
                   _query.value("SurName").toString().simplified(),
                   _query.value("ID").toString().simplified()));
        i->setData(Qt::UserRole,_query.value("Info").toString().simplified());
        i->setData(Qt::UserRole+1,_query.value("ID").toString().simplified());
        ui->listWidget->addItem(i);
    }
}

// Поиск
QSqlQuery GetDoctor::SearchQuery()
{
    QSqlQuery query;
    QStringList CondTemp;
    QString LastName,Name,SurName,Condition;
    LastName = ui->lineEdit_LastName->text();
    Name = ui->lineEdit_Name->text();
    SurName = ui->lineEdit_SurName->text();
    if (LastName.length()) CondTemp << "LastName LIKE '%" + LastName + "%'";
    if (Name.length()) CondTemp << "Name LIKE '%" + Name + "%'";
    if (SurName.length()) CondTemp << "SurName LIKE '%" + SurName + "%'";
    if (CondTemp.count()) Condition = CondTemp.join(" AND ");
    if (Condition.length())
        Condition = "WHERE Status = TRUE AND " + Condition;
    else
        Condition = "WHERE Status = TRUE";
    query.prepare("SELECT * FROM Doctors " + Condition);
    query.exec();
    return query;
}

// Кнопка поиска
void GetDoctor::on_pushButton_Search_clicked()
{
    FillTW(SearchQuery());
}

// Подтверждение выбора
void GetDoctor::on_pushButton_Select_clicked()
{
    if(ui->comboBox_Dep->currentIndex() == -1) return;
    QDialog::accept();
}

// Выбор доктора в списке
void GetDoctor::on_listWidget_itemClicked(QListWidgetItem *item)
{
    if (!Result->isEmpty()) Result->clear();
    QStringList str = item->text().split("\t");
    Result->insert("LastName",str[0]);
    Result->insert("Name",str[1]);
    Result->insert("SurName",str[2]);
    Result->insert("ID",item->data(Qt::UserRole+1).toString());
    ui->textEdit->setHtml(item->data(Qt::UserRole).toString());
    UpdateComboDep(item->data(Qt::UserRole+1).toInt());
}

// Выход
void GetDoctor::on_pushButton_Exit_clicked()
{
    QDialog::reject();
}

// Обновление комбобокса со списком отделений
int GetDoctor::UpdateComboDep(int DocID)
{
    QSqlQuery query;
    query.prepare("SELECT Department.ID AS ID, "
                  "REPLACE(Department.Name,' ','') AS Name FROM "
                  "(SELECT ID, Department_ID FROM Staff WHERE "
                  "Doctors_ID = :DocID AND Status = TRUE) AS Staff "
                  "INNER JOIN Department ON "
                  "Staff.Department_ID = Department.ID;");
    query.bindValue(":DocID", DocID);
    if (!query.exec())
    {
        QMessageBox::warning(this, tr("Ошибка запроса списка докторов:"),
                             query.lastError().text());
        return 0;
    }
    else
    {
        ui->comboBox_Dep->clear();
        while (query.next()) {
            ui->comboBox_Dep->addItem(query.value("Name").toString(),
                                      query.value("ID").toString());
        }
        return 1;
    }
}

//Обновление SID доктора
int GetDoctor::UpdateSID()
{
    QString DocID = ui->listWidget->currentItem()->data(Qt::UserRole+1).toString();
    QString DepID = ui->comboBox_Dep->currentData().toString();
    QSqlQuery query;
    query.prepare("SELECT ID FROM Staff WHERE "
                  "Doctors_ID = :DocID AND Department_ID = :DepID");
    query.bindValue(":DocID", DocID);
    query.bindValue(":DepID", DepID);
    if (!query.exec()) {
        QMessageBox::warning(this, tr("Ошибка запроса Staff_ID:"),
                             query.lastError().text());
        return 0;
    }
    else
    {
        if(query.first())
        {
            Result->insert("SID",query.value("ID").toString());
            return 1;
        }
        return 0;
    }
}

//Выбор отделения
void GetDoctor::on_comboBox_Dep_currentIndexChanged(int /*index*/)
{
    UpdateSID();
}
