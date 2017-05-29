#ifndef QDROPBOXAUTH_H
#define QDROPBOXAUTH_H

#include <QObject>

class QNetworkAccessManager;
class QDropboxAuthSocket;
class QNetworkReply;
class QNetworkRequest;

class QDropboxAuth : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString redirectUrlAfterAuth READ redirectUrlAfterAuth WRITE setRedirectUrlAfterAuth NOTIFY redirectUrlAfterAuthChanged)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)

    Q_PROPERTY(QString accessToken READ accessToken WRITE setAccessToken NOTIFY accessTokenChanged)

    Q_PROPERTY(QString appKey READ appKey WRITE setAppKey NOTIFY appKeyChanged)
    Q_PROPERTY(QString appSecret READ appSecret WRITE setAppSecret NOTIFY appSecretChanged)

    public:
        QDropboxAuth(QString appKey, QString appSecret, QObject *parent = 0);
        ~QDropboxAuth();

        Q_INVOKABLE void auth();
        QNetworkRequest &authRequest(QNetworkRequest &);

        void setRedirectUrlAfterAuth(QString url);
        QString redirectUrlAfterAuth();

        void setPort(int port);
        int port();

        void setAccessToken(QString );
        QString accessToken();

        void setAppKey(QString );
        QString appKey();

        void setAppSecret(QString );
        QString appSecret();

    private:
        int m_port;

        QString m_appKey;
        QString m_appSecret;

        QString m_accessToken;

        QString m_redirectUrlAfterAuth;

        QNetworkAccessManager *m_manager;
        QDropboxAuthSocket *m_responseServer;

        QNetworkAccessManager *manager();
        void destroyManager();


    private slots:
        void codeComplete(QString );
        void managerFinished(QNetworkReply *);

    signals:
        void authComplete();

        void error(QString );

        void accessTokenChanged(QString );
        void redirectUrlAfterAuthChanged(QString );
        void portChanged(int );

        void appKeyChanged(QString );
        void appSecretChanged(QString );
};

#endif // QDROPBOXAUTH_H
