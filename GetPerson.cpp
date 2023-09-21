#include "GetPerson.h"
#include "ui_GetPerson.h"

GetPerson::GetPerson(QWidget *parent, QMap<QString,QString>* _Result) :
    QDialog(parent), Result(_Result),
    ui(new Ui::GetPerson)
{
    QSqlQuery query;
    query.exec("SELECT * FROM People");
    ui->setupUi(this);
}


GetPerson::~GetPerson()
{
    delete ui;
}

void GetPerson::FillTW(QSqlQuery _query)
{ 
    ui->listWidget->clear();
    while(_query.next())
    {
        QListWidgetItem* i = new QListWidgetItem(ui->listWidget);
        i->setText(QString("%1 \t %2 \t %3 \t %4")
               .arg(
                   _query.value("LastName").toString().simplified(),
                   _query.value("Name").toString().simplified(),
                   _query.value("SurName").toString().simplified(),
                   _query.value("Policy").toString().simplified()));
        i->setData(Qt::UserRole,_query.value("ID").toString().simplified());
        ui->listWidget->addItem(i);
    }
}

QSqlQuery GetPerson::SearchQuery()
{
    QSqlQuery query;
    QStringList CondTemp;
    QString LastName,Name,SurName,Policy,Condition;
    LastName = ui->lineEdit_LastName->text();
    Name = ui->lineEdit_Name->text();
    SurName = ui->lineEdit_SurName->text();
    Policy = ui->lineEdit_Policy->text();
    if (LastName.length()) CondTemp << "LastName LIKE '%" + LastName + "%'";
    if (Name.length()) CondTemp << "Name LIKE '%" + Name + "%'";
    if (SurName.length()) CondTemp << "SurName LIKE '%" + SurName + "%'";
    if (Policy.length()) CondTemp << "Policy LIKE '%" + Policy + "%'";
    if (CondTemp.count()) Condition = CondTemp.join(" AND ");
    if (Condition.length()) Condition = "WHERE " + Condition;
    query.prepare("SELECT * FROM People " + Condition);
    query.exec();
    return query;
}

void GetPerson::on_pushButton_Search_clicked()
{
    FillTW(SearchQuery());
}


void GetPerson::on_pushButton_Select_clicked()
{
    QDialog::accept();
}


void GetPerson::on_listWidget_itemClicked(QListWidgetItem *item)
{
    if (!Result->isEmpty()) Result->clear();
    QStringList str = item->text().split("\t");
    Result->insert("LastName",str[0]);
    Result->insert("Name",str[1]);
    Result->insert("SurName",str[2]);
    Result->insert("Policy",str[3]);
    Result->insert("ID",item->data(Qt::UserRole).toString());
}


void GetPerson::on_pushButton_Exit_clicked()
{
    QDialog::reject();
}
