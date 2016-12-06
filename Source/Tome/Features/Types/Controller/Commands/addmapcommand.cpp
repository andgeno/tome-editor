#include "addmapcommand.h"

#include "../typescontroller.h"

using namespace Tome;


AddMapCommand::AddMapCommand(TypesController& typesController,
                             const QString& name,
                             const QString& keyType,
                             const QString& valueType,
                             const QString& customTypeSetName) :
    typesController(typesController),
    name(name),
    keyType(keyType),
    valueType(valueType),
    customTypeSetName(customTypeSetName)
{
    this->setText("Add Map - " + name);
}

void AddMapCommand::undo()
{
    this->typesController.removeCustomType(this->name);
}

void AddMapCommand::redo()
{
    this->typesController.addMap(this->name, this->keyType, this->valueType, this->customTypeSetName);
}
