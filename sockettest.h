#ifndef SOCKETTEST_H
#define SOCKETTEST_H

#include <QTcpSocket>
#include <QtDebug>
#include <QObject>
#include <iostream>

class socketTest : public QObject
{
public:
    socketTest();

    void connect();
    void runRPS(QTcpSocket* socket);

    QTcpSocket *socket;
    QString menu;
    QString welcome;
};

#endif // SOCKETTEST_H
