#include "mainpanel.h"

MainPanel::MainPanel()
{
	this->setScene(new QGraphicsScene(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT));
	mDisplay = new QImage(SCREEN_WIDTH, SCREEN_HEIGHT, QImage::Format_RGB888);
	mDisplay->fill(Qt::white);
	mPixMapItem = new QGraphicsPixmapItem(QPixmap::fromImage(*mDisplay));
	this->scene()->addItem(mPixMapItem);
}

void MainPanel::slotSetPixel(int x, int y, uint index_or_rgb)
{
	if(index_or_rgb == 0) {
		mDisplay->setPixel(x, y, qRgb(0, 0, 0));
	} else {
		mDisplay->setPixel(x, y, qRgb(255, 255, 255));
	}

	mPixMapItem->setPixmap(QPixmap::fromImage(*mDisplay));
}

void MainPanel::slotFillArea(int x, int y, int cx, int cy, uint index_or_rgb)
{
	for (unsigned short i = x; i < cx + x; i++) {
		for (unsigned short j = y; j < cy + y; j++) {
			if(index_or_rgb == 0) {
				mDisplay->setPixel(i, j, qRgb(0, 0, 0));
			} else {
				mDisplay->setPixel(i, j, qRgb(255, 255, 255));
			}
		}
	}
	mPixMapItem->setPixmap(QPixmap::fromImage(*mDisplay));
}
