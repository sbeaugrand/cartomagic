/*-==============================================================-

file : BandView.hpp

creation date : 07/07/2011

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __BANDVIEW_HPP__
#define __BANDVIEW_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>

#include "ProjectParams.hpp"
#include "BandModel.hpp"

using namespace std;
using namespace Qt;

class BandView : public QWidget {

  Q_OBJECT

public:

  BandView(ProjectParams *_params, BandModel *_model, QWidget *parent = NULL);
  ~BandView();

  QSize minimumSizeHint() const;
  QSize sizeHint() const;
  QSize origSize;

public slots:
  void updateView();
  void updateWarp(int warpId);
  void updateLettering(int warpId);


  void changePickBaseLength(int length);
  void changePickTotalLength(int length);
  void changePickWidth(int width);

protected:
  void paintEvent(QPaintEvent *event);
  void mousePressEvent(QMouseEvent *event);

private:
  ProjectParams *params;
  BandModel *model;

  QList<QList<QPainterPath> *> pathes;

  int pickWidth; // copied from params, maybe to speed-up ??
  int pickBaseLength;
  int pickTotalLength;

  bool noPathes;

  int numWidth;
  int numHeight;
  int idWidth;
  int idHeight;
  int marginUp;
  int marginDown;
  int marginLeft;
  int marginRight;
  int gapToFirstWarp;
  int gapIdToWarps;
  int idStartX;
  int idEndY;
  int warpsStartX;
  int warpsStartY;
  int warpsEndY;
  int totalWidth;
  int totalHeight;

  void updatePathes();
  void clearPathes();
  void updatePath(int warpId);

  void updateLayout();

signals:
  void pickClicked(int warpId, int pickId, bool rightButton);  

};
#endif //__BANDVIEW_HPP__
