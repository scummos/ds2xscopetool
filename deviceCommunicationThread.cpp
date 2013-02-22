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
        requestsLock.lock();
        CommunicationRequest* request = requests.dequeue();
        requestsLock.unlock();

        CommunicationReply* reply = request->execute(device);
        qDebug() << "got reply: " << reply << reply->status << reply->reply;
        QMetaObject::invokeMethod(request->notifyReady, request->notifyMethod.toAscii(),
                                  Qt::DirectConnection, Q_ARG(CommunicationReply*, reply));
        delete request;

        requestsLock.lock();
        if ( requests.isEmpty() ) {
            break;
        }
        requestsLock.unlock();
    }
}

void DeviceCommunicationWorker::enqueue(CommunicationRequest* request)
{
    requestsLock.lock();
    requests.enqueue(request);
    requestsLock.unlock();
    doWork();
}