#include "Consultation.h"
#include "ui_Consultation.h"

Consultation::Consultation(QString& DocName, QString& PeopName,
                           QString& Text, QString CID, QString PID,
                           QString SID, QString DID, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Consultation),
    _CID(CID),
    _PID(PID),
    _SID(SID),
    _DID(DID)
{
    ui->setupUi(this);
    ui->comboBox_list->setVisible(false);
    ui->lineEdit_Doc->setText(DocName);
    ui->lineEdit_Peop->setText(PeopName);
    ui->plainTextEdit->insertPlainText(Text);
    QSqlQuery query;
    query.prepare("SELECT ID, Name FROM Diagnosis WHERE Department_ID = :DID");
    query.bindValue(":DID",_DID);
    if (!query.exec())
    {
        QMessageBox::warning(this,
            tr("Ошибка запроса списка диагнозов:"),
                             query.lastError().text());
    }
    else
    {
        Diag.clear();
        while(query.next())
            Diag.insert(query.value("Name").toString().simplified(),
                        query.value("ID").toString().simplified());
    }
}

Consultation::~Consultation()
{
    delete ui;
}

// Обработка кнопок
void Consultation::on_buttonBox_clicked(QAbstractButton *button)
{
    if (button == (QAbstractButton*) ui->buttonBox->button(QDialogButtonBox::Save))
    {
        // Сохранение данных консультации
        QSqlDatabase::database().transaction();
        QSqlQuery query;
        query.prepare("UPDATE Consultations "
                      "SET Content = :Con,"
                      "Status = :Stat,"
                      "Type = :Typ "
                      "WHERE ID = :ID");
        query.bindValue(":Con",ui->plainTextEdit->toPlainText());
        query.bindValue(":Stat",1);
        query.bindValue(":Typ",
            ui->comboBox_type->currentIndex() == 0 ? "primary": "secondary");
        query.bindValue(":ID",_CID);
        if (!query.exec())
        {
            QMessageBox::warning(this,
                tr("Ошибка сохранения данных консультации:"),
                                 query.lastError().text());
            QSqlDatabase::database().rollback();
            return;
        }

        // Создание больничного листа
        if (ui->checkNeedList->isChecked())
        {
            bool ok = false;
            QString Dep = QInputDialog::getItem(this,tr("Выбор диагноза"),
                                                tr("диагноз:"),Diag.keys(),0,false, &ok);
            if (!ok)
            {
                QSqlDatabase::database().rollback();
                return;
            }
            QSqlQuery query2;
            query2.prepare("INSERT INTO Lists(ID,People_ID,Staff_ID,"
                           "Diagnos_ID,Date_start,Date_end) "
                           "SELECT MAX(ID)+1, :PID, :SID, :Diag, :Dstart, :Dend "
                           "FROM Lists");
            query2.bindValue(":PID",_PID);
            query2.bindValue(":SID",_SID);
            query2.bindValue(":Diag",Diag.find(Dep).value());
            query2.bindValue(":Dstart", QDate::currentDate());
            query2.bindValue(":Dend", QDate(1970,1,1));
            if (!query2.exec())
            {
                QMessageBox::warning(this,
                    tr("Ошибка запроса создания больничного листа:"),
                                     query2.lastError().text());
                QSqlDatabase::database().rollback();
                return;
            }
            else
            {
                query2.prepare("INSERT INTO Register(ID_List,ID_Cons) "
                               "SELECT MAX(Lists.ID), :CID FROM Lists");
                query2.bindValue(":CID",_CID);
                if (!query2.exec())
                {
                    QMessageBox::warning(this,
                        tr("Ошибка запроса записи в регистратор:"),
                                         query2.lastError().text());
                    QSqlDatabase::database().rollback();
                    return;
                }
            }
        }

        // Закрытие больничного листа
        if (ui->checkBox_closeList->isChecked())
        {
            if (ui->comboBox_list->currentIndex() == -1)
            {
                QSqlDatabase::database().rollback();
                return;
            }
            QString ID = ui->comboBox_list->currentData().toString();
            QSqlQuery query3;
            query3.prepare("UPDATE Lists "
                           "SET Date_end = :DEnd "
                           "WHERE ID = :ID "
                           "AND Date_end = :Dopen");
            query3.bindValue(":ID", ID);
            query3.bindValue(":DEnd", QDate::currentDate());
            query3.bindValue(":Dopen", QDate(1970,1,1));
            if (!query3.exec())
            {
                QMessageBox::warning(this,
                    tr("Ошибка запроса создания больничного листа:"),
                                     query3.lastError().text());
                QSqlDatabase::database().rollback();
                return;
            }
        }

        // Закрытие транзакции
        QSqlDatabase::database().commit();
        QMessageBox::information(this,
            tr("Сообщение:"),
            tr("Операция успешно выполнена :)"));
    }
}

// Чек-лист Закрыть больничный лист
void Consultation::on_checkBox_closeList_stateChanged(int arg1)
{
    if(arg1)
    {
        ui->comboBox_list->setVisible(true);
        ui->horizontalSpacer->changeSize(0,5,QSizePolicy::Fixed);
        // Заполняем список больничных
        QSqlQuery query;
        query.prepare("SELECT Lists.ID, Lists.ID & '/' & "
                      "REPLACE(Diagnosis.Name,' ','') & '/' & "
                      "Date_start AS Val "
                      "FROM Lists LEFT JOIN Diagnosis ON "
                      "Lists.Diagnos_ID = Diagnosis.ID "
                      "WHERE People_ID = :PID "
                      "AND Date_end = :Dopen");
        query.bindValue(":PID",_PID);
        query.bindValue(":Dopen", QDate(1970,1,1));
        if(!query.exec())
        {
            QMessageBox::warning(this,
                tr("Ошибка получения списка больничных:"),
                                 query.lastError().text());
            return;
        }
        else
        {
            ui->comboBox_list->clear();
            while (query.next()) {
                ui->comboBox_list->addItem(query.value("Val").toString(),
                                           query.value("ID").toString());
            }
        }
    }
    else
    {
        ui->comboBox_list->clear();
        ui->comboBox_list->setVisible(false);
        ui->horizontalSpacer->changeSize(5,5,QSizePolicy::Expanding);
    }
}

void Consultation::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QLabel* l = ui->label_pic;
    QPainter painter(this); // Создаём объект отрисовщика
    // Устанавливаем кисть абриса
    painter.setPen(QPen(Qt::red, 1, Qt::SolidLine, Qt::FlatCap));
    painter.setBrush(QBrush(Qt::red, Qt::SolidPattern));
    painter.drawRect(l->x()+l->width()/2-5,l->y()+(l->height()-l->width())/2,10,l->width());
    painter.drawRect(l->x(),l->y()+l->height()/2-5,l->width(),10);
}
