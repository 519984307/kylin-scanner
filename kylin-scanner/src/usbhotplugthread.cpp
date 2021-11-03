#include "usbhotplugthread.h"
#include "usb.h"
#include <ukui-log4qt.h>

UsbHotplugThread::UsbHotplugThread()
{

}

void UsbHotplugThread::run()
{
    int hotplug_sock = init_hotplug_sock();
    while (1) {
        /* Netlink message buffer */
        char buf[UEVENT_BUFFER_SIZE * 2] = {0};
        recv(hotplug_sock, &buf, sizeof(buf), 0);


        QString recvData = QString(QLatin1String(buf));


#if 0
        if (! recvData.isEmpty()) {
            KyInfo() << "recvData: " << recvData;
        }
#endif

        if (recvData.contains("add@", Qt::CaseInsensitive)
                && recvData.contains("devices", Qt::CaseInsensitive)
                && recvData.contains("usb", Qt::CaseInsensitive)
                && recvData.endsWith(":1.0", Qt::CaseInsensitive)) {
            // ":1.0" means each usb device first directory, which is must
            emit usbAdd(recvData);
        } else if (recvData.contains("remove@", Qt::CaseInsensitive)
                   && recvData.contains("devices", Qt::CaseInsensitive)
                   && recvData.contains("usb", Qt::CaseInsensitive)
                   && recvData.endsWith(":1.0", Qt::CaseInsensitive)) {
            emit usbRemove(recvData);
        }
    }
}
