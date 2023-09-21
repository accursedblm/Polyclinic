#include "StaffEditor.h"
#include "ui_StaffEditor.h"

/* *********************************************************************
                            Делегат
********************************************************************** */

ComboBoxDelegate::ComboBoxDelegate(QString *query,
                                   QString *querySearch,
                                   QObject *parent,
                                   StaffEditor* pstaff):
    QStyledItemDelegate(parent),squery {query},searchquery {querySearch}, staff{pstaff} {}

ComboBoxDelegate::~ComboBoxDelegate(){
    delete squery;
    delete searchquery;
}


QWidget* ComboBoxDelegate::createEditor(QWidget *parent,
                                        const QStyleOptionViewItem &/*option*/,
                                        const QModelIndex &index) const
{
    QComboBox* comboBox = new QComboBox(parent);
    QSqlQuery query;
    QStringList comp;
    //Заполняем варианты выбора в комбобоксе
    if (squery) {
        if(query.exec(*squery))
        {
            while (query.next()) {
                comboBox->addItem(query.value("Val").toString().simplified(),
                                  query.value("ID").toInt());
                comp << query.value("Val").toString().simplified();
            }
        }
        else
        {
            QMessageBox::warning(parent, tr("Ошибка запроса делегата!:"),
                                 query.lastError().text());
        }
        //Получаем текущее значение ячейки модели и выбираем в его в combobox
        QString value = index.model()->data(index, Qt::EditRole).toString();
        int cindex = comboBox->findText(value);
        if (cindex < 0)
            comboBox->setCurrentIndex(0);
        else
            comboBox->setCurrentIndex(cindex);
    }
    //Комплитер
    QCompleter *completer = new QCompleter(comp, comboBox);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    comboBox->setCompleter(completer);
    comboBox->setEditable(true);

    //Отдаем делегата
    return comboBox;
}

void ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString value = index.model()->data(index, Qt::EditRole).toString();
    QComboBox* comboBox = static_cast<QComboBox*>(editor);
    int cindex = comboBox->findText(value);
    if (cindex < 0)
        comboBox->setCurrentIndex(0);
    else
        comboBox->setCurrentIndex(cindex);
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
{
    QComboBox *edit = static_cast<QComboBox *>(editor);
    QString value = edit->currentText();
    QString data = edit->currentData().toString();
    if (!searchquery) return;
    QSqlQuery query;
    //Заполняем варианты выбора в комбобоксе
    if(query.exec(QString(*searchquery).arg(value)))
    {
        if (query.last())
        {
            model->setData(index,value,Qt::EditRole);
            model->setData(index,data,Qt::UserRole);
            if (model->data(model->index(index.row(),0)).toString().isEmpty())
            {
                int col = index.column() == 1 ? 2 : 1;
                staff->tw->setCurrentIndex(model->index(index.row(),col));
            }
        }
        else return;
    }
    else
    {
        QMessageBox::warning(editor, tr("Ошибка запроса делегата!:"),
                             query.lastError().text());
    }
}

void ComboBoxDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const
{
    editor->setGeometry(option.rect);
}

void ComboBoxDelegate::paint(QPainter *painter,
                             const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
//    if(index.column() == 1)
//    {
//        QStyleOptionComboBox comboBoxStyleOption;
//        comboBoxStyleOption.state = option.state;
//        comboBoxStyleOption.rect = option.rect;
//        comboBoxStyleOption.currentText = index.data(Qt::EditRole).toString();
//        /*QApplication::style()->
//          drawComplexControl(QStyle::CC_ComboBox, &comboBoxStyleOption, painter, 0);*/
//        QApplication::style()->drawControl(QStyle::CE_ComboBoxLabel,
//                                           &comboBoxStyleOption, painter, 0);
//        return;
//    }

    QStyledItemDelegate::paint(painter,option, index);
}


/* *********************************************************************
                            Диалог
********************************************************************** */

StaffEditor::StaffEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StaffEditor)
{
    ui->setupUi(this);
    // Инициализация таблицы
    setupTableView();
}

// Инициализация таблицы
void StaffEditor::setupTableView()
{
    // Инициализация указателей
    tw = ui->tableView;
    tw->hide();
    model = new QStandardItemModel(tw);
    //sortmodel->setSourceModel(model);
    tw->setModel(model);
    tw->setSortingEnabled(true);
    smodel = tw->selectionModel();
    // Заполнение данными
    if (RefreshTable()) return;
    // Устанавливаем свойства таблицы
    tw->setAlternatingRowColors(true);
    tw->verticalHeader()->setVisible(false);
    tw->horizontalHeader()->setStretchLastSection(true);
    tw->setSelectionBehavior(QAbstractItemView::SelectItems); //Разрешаем выделение строк
    tw->setEditTriggers(QAbstractItemView::AllEditTriggers);
    // Устанавливаем названия колонок в модели
    model->setHeaderData(0,Qt::Horizontal,tr("ИН"));
    model->setHeaderData(1,Qt::Horizontal,tr("Отделение"));
    model->setHeaderData(2,Qt::Horizontal,tr("Врач"));
    // Загружаем делегата для каждого столбца
    QString* queryDepartment =
            new QString("SELECT ID, REPLACE(Name,' ','') "
                        "AS Val FROM Department;");
    QString* qsearchDepartment =
            new QString("SELECT ID FROM Department WHERE REPLACE(Name,' ','') ='%1'");
    QString* queryDoctor =
            new QString("SELECT ID, REPLACE(lastName,' ','') & ' ' & "
                        "REPLACE(Name,' ','') & ' ' & "
                        "REPLACE(SurName,' ','') & ' ' & ID "
                        "AS Val FROM Doctors;");
    QString* qsearchDoctor =
            new QString("SELECT ID FROM Doctors WHERE "
                        "REPLACE(lastName,' ','') & ' ' & "
                        "REPLACE(Name,' ','') & ' ' & "
                        "REPLACE(SurName,' ','') & ' ' & ID='%1';");
    ComboBoxDelegate* comboBoxDepartment =
            new ComboBoxDelegate(queryDepartment, qsearchDepartment, this, this);
    ComboBoxDelegate* comboBoxDoctors =
            new ComboBoxDelegate(queryDoctor, qsearchDoctor, this, this);
    ReadOnlyDelegate* readOnlyDelegate =
            new ReadOnlyDelegate(this);
    tw->setItemDelegateForColumn(0, readOnlyDelegate);
    tw->setItemDelegateForColumn(1, comboBoxDepartment);
    tw->setItemDelegateForColumn(2, comboBoxDoctors);
    tw->setSelectionMode(QAbstractItemView::SingleSelection);
    tw->resizeColumnsToContents();
    // Подключаем сигналы смены строки и смены ячейки
    connectSignalRowChange(true);
    connectSignalCurrentChange(true);
    tw->show();
}

// Деструктор
StaffEditor::~StaffEditor()
{
    if (smodel) delete smodel;
    if (model) delete model;
    if (tw) delete tw;
    delete ui;
}

// Слот выхода
void StaffEditor::on_pushButton_Exit_clicked()
{
    QDialog::reject();
}

// Слот вставки пустой строки
void StaffEditor::on_pushButton_Add_clicked()
{
    model->insertRow(model->rowCount());
}

// Слот принудительного обновления таблицы
void StaffEditor::on_pushButton_Update_clicked()
{
    RefreshTable();
}

// Отследживание смены строки
void StaffEditor::currentRowChanged(const QModelIndex& /*current*/,const QModelIndex& previous)
{
    int row = previous.row();
    if (row == -1) return;
    QString ID = model->data(model->index(row,0)).toString();
    if (!ID.isEmpty()) return;
    QString Dep = model->data(model->index(row,1)).toString();
    QString Doc = model->data(model->index(row,2)).toString();
    if (Dep.isEmpty() || Doc.isEmpty())
    {
        //Не заполнены обязательные колонки
        connectSignalRowChange(false);
        QMessageBox::warning(tw, tr("Ошибка заполнения:"), tr("Не заполнено отделение или доктор"));
        tw->setCurrentIndex(previous);
        connectSignalRowChange(true);
    }
}
// Отследживание смены ячейки
void StaffEditor::currentChanged(const QModelIndex& /*current*/,const QModelIndex& previous)
{
    int row = previous.row();
    if (row == -1) return;
    QString ID = model->data(model->index(row,0)).toString();
    QString DepID = model->data(model->index(row,1),Qt::UserRole).toString();
    QString DocID = model->data(model->index(row,2),Qt::UserRole).toString();
    if (!ID.isEmpty())
    {
        if (BaseUpdateString(ID,DepID,DocID))
        {
//            model->setData(previous,"",Qt::EditRole);
//            model->setData(previous,"",Qt::UserRole);
            return;
        }
        else {
            RefreshTable();
        }
    }
    else if (!DepID.isEmpty() && !DocID.isEmpty())
    {
        if (BaseInsertString(DepID,DocID))
        {
            model->setData(previous,"",Qt::EditRole);
            model->setData(previous,"",Qt::UserRole);
            return;
        }
        else {
            RefreshTable();
        }
    }
}

//Внесение новой строки в базу данных
int StaffEditor::BaseInsertString(QString& Dep, QString& Doc)
{
    QSqlQuery query;
    query.prepare("INSERT INTO Staff (ID,Department_ID,Doctors_ID,Status) "
                  "SELECT MAX(ID) + 1, :Dep, :Doc, TRUE FROM Staff");
    query.bindValue(":Dep", Dep);
    query.bindValue(":Doc", Doc);
    if (!query.exec())
    {
        QMessageBox::warning(this, tr("Ошибка внесения строки в базу:"),
                             query.lastError().text());
        return 1;
    }
    return 0;
}

int StaffEditor::BaseUpdateString(QString& ID, QString& Dep, QString& Doc)
{
    QSqlQuery query;
    query.prepare("UPDATE Staff "
                  "SET Department_ID = :Dep, Doctors_ID = :Doc "
                  "WHERE ID = :ID "
                  "AND NOT EXISTS(SELECT ID FROM staff WHERE Department_ID = :Dep AND Doctors_ID = :Doc) "
                  "AND NOT EXISTS(SELECT Staff_ID FROM Lists WHERE Staff_ID = :ID) "
                  "AND NOT EXISTS(SELECT Staff_ID FROM TimeTable WHERE Staff_ID = :ID);");
    query.bindValue(":ID", ID);
    query.bindValue(":Dep", Dep);
    query.bindValue(":Doc", Doc);
    if (!query.exec())
    {
        QMessageBox::warning(this, tr("Ошибка обновления строки в базе:"), query.lastError().text());
        return 1;
    }
    return 0;
}

//Обновление наполнения таблицы
int StaffEditor::RefreshTable()
{
    //Отключаем сигнал
    connectSignalRowChange(false);
    connectSignalCurrentChange(false);
    //Очистка
    if (model->rowCount() > 0)
        model->removeRows(0, model->rowCount());
    //Заполнение
    QSqlQuery query;
    if(query.exec("SELECT Staff.ID AS StaffID, "
                  "REPLACE(Department.Name,' ','') AS DepName, "
                  "REPLACE(Doctors.LastName,' ','') & ' ' & "
                  "REPLACE(Doctors.Name,' ','') & ' ' & "
                  "REPLACE(Doctors.SurName,' ','') & ' ' & Doctors.ID AS Doctor, "
                  "Department.ID AS DepID, Doctors.ID AS DocID "
                  "FROM (Staff "
                  "INNER JOIN Doctors ON Staff.Doctors_ID=Doctors.ID) "
                  "INNER JOIN Department ON Staff.Department_ID=Department.ID "
                  "WHERE Staff.Status = TRUE;"))
    {
        while(query.next())
        {
            QList<QStandardItem *> items;

            items << new QStandardItem(query.value("StaffID").toString());
            QStandardItem* DepID = new QStandardItem(query.value("DepName").toString());
                           DepID->setData(query.value("DepID").toString(),Qt::UserRole);
            QStandardItem* DocID = new QStandardItem(query.value("Doctor").toString());
                           DocID->setData(query.value("DocID").toString(),Qt::UserRole);
            //items << new QStandardItem(query.value("DepName").toString());
            //items << new QStandardItem(query.value("Doctor").toString());
            items << DepID << DocID;
            model->appendRow(items);
        }
    }
    //Включаем сигнал
    connectSignalCurrentChange(true);
    connectSignalRowChange(true);
    return 0;
}

//Подключатель сигнала смены строки
void StaffEditor::connectSignalRowChange(bool b)
{
    if (!smodel) return;
    if (b) connect(smodel, &QItemSelectionModel::currentRowChanged,
                    this, &StaffEditor::currentRowChanged);
    else disconnect(smodel, &QItemSelectionModel::currentRowChanged,
                    this, &StaffEditor::currentRowChanged);
}

//Подключатель сигнала смены ячейки
void StaffEditor::connectSignalCurrentChange(bool b)
{
    if (!smodel) return;
    if (b) connect(smodel, &QItemSelectionModel::currentChanged,
                   this, &StaffEditor::currentChanged);
    else disconnect(smodel, &QItemSelectionModel::currentChanged,
                    this, &StaffEditor::currentChanged);
}

//Удаление строки записи из базы
void StaffEditor::on_pushButton_Delete_clicked()
{
    int row = tw->currentIndex().row();
    QSqlQuery query;
    query.prepare("UPDATE Staff SET Status = FALSE WHERE ID=:ID");
    query.bindValue(":ID",model->index(row,0).data(Qt::EditRole));
    if (!query.exec())
    {
        QMessageBox::warning(this, tr("Ошибка удаления записи в баду:"),
                             query.lastError().text());
    }
    else
    {
        RefreshTable();
    }
}
