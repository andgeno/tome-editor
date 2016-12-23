#include "importcontroller.h"

using namespace Tome;

#include "recorddatasource.h"
#include "csvrecorddatasource.h"
#include "googlesheetsrecorddatasource.h"
#include "xlsxrecorddatasource.h"
#include "../../Fields/Controller/fielddefinitionscontroller.h"
#include "../../Records/Controller/recordscontroller.h"


ImportController::ImportController(FieldDefinitionsController& fieldDefinitionsController, RecordsController& recordsController)
    : fieldDefinitionsController(fieldDefinitionsController),
      recordsController(recordsController)
{
}

void ImportController::addRecordImportTemplate(const RecordTableImportTemplate& importTemplate)
{
    qInfo(QString("Adding import template %1.").arg(importTemplate.name).toUtf8().constData());

    // Update model.
    this->model->push_back(importTemplate);

    // Notify listeners.
    emit this->importTemplatesChanged();
}

const RecordTableImportTemplate ImportController::getRecordTableImportTemplate(const QString& name) const
{
    for (RecordTableImportTemplateList::iterator it = this->model->begin();
         it != this->model->end();
         ++it)
    {
        if (it->name == name)
        {
            return *it;
        }
    }

    QString errorMessage = QObject::tr("Import template not  found: ") + name;
    qCritical(errorMessage.toUtf8().constData());
    throw std::runtime_error(errorMessage.toStdString());
}

const RecordTableImportTemplateList ImportController::getRecordTableImportTemplates() const
{
    return *this->model;
}

void ImportController::importRecords(const RecordTableImportTemplate& importTemplate, const QVariant& context)
{
    qInfo(QString("Importing data from %1 with import template %2.")
             .arg(context.toString(), importTemplate.name).toUtf8().constData());

    // Create data source.
    RecordDataSource* dataSource;

    switch (importTemplate.sourceType)
    {
        case TableType::Csv:
            dataSource = new CsvRecordDataSource();
            break;

        case TableType::GoogleSheets:
            dataSource = new GoogleSheetsRecordDataSource();
            break;

        case TableType::Xlsx:
            dataSource = new XlsxRecordDataSource();
            break;

        default:
            emit this->dataUnavailable(importTemplate.name, "Unknown import type.");
            return;
    }

    // Read data from source.
    connect(dataSource,
            SIGNAL(dataAvailable(const QString&, const QMap<QString,RecordFieldValueMap>&)),
            SLOT(onDataAvailable(const QString&, const QMap<QString,RecordFieldValueMap>&)));

    connect(dataSource,
            SIGNAL(dataUnavailable(const QString&, const QString&)),
            SLOT(onDataUnavailable(const QString&, const QString&)));

    dataSource->importData(importTemplate, context);
}

void ImportController::removeImportTemplate(const QString& name)
{
    qInfo(QString("Removing import template %1.").arg(name).toUtf8().constData());

    // Update model.
    for (RecordTableImportTemplateList::iterator it = this->model->begin();
         it != this->model->end();
         ++it)
    {
        if (it->name == name)
        {
            this->model->erase(it);

            // Notify listeners.
            emit this->importTemplatesChanged();
            return;
        }
    }
}

void ImportController::setRecordTableImportTemplates(RecordTableImportTemplateList& importTemplates)
{
    this->model = &importTemplates;
}

void ImportController::onDataAvailable(const QString& importTemplateName, const QMap<QString, RecordFieldValueMap>& data) const
{
    const RecordTableImportTemplate& importTemplate = this->getRecordTableImportTemplate(importTemplateName);
    const QStringList& recordSetNames = this->recordsController.getRecordSetNames();
    const QString& recordSetName = recordSetNames.first();

    // Update records.
    int recordsAdded = 0;
    int fieldsUpdated = 0;
    int fieldsSkipped = 0;
    int fieldsUpToDate = 0;

    for (QMap<QString, RecordFieldValueMap>::const_iterator itRecords = data.begin();
         itRecords != data.end();
         ++itRecords)
    {
        // Get record.
        const QString& recordId = itRecords.key();
        const RecordFieldValueMap& newRecordFieldValues = itRecords.value();

        if (!this->recordsController.hasRecord(recordId))
        {
            this->recordsController.addRecord(recordId, recordId, QStringList(), recordSetName);
            this->recordsController.reparentRecord(recordId, importTemplate.rootRecordId);
            ++recordsAdded;
        }
        else
        {
            qInfo(QString("Updating record %1.").arg(recordId).toUtf8().constData());
        }

        // Get current record field values.
        const RecordFieldValueMap oldRecordFieldValues = this->recordsController.getRecordFieldValues(recordId);

        for (RecordFieldValueMap::const_iterator itFields = newRecordFieldValues.begin();
             itFields != newRecordFieldValues.end();
             ++itFields)
        {
            // Get field.
            QString fieldId = itFields.key();
            QVariant fieldValue = itFields.value();

            // Check if field is mapped.
            if (importTemplate.columnMap.contains(fieldId))
            {
                fieldId = importTemplate.columnMap[fieldId];
            }

            if (!this->fieldDefinitionsController.hasFieldDefinition(fieldId))
            {
                qWarning(QString("Skipping unknown field: %1").arg(fieldId).toUtf8().constData());
                ++fieldsSkipped;
                continue;
            }

            // Check if needs update.
            if (oldRecordFieldValues.contains(fieldId) && oldRecordFieldValues[fieldId] == newRecordFieldValues[fieldId])
            {
                ++fieldsUpToDate;
            }
            else
            {
                this->recordsController.updateRecordFieldValue(recordId, fieldId, fieldValue);
                ++fieldsUpdated;
            }
        }
    }

    qInfo(QString("Import finished. %1 new records added. %2 field values updated, %3 skipped, %4 up-to-date.")
          .arg(QString::number(recordsAdded),
               QString::number(fieldsUpdated),
               QString::number(fieldsSkipped),
               QString::number(fieldsUpToDate))
          .toUtf8().constData());
}

void ImportController::onDataUnavailable(const QString& importTemplateName, const QString& error) const
{
    emit this->dataUnavailable(importTemplateName, error);
}
