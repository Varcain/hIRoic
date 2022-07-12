#include <QApplication>
#include <QObject>
#include <QDebug>
#include <QPushButton>

#include "config.h"
#include "serialhandler.h"
#include "mainpanel.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	SerialHandler handler;
	MainPanel panel;
	QPushButton button;

	QObject::connect(&handler, SIGNAL(sigSetPixel(int, int, uint)),
					 &panel, SLOT(slotSetPixel(int, int, uint)));
	QObject::connect(&handler, SIGNAL(sigFillArea(int, int, int, int, uint)),
					 &panel, SLOT(slotFillArea(int, int, int, int, uint)));
	QObject::connect(&button, SIGNAL(pressed()), &handler, SLOT(buttonPressed()));
	panel.show();

	button.setText("Next IR");
	button.show();
	handler.start();

    return a.exec();
}

