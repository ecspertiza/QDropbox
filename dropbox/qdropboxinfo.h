#ifndef QDROPBOXINFO_H
#define QDROPBOXINFO_H

#include <QObject>

class QDropboxAuth;

class QNetworkAccessManager;
class QNetworkReply;

class QDropboxInfo : public QObject
{
    Q_PROPERTY(QString giveName READ giveName NOTIFY giveNameChanged)
    Q_PROPERTY(QString surname READ surname NOTIFY surnameChanged)
    Q_PROPERTY(QString familiarName READ familiarName NOTIFY familiarNameChanged)
    Q_PROPERTY(QString displayName READ displayName NOTIFY displayNameChanged)
    Q_PROPERTY(QString email READ email NOTIFY emailChanged)

    Q_PROPERTY(quint64 used READ used NOTIFY usedChanged)
    Q_PROPERTY(quint64 allocated READ allocated NOTIFY allocatedChanged)

    Q_OBJECT
    public:
        explicit QDropboxInfo(QDropboxAuth *dropboxAuth, QObject *parent = 0);
        ~QDropboxInfo();

        quint64 used();
        quint64 allocated();
        QString giveName();
        QString surname();
        QString familiarName();
        QString displayName();
        QString email();


    public slots:
        void updateInfo();
        void updateSpace();

    private:
        QDropboxAuth *m_auth;

        QNetworkAccessManager *m_manager;
        QNetworkAccessManager *manager();
        void destroyManager();

        QString m_giveName;
        QString m_surname;
        QString m_familiarName;
        QString m_displayName;
        QString m_email;

        quint64 m_allocated;
        quint64 m_used;

        void setUsed(quint64 );
        void setAllocated(quint64 );
        void setGiveName(QString );
        void setSurname(QString );
        void setFamiliarName(QString );
        void setDisplayName(QString );
        void setEmail(QString );

    private slots:
        void managerFinished(QNetworkReply *);

    signals:
        void error(QString );

        void giveNameChanged(QString );
        void surnameChanged(QString );
        void familiarNameChanged(QString );
        void displayNameChanged(QString );
        void emailChanged(QString );

        void usedChanged(quint64 );
        void allocatedChanged(quint64 );

        void infoUpdated();
        void spaceUpdated();
};

#endif // QDROPBOXINFO_H
