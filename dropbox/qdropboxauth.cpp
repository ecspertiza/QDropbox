#include "qdropboxauth.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QHttpPart>
#include <QHttpMultiPart>

#include <QDesktopServices>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include "qdropboxauthsocket.h"

QDropboxAuth::QDropboxAuth(QString appKey, QString appSecret, QObject *parent)
    :QObject(parent),m_appKey(appKey), m_appSecret(appSecret), m_accessToken(""),
      m_port(6609), m_manager(0), m_responseServer(0), m_redirectUrlAfterAuth("")
{
    connect(this, SIGNAL(accessTokenChanged(QString)), this, SIGNAL(authComplete()));
}

QDropboxAuth::~QDropboxAuth()
{
    if (m_manager != 0) {
        delete m_manager;
    }
}

QNetworkAccessManager *QDropboxAuth::manager()
{
    if (m_manager == 0) {
        m_manager = new QNetworkAccessManager(this);
        connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(managerFinished(QNetworkReply*)));
    }

    return m_manager;
}

void QDropboxAuth::destroyManager()
{
    if (m_manager != 0) {
        m_manager->deleteLater();
        m_manager = 0;
    }
}

QNetworkRequest &QDropboxAuth::authRequest(QNetworkRequest &request)
{
    request.setRawHeader("Authorization", "Bearer " + m_accessToken.toUtf8());
    return request;
}

void QDropboxAuth::auth()
{
    if (m_responseServer != 0)
        delete m_responseServer;

    m_responseServer = new QDropboxAuthSocket(m_redirectUrlAfterAuth);
    m_responseServer->listen(QHostAddress::LocalHost,m_port);

    connect(m_responseServer, SIGNAL(codeComplete(QString)), this, SLOT(codeComplete(QString)));


    QString url = "https://www.dropbox.com/oauth2/authorize?client_id="+m_appKey+"&redirect_uri=http://127.0.0.1:"+QString::number(m_port)+"&response_type=code";
    QDesktopServices::openUrl(url);
}

void QDropboxAuth::setRedirectUrlAfterAuth(QString redirectUrl)
{
    if (m_redirectUrlAfterAuth != redirectUrl) {
        m_redirectUrlAfterAuth = redirectUrl;
        emit redirectUrlAfterAuthChanged(m_redirectUrlAfterAuth);
    }
}

QString QDropboxAuth::redirectUrlAfterAuth()
{
    return m_redirectUrlAfterAuth;
}

void QDropboxAuth::setPort(int port)
{
    if (m_port != port) {
        m_port = port;
        emit portChanged(m_port);
    }
}

int QDropboxAuth::port()
{
    return m_port;
}

void QDropboxAuth::setAccessToken(QString arg)
{
    if (m_accessToken != arg) {
        m_accessToken = arg;
        emit accessTokenChanged(m_accessToken);
    }
}

QString QDropboxAuth::accessToken()
{
    return m_accessToken;
}

void QDropboxAuth::setAppKey(QString key)
{
    if (m_appKey != key) {
        m_appKey = key;
        emit appKeyChanged(m_appKey);
    }
}

QString QDropboxAuth::appKey()
{
    return m_appKey;
}

void QDropboxAuth::setAppSecret(QString secret)
{
    if (m_appSecret != secret) {
        m_appSecret = secret;
        emit appSecretChanged(m_appSecret);
    }
}

QString QDropboxAuth::appSecret()
{
    return m_appSecret;
}

void QDropboxAuth::codeComplete(QString code)
{
    QString authBase = QByteArray(m_appKey.toUtf8() + ":" + m_appSecret.toUtf8()).toBase64();

    QString body = "code=" + code + "&redirect_uri=http://127.0.0.1:"+QString::number(m_port)+"&grant_type=authorization_code";

    QNetworkRequest request(QUrl("https://api.dropboxapi.com/oauth2/token/"));
    request.setRawHeader("Authorization","Basic " + authBase.toUtf8());

    manager()->post(request, body.toUtf8());
}

void QDropboxAuth::managerFinished(QNetworkReply *reply)
{
    if (reply->error()) {
        emit error("Auth error: " + reply->readAll());
        destroyManager();
        return;
    }

    QByteArray data = reply->readAll();
    QJsonDocument document = QJsonDocument::fromJson(data);
    QJsonObject object = document.object();

    QString access_toket = object["access_token"].toString();
    setAccessToken(access_toket);

    if (m_responseServer != 0) {
        m_responseServer->close();
        delete m_responseServer;
        m_responseServer = 0;
    }

    destroyManager();
}
