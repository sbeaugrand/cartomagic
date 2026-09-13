/*-==============================================================-

file : CopyGroupWarpDialog.cpp

creation date : 08/02/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/

#include "CopyGroupWarpDialog.hpp"

CopyGroupWarpDialog::CopyGroupWarpDialog(ProjectParams *_params, DispatchControl *_control, QWidget *_parent) : BandDialog (tr("Copy a group of warps"),tr("Parameters"),_params,_control,_parent) {

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

  QLabel *copyLabel;
  QLabel *warpLabel;
  QLabel *toLabel;
  copyLabel = new QLabel(tr("Copy "));
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

  threadCheck = new QCheckBox(tr("Copy card threading"));
  threadCheck->setChecked(true);

  cpLabel = new QLabel("");
  setMessage();

  QHBoxLayout *layFromTo = new QHBoxLayout;
  layFromTo->addWidget(copyLabel);
  layFromTo->addWidget(nbWarp);
  layFromTo->addWidget(warpLabel);
  layFromTo->addWidget(from);
  layFromTo->addWidget(toLabel);
  layFromTo->addWidget(to);

  QVBoxLayout *layAll = new QVBoxLayout;
  layAll->addLayout(layFromTo);
  if (params->createMode == MODE_FROMMOVES) {
    layAll->addWidget(threadCheck);
  }
  layAll->addWidget(mirrorCheck);
  layAll->addWidget(cpLabel);

  QVBoxLayout *layBox = new QVBoxLayout;

  layBox->addLayout(layAll);

  connect(nbWarp,SIGNAL(valueChanged(int)),this,SLOT(nbWarpChanged(int)));
  connect(from,SIGNAL(valueChanged(int)),this,SLOT(fromChanged(int)));
  connect(to,SIGNAL(valueChanged(int)),this,SLOT(toChanged(int)));
  connect(mirrorCheck,SIGNAL(clicked()), this, SLOT(setMessage()));
  setContent(layBox);
}

void CopyGroupWarpDialog::nbWarpChanged(int val) {

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

void CopyGroupWarpDialog::fromChanged(int val) {

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

void CopyGroupWarpDialog::toChanged(int val) {

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

void CopyGroupWarpDialog::setMessage() {

  int id1,id2;
  int nb;
  QString msg;


  id1 = from->value();
  id2 = to->value();
  nb = nbWarp->value();


  if (mirrorCheck->isChecked()) {
    if (params->createMode == MODE_FROMBAND) {
      msg = tr("copy warps %1 -> %2 to %4 -> %3, then twist the lasts");
    }
    else {
      msg = tr("copy rotations %1 -> %2 to %4 -> %3, then twist the associated warps of the lasts");
    }
  }
  else {
    if (params->createMode == MODE_FROMBAND) {
      msg = tr("copy warps %1 -> %2 to %3 -> %4");
    }
    else {
      msg = tr("copy rotations %1 -> %2 to %3 -> %4");
    }
  }
  cpLabel->setText(msg.arg(id1).arg(id1+nb-1).arg(id2).arg(id2+nb-1));
}

void CopyGroupWarpDialog::checkBeforeAccept() {  

  if (confirmAccept()) {
    bool isTwist = false;
    bool isThread = false;
    if (mirrorCheck->isChecked()) {
      isTwist = true;
    }
    if (threadCheck->isChecked()) {
      isThread = true;
    }
    int i;
    int id1,id2;
    int nb;
    id1 = from->value();
    id2 = to->value();
    nb = nbWarp->value();

    for(i=0;i<nb;i++) {
      if (params->createMode == MODE_FROMBAND) {
	if (!isTwist) {
	  control->copyWarp(id1+i-1,id2+i-1,false);
	}
	else {
	  control->copyWarp(id1+i-1,id2+nb-1-i-1,true);
	}
      }
      else {
	if (!isTwist) {
	  control->copyMoves(id1+i-1,id2+i-1,false,isThread);
	}
	else {
	  control->copyMoves(id1+i-1,id2+nb-1-i-1,true,isThread);
	}
      }
    }
    accept();
  }
}

