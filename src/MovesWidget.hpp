/*-==============================================================-

file : MovesWidget.hpp

creation date : 06/07/2011

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __MOVESWIDGET_HPP__
#define __MOVESWIDGET_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>

#include "ProjectParams.hpp"
#include "MovesView.hpp"
#include "MovesControl.hpp"
#include "MovesModel.hpp"
#include "MovesSelectorsView.hpp"
#include "MovesLockersView.hpp"
#include "MovesSZView.hpp"

using namespace std;
using namespace Qt;

class MovesWidget : public QWidget {

  Q_OBJECT

public:

  MovesWidget(ProjectParams *_params, QWidget *parent = NULL);
  ~MovesWidget();

  /* NB : the control is the only part accessible
     from "upper" widgets, i.e. widget that contains
     MovesWidget. In this case, it is the Design widget
    */
  MovesControl *control;
  MovesView *view;
  MovesModel *model;
  MovesSelectorsView *selectors;
  MovesLockersView *lockers;
  MovesSZView *szView;
  QPushButton *guessButton;  

private:

  ProjectParams *params;  

  // main boxes of the widget
  QScrollArea *scrollMoves;  
  QScrollArea *scrollSelectors;
  QScrollArea *scrollLockers;
  QScrollArea *scrollSZ;

  QGroupBox *movesBox;
  QCheckBox *guessIdle;
  QCheckBox *allowNoCyclic;

  void createWidget();
  void createConnection();

public slots:
  void updateForNewCreationMode();
  void resizeScrollArea();

private slots:  
  void slotUpdateScrollAreas(int dx);
  void slotUpdateGuessCheck();

signals:
  void guessIdleToggled();
  void allowCyclicToggled();
  void refreshClicked();

};

#endif //__MOVESWIDGET_HPP__
