#include <QDesktopServices>
#include <QUrl>

#include "searchbox.h"
#include "googlesuggest.h"

#define GSEARCH_URL "http://www.google.com/search?q=%1"

SearchBox::SearchBox(QWidget*parent): QLineEdit(parent)
{
    completer = new GSuggestCompletion(this);

    connect(this, SIGNAL(returnPressed()),this, SLOT(doSearch()));

    setWindowTitle("Search with Google");

    adjustSize();
    resize(400, height());
    setFocus();
}

void SearchBox::setHost( CSsArray& hostArray )
{
    completer->setHost( hostArray );
}

void SearchBox::doSearch()
{
    //completer->preventSuggest();
    //QString url = QString(GSEARCH_URL).arg(text());
    //QDesktopServices::openUrl(QUrl(url));
    fprintf( stderr, "%s", text().toStdString().c_str() );
    hide();
    emit searchFinish();
}
