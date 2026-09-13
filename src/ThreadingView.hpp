/*-==============================================================-

file : ThreadingView.hpp

creation date : 07/07/2011

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __THREADINGVIEW_HPP__
#define __THREADINGVIEW_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>
#include <QWidget>

#include "ProjectParams.hpp"
#include "ThreadingModel.hpp"


using namespace std;
using namespace Qt;

class ThreadingView : public QWidget {

  Q_OBJECT

public:

  ThreadingView(ProjectParams *_params, ThreadingModel *_model, QWidget *parent = NULL);

  QSize minimumSizeHint() const;
  QSize sizeHint() const;  

public slots:
  void updateView();
  void updateCard(int cardId);


protected:
  void paintEvent(QPaintEvent *event);
  void mousePressEvent(QMouseEvent *event);

private:
  ProjectParams *params;
  ThreadingModel *model;
  int nameWidth;
  int nameHeight;
  int zWidth;
  int zHeight;
  int numWidth;
  int numHeight;

  int marginUp;
  int marginDown;
  int marginLeft;
  int marginRight;
  int gapToFirstColor;
  int gapLetterToColor;
  int gapColorToSZ;
  int gapNumToColor;
  int numStartX;
  int numStartY;
  int colorSize;
  int colorStartX;
  int colorStartY;
  int colorEndY;
  int letterStartX;
  int letterStartY;
  int szStartX;
  int szStartY;
  int totalWidth;
  int totalHeight;

  void updateLayout();

signals:
  void holeClicked(int cardId, int holeId, bool rightButton);


};
#endif //__THREADINGVIEW_HPP__
