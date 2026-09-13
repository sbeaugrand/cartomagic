/*-==============================================================-

file : MovesSelectorsView.hpp

creation date : 07/07/2011

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __MOVESSELECTORSVIEW_HPP__
#define __MOVESSELECTORSVIEW_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>

#include "ProjectParams.hpp"
#include "MovesModel.hpp"

using namespace std;
using namespace Qt;

class MovesSelectorsView : public QWidget {

  Q_OBJECT

public:

  MovesSelectorsView(ProjectParams *_params, MovesModel *_model, QWidget *parent = NULL);

  QSize minimumSizeHint() const;
  QSize sizeHint() const;
  QSize origSize;

public slots:
  void updateView();
  void updateSelectors(int warpId);

  void changeMoveWidth(int width);

protected:
  void paintEvent(QPaintEvent *event);
  void mousePressEvent(QMouseEvent *event);

private:
  ProjectParams *params;
  MovesModel *model;

  int moveWidth; // copied from params, maybe to speed-up ??
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
#endif //__MOVESSELECTORSVIEW_HPP__
