/*-==============================================================-

file : TwistWarpDialog.cpp

creation date : 08/02/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/

#include "TwistWarpDialog.hpp"

TwistWarpDialog::TwistWarpDialog(ProjectParams *_params, DispatchControl *_control, QList<int> &_selIndex, QWidget *_parent) : BandDialog (tr("Twisting warps"),tr("Question"),_params,_control,_parent) {


  selIndex = _selIndex;
  QVBoxLayout *layBox = new QVBoxLayout;
  QLabel *remLabel = new QLabel(tr("Twist selected warps ?"));

  layBox->addWidget(remLabel);

  setContent(layBox);
}

void TwistWarpDialog::checkBeforeAccept() {  

  int i;
  int nbSel = selIndex.size();
  int index;


  for(i=nbSel-1;i>=0;i--) {
    index = selIndex.at(i);
    control->twistWarp(index);
  }
  accept();

}

