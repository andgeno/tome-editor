#ifndef RECORD_H
#define RECORD_H

#include <QMap>
#include <QString>


namespace Tome
{
    class Record
    {
        public:
            Record();

            QString displayName;
            QString id;
            QMap<QString, QString> fieldValues;
    };

    inline bool operator==(const Record& lhs, const Record& rhs){ return lhs.id == rhs.id; }
    inline bool operator!=(const Record& lhs, const Record& rhs){ return !(lhs == rhs); }

    inline bool recordLessThanDisplayName(const Record& e1, const Record& e2)
    {
        return e1.displayName.toLower() < e2.displayName.toLower();
    }
}

#endif // RECORD_H