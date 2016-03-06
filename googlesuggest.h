#ifndef GOOGLESUGGEST_H
#define GOOGLESUGGEST_H

#include <QtWidgets>
//#include <QtNetwork>
#include <QObject>
#include "gen/ssArray.h"

class QLineEdit;
//class QNetworkReply;
class QTimer;
class QTreeWidget;

class GSuggestCompletion : public QObject
{
    Q_OBJECT

public:
    GSuggestCompletion(QLineEdit *parent = 0);
    ~GSuggestCompletion();
    bool eventFilter(QObject *obj, QEvent *ev) Q_DECL_OVERRIDE;
    void showCompletion(const QStringList &choices);
    void setHost( CSsArray& hostArray ){ m_hostArray = hostArray; }

public slots:

    void doneCompletion();
    void preventSuggest();
    void autoSuggest();
 //   void handleNetworkData(QNetworkReply *networkReply);

private:
    QLineEdit *editor;
    QTreeWidget *popup;
    QTimer *timer;
    CSsArray m_hostArray;

 //   QNetworkAccessManager networkManager;
};

#endif // GOOGLESUGGEST_H
