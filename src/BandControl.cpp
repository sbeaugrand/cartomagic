/*-==============================================================-

file : BandControl.cpp

creation date : 07/07/2011

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#include "BandControl.hpp"

BandControl::BandControl(ProjectParams *_params, BandModel *_model, BandView *_view, BandLockersView *_lockers,  BandSelectorsView *_selectors) : QObject() {
  params = _params;
  model = _model;
  view = _view;
  lockers = _lockers;
  selectors = _selectors;
  mode = MODE_COLOR;

  connect(view, SIGNAL(pickClicked(int,int,bool)),this,SLOT(slotSelectPick(int,int,bool)));

  connect(selectors,SIGNAL(warpSelected(int)),model,SLOT(selectWarp(int)));
  connect(selectors,SIGNAL(allWarpsSelected()),model,SLOT(selectAllWarps()));

  connect(lockers,SIGNAL(warpTotallyLocked(int)),model, SLOT(totalLockWarp(int)));
  connect(lockers,SIGNAL(warpShapeLocked(int)),model, SLOT(shapeLockWarp(int)));

}

void BandControl::setModel(const QList<QList<quint16> *> &_warps) {
  model->setModel(_warps);
}

void BandControl::setMode(int _mode) {
  mode = _mode;
}

void BandControl::slotSelectPick(int warpId, int pickId, bool rightButton) {

  if (mode == MODE_COLOR) {   
    if (!rightButton) {
      model->colorPick(warpId,pickId);
    }
    else {
      if (! (model->shapeLockedWarps).at(warpId)) {
        model->twistPick(warpId,pickId);
      }
    }
  }
  else if (mode == MODE_TWIST) {
    if (!rightButton) {
      if (! (model->shapeLockedWarps).at(warpId)) {
        model->twistPick(warpId,pickId);
      }
    }
    else {
      model->colorPick(warpId,pickId);
    }
  }
}
