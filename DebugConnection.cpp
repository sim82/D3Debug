#include "DebugConnection.h"
#include <QtNetwork/QHostAddress>
#include "game.capnp.h"
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <QDialog>
#include <iostream>

DebugConnection::DebugConnection(QObject *parent)
    : QObject (parent)
{
    socket = new QTcpSocket(this);

    QMetaObject::connectSlotsByName(this);
    QObject::connect(socket, SIGNAL(connected()), this, SLOT(on_socket_connected()));
QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(on_socket_readyRead()));
    //    socket->bind(QHostAddress("127.0.0.1"), 8094);
}

void DebugConnection::test()
{
    socket->connectToHost("localhost", 8094);


}

void DebugConnection::on_socket_connected(void)
{
    capnp::MallocMessageBuilder mb;
    auto req = mb.initRoot<cp::game::DebugRequest>();

    auto scriptGet = req.initScriptGet();
    scriptGet.setId(0);

    kj::VectorOutputStream vos;
    uint32_t size = 0;
    vos.write(&size, sizeof(uint32_t));

    capnp::writeMessage(vos, mb);

    auto array = vos.getArray();
    size = array.size() - sizeof(uint32_t);
    memcpy( array.begin(), &size, sizeof(uint32_t));

    socket->write((const char*)array.begin(), array.size());

}

void DebugConnection::on_socket_error()
{
    std::cout << "error\n";
}

void DebugConnection::on_socket_hostFound()
{
    std::cout << "host found\n";
}

void DebugConnection::on_socket_readyRead()
{
    while(true)
    {
        if (nextMessageSize_ == 0 && socket->bytesAvailable() >= sizeof(uint32_t))
        {
            socket->read((char*)&nextMessageSize_, sizeof(uint32_t));
        }
        else if (nextMessageSize_ != 0 && nextMessageSize_ <= socket->bytesAvailable())
        {
            std::vector<char> inputBuffer(nextMessageSize_);
            socket->read(inputBuffer.data(), nextMessageSize_);
            messageReceived(inputBuffer);

        }
        else
        {
            break;
        }
    }
}

void DebugConnection::messageReceived(std::vector<char> message)
{
    auto first = (capnp::word *)message.data();
    auto last = first + message.size() / sizeof(capnp::word);

    capnp::FlatArrayMessageReader mr(kj::ArrayPtr<capnp::word>(first, last));

    auto reply = mr.getRoot<cp::game::DebugReply>();

    if (reply.hasScriptGet())
    {
        auto scriptGet = reply.getScriptGet();

        auto lines = scriptGet.getSourceLines();

        QVector<QString> qtLines;

        for( auto const &line : lines )
        {
            qtLines.push_back(QString(line.cStr()));
        }

        emit scriptGetReceived(0, std::move(qtLines));
    }
}
