#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H

#include <QObject>
#include <QtSerialPort>
#include <QBitmap>
#include "protocolparser.h"

class SerialHandler : public QThread
{
	Q_OBJECT
public:
	explicit SerialHandler();
	void run();

signals:
	void sigSetPixel(int x, int y, uint index_or_rgb);
	void sigFillArea(int x, int y, int cx, int cy, uint index_or_rgb);
public slots:
	void receiveData();
	void buttonPressed();

private:
	QSerialPort* port;
	QBitmap* bitmap;
	ProtocolParser parser;
};

#endif // SERIALHANDLER_H
