/*-==============================================================-

file : BandSelectorsView.hpp

creation date : 07/07/2011

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __BANDSELECTORSVIEW_HPP__
#define __BANDSELECTORSVIEW_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>

#include "ProjectParams.hpp"
#include "BandModel.hpp"

using namespace std;
using namespace Qt;

class BandSelectorsView : public QWidget {

  Q_OBJECT

public:

  BandSelectorsView(ProjectParams *_params, BandModel *_model, QWidget *parent = NULL);

  QSize minimumSizeHint() const;
  QSize sizeHint() const;
  QSize origSize;

public slots:
  void updateView();
  void updateSelectors(int warpId);

  void changePickWidth(int width);

protected:
  void paintEvent(QPaintEvent *event);
  void mousePressEvent(QMouseEvent *event);

private:
  ProjectParams *params;
  BandModel *model;

  int pickWidth; // copied from params, maybe to speed-up ??
  int numWidth;
  int numHeight;
  int idWidth;
  int idHeight;
  int marginUp;
  int marginDown;
  int marginLeft;
  int marginRight;
  int gapToFirstSel;
  int gapNumToSel;
  int numStartX;
  int numStartY;
  int numEndY;
  int setStartX;
  int selStartY;
  int selSize;
  int totalWidth;
  int totalHeight;

  void updateLayout();

signals:

  void warpSelected(int warpId);
  void allWarpsSelected();

};
#endif //__BANDSELECTORSVIEW_HPP__
