#ifndef MYSERVER_H
#define MYSERVER_H

#include <QObject>
#include <QtDebug>
#include <QTcpServer>
#include <QTcpSocket>
#include <stdlib.h>
#include <time.h>

class MyServer : public QObject
{
public:
    explicit MyServer();
    void runRPS(QTcpSocket* socket);

signals:

public slots:
    void onNewConnection();

private:
    QTcpServer *server;
    char result[2];
    char cpuChoice;
    char getCPUAnswer();
    char getResult(char player1, char player2);
};

#endif // MYSERVER_H
