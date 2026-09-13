/*-==============================================================-

file : CopyWarpDialog.cpp

creation date : 08/02/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/

#include "CopyWarpDialog.hpp"

CopyWarpDialog::CopyWarpDialog(ProjectParams *_params, BandModel *_model, QWidget *_parent) : BandDialog (tr("Copying warps"),tr("Parameters"),_params,_model,_parent) {

  id1 = -1;
  id2 = -1;

  quint32 i;
  for(i=0;i<(params->bandWidth);i++) {      
    if((model->selectedWarps).at(i)) {
      if (id1 == -1) {
        id1 = i;
      }
      else {
        id2 = i;
        i = params->bandWidth;
      }
    }
  }

  choiceBox = new QGroupBox();
  QString txt = tr("warp %1 into %2");
  
  left = new QRadioButton(txt.arg(id2).arg(id1));
  right = new QRadioButton(txt.arg(id1).arg(id2));
  right->setChecked(true);
  QLabel *copyLabel = new QLabel(tr("Copy"));

  QVBoxLayout *layRadio = new QVBoxLayout;
  layRadio->addWidget(right);
  layRadio->addWidget(left);
  choiceBox->setLayout(layRadio);

  QHBoxLayout *layAll = new QHBoxLayout;

  layAll->addWidget(copyLabel);
  layAll->addWidget(choiceBox);

  QVBoxLayout *layBox = new QVBoxLayout;

  layBox->addLayout(layAll);

  setContent(layBox);
}

void CopyWarpDialog::checkBeforeAccept() {  

  if (confirmAccept()) {

    if (right->isChecked()) {
      model->copyWarp(id1,id2);
    }
    else {
       model->copyWarp(id2,id1);
    }

    accept();
  }
}

