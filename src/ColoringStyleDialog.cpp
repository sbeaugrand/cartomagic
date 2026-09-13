/*-==============================================================-

file : ColoringStyleDialog.cpp

creation date : 08/02/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/

#include "ColoringStyleDialog.hpp"

ColoringStyleDialog::ColoringStyleDialog(ProjectParams *_params, DispatchControl *_control, QWidget *_parent) : BandDialog (tr("Set coloring style of warps"),tr("Style"),_params,_control,_parent) {

  choiceBox = new QGroupBox();

  cyclic = new QRadioButton(tr("cyclically (default)"));  
  free = new QRadioButton(tr("not cyclically"));
  holes = new QRadioButton(tr("following hole letters"));

  if (params->coloringStyle == COLORING_CYCLIC) {
    cyclic->setChecked(true);
  }
  else if (params->coloringStyle == COLORING_FREE) {
    free->setChecked(true);
  }
  else if (params->coloringStyle == COLORING_HOLE) {
    holes->setChecked(true);
  }

  QVBoxLayout *layRadio = new QVBoxLayout;
  layRadio->addWidget(cyclic);
  layRadio->addWidget(free);
  layRadio->addWidget(holes);
  choiceBox->setLayout(layRadio);

  QVBoxLayout *layBox = new QVBoxLayout;
  layBox->addWidget(choiceBox);

  setContent(layBox);
}

void ColoringStyleDialog::checkBeforeAccept() {

  if (cyclic->isChecked()) {
    params->setColoringStyle(COLORING_CYCLIC);
  }
  else if (free->isChecked()) {
    params->setColoringStyle(COLORING_FREE);
  }
  else if (holes->isChecked()) {
    params->setColoringStyle(COLORING_HOLE);
  }
  accept();
}

