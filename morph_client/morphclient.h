#ifndef MORPHCLIENT_H
#define MORPHCLIENT_H

#include "morph_client_global.h"

#include <QTcpSocket>

typedef QList<QPair<QString, qreal> > MorphResultType;

class MORPH_CLIENTSHARED_EXPORT MorphClient : public QObject
{
public:
    MorphClient();

    void connectToHost(const QString &serverName, const int &port);

    QList<MorphResultType> analyze(const QStringList &sentence);
    MorphResultType analyzeWord(const QString &word);

    QAbstractSocket::SocketState state() const { return _socket.state();}

private:
    MorphResultType getAnswer();
    void displayError(QAbstractSocket::SocketError socketError);
    void readAnalyzed();

private:
    QTcpSocket _socket;
    QList<QStringList> _analyzedWords;
    int _blockSize;
};

#endif // MORPHCLIENT_H
