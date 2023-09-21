#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Dregistr.h"
#include "RegisterNewPerson.h"
#include "RegisterNewDoctor.h"
#include "DepartmentEditor.h"
#include "StaffEditor.h"
#include "TimeEditor.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //ResizeBackground();
    //Создаем объект базы данных и подключаемся к базе
    db = new DataBase(this);
    db->connectToDataBase();
    ui->statusbar->setVisible(false);
}

MainWindow::~MainWindow()
{
    delete db;
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *evt)
{
    ResizeBackground();
    QMainWindow::resizeEvent(evt); //Вызов стандартного события
}

void MainWindow::on_action_Registr_triggered()
{
    DRegistr Dr(this);
    Dr.exec();
}


void MainWindow::on_action_AddPerson_triggered()
{
    RegisterNewPerson Rg(this);
    Rg.exec();
}


void MainWindow::on_action_AddDoctor_triggered()
{
    RegisterNewDoctor Rg(this);
    Rg.exec();
}


void MainWindow::on_action_Departments_triggered()
{
    DepartmentEditor De(this);
    De.exec();
}

void MainWindow::ResizeBackground()
{
    QSize r = size();
    r.setWidth(r.height()*16/9);
    ui->label_background->
        setGeometry(width()/2-r.width()/2-1,0,r.width(),r.height());
}

void MainWindow::on_action_StaffEditor_triggered()
{
    StaffEditor Se(this);
    Se.exec();
}


void MainWindow::on_action_TimeEditor_triggered()
{
    TimeEditor Te(this);
    Te.exec();
}

void MainWindow::on_action_Cons_triggered()
{
    DRegistr Dr(this);
    Dr.setHiddenTabs(0,true);
    Dr.exec();
}

