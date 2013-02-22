#ifndef ABSTRACTTMCDEVICE_H
#define ABSTRACTTMCDEVICE_H

#include <QtCore/QByteArray>
#include <QtCore/QString>

class AbstractTMCDevice {
public:
    /**
     * @brief Write raw data to the device.
     *
     * @param command Byte array with bytes to be written to the device. Don't forget the \n.
     * @return void
     */
    virtual void write(const QByteArray& command) = 0;

    /**
     * @brief Read at most @param bytes bytes of data from the device, and return the result.
     *
     * @param bytes Maximum amount of bytes to read
     * @return QByteArray
     */
    virtual QByteArray read(int bytes = -1) = 0;

    /**
     * @brief Read a reply from the device, taking a TMC length header, like "#90000321" into account.
     *
     * @return QByteArray The data received from the device. This has length "321", in the above case.
     */
    virtual QByteArray readReplyData();

    /**
     * @brief Send @param command to the device, and read a reply of at most length @param maxReplyLength.
     *
     * @param command The command to send to the device
     * @param maxReplyLength Maximum about of bytes to receive in return
     * @return QByteArray
     */
    virtual QByteArray ask(const QByteArray& command, unsigned int maxReplyLength = -1);

    /**
     * @brief Parse the given reply from the device and find the length from its header.
     *
     * @param replyHeader At least the first few bytes of the data received from the device.
     * The reply length information will be removed from this byte array when the function call succeeds.
     * @return unsigned int reply length on success, -1 if parse error. In case of a parse error,
     * the replyHeader argument will not be changed.
     */
    int parseReplyLength(QByteArray& replyHeader) const;

};

#endif