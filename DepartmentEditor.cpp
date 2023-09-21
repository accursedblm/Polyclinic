#include "DepartmentEditor.h"
#include "ui_DepartmentEditor.h"
#include <QMessageBox>
#include <QInputDialog>

DepartmentEditor::DepartmentEditor(QWidget *parent, int Composition) :
    QDialog(parent),
    ui(new Ui::DepartmentEditor)
{
    ui->setupUi(this);
    this->window()->hide();
    if (Composition < 3) SetComposition(Composition);
    UpdateList();
    UpdateListDiag();
    this->window()->show();
}

DepartmentEditor::~DepartmentEditor()
{
    delete ui;
}

//Добавить отделение
void DepartmentEditor::on_pushButton_Add_clicked()
{
    QString newName;
    newName = QInputDialog::getText(this, tr("Новое название"), tr("Ведите:"));
    if (!newName.isEmpty())
    {
        QSqlQuery query;
        if (!query.exec("INSERT INTO Department (ID, Name) "
                        "SELECT MAX(ID)+1,'" + newName + "' FROM Department"))
        {
            QMessageBox::warning(this, tr("Ошибка создания нового отделения:"),
                              QString(query.lastError().number()) + " " +
                                 query.lastError().text());
            return;
        }
        else
        {
            QMessageBox::information(this, tr("Успех:"),tr("Запись выполнена!"));
            UpdateList();
        }
    }
}

//Переименовать отделение
void DepartmentEditor::on_pushButton_Rename_clicked()
{
    QString ID = SelRowText(0);
    QString oldName = SelRowText(1);
    QString newName = QInputDialog::getText(this, tr("Новое название"),
                                       tr("Ведите:"),QLineEdit::Normal,oldName);
    if (!newName.isEmpty() && !ID.isEmpty())
        Rename(ID,newName);
}

//Удалить отделение
void DepartmentEditor::on_pushButton_Delete_clicked()
{
    DeleteRow(SelRowText(0));
}

//Выйти
void DepartmentEditor::on_pushButton_Exit_clicked()
{
    QDialog::accept();
}

//Обновить список
void DepartmentEditor::on_pushButton_Update_clicked()
{
    UpdateList();
}

//Изменение ячейки
void DepartmentEditor::on_tableWidget_Departments_cellChanged(int row, int column)
{
    QString ID, newName;
    ID = ui->tableWidget_Departments->item(row,0)->text();
    newName = ui->tableWidget_Departments->item(row,column)->text();
    if (!newName.isEmpty() && !ID.isEmpty())
        Rename(ID,newName);
}

//Процедура переименования
void DepartmentEditor::Rename(const QString& ID, const QString& newName)
{
    QSqlQuery query;
    if (!query.exec("UPDATE (SELECT Name FROM Department WHERE ID = " + ID + ") "
                    "SET Name = '" + newName + "'"))
    {
        QMessageBox::warning(this, tr("Ошибка переименования:"),
                             query.lastError().text());
        return;
    }
    else
    {
        UpdateList();
    }
}

//Процедура удаления
void DepartmentEditor::DeleteRow(const QString& ID)
{
    QSqlQuery query;
    if (!query.exec("DELETE FROM Department WHERE ID=" + ID))
    {
        QMessageBox::warning(this, tr("Ошибка удаления отделения:"),
                             query.lastError().text());
        return;
    }
    else
    {
        UpdateList();
    }
}

//Получение ID выделенной строки в таблице отделений
QString DepartmentEditor::SelRowText(int column)
{
    QTableWidget* T = ui->tableWidget_Departments;
    QList<QTableWidgetItem *> S = T->selectedItems();
    if (S.count()) {
        return T->item(S[0]->row(),column)->text();
    }
    else
    {
        return "";
    }
}

//Процедура обновления списка
void DepartmentEditor::UpdateList()
{
    QSqlQuery query;
    if (!query.exec("SELECT * FROM Department"))
    {
        QMessageBox::warning(this, tr("Ошибка получения списка отделений:"),
                             query.lastError().text());
        return;
    }
    else
    {
        int i = 0,j;
        QTableWidget* T = ui->tableWidget_Departments;
        T->clearContents();
        T->blockSignals(true);
        for(; query.next();++i)
        {
            if (i > T->rowCount()-1) T->insertRow(i);
            T->setItem(i,0,new QTableWidgetItem(query.value(0).toString().simplified()));
            T->setItem(i,1,new QTableWidgetItem(query.value(1).toString().simplified()));
        }
        j=T->rowCount();
        for (; i < j;++i)
            T->removeRow(i);
        ui->tableWidget_Departments->blockSignals(false);
    }
}

//Процедура обновления списка диагнозов
void DepartmentEditor::UpdateListDiag()
{
    QString Dep_ID = SelRowText(0);
    if (Dep_ID.isEmpty()) Dep_ID = "Department_ID";
    QSqlQuery query;
    if (!query.exec("SELECT ID, Name FROM Diagnosis WHERE Department_ID = " + Dep_ID))
    {
        QMessageBox::warning(this, tr("Ошибка получения списка отделений:"),
                             query.lastError().text());
        return;
    }
    else
    {
        int i = 0;
        QTableWidget* T = ui->tableWidget_Diagnosis;
        T->clearContents();
        T->blockSignals(true);
        for(; query.next();++i)
        {
            if (i > T->rowCount()-1) T->insertRow(i);
            T->setItem(i,0,new QTableWidgetItem(query.value(0).toString().simplified()));
            T->setItem(i,1,new QTableWidgetItem(query.value(1).toString().simplified()));
        }
        while (T->rowCount() > i)
            T->removeRow(i);
        ui->tableWidget_Departments->blockSignals(false);
    }
}

//Изменение выбраной ячейки департамента
void DepartmentEditor::on_tableWidget_Departments_itemSelectionChanged()
{
    UpdateListDiag();
}

//Выйти 2
void DepartmentEditor::on_pushButton_Exit_2_clicked()
{
   QDialog::accept();
}


//Обновить список диагнозов
void DepartmentEditor::on_pushButton_Update_2_clicked()
{
    UpdateListDiag();
}

//Добавить диагноз
void DepartmentEditor::on_pushButton_Add_2_clicked()
{
    //Ввести имя нового диагноза
    bool ok = false;
    QString newName = QInputDialog::getText(this, tr("Новое название"),
                                            tr("Ведите:"),QLineEdit::Normal,"",&ok);
    if (!ok || newName.isEmpty()) return;

    //Выбор отделения для привязки диагноза
    ok = false;
    QStringList variants;
    QTableWidget * T = ui->tableWidget_Departments;
    for (int i = 0; i<T->rowCount();++i) {
        variants << T->item(i,1)->text();
    }
    int current = 0;
    if (T->selectedItems().count())
        current = T->selectedItems()[0]->row();
    QString dep = QInputDialog::getItem(this, tr("Выберите отделение для привязки диагноза"),
                                         tr("Отделение:"), variants, current, false, &ok);
    if (!ok || dep.isEmpty()) return;
    QString dep_ID;
    for (int i=0; i<T->rowCount();++i) {
        if (T->item(i,1)->text() == dep)
        {
            dep_ID = T->item(i,0)->text();
            break;
        }
    }
    if (dep_ID.isEmpty()) return;
    //Создаем транзакцию
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    //Проверяем есть ли такой диагноз уже в базе
    if (!query.exec("SELECT ID FROM Diagnosis WHERE "
                    "Name = '" + newName + "'"
                    ))
    {
        QMessageBox::warning(this, tr("Ошибка проверки на дублирование:"),
                             query.lastError().text());
        QSqlDatabase::database().rollback();
        return;
    }
    else
    {
        if (query.first())
        {
            QMessageBox::warning(this, tr("Обнаружен дубликат:"),
                                 tr("Такой диагноз уже есть"));
            QSqlDatabase::database().rollback();
            return;
        }
    }
    //Получаем ID нового диагноза
    QString ID;
    if (!query.exec("SELECT MAX(ID) + 1 FROM Diagnosis"))
    {
        QMessageBox::warning(this, tr("Ошибка получения нового ID:"),
                             query.lastError().text());
        QSqlDatabase::database().rollback();
        return;
    }
    else
    {
        query.first();
        ID = query.value(0).toString();
    }

    //Подготовка и запись в базу
    query.prepare("INSERT INTO Diagnosis (ID,Name,Department_ID) VALUES("
                "'" + ID + "',"
                "'" + newName + "',"
                "'" + dep_ID + "')"
               );
    if (!query.exec())
    {
        QMessageBox::warning(this, tr("Ошибка записи в базу!:"),
                             query.lastError().text());
        QSqlDatabase::database().rollback();
        return;
    }
    else
    {
        QSqlDatabase::database().commit();
        QMessageBox::information(this, tr("Успех:"),tr("Запись выполнена!"));
        UpdateListDiag();
    }
}

//Флажок группы отделений
void DepartmentEditor::on_groupBox_clicked(bool /*checked*/)
{
    SetComposition(GetComposition());
}

//Флажок группы диагнозов
void DepartmentEditor::on_groupBox_2_clicked(bool /*checked*/)
{
    SetComposition(GetComposition());
}

void DepartmentEditor::ChangeTitle(int variant)
{
    QStringList sTitle;
    sTitle << tr("Ничего не выбрано")<< tr("Отделения")
           << tr("Диагнозы") << tr("Отделения и Диагнозы");
    if (variant < 3) setWindowTitle(sTitle[variant]);
    else setWindowTitle(sTitle[3]);
}

int DepartmentEditor::GetComposition()
{
    if (ui->groupBox->isChecked() && ui->groupBox_2->isChecked())           return 3;
    else if(!ui->groupBox->isChecked() && !ui->groupBox_2->isChecked())     return 0;
    else if(ui->groupBox->isChecked())                                      return 1;
    else                                                                    return 2;
}

void DepartmentEditor::SetComposition(int variant)
{
    if (variant == 0)
    {
        gbColapse(ui->groupBox, false);
        gbColapse(ui->groupBox_2, false);
        ui->verticalSpacer_3->changeSize(6,20,QSizePolicy::Fixed,
                                         QSizePolicy::Expanding);
    }
    else if (variant == 1)
    {
        gbColapse(ui->groupBox, true);
        gbColapse(ui->groupBox_2, false);
        ui->verticalSpacer_3->changeSize(6,0,QSizePolicy::Fixed);
    }
    else if (variant == 2)
    {
        gbColapse(ui->groupBox, false);
        gbColapse(ui->groupBox_2, true);
        ui->verticalSpacer_3->changeSize(6,0,QSizePolicy::Fixed);
    }
    else
    {
        gbColapse(ui->groupBox, true);
        gbColapse(ui->groupBox_2, true);
        ui->verticalSpacer_3->changeSize(6,0,QSizePolicy::Fixed);
    }
    ChangeTitle(variant);
}

void DepartmentEditor::gbColapse(QGroupBox* gb, bool&& status)
{
    if(!status)
    {
        gb->setMaximumHeight(18);
        gb->setChecked(false);
    }
    else
    {
        gb->setMaximumHeight(16777215);
        gb->setChecked(true);
    }

}

//Процедура переименования диагноза
void DepartmentEditor::RenameDiag(const QString& ID, const QString& newName)
{
    QSqlQuery query;
    if (!query.exec("UPDATE (SELECT Name FROM Diagnosis WHERE ID = " + ID + ") "
                    "SET Name = '" + newName + "'"))
    {
        QMessageBox::warning(this, tr("Ошибка переименования:"),
                             query.lastError().text());
        return;
    }
    else
    {
        UpdateList();
    }
}

//Переименовать диагноз
void DepartmentEditor::on_pushButton_Rename_2_clicked()
{
    QTableWidget* t = ui->tableWidget_Diagnosis;
    int row = t->currentRow();
    QString ID = t->item(row,0)->text();
    QString oldName = t->item(row,1)->text();
    QString newName = QInputDialog::getText(this, tr("Новое название"),
                                       tr("Ведите:"),QLineEdit::Normal,oldName);
    if (!newName.isEmpty() && !ID.isEmpty())
        RenameDiag(ID,newName);
}

//Удалить диагноз
void DepartmentEditor::on_pushButton_Delete_2_clicked()
{
    DeleteRowD(ui->tableWidget_Diagnosis->
               item(ui->tableWidget_Diagnosis->currentRow(),0)->text());
}

//Процедура удаления диагноза
void DepartmentEditor::DeleteRowD(const QString& ID)
{
    QSqlQuery query;
    if (!query.exec("DELETE FROM Diagnosis WHERE ID=" + ID))
    {
        QMessageBox::warning(this, tr("Ошибка удаления диагноза:"),
                             query.lastError().text());
        return;
    }
    else
    {
        UpdateList();
    }
}
