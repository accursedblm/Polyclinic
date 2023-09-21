#include "TimeEditor.h"
#include "ui_TimeEditor.h"

TimeEditor::TimeEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TimeEditor)
{
    ui->setupUi(this);
    UpdateComboDoc();
    ui->dateEdit_VStart->setDate(QDate::currentDate());
    ui->dateEdit_VEnd->setDate(QDate::currentDate().addDays(7));
    ui->dateEdit_Start->setDate(QDate::currentDate());
    ui->dateEdit_End->setDate(QDate::currentDate().addDays(7));
    ui->timeEdit_Start->setTime(QTime(8,0,0));
    ui->timeEdit_End->setTime(QTime(17,0,0));
}

TimeEditor::~TimeEditor()
{
    delete ui;
}

// Обновление таблицы
int TimeEditor::SetupTable()
{
    // Установка общих свойств
    QTableWidget* tw = ui->tableWidget;
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

//Обновление таблицы
void TimeEditor::on_pushButton_Update_clicked()
{
    if (SID.isEmpty()) return;
    SetupTable();
}

//Обновление комбобокса cо списком докторов
int TimeEditor::UpdateComboDoc()
{
    QSqlQuery query;
    query.prepare("SELECT ID, REPLACE(lastName,' ','') & ' ' & "
                  "REPLACE(Name,' ','') & ' ' & "
                  "REPLACE(SurName,' ','') & ' ' & ID "
                  "AS Val FROM Doctors;");
    if (!query.exec())
    {
        QMessageBox::warning(this, tr("Ошибка запроса списка докторов:"),
                             query.lastError().text());
        return 0;
    }
    else
    {
        ui->comboBox_Doc->clear();
        while (query.next()) {
            ui->comboBox_Doc->addItem(query.value("Val").toString(),
                                      query.value("ID").toString());
        }
        return 1;
    }
}

//Обновление комбобокса со списком отделений
int TimeEditor::UpdateComboDep(int DocID)
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

//Изменение Доктора
void TimeEditor::on_comboBox_Doc_currentIndexChanged(int index)
{
    int DocID = ui->comboBox_Doc->itemData(index).toInt();
    UpdateComboDep(DocID);
    UpdateSID();
}

//Изменение Отделения
void TimeEditor::on_comboBox_Dep_currentIndexChanged(int /*index*/)
{
    UpdateSID();
}

//Обновление SID доктора
int TimeEditor::UpdateSID()
{
    QString DocID = ui->comboBox_Doc->currentData().toString();
    QString DepID = ui->comboBox_Dep->currentData().toString();
    QSqlQuery query;
    query.prepare("SELECT ID FROM Staff WHERE "
                  "Doctors_ID = :DocID AND Department_ID = :DepID");
    query.bindValue(":DocID", DocID);
    query.bindValue(":DepID", DepID);
    if (!query.exec()) {
        QMessageBox::warning(this, tr("Ошибка запроса Staff_ID:"),
                             query.lastError().text());
        SID = "";
        return 0;
    }
    else
    {
        if(query.first())
        {
            SID = query.value("ID").toString();
            return 1;
        }
        return 0;
    }
}

//Выход
void TimeEditor::on_pushButton_Exit_clicked()
{
    this->accept();
}

//Внести график
void TimeEditor::on_pushButton_insGrath_clicked()
{
    // Проверяем есть ли SID
    if (SID.isEmpty())
    {
        QMessageBox::warning(this, tr("Ошибка:"),
                             tr("Сначала нужно выбрать Доктора и отделение"));
        return;
    }
    // Обновляем статусы дней
    UpdateCb();
    // Прочее
    QDate SDate = ui->dateEdit_Start->date();
    QDate EDate = ui->dateEdit_End->date();
    QTime iTime, STime = ui->timeEdit_Start->time();
    QTime ETime = ui->timeEdit_End->time();
    if (STime.minute()%30 > 0) STime = QTime(STime.hour(), STime.minute()-STime.minute()%30);
    if (ETime.minute()%30 > 0) ETime = QTime(ETime.hour(), ETime.minute()-ETime.minute()%30);
    QDateTime t;
    QSqlQuery query;
    QString DocID = ui->comboBox_Doc->currentData().toString();
    int good = 0, all=0;
    for (;SDate<=EDate;SDate = SDate.addDays(1)) {
        if (job[SDate.dayOfWeek()])
        {
            for (iTime=STime;iTime<=ETime;iTime=iTime.addSecs(1800),++all) {

                t = QDateTime(SDate,iTime);
                QSqlDatabase::database().transaction();
                query.prepare("SELECT Staff.Doctors_ID  FROM "
                              "((SELECT * FROM TimeTable WHERE Staff_ID = :SID) "
                              "AS TimeTable INNER JOIN Staff ON "
                              "TimeTable.Staff_ID = Staff.ID) "
                              "WHERE Staff.Doctors_ID = :ID AND "
                              "TimeTable.DTime = :DTame");
                query.bindValue(":SID",SID);
                query.bindValue(":ID",DocID);
                query.bindValue(":DTame",t);
                if (!query.exec())
                {
                    QSqlDatabase::database().rollback();
                    qDebug()<<query.lastError().text();
                    continue;
                }
                if (!query.last())
                {
                    query.prepare("INSERT INTO TimeTable(ID,Staff_ID,DTime,Reserved) "
                                  "SELECT MAX(ID)+1, :SID, :DTime, FALSE FROM TimeTable");
                    query.bindValue(":SID",SID);
                    query.bindValue(":DTime",t);
                    if (!query.exec())
                    {
                        QSqlDatabase::database().rollback();
                        qDebug()<<query.lastError().text();
                    }
                    else
                    {
                        QSqlDatabase::database().commit();
                        ++good;
                    }
                }
                else
                {
                    QSqlDatabase::database().rollback();
                }
            }
        }
    }
    // Сообщение о успешном внесении графика
    QMessageBox::information(this, tr("Сообщение"),
           tr("Успешно внесено"));
    //%1 из %2 строк").arg(good,all)
}

//Удалить из графика
void TimeEditor::on_pushButton_Delete_clicked()
{
    if (SID.isEmpty())
    {
        QMessageBox::warning(this, tr("Ошибка:"),
                             tr("Сначала нужно выбрать Доктора и отделение"));
        return;
    }
    // Обновляем статусы дней
    UpdateCb();
    QDate SDate = ui->dateEdit_Start->date();
    QDate EDate = ui->dateEdit_End->date();
    QTime STime = ui->timeEdit_Start->time();
    QTime ETime = ui->timeEdit_End->time();
    QSqlQuery query;
    for (;SDate<=EDate;SDate = SDate.addDays(1)) {
        if (!job[SDate.dayOfWeek()]) continue;
        query.prepare("DELETE FROM TimeTable "
                      "WHERE Reserved = FALSE AND "
                      "DTime >= :SDTime AND DTime <= :EDTime");
        query.bindValue(":SDTime",QDateTime(SDate,STime));
        query.bindValue(":EDTime",QDateTime(SDate,ETime));
        if (!query.exec())
        {
            QMessageBox::warning(this, tr("Ошибка удаления графика"),
                                 query.lastError().text());
            return;
        }
    }
    QMessageBox::information(this, tr("Сообщение"),
           tr("Удаление выполнено"));
}

// Считывание данных с чекбокса
void TimeEditor::UpdateCb()
{
    job.clear();
    job.insert(1,ui->checkBox_Monday->isChecked());
    job.insert(2,ui->checkBox_Tuesday->isChecked());
    job.insert(3,ui->checkBox_Wednesday->isChecked());
    job.insert(4,ui->checkBox_Thursday->isChecked());
    job.insert(5,ui->checkBox_Friday->isChecked());
    job.insert(6,ui->checkBox_Saturday->isChecked());
    job.insert(7,ui->checkBox_Sunday->isChecked());
}
