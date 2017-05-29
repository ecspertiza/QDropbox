# QDropbox

## How to use

clone from git 

	git clone https://github.com/ecspertiza/QDropbox.git

copy dropbox path in your project

add in your .pro file 

	include(./dropbox/qdropbox.pri)

in your code use

	QDropbox *dropbox = new QDropbox("app_key", "app_secret");
	dropbox->auth();

now, your is auth, if your need upload file, please use

	QDropboxFile *file = new QDropboxFile(dropbox->dropboxAuth());
	file->upload(fileToUpload, true); // if is true, get share link, if false, no

please see main.cpp, this exaple shows how are your can use this lib for get user info, and get share link to upload file.