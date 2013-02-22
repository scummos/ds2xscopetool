#ifndef UTILITIES_H
#define UTILITIES_H

#include <QByteArray>
#include <QMap>

#include "tmcdevice.h"

class Utils {
public:
    static QMap<int, float> parseScopeChannelReply(QByteArray data, float yref = 0, float scale = 1, float offset = 0) {
        int index = 0;
        QMap<int, float> result;
        foreach ( const char byte, data ) {
            result[index] = (byte - yref) * scale - offset;
            index += 1;
        }
        return result;
    }
};

#endif