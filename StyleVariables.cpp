#include "StyleVariables.h"

StyleVariables::StyleVariables(QString& path): _path(path)
{
    updateVar();
}
StyleVariables::StyleVariables(QString&& path): _path(path)
{
    updateVar();
}
int StyleVariables::updateVar()
{
    QFile qssFile(_path);
    // Открыть файл только для чтения
    qssFile.open(QFile::ReadOnly);
    if(qssFile.isOpen())
    {
        //Считываем все из файла стилей
        QString qss = QLatin1String(qssFile.readAll());
        //Закрываем файл
        qssFile.close();
        //Находим переменные, записываем в массив
        QRegularExpression re("(\\$\\w+)\\s*=\\s*([^\\r\\n;]+)");
        auto matches = re.globalMatch(qss);
        if(!variables.isEmpty())
            variables.clear();
        while (matches.hasNext()) {
            auto match = matches.next();
            variables[match.captured(1)] = match.captured(2);
        }
    }
    else
    {
        qDebug() << "Не найден файл стиля";
        return 0;
    }
    return 1;
}
