/*-==============================================================-

file : MovesView.hpp

creation date : 07/07/2011

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __MOVESVIEW_HPP__
#define __MOVESVIEW_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>
#include <QWidget>

#include "ProjectParams.hpp"
#include "MovesModel.hpp"

using namespace std;
using namespace Qt;

class MovesView : public QWidget {

  Q_OBJECT

public:

  MovesView(ProjectParams *_params, MovesModel *_model, QWidget *parent = NULL);

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
  int numWidth;
  int numHeight;  
  int idWidth;
  int idHeight;
  int marginUp;
  int marginDown;
  int marginLeft;
  int marginRight;
  int gapNumToMove;
  //int gapMoveToNum;
  int gapIdToMove;  
  int gapToFirstMove;
  int moveSize;
  int twistSize;
  int numUpStartX;
  int numUpStartY;
  //int numDownStartX;
  //int numDownStartY;
  int moveStartX;
  int moveStartY;
  int idStartX;
  int idStartY;
  int totalWidth;
  int totalHeight;

  void updateLayout();

signals:
  void moveClicked(int warpId, int pickId, bool rightButton);


};
#endif //__MOVESVIEW_HPP__
