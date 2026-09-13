/*-==============================================================-

file : ColorWarpDialog.cpp

creation date : 08/02/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/

#include "ColorWarpDialog.hpp"

ColorWarpDialog::ColorWarpDialog(ProjectParams *_params, DispatchControl *_control, QList<int> &_selIndex, QWidget *_parent) : BandDialog (tr("Painting warps"),tr("Question"),_params,_control,_parent) {

  selIndex = _selIndex;
  QVBoxLayout *layBox = new QVBoxLayout;
  QLabel *paintLabel = new QLabel(tr("Paint selected warps with current color ?"));

  layBox->addWidget(paintLabel);

  setContent(layBox);
}

void ColorWarpDialog::checkBeforeAccept() {  

  if (confirmAccept()) {

    int i,insIndex;
    int nbSel = selIndex.size();

    for(i=0;i<nbSel;i++) {
      insIndex = selIndex.at(i);
      control->colorWarp(insIndex);
    }
    accept();
  }
}

