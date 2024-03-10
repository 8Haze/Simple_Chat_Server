#ifndef SERVER_H
#define SERVER_H

#include <set>

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>

class Server : public QObject
{
private:

    Q_OBJECT

    QTcpServer chat_server;
    QVector<QTcpSocket*> clients;
    std::set<QString> members_names;

public:

    explicit Server(QObject* parent = nullptr);

    void send_message_to_clients(QString message);

private slots:

    void on_new_client_connection();
    void on_socket_disconnected();
    void on_socket_ready_read();

};

#endif // SERVER_H
