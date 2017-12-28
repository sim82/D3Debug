#include "DebugConnection.h"
#include "game.capnp.h"
#include <QDialog>
#include <QtNetwork/QHostAddress>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <iostream>

DebugConnection::DebugConnection(QObject *parent)
    : QObject(parent)
{
    socket = new QTcpSocket(this);
    socket->setObjectName("socket");
    QMetaObject::connectSlotsByName(this);
    socket->connectToHost("localhost", 8094);
}

void DebugConnection::test()
{
}

void DebugConnection::on_socket_connected(void)
{
    emit connected();
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
    while (true)
    {
        if (nextMessageSize_ == 0 && socket->bytesAvailable() >= sizeof(uint32_t))
        {
            socket->read((char *)&nextMessageSize_, sizeof(uint32_t));
        }
        else if (nextMessageSize_ != 0 && nextMessageSize_ <= socket->bytesAvailable())
        {
            std::vector<char> inputBuffer(nextMessageSize_);
            socket->read(inputBuffer.data(), nextMessageSize_);
            messageReceived(inputBuffer);
            nextMessageSize_ = 0;
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
    auto last  = first + message.size() / sizeof(capnp::word);

    capnp::FlatArrayMessageReader mr(kj::ArrayPtr<capnp::word>(first, last));

    auto reply = mr.getRoot<cp::game::DebugReply>();

    if (reply.hasScriptGet())
    {
        auto scriptGet = reply.getScriptGet();

        auto lines = scriptGet.getSourceLines();

        QVector<QString> qtLines;

        for (auto const &line : lines)
        {
            qtLines.push_back(QString(line.cStr()));
        }

        emit scriptGetReply(reply.getToken(), std::move(qtLines));
    }
    else if (reply.hasScriptInfo())
    {
        auto scriptInfo = reply.getScriptInfo();
        QVector<ScriptInfo> infos;
        for (auto const &si : scriptInfo)
        {
            infos.push_back(ScriptInfo{si.getId(), si.getSourceName().cStr()});
        }
        emit scriptInfoReply(reply.getToken(), std::move(infos));
    }
}

class CDebugRequestBuilder
{
public:
    CDebugRequestBuilder(int64_t token)
        : builder_(mb_.initRoot<cp::game::DebugRequest>())
    {
        builder_.setToken(token);
        uint32_t tmp;
        vos_.write(&tmp, sizeof(uint32_t));
    }

    cp::game::DebugRequest::Builder getBuilder()
    {
        return builder_;
    }

    void writeMessage(QIODevice *target)
    {
        capnp::writeMessage(vos_, mb_);
        auto array    = vos_.getArray();
        uint32_t size = array.size() - sizeof(uint32_t);
        memcpy(array.begin(), &size, sizeof(uint32_t));
        target->write((const char *)array.begin(), array.size());
    }

private:
    capnp::MallocMessageBuilder mb_;
    cp::game::DebugRequest::Builder builder_;
    kj::VectorOutputStream vos_;
};

int64_t DebugConnection::scriptGet(int id)
{
    CDebugRequestBuilder builder(nextToken_);

    auto req = builder.getBuilder();

    auto scriptGet = req.initScriptGet();
    scriptGet.setId(id);
    builder.writeMessage(socket);

    return nextToken_++;
}

int64_t DebugConnection::scriptInfo()
{
    CDebugRequestBuilder builder(nextToken_);

    auto req = builder.getBuilder();
    req.initScriptInfo();

    builder.writeMessage(socket);
    return nextToken_++;
}
