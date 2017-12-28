#ifndef DEBUGCONNECTIONTYPES_H
#define DEBUGCONNECTIONTYPES_H
#include <QString>
#include <QMetaType>

class DebugConnection;

struct ScriptInfo
{
public:
    int id;
    QString name;
};

Q_DECLARE_METATYPE(ScriptInfo);


#endif // DEBUGCONNECTIONTYPES_H
