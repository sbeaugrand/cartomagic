/*-==============================================================-

file : MovesControl.cpp

creation date : 07/07/2011

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#include "MovesControl.hpp"

MovesControl::MovesControl(ProjectParams *_params, MovesModel *_model, MovesView *_view, MovesSelectorsView *_selectors, MovesLockersView *_lockers, MovesSZView *_szView) : QObject() {
  params = _params;
  model = _model;
  view = _view;
  selectors = _selectors;
  lockers = _lockers;
  szView = _szView;

  connect(szView,SIGNAL(mountClicked(int)),this,SIGNAL(mountClicked(int)));
  connect(szView,SIGNAL(cycleClicked(int)),this,SIGNAL(cycleClicked(int)));

  connect(selectors,SIGNAL(warpSelected(int)),model,SLOT(selectMove(int)));
  connect(selectors,SIGNAL(allWarpsSelected()),model,SLOT(selectAllMoves()));

  connect(lockers,SIGNAL(warpTotallyLocked(int)),model, SLOT(totalLockMove(int)));

  connect(view,SIGNAL(moveClicked(int,int,bool)),this,SLOT(slotSelectMove(int,int,bool)));
}

void MovesControl::slotSelectMove(int warpId, int moveId, bool rightButton) {

  if ( ((model->totallyLockedMoves).at(warpId) == false) && (model->isVoidMove(warpId) == false)) {
    if (!rightButton) {
      model->toggleMove(warpId,moveId);
    }
    else {
      model->putIdle(warpId,moveId);
    }
  }
}
