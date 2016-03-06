#ifndef SEARCHBOX_H
#define SEARCHBOX_H

#include <QLineEdit>
#include "gen/ssArray.h"

class GSuggestCompletion;

class SearchBox: public QLineEdit
{
    Q_OBJECT

public:
    SearchBox(QWidget*parent = 0);
    void setHost( CSsArray & hostArray );

signals:
    void searchFinish();

protected slots:
    void doSearch();

private:
  GSuggestCompletion *completer;
};

#endif // SEARCHBOX_H
