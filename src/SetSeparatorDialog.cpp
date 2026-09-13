/*-==============================================================-

file : SetSeparatorDialog.cpp

creation date : 08/02/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/

#include "SetSeparatorDialog.hpp"

SetSeparatorDialog::SetSeparatorDialog(ProjectParams *_params, DispatchControl *_control, QWidget *_parent) : BandDialog (tr("Set vertical separator locations"),tr("Parameters"),_params,_control,_parent) {

  regBox = new QGroupBox(tr("Gap"));
  gap = new QSpinBox();
  gap->setValue(5);
  gap->setRange(1,params->bandWidth);

  QLabel *labVertSep1 = new QLabel(tr("separators each"));
  QLabel *labVertSep2 = new QLabel(tr("warps"));
  QHBoxLayout *layL1 = new QHBoxLayout;
  layL1->addWidget(labVertSep1);
  layL1->addWidget(gap);
  layL1->addWidget(labVertSep2);
  layL1->addStretch();
  setGap = new QPushButton(tr("Set"));
  QHBoxLayout *layL2 = new QHBoxLayout;
  layL2->addStretch();
  layL2->addWidget(setGap);
  QVBoxLayout *layBox = new QVBoxLayout;
  layBox->addLayout(layL1);
  layBox->addLayout(layL2);
  regBox->setLayout(layBox);

  QGridLayout *layT = new QGridLayout;
  QLabel *numLab = new QLabel(tr("warp #"));
  QLabel *sepLab = NULL;
  sepLab = new QLabel(tr("sep. after ?"));
  layT->addWidget(sepLab,0,0);
  layT->addWidget(numLab,1,0);
  quint32 i;
  QCheckBox *c = NULL;
  QLabel *l;
  QString numPick;  
  for(i=0;i<params->bandWidth;i++) {
    c= new QCheckBox();
    if ((params->movesVertSep).contains(i+1)) {
      c->setChecked(true);
    }
    lstCheck.append(c);
    numPick.setNum(i+1);
    l = new QLabel(numPick);
    layT->addWidget(c,0,i+1);
    layT->addWidget(l,1,i+1);
  }

  QVBoxLayout *layAll = new QVBoxLayout;

  layAll->addWidget(regBox);
  layAll->addLayout(layT);

  setContent(layAll);

  connect(setGap,SIGNAL(clicked()),this,SLOT(setRegularGap()));
}


void SetSeparatorDialog::setRegularGap() {

  quint32 i;
  int g = gap->value();
  QCheckBox *c;

  for(i=0;i<params->bandWidth;i++) {
    c = lstCheck.at(i);
    if (((i+1)%g) == 0) {
      c->setChecked(true);
    }
    else {
      c->setChecked(false);
    }
  }

}

void SetSeparatorDialog::checkBeforeAccept() {

  QList<int> sep;
  QCheckBox *c;
  quint32 i;
  for(i=0;i<params->bandWidth;i++) {
    c = lstCheck.at(i);
    if (c->isChecked()) {
      sep.append(i+1);
    }
  }
  params->setMovesVertSep(sep);

  accept();
}

