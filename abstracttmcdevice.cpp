#include "abstracttmcdevice.h"
#include <QDebug>

int AbstractTMCDevice::parseReplyLength(QByteArray& replyHeader) const
{
    if ( ! replyHeader.startsWith('#') || replyHeader.length() < 2 ) {
        return -1;
    }
    bool ok = false;
    unsigned char digits = QString(replyHeader.at(1)).toInt(&ok);
    if ( ! ok || digits == 0 || replyHeader.length() < 2 + digits ) {
        return -1;
    }
    unsigned int replyLength = replyHeader.mid(2, digits).toInt(&ok);
    if ( ! ok ) {
        return -1;
    }
    // remove "#", "<digit count>", and the digits from the data
    replyHeader.remove(0, 2+digits);
    return replyLength;
}

QByteArray AbstractTMCDevice::ask(const QByteArray& command, unsigned int maxReplyLength)
{
    write(command);
    return read(maxReplyLength);
}

QByteArray AbstractTMCDevice::readReplyData()
{
    QByteArray replyData = read();
    int replyLength = parseReplyLength(replyData);
    if ( replyLength == -1 || replyLength == replyData.length() - 1 ) {
        return replyData;
    }
    // read all available data, minus what has already been read
    // in the first chunk
    replyData += read(replyLength - replyData.length() + 1);
    return replyData;
}
