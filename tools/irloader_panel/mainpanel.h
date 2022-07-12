#ifndef MAINPANEL_H
#define MAINPANEL_H

#include <QObject>
#include <QImage>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include "config.h"

class MainPanel : public QGraphicsView
{
	Q_OBJECT
public:
	explicit MainPanel();

signals:

public slots:
	void slotSetPixel(int x, int y, uint index_or_rgb);
	void slotFillArea(int x, int y, int cx, int cy, uint index_or_rgb);
private:
	QImage *mDisplay;
	QGraphicsPixmapItem *mPixMapItem;
};

#endif // MAINPANEL_H
