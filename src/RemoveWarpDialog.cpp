/*-==============================================================-

file : RemoveWarpDialog.cpp

creation date : 08/02/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/

#include "RemoveWarpDialog.hpp"

RemoveWarpDialog::RemoveWarpDialog(ProjectParams *_params, DispatchControl *_control, QList<int> &_selIndex, QWidget *_parent) : BandDialog (tr("Removing warps"),tr("Question"),_params,_control,_parent) {


  selIndex = _selIndex;
  QVBoxLayout *layBox = new QVBoxLayout;
  QLabel *remLabel = new QLabel(tr("Remove selected warps ?"));

  layBox->addWidget(remLabel);

  setContent(layBox);
}

void RemoveWarpDialog::checkBeforeAccept() {  

  if (confirmAccept()) {

    int i;    
    int nbSel = selIndex.size();
    int index;
    int count = 0;

    for(i=nbSel-1;i>=0;i--) {
      index = selIndex.at(i);

      count += 1;
      if (count == nbSel) {
        control->removeWarp(index,true);
      }
      else {
        control->removeWarp(index,false);
      }
    }
    accept();
  }
}

