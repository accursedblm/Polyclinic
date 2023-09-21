#ifndef STAFFEDITOR_H
#define STAFFEDITOR_H

#include <QDialog>
#include <QItemDelegate>
#include <QComboBox>
#include <QTableView>
#include <QStyledItemDelegate>
#include <QApplication>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QCompleter>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QKeyEvent>
#include <QSortFilterProxyModel>


namespace Ui {

/* *********************************************************************
                         Прототипы делегата
********************************************************************** */
class ReadOnlyDelegate;
class ComboDelegate;
class StaffEditor;
class Combo;
}

/* *********************************************************************
                               Диалог
********************************************************************** */
class StaffEditor : public QDialog
{
    Q_OBJECT

public:
    explicit StaffEditor(QWidget *parent = nullptr);
    ~StaffEditor();
    QTableView *tw;
private:
    Ui::StaffEditor *ui;
    QStandardItemModel *model;
    QItemSelectionModel *smodel;
    void setupTableView();
    void connectSignalRowChange(bool b);
    void connectSignalCurrentChange(bool b);
public:
    int BaseInsertString(QString& Dep, QString& Doc);
    int BaseUpdateString(QString& ID, QString& Dep, QString& Doc);
    int RefreshTable();
private slots:
    void on_pushButton_Exit_clicked();
    void on_pushButton_Add_clicked();
    void on_pushButton_Update_clicked();
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);
    void currentRowChanged(const QModelIndex &current, const QModelIndex &previous);
    void on_pushButton_Delete_clicked();
};

/* *********************************************************************
                  Делегат ячейки только для чтения
********************************************************************** */
class ReadOnlyDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ReadOnlyDelegate(QObject *parent = 0): QStyledItemDelegate(parent) { }
    QWidget *createEditor(QWidget */*parent*/,
        const QStyleOptionViewItem &/*option*/,
        const QModelIndex &/*index*/) const
    {
        return NULL;
    }
};


/* *********************************************************************
                         Делегат c ComboBox
********************************************************************** */
class ComboBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
private:
    QString *squery;
    QString *searchquery;
    StaffEditor* staff;
public:
    ComboBoxDelegate(QString *query, QString *querySearch,
                     QObject *parent = 0, StaffEditor* pstaff = 0);
    ~ComboBoxDelegate();
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option,
        const QModelIndex &index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
        const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void paint(QPainter* painter,
        const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

#endif
