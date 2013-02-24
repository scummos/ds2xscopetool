#ifndef UTILITIES_H
#define UTILITIES_H

#include <QByteArray>
#include <QMap>
#include <qvector.h>

#include "tmcdevice.h"
#include "itpp/itsignal.h"

using namespace itpp;

class Utils {
public:
    static QMap<int, float> parseScopeChannelReply(QByteArray data, unsigned char yref = 0, float scale = 1, float offset = 0) {
        int index = 0;
        QMap<int, float> result;
        foreach ( const unsigned char byte, data ) {
            // The 10.0 is not mentioned in the documentation, but necessary:
            // The full 8 bits dynamic range is *per screen height*,
            // so per *ten divisions*. The scale returned by the scope
            // is per *one division*. (25.6 = 2^8 / 10.0)
            result[index] = (byte - yref)/25.6 * scale - offset;
            qDebug() << (int) byte << yref << scale << offset << (byte-yref)*scale/10 << result[index];
            index += 1;
        }
        return result;
    };
    static QMap<int, float> crossCorrelation(QMap<int, float> ch1, QMap<int, float> ch2) {
        Q_ASSERT(ch1.count() == ch2.count() && "channels must have the same amount of sample points");
        Q_ASSERT(ch1.count() % 2 == 0 && "number of points in channel must be even");
        if ( ch1.count() == 0 ) {
            return QMap<int, float>();
        }
        const int mid = ch1.count() / 2;
        const QList<float> ch1_values = ch1.values();
        const QList<float> ch2_values = ch2.values();
        const unsigned int sizeHint = ch1.count();
        cvec ch1_fft(sizeHint), ch2_fft(sizeHint);
        {
            vec ch1_vec(sizeHint), ch2_vec(sizeHint);
            for ( int i = 0; i < ch1_values.size(); i++ ) {
                ch1_vec[i] = ch1_values[i];
                ch2_vec[i] = ch2_values[i];
            }
            ch1_fft = fft_real(ch1_vec);
            ch2_fft = fft_real(ch2_vec);
        }

        // calculate cross-correlation
        elem_mult_inplace(conj(ch1_fft), ch2_fft);
        cvec& crossCorrelation = ch2_fft;

        // enhance resolution
        std::complex<double> midValue = crossCorrelation.get(mid);
        crossCorrelation[mid] = midValue / 2;
        crossCorrelation.ins(mid, midValue / 2);
        for ( int i = 0; i < ch1.size() * 4; i++ ) {
            crossCorrelation.ins(mid, std::complex<double>(0));
        }

        // transform back
        vec result = real(ifft(crossCorrelation));
        QMap<int, float> resultMap;
        for ( int i = 0; i < result.size(); i++ ) {
            resultMap[i] = result.get(i);
        }
        return resultMap;
    };
};

#endif