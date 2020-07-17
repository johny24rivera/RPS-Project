#include "myserver.h"
#include <iostream>

MyServer::MyServer()
{
    server = new QTcpServer(this); // initiate server

    // When signal newConnection is occurs onNewConnection function runs
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

/*
 * Client connnection handler
 */
void MyServer::onNewConnection()
{
    qDebug() << "Entered new connection";
    QTcpSocket *socket = server->nextPendingConnection(); //attaches socket to pending connection
    socket->waitForReadyRead(1000); // Waits until message arrives or 1000ms timeout whichever comes first
    qDebug() << socket->readAll();
    socket->write("hello client\r\n");
    socket->flush();

    // Call Rock Paper Scissors game
    runRPS(socket);
    socket->close();
}

/*
 * Rock Paper Scissors game logic
 */
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
            return;
        }

        socket->write(result);
    }
}

/*
 *  Random number generator to create AI response
 */
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

/*
 *  Determine Round winner
 */
char MyServer::getResult(char player1, char player2)
{
    qDebug() << player1 << player2;

    // Quit condition
    if (player1 == 'Q')
    {
        return 'Q';
    }

    // Draw Condition
    else if (player1 == player2)
    {
        return 'D';
    }

    // Player 1 Win Conditions
    else if((player1 == 'R' && player2 == 'S') ||
            (player1 == 'S' && player2 == 'P') ||
            (player1 == 'P' && player2 == 'R'))
    {
        return 'W';
    }

    // Rest deteremines Computer Win conditions
    else
    {
        return 'L';
    }
}
