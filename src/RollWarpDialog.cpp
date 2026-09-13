/*-==============================================================-

file : RollWarpDialog.cpp

creation date : 08/02/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/

#include "RollWarpDialog.hpp"

RollWarpDialog::RollWarpDialog(ProjectParams *_params, DispatchControl *_control, QList<int> &_selIndex, QWidget *_parent) : BandDialog (tr("Rolling warps"),tr("Parameters"),_params,_control,_parent) {

  selIndex = _selIndex;
  choiceBox = new QGroupBox();
  up = new QRadioButton(tr("up"));
  down = new QRadioButton(tr("down"));
  down->setChecked(true);
  nbRoll = new QSpinBox();
  nbRoll->setValue(1);
  nbRoll->setRange(1,params->patternLength-1);
  QLabel *rollLabel = new QLabel(tr("Roll"));
  QLabel *selLabel = new QLabel(tr("selected warps"));

  QVBoxLayout *layRadio = new QVBoxLayout;
  layRadio->addWidget(up);
  layRadio->addWidget(down);
  choiceBox->setLayout(layRadio);

  QHBoxLayout *layAll = new QHBoxLayout;

  layAll->addWidget(rollLabel);
  layAll->addWidget(nbRoll);
  layAll->addWidget(choiceBox);
  layAll->addWidget(selLabel);

  QVBoxLayout *layBox = new QVBoxLayout;

  layBox->addLayout(layAll);

  setContent(layBox);
}

void RollWarpDialog::checkBeforeAccept() {  

  int i;
  int nbSel = selIndex.size();
  int index;
  int nbR = nbRoll->value();

  for(i=nbSel-1;i>=0;i--) {
    index = selIndex.at(i);

    if (up->isChecked()) {
      control->rollWarp(index,nbR,false);
    }
    else {
      control->rollWarp(index,nbR,true);
    }
  }
  accept();

}

