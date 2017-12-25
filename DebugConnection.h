#ifndef DEBUGCONNECTION_H
#define DEBUGCONNECTION_H

#include <QObject>
#include <QtNetwork/QTcpSocket>

class DebugConnection : public QObject
{
    Q_OBJECT
public:
    DebugConnection(QObject *parent);

    void test();

signals:
    void scriptGetReceived(int id, QVector<QString> lines);

public slots:
    void on_socket_connected(void);
    void on_socket_error();
    void on_socket_hostFound();
    void on_socket_readyRead();

private:
    void messageReceived(std::vector<char> message);

    QTcpSocket *socket;
    uint32_t nextMessageSize_{0};
};

#endif // DEBUGCONNECTION_H
