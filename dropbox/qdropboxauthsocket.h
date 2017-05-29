#ifndef QDROPBOXAUTHSOCKET_H
#define QDROPBOXAUTHSOCKET_H

#include <QObject>
#include <QTcpServer>


class QDropboxAuthSocket : public QTcpServer
{
    Q_OBJECT

    public:
        QDropboxAuthSocket(QString redirectUrl = "", QObject *parent = 0);

    private:
        QString m_redirectUrl;

    private slots:
        void newClient();
        void clientReady();

    signals:
        void codeComplete(QString );
};

#endif // QDROPBOXAUTHSOCKET_H
