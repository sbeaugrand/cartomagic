/*-==============================================================-

file : SwapGroupWarpDialog.cpp

creation date : 08/02/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/

#include "SwapGroupWarpDialog.hpp"

SwapGroupWarpDialog::SwapGroupWarpDialog(ProjectParams *_params, DispatchControl *_control, QWidget *_parent) : BandDialog (tr("Copy a group of warps"),tr("Parameters"),_params,_control,_parent) {

  choiceBox = new QGroupBox();
  from = new QSpinBox();
  from->setValue(1);
  lastFromValue = 1;
  from->setRange(1,params->bandWidth);

  nbWarp = new QSpinBox();
  nbWarp->setValue(1);
  nbWarp->setRange(1,params->bandWidth);

  to = new QSpinBox();
  to->setValue(2);
  lastToValue = 2;
  to->setRange(1,params->bandWidth);

  QLabel *swapLabel = new QLabel(tr("Swap "));
  QLabel *warpLabel;
  QLabel *toLabel;

  if (params->createMode == MODE_FROMBAND) {
    warpLabel = new QLabel(tr(" warps at "));
    toLabel = new QLabel(tr(" to warps at "));
  }
  else {
    warpLabel = new QLabel(tr(" rotations at "));
    toLabel = new QLabel(tr(" to rotations at "));
  }

  mirrorCheck = new QCheckBox(tr("Mirror mode"));
  mirrorCheck->setChecked(false);

  swLabel = new QLabel("");
  setMessage();

  QHBoxLayout *layFromTo = new QHBoxLayout;
  layFromTo->addWidget(swapLabel);
  layFromTo->addWidget(nbWarp);
  layFromTo->addWidget(warpLabel);
  layFromTo->addWidget(from);
  layFromTo->addWidget(toLabel);
  layFromTo->addWidget(to);

  QVBoxLayout *layAll = new QVBoxLayout;
  layAll->addLayout(layFromTo);
  layAll->addWidget(mirrorCheck);
  layAll->addWidget(swLabel);

  QVBoxLayout *layBox = new QVBoxLayout;

  layBox->addLayout(layAll);

  connect(nbWarp,SIGNAL(valueChanged(int)),this,SLOT(nbWarpChanged(int)));
  connect(from,SIGNAL(valueChanged(int)),this,SLOT(fromChanged(int)));
  connect(to,SIGNAL(valueChanged(int)),this,SLOT(toChanged(int)));
  connect(mirrorCheck,SIGNAL(clicked()),this,SLOT(setMessage()));
  setContent(layBox);
}

void SwapGroupWarpDialog::nbWarpChanged(int val) {

  if ((from->value() < to->value()) && ((from->value()+val) > to->value())) {
    nbWarp->setValue(to->value()-from->value());
  }
  else if ((to->value() < from->value()) && ((to->value()+val) > from->value())) {
    nbWarp->setValue(from->value()-to->value());
  }
  else if ((from->value() < to->value()) && ((to->value()+val-1) > (int)params->bandWidth)) {
    nbWarp->setValue(1+params->bandWidth-to->value());
  }
  else if ((to->value() < from->value()) && ((from->value()+val-1) > (int)params->bandWidth)) {
    nbWarp->setValue(1+params->bandWidth-from->value());
  }
  else {
    setMessage();
  }

}

void SwapGroupWarpDialog::fromChanged(int val) {

  if (val == to->value()) {
    if (val == (int)params->bandWidth) {
      lastFromValue = params->bandWidth-1;
    }
    else if (val == 1) {
      lastFromValue = 2;
    }
    else if (lastFromValue < val) {
      lastFromValue = val+1;
    }
    else if (lastFromValue > val) {
      lastFromValue = val-1;
    }
    from->setValue(lastFromValue);
  }
  else {
    nbWarpChanged(nbWarp->value());
  }
}

void SwapGroupWarpDialog::toChanged(int val) {

  if (val == from->value()) {
    if (val == (int)params->bandWidth) {
      lastToValue = params->bandWidth-1;
    }
    else if (val == 1) {
      lastToValue = 2;
    }
    else if (lastToValue < val) {
      lastToValue = val+1;
    }
    else if (lastToValue > val) {
      lastToValue = val-1;
    }
    to->setValue(lastToValue);
  }
  else {
    nbWarpChanged(nbWarp->value());
  }
}

void SwapGroupWarpDialog::setMessage() {

  int id1,id2;
  int nb;
  QString msg;


  id1 = from->value();
  id2 = to->value();
  nb = nbWarp->value();

  if (mirrorCheck->isChecked()) {
    if (params->createMode == MODE_FROMBAND) {
      msg = tr("swap warps %1 -> %2 to %4 -> %3, then twist all");
    }
    else {
      msg = tr("swap rotations %1 -> %2 to %4 -> %3, then twist all the associated warps");
    }
  }
  else {
    if (params->createMode == MODE_FROMBAND) {
      msg = tr("swap warps %1 -> %2 to %3 -> %4");
    }
    else {
      msg = tr("swap rotations %1 -> %2 to %3 -> %4");
    }
  }

  swLabel->setText(msg.arg(id1).arg(id1+nb-1).arg(id2).arg(id2+nb-1));
}

void SwapGroupWarpDialog::checkBeforeAccept() {  

  if (confirmAccept()) {
    bool isTwist = false;

    if (mirrorCheck->isChecked()) {
      isTwist = true;
    }

    int i;
    int id1,id2;
    int nb;
    id1 = from->value();
    id2 = to->value();
    nb = nbWarp->value();

    for(i=0;i<nb;i++) {
      if(!mirrorCheck->isChecked()) {
	control->swapWarp(id1+i-1,id2+i-1,false);
      }
      else {
	control->swapWarp(id1+i-1,id2+nb-1-i-1,true);
      }
    }
    accept();
  }
}

