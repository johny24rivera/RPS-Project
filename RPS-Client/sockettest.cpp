#include "sockettest.h"

socketTest::socketTest()
{
    welcome = "\n\nWelcome!!! Let's play rock paper scissors\n";
    menu = "Enter R P S or Q\nR = Rock\nP = Paper\nS = Scissors\nQ = Quit\n";
    connect();
}

void socketTest::connect()
{
    socket = new QTcpSocket(this);
    socket->connectToHost("localhost", 1234);

    if (socket->waitForConnected(3000))
    {
        qDebug() << "Connected!";
        socket->write("hello server\r\n\r\n");
        socket->flush();                                                //Send all bytes to server
        socket->waitForReadyRead(1000);                                 //Wait before reading
        qDebug() <<  "Reading response: " << socket->bytesAvailable();  // amount of bytes
        qDebug() <<  socket->readAll();

        runRPS(socket);

        socket->close();
    }
    else
    {
        qDebug() << "not connected...";
    }

    // close connection
}

/*
 * Rock Paper Scissors game algorithm
 */

void socketTest::runRPS(QTcpSocket* _socket)
{
    bool run = true;
    std::cout << welcome.toStdString();
    while(run)
    {
        std::cout  << menu.toStdString();
        char response[2];
        qDebug() <<  "Enter choice: ";
        std::cin >> response[0];

        // Input Validation
        while (response[0] != 'R' && response[0] != 'P' &&
                     response[0] != 'S' && response[0] != 'Q')
        {
            qDebug() << "Invalid entry\n";
            qDebug() << "Enter input: ";
            std::cin >> response;
        }

        if (response[0] == 'Q')
           run = false;

        _socket->write(response);
        _socket->waitForReadyRead();
        qDebug() << _socket->readAll();
    }
}
