#include "deviceCommunicationThread.h"
#include <QDebug>
#include "unistd.h"

DeviceCommunicationWorker::DeviceCommunicationWorker(QObject* parent): device(0)
{

}

void DeviceCommunicationWorker::connectToDevice()
{
    // TODO fixme
    qDebug() << "starting up";
    device = new TMCDevice("/dev/usbtmc1");
}

void DeviceCommunicationWorker::doWork()
{
    while ( ! device ) {
        qDebug() << "device is not set, waiting for startup...";
        sleep(1);
    }
    while ( true ) {
        QMutexLocker lock(&requestsLock);
        if ( requests.isEmpty() ) {
            return;
        }
        CommunicationRequest* request = requests.dequeue();
        lock.unlock();

        CommunicationReply* reply = request->execute(device);
        QMetaObject::invokeMethod(request->notifyReady, request->notifyMethod.toAscii(),
                                  Qt::QueuedConnection, Q_ARG(CommunicationReply*, reply));
        delete request;
    }
}

void DeviceCommunicationWorker::enqueue(CommunicationRequest* request)
{
    requestsLock.lock();
    requests.enqueue(request);
    requestsLock.unlock();
    QMetaObject::invokeMethod(this, "doWork");
}