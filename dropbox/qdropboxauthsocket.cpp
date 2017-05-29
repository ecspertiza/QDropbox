#include "qdropboxauthsocket.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>

QDropboxAuthSocket::QDropboxAuthSocket(QString redirectUrl, QObject *parent)
    :QTcpServer(parent), m_redirectUrl(redirectUrl)
{
    connect(this, SIGNAL(newConnection()), this, SLOT(newClient()));
}

void QDropboxAuthSocket::newClient()
{
    QTcpSocket *clientConnection = this->nextPendingConnection();
    connect(clientConnection,SIGNAL(readyRead()),this,SLOT(clientReady()));
}

void QDropboxAuthSocket::clientReady()
{
    QTcpSocket *clientConnection = static_cast<QTcpSocket *>(QObject::sender());
    QString request = clientConnection->readAll();

    QStringList headerLines = request.split("\r\n");
    int  find_code= headerLines.first().indexOf("code=");

    if (find_code != -1) {
        QString code =  headerLines.first().mid(find_code + 5,  headerLines.first().indexOf(" ", find_code + 5) - (find_code + 5));
        emit codeComplete(code);
    }

    if (m_redirectUrl.isEmpty()) {
        QString header = "HTTP/1.1 200 OK\r\n"
                           "Date: Wed, 11 Feb 2009 11:20:59 GMT\r\n"
                           "Server: Apache\r\n"
                           "X-Powered-By: PHP/5.2.4-2ubuntu5wm1\r\n"
                           "Last-Modified: Wed, 11 Feb 2009 11:20:59 GMT\r\n"
                           "Content-Language: ru\r\n"
                           "Connection: close\r\n\r\n";

        QString body = "drpbox auth complete";

        QString response = header + body;

        clientConnection->write(response.toUtf8());
        clientConnection->close();
    }else {
        QString header = "HTTP/1.1 303 OK\r\n"
                           "Date: Wed, 11 Feb 2009 11:20:59 GMT\r\n"
                           "Server: Apache\r\n"
                           "X-Powered-By: PHP/5.2.4-2ubuntu5wm1\r\n"
                           "Last-Modified: Wed, 11 Feb 2009 11:20:59 GMT\r\n"
                           "Content-Language: ru\r\n"
                           "Location: " + m_redirectUrl + "\r\n";
                           "Connection: close\r\n\r\n";

        QString response = header;

        clientConnection->write(response.toUtf8());
        clientConnection->close();
    }
}
