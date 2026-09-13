/*-==============================================================-

file : BandLockersView.hpp

creation date : 07/07/2011

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __BANDLOCKERSVIEW_HPP__
#define __BANDLOCKERSVIEW_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>

#include "ProjectParams.hpp"
#include "BandModel.hpp"

using namespace std;
using namespace Qt;

class BandLockersView : public QWidget {

  Q_OBJECT

public:

  BandLockersView(ProjectParams *_params, BandModel *_model, QWidget *parent = NULL);  

  QSize minimumSizeHint() const;
  QSize sizeHint() const;
  QSize origSize;

public slots:
  void updateView();
  void updateLockers(int warpId);
  void changePickWidth(int width);

protected:
  void paintEvent(QPaintEvent *event);
  void mousePressEvent(QMouseEvent *event);

private:
  ProjectParams *params;
  BandModel *model;

  QImage lockIcon;
  QImage unlockIcon;
  QImage lockAllIcon;
  QImage unlockAllIcon;
  QImage twistIcon;

  int pickWidth; // copied from params, maybe to speed-up ??

  int numWidth;
  int numHeight;

  int idWidth;
  int idHeight;

  // properties to define the view layout
  int marginUp;
  int marginDown;
  int marginLeft;
  int marginRight;
  int gapToFirstLock;
  int gapTotalLockToShapeLock;
  int gapShapeLockToNumLock;

  int lockSize;

  int totalLockStartX;
  int totalLockStartY;

  int shapeLockStartX;
  int shapeLockStartY;

  int numLockStartX;
  int numLockStartY;
  int numLockEndY;

  int totalWidth;
  int totalHeight;

  void updateLayout();

signals:
  void warpTotallyLocked(int warpId);
  void warpShapeLocked(int warpId);
};
#endif //__BANDLOCKERSVIEW_HPP__
