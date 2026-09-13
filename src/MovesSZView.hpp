/*-==============================================================-

file : MovesSZView.hpp

creation date : 07/07/2011

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __MOVESSZVIEW_HPP__
#define __MOVESSZVIEW_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>
#include <QWidget>

#include "ProjectParams.hpp"
#include "MovesModel.hpp"

using namespace std;
using namespace Qt;

class MovesSZView : public QWidget {

  Q_OBJECT

public:

  MovesSZView(ProjectParams *_params, MovesModel *_model, QWidget *parent = NULL);

  QSize minimumSizeHint() const;
  QSize sizeHint() const;  

public slots:
  void updateView();
  void updateMove(int warpId);
  void updateCyclic(int warpId);

protected:
  void paintEvent(QPaintEvent *event);
  void mousePressEvent(QMouseEvent *event);

private:
  ProjectParams *params;
  MovesModel *model;
  int szWidth;
  int szHeight;
  int idWidth;
  int idHeight;
  QImage cycleIcon;
  QImage uncycleIcon;

  int moveSize;
  int marginUp;
  int marginDown;
  int marginLeft;
  int marginRight;
  int gapSZToCycle;
  int gapToFirstSel;
  int cycleSize;
  int szStartX;
  int szStartY;
  int cycleStartX;
  int cycleStartY;
  int totalWidth;
  int totalHeight;

  void updateLayout();

signals:
  void mountClicked(int warpId);
  void cycleClicked(int warpId);


};
#endif //__MOVESSZVIEW_HPP__
