/*-==============================================================-

file : MirrorWarpDialog.cpp

creation date : 08/02/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/

#include "MirrorWarpDialog.hpp"

MirrorWarpDialog::MirrorWarpDialog(ProjectParams *_params, BandModel *_model, QWidget *_parent) : BandDialog (tr("Swap symetrically two groups of warps"),tr("Parameters"),_params,_model,_parent) {

  choiceBox = new QGroupBox();
  from = new QSpinBox();
  from->setValue(1);
  from->setRange(1,params->bandWidth/2);
  to = new QSpinBox();
  to->setValue(1);
  to->setRange(1,params->bandWidth/2);

  QLabel *mirrorLabel = new QLabel(tr("Swap warps from "));
  QLabel *toLabel = new QLabel(tr(" to "));

  mirrorCheck = new QCheckBox(tr("Mirror mode"));
  mirrorCheck->setChecked(true);

  xchgLabel = new QLabel("");

  QHBoxLayout *layFromTo = new QHBoxLayout;
  layFromTo->addWidget(mirrorLabel);
  layFromTo->addWidget(from);
  layFromTo->addWidget(toLabel);
  layFromTo->addWidget(to);

  QVBoxLayout *layAll = new QVBoxLayout;
  layAll->addLayout(layFromTo);
  layAll->addWidget(mirrorCheck);
  layAll->addWidget(xchgLabel);

  QVBoxLayout *layBox = new QVBoxLayout;

  layBox->addLayout(layAll);

  connect(from,SIGNAL(valueChanged(int)),this,SLOT(fromChanged(int)));  
  connect(to,SIGNAL(valueChanged(int)),this,SLOT(toChanged(int)));
  setContent(layBox);
}

void MirrorWarpDialog::fromChanged(int val) {

  if (val > to->value()) {
    from->setValue(to->value());
  }
  else {
    setMessage();
  }
}

void MirrorWarpDialog::toChanged(int val) {
  if (val < from->value()) {
    to->setValue(from->value());
  }
  else {
    setMessage();
  }
}

void MirrorWarpDialog::setMessage() {

  int id1,id2;
  QString msg;

  id1 = from->value();
  id2 = to->value();


  if (mirrorCheck->isChecked()) {
    msg = tr("swap warps %1 -> %2 with %3 -> %4, then twist all");
  }
  else {
    msg = tr("swap warps %1 -> %2 with %3 -> %4");
  }
  xchgLabel->setText(msg.arg(id1).arg(id2).arg(params->bandWidth+1 -id1).arg(params->bandWidth+1 -id2));
}

void MirrorWarpDialog::checkBeforeAccept() {  

  int i;
  int id1,id2;
  id1 = from->value();
  id2 = to->value();

  bool twist = mirrorCheck->isChecked();
  for(i=id1;i<=id2;i++) {
    params->swapWarp(i-1,params->bandWidth - i,twist);
  }
  accept();
}

