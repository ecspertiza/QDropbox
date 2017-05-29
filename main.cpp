#include <QGuiApplication>

#include "dropbox/qdropbox.h"
#include "dropbox/qdropboxauth.h"
#include "dropbox/qdropboxfile.h"
#include "dropbox/qdropboxinfo.h"

#include <QDebug>
#include <QFile>

QDropbox *dropbox;
QString fileToUpload;

void shareLinkComplete(QString url)
{
    qDebug() << Q_FUNC_INFO << "file upload " << url;
}

void authComplete() {
    qDebug() << Q_FUNC_INFO << "Dropbox auth complete";

    QDropboxFile *file = new QDropboxFile(dropbox->dropboxAuth());
    QObject::connect(file, &QDropboxFile::shareLinkComplete, shareLinkComplete);
    file->setShortUrl(true);
    file->upload(fileToUpload, true);
}

void loadAllInfo() {
    qDebug() << Q_FUNC_INFO << "Dropbox update info complete";

    if (dropbox != 0) {
        qDebug() << "give name: " << dropbox->info()->giveName() << "\n"
                 << "surname: " << dropbox->info()->surname() << "\n"
                 << "familiar name: " << dropbox->info()->familiarName() << "\n"
                 << "display name: " << dropbox->info()->displayName() << "\n"
                 << "email: " << dropbox->info()->email() << "\n"
                 << "allocated: " << dropbox->info()->allocated() << "\n"
                 << "used: " << dropbox->info()->used() << "\n";
    }
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    if (app.arguments().count() < 2) {
        qDebug() << "User app.exe <filename> - to upload file in dropbox";
        return 0;
    }else {
        fileToUpload = app.arguments().at(1);

        QFile file(fileToUpload);
        if (!file.exists()) {
             qDebug() << "file " << fileToUpload << " not exist";
             qDebug() << "User app.exe <filename> - to upload file in dropbox";
             return 0;
        }
    }

    dropbox = new QDropbox("app_key", "app_secret");
    QObject::connect(dropbox, &QDropbox::authComplete, authComplete);
    QObject::connect(dropbox, &QDropbox::loadAllInfo, loadAllInfo);
    dropbox->auth();

    return app.exec();
}
