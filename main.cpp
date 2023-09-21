#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QRegularExpression>

void QSSConnect(const QString &path);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //Загружаем перевод по умолчанию
    QTranslator qt_translator;
    qt_translator.load(":/.qm/.qm/qtbase_ru.qm");
    qApp->installTranslator(&qt_translator);
    //Загружаем qss файл
    QSSConnect(":/Qss/styles/style.qss");
    //Запуск главного окна и приложения
    MainWindow w;
    w.show();
    return a.exec();
}

//Препроцессинг qss файла
void QSSPreproc(QString& qss)
{
    //Находим переменные
    QRegularExpression re("(\\$\\w+)\\s*=\\s*([^\\r\\n;]+)");
        auto matches = re.globalMatch(qss);
        QMap<QString, QString> variables;
        while (matches.hasNext()) {
            auto match = matches.next();
            variables[match.captured(1)] = match.captured(2);
        }
    //Удаляем строки с определением переменных,
    //заменяем переменные на значения
    qss.replace(QRegExp("(\\$\\w+)\\s*=\\s*([^\\r\\n]+)"),"");
    QMapIterator<QString,QString> i(variables);
    while (i.hasNext()) {
        i.next();
        qss.replace(i.key(), i.value());
    }
    return;
}

//Подключеие QSS стиля
void QSSConnect(const QString &path)
{
    QFile qssFile(path);
    // Открыть файл только для чтения
    qssFile.open(QFile::ReadOnly);
    if(qssFile.isOpen())
    {
        //Считываем все из файла стилей
        QString qss = QLatin1String(qssFile.readAll());
        //Делаем препроцессинг файла стилей
        QSSPreproc(qss);
        //Устанавливаем стиль
        qApp->setStyleSheet(qss);
        qssFile.close();
    }
    else
    {
        qDebug() << "Не найден файл стиля";
    }
    return;
}
