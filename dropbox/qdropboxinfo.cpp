#include "qdropboxinfo.h"

#include <QNetworkReply>
#include <QNetworkRequest>

#include "qdropboxauth.h"
#include "qdropbox.h"

#include <QJsonDocument>
#include <QJsonObject>

#define USER_INFO_URL DROP_BOX_API_SERVER "users/"

QDropboxInfo::QDropboxInfo(QDropboxAuth *auth, QObject *parent)
    : QObject(parent), m_auth(auth), m_manager(0)
{
    connect(this, SIGNAL(infoUpdated()), this, SLOT(updateSpace()));
}

QDropboxInfo::~QDropboxInfo()
{
    qDebug() << Q_FUNC_INFO;
}

void QDropboxInfo::updateInfo()
{
    if (m_auth == 0) {
        emit error("DropboxFile auth is empty");
        return;
    }
    QNetworkRequest request(QUrl(USER_INFO_URL "get_current_account"));
    request = m_auth->authRequest(request);
    manager()->post(request, "");
}

void QDropboxInfo::updateSpace()
{
    if (m_auth == 0) {
        emit error("DropboxFile auth is empty");
        return;
    }

    QNetworkRequest request(QUrl(USER_INFO_URL "get_space_usage"));
    request = m_auth->authRequest(request);
    manager()->post(request, "");
}

quint64 QDropboxInfo::used()
{
    return m_used;
}

void QDropboxInfo::setUsed(quint64 arg)
{
    if (m_used != arg) {
        m_used = arg;
        usedChanged(m_used);
    }
}

quint64 QDropboxInfo::allocated()
{
    return m_allocated;
}

void QDropboxInfo::setAllocated(quint64 arg)
{
    if (m_allocated != arg) {
        m_allocated = arg;
        emit allocatedChanged(m_allocated);
    }
}

QString QDropboxInfo::giveName()
{
    return m_giveName;
}

void QDropboxInfo::setGiveName(QString arg)
{
    if (m_giveName != arg) {
        m_giveName = arg;
        emit giveNameChanged(m_giveName);
    }
}

QString QDropboxInfo::surname()
{
    return m_surname;
}

void QDropboxInfo::setSurname(QString arg)
{
    if (m_surname != arg) {
        m_surname = arg;
        emit surnameChanged(m_surname);
    }
}

QString QDropboxInfo::familiarName()
{
    return m_familiarName;
}

void QDropboxInfo::setFamiliarName(QString arg)
{
    if (m_familiarName != arg) {
        m_familiarName = arg;
        emit familiarNameChanged(m_familiarName);
    }
}

QString QDropboxInfo::displayName()
{
    return m_displayName;
}

void QDropboxInfo::setDisplayName(QString arg)
{
    if (m_displayName != arg) {
        m_displayName = arg;
        emit displayNameChanged(m_displayName);
    }
}

QString QDropboxInfo::email()
{
    return m_email;
}

void QDropboxInfo::setEmail(QString arg)
{
    if (m_email != arg) {
        m_email = arg;
        emit emailChanged(m_email);
    }
}

QNetworkAccessManager *QDropboxInfo::manager()
{
    if (m_manager == 0) {
        m_manager = new QNetworkAccessManager(this);
        connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(managerFinished(QNetworkReply*)));
    }

    return m_manager;
}

void QDropboxInfo::destroyManager()
{
    if (m_manager != 0) {
        m_manager->deleteLater();
        m_manager = 0;
    }
}

void QDropboxInfo::managerFinished(QNetworkReply *reply)
{
    if (reply->error()) {
        emit error("Dropbox info " + reply->errorString());
        destroyManager();
        return;
    }

    QByteArray data = reply->readAll();
    QJsonDocument document = QJsonDocument::fromJson(data);
    QJsonObject object = document.object();

    if (reply->url().toString().contains(USER_INFO_URL "get_current_account")) {
        QJsonObject name = object["name"].toObject();

        setGiveName(name["given_name"].toString());
        setSurname(name["surname"].toString());
        setFamiliarName(name["familiar_name"].toString());
        setDisplayName(name["display_name"].toString());
        setEmail(object["email"].toString());

        emit infoUpdated();
    }else if (reply->url().toString().contains(USER_INFO_URL "get_space_usage")) {

        QJsonObject allocation = object["allocation"].toObject();

        setUsed(object["used"].toDouble());
        setAllocated(allocation["allocated"].toDouble());

        emit spaceUpdated();
    }
}
