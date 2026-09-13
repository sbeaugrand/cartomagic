/*-==============================================================-

file : InsertWarpDialog.cpp

creation date : 08/02/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/

#include "InsertWarpDialog.hpp"

InsertWarpDialog::InsertWarpDialog(ProjectParams *_params, DispatchControl *_control, QList<int> &_selIndex, QWidget *_parent) : BandDialog (tr("Inserting new warps"),tr("Parameters"),_params,_control,_parent) {

  selIndex = _selIndex;
  choiceBox = new QGroupBox();
  before = new QRadioButton(tr("before"));
  before->setChecked(true);
  after = new QRadioButton(tr("after"));
  nbInsert = new QSpinBox();
  nbInsert->setValue(1);
  nbInsert->setRange(1,50);
  QLabel *insLabel = new QLabel(tr("Insert"));
  QLabel *selLabel = new QLabel(tr("selected warps"));

  QVBoxLayout *layRadio = new QVBoxLayout;
  layRadio->addWidget(before);
  layRadio->addWidget(after);
  choiceBox->setLayout(layRadio);

  QHBoxLayout *layAll = new QHBoxLayout;

  layAll->addWidget(insLabel);
  layAll->addWidget(nbInsert);
  layAll->addWidget(choiceBox);
  layAll->addWidget(selLabel);

  QVBoxLayout *layBox = new QVBoxLayout;
  layBox->addLayout(layAll);
  setContent(layBox);
}

void InsertWarpDialog::checkBeforeAccept() {  

  int i,j;
  bool isBefore = before->isChecked();  
  int count = 0;
  int nbToIns = nbInsert->value();
  int insIndex;
  int nbSel = selIndex.size();

  for(i=nbSel-1;i>=0;i--) {
    insIndex = selIndex.at(i);
    if (!isBefore) {
      insIndex = i+1;
    }
    for(j=0;j<nbToIns;j++) {      
      count += 1;

      if (count == nbSel*nbToIns) {
        control->insertWarp(insIndex,true);
      }
      else {
        control->insertWarp(insIndex,false);
      }
    }
  }
  accept();
}

