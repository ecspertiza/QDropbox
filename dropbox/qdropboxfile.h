#ifndef QDROPBOXFILE_H
#define QDROPBOXFILE_H

#include <QObject>

#include <QNetworkAccessManager>

class QDropboxAuth;

class QDropboxFile : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QDropboxAuth* dropboxAuth READ dropboxAuth WRITE setDropboxAuth NOTIFY dropboxAuthChanged)

    Q_PROPERTY(bool shortUrl READ shortUrl WRITE setShortUrl NOTIFY shortUrlChanged)

    public:
        explicit QDropboxFile(QObject *parent = 0);
        explicit QDropboxFile(QDropboxAuth *auth, QObject *parent = 0);
        explicit QDropboxFile(QString serverFileName, QDropboxAuth *auth, QObject *parent = 0);

        Q_INVOKABLE void upload(QString localFileName, bool link = false); // if link is true - cerate share link
        Q_INVOKABLE void shareUrl();

        void setShortUrl(bool );
        bool shortUrl();

        QDropboxAuth *dropboxAuth();
        void setDropboxAuth(QDropboxAuth *);

    private:
        QDropboxAuth *m_auth;

        bool m_createShareLink;

        bool m_shortUrl;

        QString m_serverFileName;

        QNetworkAccessManager *m_manager;
        QNetworkAccessManager *manager();
        void destroyManager();

    signals:
        void error(QString );
        void uploadProgress(qint64,qint64);

        void dropboxAuthChanged(QDropboxAuth *);
        void shortUrlChanged(bool );

        void shareLinkComplete(QString );

    private slots:
        void managerFinished(QNetworkReply*);

        void uploadFinished(QNetworkReply *);
};

#endif // QDROPBOXFILE_H
