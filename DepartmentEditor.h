#ifndef DEPARTMENTEDITOR_H
#define DEPARTMENTEDITOR_H

#include <QDialog>
#include <QSql>
#include <QSqlQuery>
#include <QSqlError>
#include <QTableWidgetItem>
#include <QGroupBox>

namespace Ui {
class DepartmentEditor;
}

class DepartmentEditor : public QDialog
{
    Q_OBJECT

public:
    explicit DepartmentEditor(QWidget *parent = nullptr, int Composition = 3);
    ~DepartmentEditor();

private slots:
    void on_pushButton_Add_clicked();
    void on_pushButton_Rename_clicked();
    void on_pushButton_Delete_clicked();
    void on_pushButton_Exit_clicked();
    void on_pushButton_Update_clicked();
    void on_tableWidget_Departments_cellChanged(int row, int column);
    void on_tableWidget_Departments_itemSelectionChanged();
    void on_pushButton_Exit_2_clicked();
    void on_pushButton_Update_2_clicked();
    void on_pushButton_Add_2_clicked();
    void on_groupBox_clicked(bool checked);
    void on_groupBox_2_clicked(bool checked);
    void on_pushButton_Rename_2_clicked();
    void on_pushButton_Delete_2_clicked();

private:
    Ui::DepartmentEditor *ui;

private:
    void UpdateList();
    void UpdateListDiag();
    void Rename(const QString& ID, const QString& newName);
    void RenameDiag(const QString& ID, const QString& newName);
    void DeleteRow(const QString& ID);
    void DeleteRowD(const QString& ID);
    QString SelRowText(int column);
    void ChangeTitle(int variant = 3);
    int GetComposition();
    void SetComposition(int variant = 3);
    void gbColapse(QGroupBox* gb, bool&& status);
};

#endif // DEPARTMENTEDITOR_H
