#include "qdropbox.h"

#include "qdropboxauth.h"
#include "qdropboxinfo.h"

QDropbox::QDropbox(QObject *parent)
    :QObject(parent), m_dropboxAuth(0), m_dropboxInfo(0), m_appKey(""), m_appSecret(""), m_authToken("")
{
    connect(this, SIGNAL(authComplete()), this, SLOT(updateInfo()));
}

QDropbox::QDropbox(QString appKey, QString appSecret, QObject *parent)
    :QObject(parent), m_appKey(appKey), m_appSecret(appSecret), m_authToken(""), m_dropboxAuth(0), m_dropboxInfo(0)
{
    connect(this, SIGNAL(authComplete()), this, SLOT(updateInfo()));
}

QDropbox::~QDropbox()
{
    if (m_dropboxAuth != 0)
        delete m_dropboxAuth;

    if (m_dropboxInfo != 0)
        delete m_dropboxInfo;
}

void QDropbox::auth()
{
    if (m_appKey.isEmpty()) {
        emit error("App key can`t be empty");
        return;
    }

    if (m_appSecret.isEmpty()) {
        emit error("App secret can`t be empty");
        return;
    }

    if (m_dropboxAuth != 0)
        delete m_dropboxAuth;

    m_dropboxAuth = new QDropboxAuth(m_appKey, m_appSecret, this);
    connect(m_dropboxAuth, SIGNAL(authComplete()), this, SIGNAL(authComplete()));
    m_dropboxAuth->auth();
    emit dropboxAuthChanged(m_dropboxAuth);
}

void QDropbox::setDropboxAuth(QDropboxAuth *arg)
{
    if(m_dropboxAuth != arg) {
        m_dropboxAuth = arg;
        emit dropboxAuthChanged(m_dropboxAuth);
    }
}

QDropboxAuth *QDropbox::dropboxAuth()
{
    return m_dropboxAuth;
}

void QDropbox::setAppKey(QString key)
{
    if (m_appKey != key) {
        m_appKey = key;
        emit appKeyChanged(m_appKey);
    }
}

QString QDropbox::appKey()
{
    return m_appKey;
}

void QDropbox::setAppSecret(QString secret)
{
    if (m_appSecret != secret) {
        m_appSecret = secret;
        emit appSecretChanged(m_appSecret);
    }
}

QString QDropbox::appSecret()
{
    return m_appSecret;
}

QDropboxInfo *QDropbox::info()
{
    return m_dropboxInfo;
}

void QDropbox::updateInfo()
{
    if (m_dropboxAuth == 0) {
        emit error("Can`t update info user, auth is empty");
        return;
    }

    if (m_dropboxInfo == 0) {
        m_dropboxInfo = new QDropboxInfo(m_dropboxAuth, this);
        connect(m_dropboxInfo, SIGNAL(infoUpdated()), this, SIGNAL(infoUpdated()));
        connect(m_dropboxInfo, SIGNAL(spaceUpdated()), this, SIGNAL(infoUpdated()));
        connect(m_dropboxInfo, SIGNAL(spaceUpdated()), this, SIGNAL(loadAllInfo()));
    }

    m_dropboxInfo->updateInfo();
}
