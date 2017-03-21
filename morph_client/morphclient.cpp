#include "morphclient.h"

#include "rutextparser_extensions.h"

#include <QHostAddress>
#include <QStringList>

MorphClient::MorphClient()
{
}

void MorphClient::connectToHost(const QString &serverName, const int &port)
{
    _socket.abort();
    _socket.connectToHost(serverName, port);

    if (!_socket.waitForConnected(3000)) {
        qDebug() << "Error: " << _socket.errorString();
    }
}

QList<MorphResultType> MorphClient::analyze(const QStringList &sentence)
{
    if (_socket.state() != QAbstractSocket::ConnectedState) {
        printWarning("analyze:: QTcpSocket not in ConnectedState");
        return QList<MorphResultType>();
    }
    QList<MorphResultType> result;
    for (int i=0; i < sentence.size(); ++i) {
        MorphResultType word = analyzeWord(sentence.at(i));
        qDebug() << "analyzed word" << word;
        result.append(word);
    }
    qDebug() << sentence << "\n\tANALYZED";
    return result;
}

MorphResultType MorphClient::analyzeWord(const QString &word)
{
    if (_socket.state() != QAbstractSocket::ConnectedState) {
        printWarning("analyzeWord:: QTcpSocket not in ConnectedState");
        return MorphResultType();
    }
    // write word
//    qDebug() << "analyzeWord" << word;
    QByteArray wordArray=word.toUtf8();
    const char *wordData = wordArray.constData();
//    int i = 0;
//    while(wordData[i] != '\0') {
////        qDebug() << QString::number(wordData[i], 16);
//        ++i;
//    }
//    if (i > 5) {
//        char *p1 = new char[4];
//        char *p2 = new char[i-2];

//        memcpy(p1, wordData, 3);
//        p1[3] = 0;

//        memcpy(p2, wordData+3, i-3);
//        p2[i-3] = 0;

//        qDebug() << "write sep p1";
//        int i = 0;
//        while(p1[i] != '\0') {
//            qDebug() << QString::number(p1[i], 16);
//            ++i;
//        }
//        int writtenBytes = _socket.write(p1);
//        if (writtenBytes == -1) {
//            qDebug() << "Error:" << _socket.error();
//            return MorphResultType();
//        }
//        qDebug() << writtenBytes <<"written";
//        qDebug() << "write sep p2";
//        i = 0;
//        while(p2[i] != '\0') {
//            qDebug() << QString::number(p2[i], 16);
//            ++i;
//        }
//        qDebug() << writtenBytes <<"written";
//        writtenBytes = _socket.write(p2);
//        if (writtenBytes == -1) {
//            qDebug() << "Error:" << _socket.error();
//            return MorphResultType();
//        }
//        qDebug() << writtenBytes <<"written";

//        delete []p1;
//        delete []p2;
//    }
//    else {
//        qDebug() << word << "UTF SIZE:" << i;
        int writtenBytes = _socket.write(wordData);
        if (writtenBytes == -1) {
            printWarning("Error:", _socket.errorString());
            return MorphResultType();
        }
//        qDebug() << writtenBytes <<"written";
//    }
    // write separator
    writtenBytes = _socket.write(QString("\t").toUtf8().data());
    if (writtenBytes == -1) {
        printWarning("Error:", _socket.errorString());
        return MorphResultType();
    }

//    qDebug() << writtenBytes <<"written";
    _socket.flush();
//    qDebug() << "socket flush";

    _blockSize = 0;
    return getAnswer();
}

MorphResultType MorphClient::getAnswer()
{
    if (!_socket.waitForReadyRead(2000)) {
        printWarning("MorphClient::getAnswer didn't wait for an answer");
        displayError(_socket.error());
        return MorphResultType();
    }
    MorphResultType result;
    QDataStream in(&_socket);
    in.setVersion(QDataStream::Qt_4_8);

//    qDebug() << "on start bytes available" << _socket.bytesAvailable();
    while (_socket.bytesAvailable() < sizeof(int))
        if (!_socket.waitForReadyRead(4000)) {
            printWarning("MorphClient::getAnswer failed to get msg size");
            displayError(_socket.error());
            return MorphResultType();
        }
    Q_ASSERT(_blockSize == 0);
//    qDebug() << "_blockSize =0? " << _blockSize;
    // gets msg size
    in >> _blockSize;
//    qDebug() << "_blockSize" << _blockSize;

    while (_socket.bytesAvailable() < _blockSize) {
//        printWarning("not full msg yet");
        if (!_socket.waitForReadyRead(40000)) {
            printWarning("MorphClient::getAnswer failed to get morphs");
            qDebug() << "\tbytesAvailable" << _socket.bytesAvailable()
                     << "\n\t_blockSize" << _blockSize;
            return MorphResultType();
        }
    }
//    qDebug() << "got whole message";

    QByteArray barray = _socket.readAll();
    int len = barray.size();
    const char *data = barray.constData();

    int wordsRead = 0;
    for (int i=0, s=0; i < len; ++i) {
        if (data[i] == '\0') {
            if(i == 0) {
                Q_ASSERT(false);
            }

            if (wordsRead % 2) {
                // reading score
                QString score = QString::fromUtf8(data + s);
                bool ok;
                qreal fl_score = score.toDouble(&ok);
                if (!ok) {
                    printWarning("MorphClient::getAnswer conversion error[%1]", score);
                    return MorphResultType();
                }
                result.last().second = fl_score;
//                qDebug() << "read score" << fl_score;
            }
            else {
                // reading morph vector
                QString morphVector = QString::fromUtf8(data + s);
//                qDebug() << "read morph" << morphVector;

                result.append(qMakePair(morphVector, (qreal)0));
            }
            s = i + 1;
            ++wordsRead;
        }
//        qDebug() << QString::number(data[i], 16);
    }

    return result;
}

void MorphClient::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        printWarning(tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        printWarning(tr("The connection was refused by the peer. "
                                    "Make sure the server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        printWarning(tr("The following error occurred: %1.")
                                 .arg(_socket.errorString()));
    }
}

void MorphClient::readAnalyzed()
{

}
