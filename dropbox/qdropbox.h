#ifndef QDROPBOX_H
#define QDROPBOX_H

#include <QObject>

#define DROP_BOX_CONTENT_SERVER "https://content.dropboxapi.com/2/"
#define DROP_BOX_API_SERVER "https://api.dropboxapi.com/2/"


class QDropboxAuth;
class QDropboxInfo;

class QDropbox : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QDropboxAuth* dropboxAuth READ dropboxAuth WRITE setDropboxAuth NOTIFY dropboxAuthChanged)
    Q_PROPERTY(QDropboxInfo* info READ info NOTIFY infoUpdated)

    Q_PROPERTY(QString appKey READ appKey WRITE setAppKey NOTIFY appKeyChanged)
    Q_PROPERTY(QString appSecret READ appSecret WRITE setAppSecret NOTIFY appSecretChanged)

    public:
        QDropbox(QObject *parent = 0);
        QDropbox(QString appKey, QString appSecret, QObject *parent = 0);
        ~QDropbox();

        void auth();

        void setDropboxAuth(QDropboxAuth *);
        QDropboxAuth *dropboxAuth();

        void setAppKey(QString );
        QString appKey();

        void setAppSecret(QString );
        QString appSecret();

        QDropboxInfo *info();

    public slots:
        void updateInfo();

    private:
        QString m_appKey;
        QString m_appSecret;

        QString m_authToken;

        QDropboxAuth *m_dropboxAuth;
        QDropboxInfo *m_dropboxInfo;

    signals:
        void error(QString error);

        void authComplete();

        void dropboxAuthChanged(QDropboxAuth *);

        void infoUpdated();
        void loadAllInfo();

        void appKeyChanged(QString );
        void appSecretChanged(QString );
};

#endif // QDROPBOX_H
