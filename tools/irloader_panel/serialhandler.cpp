#include "serialhandler.h"
#include <iostream>
#include "config.h"

SerialHandler::SerialHandler()
{
	port = new QSerialPort(QString("/dev/ttyACM0"));
	port->setBaudRate(230400);
	port->setDataBits(QSerialPort::Data8);
	port->setFlowControl(QSerialPort::NoFlowControl);
	port->setParity(QSerialPort::NoParity);
	port->setStopBits(QSerialPort::OneStop);

	if (port->open(QIODevice::ReadWrite)) {
		qDebug() << "port open";
	} else {
		qDebug() << "port failed";
	}
	port->clear();

	connect(port, SIGNAL(readyRead()), this, SLOT(receiveData()));
}

void SerialHandler::receiveData()
{
	QByteArray array = port->readAll();
	ProtocolParser::E_PARSE_RES parseRes;
	QByteArray consoleData;
	int arraySize;
	unsigned char* data;

	arraySize = array.size();
	if (arraySize <= 0) {
		return;
	}

	data = (unsigned char*)array.data();

	//qDebug() << "rcv: " << QString(array.toHex());

	do {
		parseRes = parser.parse(data, &arraySize);
		if (parseRes == ProtocolParser::PARSE_ERROR) {
			parser.reset();
			qDebug() << "Parser error";
		} else if (parseRes == ProtocolParser::PARSE_COMPLETE) {
			ProtocolParser::E_CMD_ID cmd;
			unsigned short length;

			//qDebug() << "Parse complete";

			cmd = parser.getCmdId();
			length = parser.getLength();
			switch (cmd) {
			case ProtocolParser::CMD_ID_SET_PIXEL:
				if (length != 5) {
					qDebug() << "Invalid SET_PIXEL length " << length;
				} else {
					unsigned char *data = parser.getPayload();
					unsigned short x, y;
					unsigned char c;
					x = (data[0] + (data[1] << 8));
					y = (data[2] + (data[3] << 8));
					c = data[4];
					//qDebug() << "x : " << x << "y: " << y << "c: " << c;
					emit(sigSetPixel(x, y, c));
				}
				break;

			case ProtocolParser::CMD_ID_FILL_AREA:
				if (length != 9) {
					qDebug() << "Invalid FILL_AREA length " << length;
				} else {
					unsigned char *data = parser.getPayload();
					unsigned short x, y, cx, cy;
					unsigned char c;
					x = (data[0] + (data[1] << 8));
					y = (data[2] + (data[3] << 8));
					cx = (data[4] + (data[5] << 8));
					cy = (data[6] + (data[7] << 8));
					c = data[8];
					emit(sigFillArea(x, y, cx, cy, c));
				}
				break;

			case ProtocolParser::CMD_ID_CONSOLE:
				consoleData.append((char*)parser.getPayload(), length);
				std::cout << consoleData.toStdString();
				consoleData.clear();
				break;

			case ProtocolParser::CMD_ID_SCREEN_CONTROL:
				break;

			case ProtocolParser::CMD_ID_UI_CONTROL:
				break;

			default:
				break;
			}
			parser.reset();
		}
	} while (arraySize > 0);
}

void SerialHandler::buttonPressed()
{
	bool res;

	res = port->putChar('a');
	if (res == false) {
		qDebug() << "putChar problem";
	}
}

void SerialHandler::run()
{
	exec();
}
