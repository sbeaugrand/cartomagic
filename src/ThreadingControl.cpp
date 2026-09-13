/*-==============================================================-

file : ThreadingControl.cpp

creation date : 07/07/2011

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#include "ThreadingControl.hpp"

ThreadingControl::ThreadingControl(ProjectParams *_params, ThreadingModel *_model, ThreadingView *_view) : QObject() {
  params = _params;
  model = _model;
  view = _view;

  connect(params,SIGNAL(nbHolesChanged()),this,SLOT(nbHolesChanged()));
  connect(view,SIGNAL(holeClicked(int,int,bool)),this,SLOT(slotSelectHole(int,int,bool)));

}

void ThreadingControl::nbHolesChanged() {
  model->updateForNewNbHoles();
}

void ThreadingControl::slotSelectHole(int cardId, int holeId, bool rightButton) {

  model->updateHoleColor(cardId,(quint8)holeId);
}
