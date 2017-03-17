#include "morphclient.h"

#include <QHostAddress>

MorphClient::MorphClient()
{
    connectToHost(QHostAddress(QHostAddress::LocalHost).toString(), 14147);
}

void MorphClient::connectToHost(const QString &serverName, const int &port)
{
    _socket.abort();
    _socket.connectToHost(serverName, port);

    if (!_socket.waitForConnected(3000)) {
        qDebug() << "Error: " << _socket.errorString();
    }
}

QList<QStringList> MorphClient::analyze(const QStringList &sentence)
{
    if (_socket.state() != QAbstractSocket::ConnectedState) {
        qDebug() << "analyze:: QTcpSocket not in ConnectedState";
        return QList<QStringList>();
    }
    QList<QStringList> result;
    for (int i=0; i < sentence.size(); ++i) {
        QStringList word = analyzeWord(sentence.at(i));
        qDebug() << "analyzed word" << word;
        result.append(word);
    }
    qDebug() << sentence << "\n\tANALYZED";
    return result;
}

QStringList MorphClient::analyzeWord(const QString &word)
{
    if (_socket.state() != QAbstractSocket::ConnectedState) {
        qDebug() << "analyzeWord:: QTcpSocket not in ConnectedState";
        return QStringList();
    }
    qDebug() << "analyzeWord" << word;
    int writtenBytes = _socket.write(word.toUtf8().data());
    if (writtenBytes == -1) {
        qDebug() << "Error:" << _socket.error();
        return QStringList();
    }
    qDebug() << writtenBytes <<"written";
    _socket.flush();
    qDebug() << "socket flush";


    return QStringList();
}
