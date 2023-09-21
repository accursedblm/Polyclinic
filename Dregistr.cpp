#include "Dregistr.h"
#include "ui_Dregistr.h"

/*************************************************************************
**************************************************************************
                          Диалог регистратора
**************************************************************************
*************************************************************************/

DRegistr::DRegistr(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DRegistr)
{
    ui->setupUi(this);
    tw = ui->tableWidget;
    ui->dateEdit_VStart->setDate(QDate::currentDate());
    ui->dateEdit_VEnd->setDate(QDate::currentDate().addDays(7));
    SetupTableTwp();
    connect(ui->pushButton_Exit_2,&QPushButton::clicked,
            this,&DRegistr::on_pushButton_Exit_clicked);
    // Добавляем gif анимацию
    movie = new QMovie(":/Res/res/medical.gif");
    ui->Gifka->setMovie(movie);
    movie->start();
}

void DRegistr::resizeEvent(QResizeEvent* event)
{
    resizeMovie();
    QDialog::resizeEvent(event); //Вызов стандартного события
}

void DRegistr::resizeMovie()
{
    QSize SGF = ui->Gifka->rect().size();
    QSize sgif;
    float k = (float) 6.6/3;
    if (SGF.width()/SGF.height() > k)
    {
        sgif.setWidth(SGF.height()*k);
        sgif.setHeight(SGF.height());
    }
    else
    {
        sgif.setWidth(SGF.width());
        sgif.setHeight(SGF.width()/k);
    }
    movie->setScaledSize(sgif);
}


DRegistr::~DRegistr()
{
    delete ui;
}

/*************************************************************************
**************************************************************************
                          Вкладка регистрации записей
**************************************************************************
*************************************************************************/
// Заполнение полей клиента
void DRegistr::fill_fields_People(const QMap<QString,QString>& _Result)
{
    if(!_Result.isEmpty())
    {
        ui->lineEdit_Name->setText(_Result["Name"]);
        ui->lineEdit_LastName->setText(_Result["LastName"]);
        ui->lineEdit_SurName->setText(_Result["SurName"]);
        ui->lineEdit_Policy->setText(_Result["Policy"]);
        PeopleID = _Result["ID"];
    }
}

// Заполнение полей доктора
void DRegistr::fill_fields_Doc(const QMap<QString,QString>& _Result)
{
    if(!_Result.isEmpty())
    {
        ui->lineEdit_Name_Doc->setText(_Result["Name"]);
        ui->lineEdit_LastName_Doc->setText(_Result["LastName"]);
        ui->lineEdit_SurName_Doc->setText(_Result["SurName"]);
        DoctorID = _Result["ID"];
        SID = _Result["SID"];
        // Чистим таблицу
        tw->clearContents();
        // Чистим выбор ID таблицы графиков
        TimeTableID = "";
    }
}

// Выбор клиента
void DRegistr::on_pButton_getPers_clicked()
{
    QMap<QString,QString> Result;
    GetPerson *Gp = new GetPerson(this, &Result);
    if(Gp->exec()) fill_fields_People(Result);
    delete Gp;
}

// Регистрация нового клиента
void DRegistr::on_pButton_RegisterNew_clicked()
{
     QMap<QString,QString> Result;
     RegisterNewPerson *Rg = new RegisterNewPerson(this, &Result);
     if(Rg->exec()) fill_fields_People(Result);
     delete Rg;
}

// Выбор доктора
void DRegistr::on_pButton_getDoc_clicked()
{
    QMap<QString,QString> Result;
    GetDoctor *Gd = new GetDoctor(this, &Result);
    if(Gd->exec()) fill_fields_Doc(Result);
    delete Gd;
}

/*************************************************************************
                          Таблица выбора графика
*************************************************************************/

// Обновление таблицы
int DRegistr::SetupTable()
{
    // Установка общих свойств
    tw = ui->tableWidget;
    tw->setShowGrid(true);
    tw->setSelectionMode(QAbstractItemView::SingleSelection);
    tw->setSelectionBehavior(QAbstractItemView::SelectItems);
    //tw->horizontalHeader()->setStretchLastSection(true);
    // Получаем даты диапазона вывода таблицы
    QDate DStart = ui->dateEdit_VStart->date();
    QDate DEnd = ui->dateEdit_VEnd->date();
    if (DStart > DEnd) return 0;
    // Получаем список горизонтальных заголовков
    QStringList Hheaders;
    int cols;
    QDate d = DStart;
    for (cols = 0; d <= DEnd; d=d.addDays(1),++cols) {
        Hheaders << QString("%1 \n %2").arg(d.toString("dd.MM"),
                                                dayName[d.dayOfWeek()]);
    }
    // Устанавливаем кол-во колонок, заголовки, ширину
    tw->setColumnCount(cols);
    for (int i = 0; i <= cols; ++i ) {
        tw->setColumnWidth(i,35);
    }
    tw->setHorizontalHeaderLabels(Hheaders);
    // Получаем список вертикальных заголовков
    QStringList Vheaders;
    QTime ts (8,0,0);
    QTime TStart(ts);
    QTime td (20,0,0);
    int rows;
    for (rows = 0; ts <=td ; ts=ts.addSecs(1800),++rows) {
        Vheaders << ts.toString();
    }
    // Устанавливаем кол-во строк, заголовков, высоту
    tw->setRowCount(rows);
    tw->verticalHeader()->setMaximumSectionSize(18);
    tw->verticalHeader()->setSelectionMode(QAbstractItemView::NoSelection);
    tw->horizontalHeader()->setSelectionMode(QAbstractItemView::NoSelection);
    for (int i = 0; i <= rows; ++i ) {
        tw->setRowHeight(i,18);
    }
    tw->setVerticalHeaderLabels(Vheaders);
    // Создаем за запрос к базе для получения данных
    QSqlQuery query;
    query.prepare("SELECT * FROM TimeTable WHERE "
                  "Staff_ID = :SID AND "
                  "DTime >= :DStart AND DTime <= :DEnd ");
    query.bindValue(":SID", SID);
    query.bindValue(":DStart", QDateTime(DStart));
    query.bindValue(":DEnd", QDateTime(DEnd,QTime(23,59,59)));
    if (!query.exec())
    {
        QMessageBox::warning(this,
                             tr("Ошибка запроса заполнения таблицы:"),
                             query.lastError().text());
        return 0;
    }
    else
    {
        // Вспомогательные переменные для заполнения таблицы
        qint64 row, col;
        int ID;
        QDate cDate;
        QTime rTime;
        bool Res;
        QModelIndex i;
        // Чистим таблицу
        tw->clearContents();
        // Заполнение таблицы
        while(query.next())
        {
            // Разделяем дату и время
            cDate = query.value("DTime").toDate();
            rTime = query.value("DTime").toTime();
            // Получаем ID и Res (статус резервирования)
            ID = query.value("ID").toInt();
            Res = query.value("Reserved").toBool();
            // Расчет строки и колонки в таблице
            col = DStart.daysTo(cDate);
            row = TStart.secsTo(rTime)/1800;
            i = tw->model()->index(row,col);
            // Заполнение ячейки
            tw->model()->setData(i,Res?"X":"O",Qt::EditRole);
            tw->model()->setData(i,ID,Qt::UserRole);
            tw->model()->setData(i,Res,Qt::UserRole+1);
            tw->item(row,col)->
                setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            // Заливка заполненых ячеек
            StyleVariables sv(":/Qss/styles/style.qss");
            QColor Reserved(sv.variables["$reserved_items"]);
            QColor NotReserved(sv.variables["$notreserved_items"]);
            if (Res) {
                tw->item(row,col)->setBackgroundColor(Reserved);
            }
            else
            {
                tw->item(row,col)->setBackgroundColor(NotReserved);
            }
        }
        return 1;
    }
}

//Обновление расписания
void DRegistr::on_pushButton_Update_clicked()
{
    if (SID.isEmpty()) return;
    SetupTable();
}

// Выход
void DRegistr::on_pushButton_Exit_clicked()
{
    QDialog::reject();
}

// Запись на прием
void DRegistr::on_pushButton_Write_clicked()
{
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.prepare("INSERT INTO Consultations "
                  "(ID,People_ID,TimeTable_ID,Type,Status) "
                  "SELECT MAX(ID) + 1, :People_ID, "
                  ":TimeTable_ID, :Type, :Status "
                  "FROM Consultations;");
    query.bindValue(":People_ID", PeopleID);
    query.bindValue(":TimeTable_ID", TimeTableID);
    query.bindValue(":Type", "primary");
    query.bindValue(":Status", "0");
    if (!query.exec())
    {
        QMessageBox::warning(this,
                        tr("Ошибка записи консультации:"),
                        query.lastError().text());
        QSqlDatabase::database().rollback();
    }
    else
    {
        query.prepare("UPDATE TimeTable SET Reserved = TRUE WHERE ID=:ID;");
        query.bindValue(":ID",TimeTableID);
        if(!query.exec())
        {
            QMessageBox::warning(this,
                            tr("Ошибка резервирования времени:"),
                            query.lastError().text());
            QSqlDatabase::database().rollback();
        }
        else
        {
            QMessageBox::information(this,
                            tr("Сообщение"),
                            tr("Запись на прием выполнена"));
            QSqlDatabase::database().commit();
        }
    }
}

// Выбор ячейки с расписанием
void DRegistr::on_tableWidget_itemClicked(QTableWidgetItem *item)
{
    TimeTableID = item->data(Qt::UserRole).toString();
}

/*************************************************************************
**************************************************************************
                        Вкладка просмотра записей
**************************************************************************
*************************************************************************/
int DRegistr::SetupTableTwp()
{
    // Установка общих свойств
    twp = ui->tableWidget_zap;
    twp->setColumnCount(13); // Указываем число колонок
    twp->setShowGrid(true);
    twp->setSelectionMode(QAbstractItemView::SingleSelection);
    twp->setSelectionBehavior(QAbstractItemView::SelectItems);
    twp->setEditTriggers(QAbstractItemView::NoEditTriggers);
    twp->resizeColumnsToContents();
    twp->setSortingEnabled(true);
    //twp->horizontalHeader()->setSortIndicatorShown(true);
    QStringList Hheaders{
                        tr("ИНк"),
                        tr("ИНв"),
                        tr("Дата/время приема"),
                        tr("Клиент"),
                        tr("Врач"),
                        tr("Отделение"),
                        tr("Вид_приема"),
                        tr("Статус"),
                        tr("БЛ"),
                        tr("Заключение")
    };
    twp->setHorizontalHeaderLabels(Hheaders);
    // Растягиваем последнюю колонку на всё доступное пространство
    twp->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    //twp->horizontalHeader()->setStretchLastSection(true);
    return 0;
}

int DRegistr::UpdateTableTwp()
{
    if (!twp) return 1;
    twp->model()->removeRows(0,twp->model()->rowCount());
    QDateTime startDT(ui->calendarWidget->selectedDate(),QTime(0,0,1));
    QDateTime endDT(ui->calendarWidget->selectedDate(),QTime(23,59,59));
    QString critery("");
    if (ui->comboBox_vid->currentIndex() == 0)
    {
        critery = " AND Doctors.ID = " + ui->comboBox_value->currentData().toString();
    }
    if (ui->comboBox_vid->currentIndex() == 1)
    {
        critery = " AND People.ID = " + ui->comboBox_value->currentData().toString();
    }
    QSqlQuery query;
    query.prepare("SELECT Consultations.ID AS CID, TimeTable.ID AS TID, TimeTable.DTime, "
                  "REPLACE(People.LastName,' ','') & ' ' & "
                  "REPLACE(People.Name,' ','') & ' ' & "
                  "REPLACE(People.SurName,' ','') & ' ' & Policy AS Peop, "
                  "REPLACE(Doctors.LastName,' ','') & ' ' & "
                  "REPLACE(Doctors.Name,' ','') & ' ' & "
                  "REPLACE(Doctors.SurName,' ','') & ' ' & "
                  "Doctors.ID AS Doc, REPLACE(Department.Name,' ','') AS Dep, "
                  "Consultations.Type, Register.ID_List AS LID, Consultations.Status, "
                  "Consultations.Content AS Con, "
                  "People.ID AS PID, "
                  "Staff.ID AS SID, "
                  "Department.ID AS DID "
                  "FROM (((((((SELECT * FROM Consultations WHERE Status = :Status) "
                  "AS Consultations LEFT JOIN TimeTable ON "
                  "Consultations.TimeTable_ID = TimeTable.ID) "
                  "LEFT JOIN Staff ON TimeTable.Staff_ID = Staff.ID) "
                  "LEFT JOIN Doctors ON Staff.Doctors_ID = Doctors.ID) "
                  "LEFT JOIN People ON Consultations.People_ID = People.ID) "
                  "LEFT JOIN Department ON Staff.Department_ID = Department.ID) "
                  "LEFT JOIN Register ON Consultations.ID = Register.ID_Cons) WHERE "
                  "TimeTable.DTime >= :startDT AND TimeTable.DTime <= :endDT" + critery + ";");
    query.bindValue(":Status", "0");
    query.bindValue(":startDT", startDT);
    query.bindValue(":endDT", endDT);

    if(!query.exec())
    {
        QMessageBox::warning(this,
            tr("Ошибка запроса заполнения таблицы консультаций:"),
                             query.lastError().text());
        return 2;
    }
    else
    {
        int i = -1;
        while(query.next())
        {
            twp->insertRow(++i);
            QTableWidgetItem s;
            twp->setItem(i,0,new QTableWidgetItem(query.value("CID").toString()));
            twp->setItem(i,1,new QTableWidgetItem(query.value("TID").toString()));
            twp->setItem(i,2,new QTableWidgetItem(query.value("DTime")
                                 .toDateTime().toString("yyyy-MM-dd ddd HH:mm")));
            twp->setItem(i,3,new QTableWidgetItem(query.value("Peop").toString()));
            twp->setItem(i,4,new QTableWidgetItem(query.value("Doc").toString()));
            twp->setItem(i,5,new QTableWidgetItem(query.value("Dep").toString()));
            twp->setItem(i,6,new QTableWidgetItem(query.value("Type").toString()));
            twp->setItem(i,7,new QTableWidgetItem(query.value("Status").toString()));
            twp->setItem(i,8,new QTableWidgetItem(query.value("LID").toString()));
            twp->setItem(i,9,new QTableWidgetItem(query.value("Con").toString()));
            twp->setItem(i,10,new QTableWidgetItem(query.value("PID").toString()));
            twp->setItem(i,11,new QTableWidgetItem(query.value("SID").toString()));
            twp->setItem(i,12,new QTableWidgetItem(query.value("DID").toString()));
        }
        SetAlignColumn(twp,7,Qt::AlignCenter);
        SetAlignColumn(twp,8,Qt::AlignCenter);
        return 0;
    }
}

// Кнопка обновить таблицу
void DRegistr::on_pushButton_update_clicked()
{
    UpdateTableTwp();
}

// Установить выравнивание для колонки
int DRegistr::SetAlignColumn(QTableWidget* t, int col, Qt::AlignmentFlag flag)
{
    if (!t) return 1;
    if (col < 0 && col >= t->columnCount()) return 2;
    qDebug() << t->model()->rowCount();
    for (int i = 0;i<t->model()->rowCount() ; ++i) {
        t->item(i,col)->setTextAlignment(flag);
    }
    return 0;
}

// Смена выбора вида поиска
void DRegistr::on_comboBox_vid_currentIndexChanged(int index)
{
    QStringList squary;
    squary << "SELECT ID,REPLACE(LastName,' ','') & ' ' & "
              "REPLACE(Name,' ','') & ' ' & "
              "REPLACE(SurName,' ','') & ' ' & "
              "ID AS Val FROM Doctors WHERE Status = TRUE";
    squary << "SELECT ID,REPLACE(LastName,' ','') & ' ' & "
              "REPLACE(Name,' ','') & ' ' & "
              "REPLACE(SurName,' ','') & ' ' & "
              "Policy AS Val "
              "FROM People WHERE Status = TRUE";
    QSqlQuery query;
    query.prepare(squary[index]);
    if (!query.exec())
    {
        QMessageBox::warning(this,
            tr("Ошибка запроса заполнения критериев:"),
                             query.lastError().text());
    }
    else
    {
        ui->comboBox_value->clear();
        while(query.next())
        {
            ui->comboBox_value->addItem(
              query.value("Val").toString(),query.value("ID").toString());
        }
    }
}

// Скрытие вкладок
void DRegistr::setHiddenTabs(int index, bool hide)
{
    if (index >= 0 && index < ui->tabWidget->count())
    {
        ui->tabWidget->setTabEnabled(!hide,index);
    }
}

// Просмотр записи
void DRegistr::on_pushButton_ShowRecord_clicked()
{
    QTableWidget* tr = ui->tableWidget_zap;
    int row = tr->currentRow();
    if(row == -1) return;
    QString Doc(tr->item(row,4)->text());
    QString Peop(tr->item(row,3)->text());
    QString Con(tr->item(row,9)->text());
    QString CID(tr->item(row,0)->text());
    QString PID(tr->item(row,10)->text());
    QString SID(tr->item(row,11)->text());
    QString DID(tr->item(row,12)->text());
    Consultation Cons(Doc,Peop,Con,CID,PID,SID,DID,this);
    Cons.setWindowFlags(Qt::Window |  Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    if(Cons.exec())
        UpdateTableTwp();
}

