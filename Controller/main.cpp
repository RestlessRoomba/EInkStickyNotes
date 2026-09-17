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

int clearScreen()
{
    // Ich existiere nur, um den Compiler zufriden zu stellen
    return 0;
}