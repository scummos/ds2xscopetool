#ifndef DEVICE_COMMUNICATION_THREAD_H
#define DEVICE_COMMUNICATION_THREAD_H

#include <QThread>
#include <QQueue>
#include <QMutex>
#include <QSharedPointer>
#include "tmcdevice.h"

struct CommunicationReply {
    enum Status {
        Success,
        Fail
    };
    Status status;
    QByteArray reply;
};

struct CommunicationRequest {
    // invoke notifyMethod on notifyReady when done.
    // ownership of the reply data must be taken by the notified method!
    QObject* notifyReady;
    QString notifyMethod;
    // only for identification from outside; usually zero
    unsigned long requestIdentifier;
    CommunicationRequest(QObject* notifyReady, QString notifyMethod, unsigned long requestIdentifier = 0)
     : notifyReady(notifyReady), notifyMethod(notifyMethod), requestIdentifier(requestIdentifier) { };

    virtual CommunicationReply* execute(TMCDevice* device) = 0;
};

struct AskCommandCommunicationRequest : public CommunicationRequest {
    AskCommandCommunicationRequest(QObject* notifyReady, QString notifyMethod, long unsigned int requestIdentifier = 0)
     : CommunicationRequest(notifyReady, notifyMethod, requestIdentifier) { };
    QString commandString;
    virtual CommunicationReply* execute(TMCDevice* device) {
        CommunicationReply* reply = new CommunicationReply;
        reply->reply = device->ask(commandString.toAscii());
        reply->status = CommunicationReply::Success;
        return reply;
    };
};

struct WriteCommandCommunicationRequest : public CommunicationRequest {
    WriteCommandCommunicationRequest(QObject* notifyReady, QString notifyMethod, long unsigned int requestIdentifier = 0)
     : CommunicationRequest(notifyReady, notifyMethod, requestIdentifier) { };
    QString commandString;
    virtual CommunicationReply* execute(TMCDevice* device) {
        CommunicationReply* reply = new CommunicationReply;
        device->write(commandString.toAscii());
        reply->reply = QByteArray();
        reply->status = CommunicationReply::Success;
        return reply;
    };
};

class DeviceCommunicationWorker : public QObject {
    Q_OBJECT
public:
    DeviceCommunicationWorker(QObject* parent = 0);
public slots:
    void connectToDevice();
    // The request is deleted when it has been handled.
    void enqueue(CommunicationRequest* request);

private:
    void doWork();
    CommunicationReply* processRequest(CommunicationRequest* request);

    TMCDevice* device;
    QQueue<CommunicationRequest*> requests;
    QMutex requestsLock;
};

#endif