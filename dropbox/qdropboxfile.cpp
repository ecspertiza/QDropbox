#include "qdropboxfile.h"

#include "qdropboxauth.h"
#include "qdropbox.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include <QFile>

#define FILE_UPLOAD_URL DROP_BOX_CONTENT_SERVER "files/"
#define FILE_SHARE_URL DROP_BOX_API_SERVER "sharing/"

QDropboxFile::QDropboxFile(QObject *parent)
    : QObject(parent), m_auth(0), m_manager(0), m_serverFileName(""), m_shortUrl(false)
{}

QDropboxFile::QDropboxFile(QDropboxAuth *auth, QObject *parent)
    : QObject(parent), m_auth(auth), m_manager(0), m_serverFileName(""), m_shortUrl(false)
{}

QDropboxFile::QDropboxFile(QString serverFileName, QDropboxAuth *auth, QObject *parent)
    :QObject(parent), m_auth(auth), m_manager(0), m_serverFileName(serverFileName)
{}

void QDropboxFile::upload(QString localFileName, bool link)
{
    if (m_auth == 0) {
        emit error("DropboxFile auth is empty");
        return;
    }

    m_createShareLink = link;

    QUrl localFileUrl(localFileName);

    QString jsonData = "{\"path\": \"/"+localFileUrl.fileName()+"\",\"mode\": \"add\",\"autorename\": true,\"mute\": true}";

    QNetworkRequest request(QUrl(FILE_UPLOAD_URL "upload"));
    request = m_auth->authRequest(request);
    request.setRawHeader("Content-Type", "application/octet-stream");
    request.setRawHeader("Dropbox-API-Arg", jsonData.toUtf8());

    QFile *uploadFile = new QFile(localFileName);
    uploadFile->open(QIODevice::ReadOnly);

    QNetworkReply *reply = manager()->post(request, uploadFile);
    connect(reply, SIGNAL(uploadProgress(qint64,qint64)), this, SIGNAL(uploadProgress(qint64,qint64)));
    uploadFile->setParent(manager());
}

void QDropboxFile::shareUrl()
{
    if (m_auth == 0) {
        emit error("DropboxFile auth is empty");
        return;
    }

    if (m_serverFileName.isEmpty()) {
        emit error("DropboxFile serverFileName is empty");
        return;
    }

    QNetworkRequest request(QUrl(FILE_SHARE_URL "create_shared_link"));
    request = m_auth->authRequest(request);
    request.setRawHeader("Content-Type", "application/json");

    QString jsonData = "{\"path\": \""+m_serverFileName+"\",\"short_url\": "+(m_shortUrl ? "true" : "false")+"}";

    manager()->post(request, jsonData.toUtf8());
}

void QDropboxFile::setShortUrl(bool arg)
{
    if (m_shortUrl != arg) {
        m_shortUrl = arg;
        emit shortUrlChanged(m_shortUrl);
    }
}

bool QDropboxFile::shortUrl()
{
    return m_shortUrl;
}

QDropboxAuth *QDropboxFile::dropboxAuth()
{
    return m_auth;
}

void QDropboxFile::setDropboxAuth(QDropboxAuth *auth)
{
    if (m_auth != auth) {
        m_auth = auth;
        emit dropboxAuthChanged(m_auth);
    }
}

QNetworkAccessManager *QDropboxFile::manager()
{
    if (m_manager == 0) {
        m_manager = new QNetworkAccessManager(this);
        connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(managerFinished(QNetworkReply*)));
    }

    return m_manager;
}

void QDropboxFile::destroyManager()
{
    if (m_manager != 0) {
        delete m_manager;
        m_manager = 0;
    }
}

void QDropboxFile::managerFinished(QNetworkReply *reply)
{
    if (reply->error()) {
        emit error("DropboxFile: " + reply->errorString());
        destroyManager();
        return;
    }

    if (reply->url().toString().contains(FILE_UPLOAD_URL "upload")) {
        uploadFinished(reply);

        if (m_createShareLink) {
            shareUrl();
        }else {
            destroyManager();
        }
    }else if (reply->url().toString().contains(FILE_SHARE_URL "create_shared_link")) {
        QByteArray data = reply->readAll();
        QJsonDocument document = QJsonDocument::fromJson(data);
        QJsonObject object = document.object();

        shareLinkComplete(object["url"].toString());
    }
}

void QDropboxFile::uploadFinished(QNetworkReply *reply)
{
    QByteArray data = reply->readAll();
    QJsonDocument document = QJsonDocument::fromJson(data);
    QJsonObject object = document.object();

    m_serverFileName = object["path_lower"].toString();
}
