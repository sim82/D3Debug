#ifndef DEBUGCONNECTION_H
#define DEBUGCONNECTION_H

#include <QObject>
#include <QtNetwork/QTcpSocket>

struct ScriptInfo
{
public:
    int id;
    QString name;
};

Q_DECLARE_METATYPE(ScriptInfo);



class DebugConnection : public QObject
{
    Q_OBJECT
public:

    DebugConnection(QObject *parent);

    void test();
    int64_t scriptGet(int id);
    int64_t scriptInfo();

signals:
    void scriptGetReply(int64_t token, QVector<QString> lines);
    void scriptInfoReply(int64_t token, QVector<ScriptInfo> infos);
    void connected();

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
