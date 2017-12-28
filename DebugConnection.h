#ifndef DEBUGCONNECTION_H
#define DEBUGCONNECTION_H

#include "DebugConnectionTypes.h"
#include <QObject>
#include <QtNetwork/QTcpSocket>

class DebugConnection : public QObject
{
    Q_OBJECT
public:
    DebugConnection(QObject *parent);

    void test();
    int64_t scriptGet(int id);
    int64_t scriptInfo();
    int64_t execute(QString scriptText, bool immediate = true);
signals:
    void connected();

    void scriptGetReply(int64_t token, QVector<QString> lines);
    void scriptInfoReply(int64_t token, QVector<ScriptInfo> infos);
    void executeReply(int64_t, QString consoleOutput, bool error);

    void eventWatchpoint(int watchpointId, int scriptId, int lineNumbers, QVector<QString> localNames,
                         QVector<QVariant> localValues);

private slots:
    void on_socket_connected(void);
    void on_socket_error();
    void on_socket_hostFound();
    void on_socket_readyRead();

private:
    void messageReceived(std::vector<char> message);

    QTcpSocket *socket;
    uint32_t nextMessageSize_{0};
    int64_t nextToken_{1};
};

#endif // DEBUGCONNECTION_H
