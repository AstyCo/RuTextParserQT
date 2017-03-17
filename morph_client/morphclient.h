#ifndef MORPHCLIENT_H
#define MORPHCLIENT_H

#include "morph_client_global.h"

#include <QTcpSocket>

class MORPH_CLIENTSHARED_EXPORT MorphClient
{
    QTcpSocket _socket;
public:
    MorphClient();

    void connectToHost(const QString &serverName, const int &port);

    QList<QStringList> analyze(const QStringList &sentence);
    QStringList analyzeWord(const QString &word);
};

#endif // MORPHCLIENT_H
