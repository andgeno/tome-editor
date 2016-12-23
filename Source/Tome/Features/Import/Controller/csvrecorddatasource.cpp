#include "csvrecorddatasource.h"

#include <QFile>
#include <QTextStream>

using namespace Tome;


CsvRecordDataSource::CsvRecordDataSource()
{
}

void CsvRecordDataSource::importData(const RecordTableImportTemplate& importTemplate, const QVariant& context)
{
    // Open CSV file.
    const QString filePath = context.toString();
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly))
    {
        QString errorMessage = QObject::tr("Source file could not be read:\r\n") + filePath;
        qCritical(errorMessage.toUtf8().constData());
        emit this->dataUnavailable(errorMessage);
        return;
    }

    QTextStream textStream(&file);
    textStream.setCodec("UTF-8");

    // Read headers.
    QString line;
    line = textStream.readLine();

    if (line.isEmpty())
    {
        QString errorMessage = QObject::tr("Source file is empty:\r\n") + filePath;
        qCritical(errorMessage.toUtf8().constData());
        emit this->dataUnavailable(errorMessage);
        return;
    }

    QStringList headers = line.split(';');

    // Find id column.
    int idColumnIndex = -1;

    for (int i = 0; i < headers.count(); ++i)
    {
        if (headers[i] == importTemplate.idColumn)
        {
            idColumnIndex = i;
            break;
        }
    }

    if (idColumnIndex == -1)
    {
        QString errorMessage = QObject::tr("Could not find id column %1 in source file:\r\n%2")
                .arg(importTemplate.idColumn, filePath);
        qCritical(errorMessage.toUtf8().constData());
        emit this->dataUnavailable(errorMessage);
        return;
    }

    // Read rows.
    QMap<QString, RecordFieldValueMap> data;
    int rowIndex = 1;

    while (!(line = textStream.readLine()).isEmpty())
    {
        QStringList row = line.split(';');

        if (row.count() != headers.count())
        {
            QString errorMessage = QObject::tr("Row %1 has %2 columns, but the header has %3 columns.")
                    .arg(QString::number(rowIndex), QString::number(row.count()), QString::number(headers.count()));
            qCritical(errorMessage.toUtf8().constData());
            emit this->dataUnavailable(errorMessage);
            return;
        }

        RecordFieldValueMap map;

        for (int i = 0; i < row.count(); ++i)
        {
            if (i == idColumnIndex)
            {
                continue;
            }

            map[headers[i]] = row[i];
        }

        data[row[idColumnIndex]] = map;
    }

    emit this->dataAvailable(data);
}
