#include "myserver.h"
#include <iostream>

MyServer::MyServer()
{
    server = new QTcpServer(this);
    connect(server, &QTcpServer::newConnection, this, &MyServer::onNewConnection);

    if(!server->listen(QHostAddress::Any, 1234))
    {
        qDebug() << "Server could not start";
    }
    else
    {
        qDebug() << "server Started";
    }
}

void MyServer::onNewConnection()
{
    qDebug() << "Entered new connection";
    QTcpSocket *socket = server->nextPendingConnection();
    socket->waitForReadyRead(1000);
    qDebug() << socket->readAll();
    socket->write("hello client\r\n");
    socket->flush();

    runRPS(socket);
    socket->close();
}

void MyServer::runRPS(QTcpSocket* socket)
{
    bool run = true;
    while (run)
    {
        cpuChoice = getCPUAnswer();
        qDebug() << "CPU chose " << cpuChoice;
        socket->waitForReadyRead();
        QString response = socket->read(10);
        result[0] = getResult(response.toStdString().c_str()[0], cpuChoice);
        qDebug() << result;
        if (result[0] == 'Q')
        {
            run = false;
            socket->close();
            return;
        }

        socket->write(result);
    }
}

char MyServer::getCPUAnswer()
{
    srand((unsigned int)time(NULL));
    int RandomValue = rand() % 3;

    if (RandomValue == 0)
        return 'R';
    else if (RandomValue == 1)
        return 'P';
    else
        return 'S';
}

char MyServer::getResult(char player1, char player2)
{
    qDebug() << player1 << player2;
    if (player1 == 'Q')
    {
        return 'Q';
    }
    else if (player1 == player2)
    {
        return 'D';
    }
    else if((player1 == 'R' && player2 == 'S') ||
            (player1 == 'S' && player2 == 'P') ||
            (player1 == 'P' && player2 == 'R'))
    {
        return 'W';
    }
    else
    {
        return 'L';
    }
}
