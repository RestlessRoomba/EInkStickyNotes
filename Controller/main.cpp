#include "communication/CommunicationManager.h"
#include "main.h"
#include "ui/mainwindow.h"
#include "communication/SerialPort.h"

#include <QApplication>
#include <QDebug>
#include <QThread>
#include <QElapsedTimer>
#include <algorithm>
#include <vector>


int main(int argc, char *argv[]) 
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

int sendBitmap() 
{
    // --- Send a Bitmap via CommunicationManager ---
    // Open a Serial Port
    SerialPort serialPort;

    if (!serialPort.openSerial())
    {
        qDebug() << "Error while opening serial port.";

        return 1;
    }

    // Create a Communication Manager on that Serial Port
    CommunicationManager communication(serialPort);

    if (!communication.sendBitmap(screen))
    {
        qDebug() << "Error while sending bitmap.";
        return 1;
    }

    return 0;
}

int clearScreen()
{
    // Ich existiere nur, um den Compiler zufriden zu stellen
    return 0;
}
