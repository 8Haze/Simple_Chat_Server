#include "server.h"

#include <QDebug>

Server::Server(QObject* parent) :
    QObject(parent)
{
    chat_server.setMaxPendingConnections(10);

    connect(&chat_server, &QTcpServer::newConnection, this, &Server::on_new_client_connection);

    if (chat_server.listen(QHostAddress::Any, 8001))
        qDebug() << "Server has started. Listening to port 8001.";
    else
        qDebug() << "Server startup error: " + chat_server.errorString();
}

void Server::send_message_to_clients(QString message)
{
    for (int a = 0; a < clients.size(); ++a)
        if (clients[a]->isOpen() && clients[a]->isWritable())
            clients[a]->write(message.toUtf8());
}

void Server::on_new_client_connection()
{
    QTcpSocket* client = chat_server.nextPendingConnection();
    QString client_address = client->peerAddress().toString() + ":" + QString::number(client->peerPort());

    connect(client, &QTcpSocket::disconnected,  this, &Server::on_socket_disconnected);
    connect(client, &QTcpSocket::readyRead, this, &Server::on_socket_ready_read);

    clients.push_back(client);

    QString members_list;
    for (const auto& member_name : members_names)
        members_list += member_name + '#';

    members_list.chop(1);
    clients.back()->write(members_list.toUtf8() + 'E');

    qDebug() << "Socket connected from " + client_address;
}

void Server::on_socket_disconnected()
{
    QTcpSocket* client = qobject_cast<QTcpSocket*>(QObject::sender());
    QString client_address = client->peerAddress().toString() + ":" + QString::number(client->peerPort());

    qDebug() << "Socket disconnected from " + client_address;
}

void Server::on_socket_ready_read()
{
    QTcpSocket* client = qobject_cast<QTcpSocket*>(QObject::sender());

    QString message = client->readAll();
    QString chopped_message = message.chopped(1);
    char message_type = message.toStdString().back();

    switch (message_type)
    {
    case('J'): members_names.insert(chopped_message); qDebug() << "New username detected: " + chopped_message; break;
    case('L'): members_names.erase(chopped_message); qDebug() << "Username removed: " + chopped_message; break;
    }

    qDebug() << "Message (" + client->peerAddress().toString() + ":" + QString::number(client->peerPort()) + ") - " + message;

    send_message_to_clients(message);
}
