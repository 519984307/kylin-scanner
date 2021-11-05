#include "usbhotplugthread.h"
#include "usb.h"
#include <ukui-log4qt.h>

#if 0
UsbHotplugThread::UsbHotplugThread()
{

    connect(g_user_signal, &GlobalUserSignal::exitApplicationSignal, this, &UsbHotplugThread::deleteLater);
    connect(g_user_signal, &GlobalUserSignal::exitApplicationSignal, this, &UsbHotplugThread::exitWindow);

}

void UsbHotplugThread::run()
{
        KyInfo() << "000000";
        hotplug_sock = init_hotplug_sock();
        KyInfo() << "000000";
        do {
            /* Netlink message buffer */
            char buf[UEVENT_BUFFER_SIZE * 2] = {0};
            recv(hotplug_sock, &buf, sizeof(buf), 0);


            QString recvData = QString(QLatin1String(buf));

            KyInfo() << "aaaaaaaa";

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
        } while (! exitWindowFlag);
        quit();

}

void UsbHotplugThread::exitWindow()
{
    KyInfo() << "qqqqqqq";
    exitWindowFlag = true;
    close(hotplug_sock);
    KyInfo() << "qqqqqqq";
}

#endif
