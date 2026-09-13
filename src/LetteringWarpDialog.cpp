/*-==============================================================-

file : LetteringWarpDialog.cpp

creation date : 08/02/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/

#include "LetteringWarpDialog.hpp"

LetteringWarpDialog::LetteringWarpDialog(ProjectParams *_params, DispatchControl *_control, QWidget *_parent) : BandDialog (tr("Show hole letters on picks or not"),tr("Parameters"),_params,_control,_parent) {

  selectAllBut = new QPushButton(tr("Select all"));
  deselectAllBut = new QPushButton(tr("Deselect all"));

  QHBoxLayout * layBut = new QHBoxLayout;
  layBut->addWidget(selectAllBut);
  layBut->addStretch();
  layBut->addWidget(deselectAllBut);

  quint32 nbRow = 1+ ((params->bandWidth-1)/25);
  QGridLayout *layT = new QGridLayout;
  QLabel *numLab = new QLabel(tr("warp #"));
  QLabel *stateLab = new QLabel(tr("lettering ?"));
  layT->addWidget(stateLab,0,0);
  layT->addWidget(numLab,1,0);
  quint32 i,j;
  QCheckBox *c = NULL;
  QLabel *l;
  QString numWarp;
  quint32 nbCheckPerRow;
  lstLettered.clear();
  for(i=0;i<nbRow;i++) {
    if ((i+1) == nbRow) {
      nbCheckPerRow = params->bandWidth % 25;
      if (nbCheckPerRow == 0) nbCheckPerRow = 25;
    }
    else {
      nbCheckPerRow = 25;
    }
    for(j=0;j<nbCheckPerRow;j++) {
      c= new QCheckBox();
      if (params->isWarpHoleNumber.at(25*i+j)) {
        c->setChecked(true);
      }
      else {
        c->setChecked(false);
      }
      lstLettered.append(c);
      numWarp.setNum(25*i+j+1);
      l = new QLabel(numWarp);
      layT->addWidget(c,2*i,j+1);
      layT->addWidget(l,2*i+1,j+1);
    }
  }  

  QVBoxLayout *layBox = new QVBoxLayout;

  layBox->addLayout(layBut);
  layBox->addLayout(layT);

  setContent(layBox);

  connect(selectAllBut,SIGNAL(clicked()),this, SLOT(selectAll()));
  connect(deselectAllBut,SIGNAL(clicked()),this, SLOT(deselectAll()));
}



void LetteringWarpDialog::selectAll() {

  quint32 i;
  for(i=0;i<params->bandWidth;i++) {
    lstLettered.at(i)->setChecked(true);
  }
}

void LetteringWarpDialog::deselectAll() {
  quint32 i;
  for(i=0;i<params->bandWidth;i++) {
    lstLettered.at(i)->setChecked(false);
  }
}

void LetteringWarpDialog::checkBeforeAccept() {  

  quint32 i;  
  bool state;  

  for(i=0;i<params->bandWidth;i++) {
    state = lstLettered.at(i)->isChecked();
    control->setWarpLettering(i,state);
  }
  accept();
}

