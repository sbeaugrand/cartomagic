/*-==============================================================-

file : MovesControl.hpp

creation date : 01/01/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __MOVESCONTROL_HPP__
#define __MOVESCONTROL_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>

#include "ProjectParams.hpp"
#include "MovesModel.hpp"
#include "MovesView.hpp"
#include "MovesSelectorsView.hpp"
#include "MovesLockersView.hpp"
#include "MovesSZView.hpp"

using namespace std;
using namespace Qt;

class MovesControl : public QObject {

  Q_OBJECT

public:

  MovesControl(ProjectParams *_params, MovesModel *_model, MovesView *_view, MovesSelectorsView *_selectors, MovesLockersView *_lockers, MovesSZView *_szView);

private slots:

  void slotSelectMove(int warpId, int moveId, bool rightButton);

private:
  ProjectParams *params;
  MovesModel *model;
  MovesView *view;
  MovesSelectorsView *selectors;
  MovesLockersView * lockers;
  MovesSZView *szView;

signals:

  void mountClicked(int warpId);
  void cycleClicked(int warpId);  

};
#endif //__MOVESCONTROL_HPP__
