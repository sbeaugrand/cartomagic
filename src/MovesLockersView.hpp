/*-==============================================================-

file : MovesLockersView.hpp

creation date : 07/07/2011

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __MOVESLOCKERSVIEW_HPP__
#define __MOVESLOCKERSVIEW_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>

#include "ProjectParams.hpp"
#include "MovesModel.hpp"

using namespace std;
using namespace Qt;

class MovesLockersView : public QWidget {

  Q_OBJECT

public:

  MovesLockersView(ProjectParams *_params, MovesModel *_model, QWidget *parent = NULL);

  QSize minimumSizeHint() const;
  QSize sizeHint() const;
  QSize origSize;

public slots:
  void updateView();
  void updateLockers(int warpId);
  void changeMoveWidth(int width);

protected:
  void paintEvent(QPaintEvent *event);
  void mousePressEvent(QMouseEvent *event);

private:
  ProjectParams *params;
  MovesModel *model;

  QImage lockAllIcon;
  QImage unlockAllIcon;

  int moveWidth; // copied from params, maybe to speed-up ??

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
  int gapTotalLockToNumLock;

  int lockSize;

  int totalLockStartX;
  int totalLockStartY;

  int numLockStartX;
  int numLockStartY;
  int numLockEndY;

  int totalWidth;
  int totalHeight;

  void updateLayout();

signals:
  void warpTotallyLocked(int warpId);  
};
#endif //__MOVESLOCKERSVIEW_HPP__
