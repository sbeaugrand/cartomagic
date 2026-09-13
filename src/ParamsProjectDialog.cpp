/*-==============================================================-

file : ProjectParamsDialog.cpp

creation date : 08/02/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description :

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#include "ParamsProjectDialog.hpp"

ParamsProjectDialog::ParamsProjectDialog(ProjectParams *_params, DispatchControl *_control, QWidget *parent) : BandDialog(tr("Set project parameters"),tr("Parameters"), _params, _control, parent) {  

  QHBoxLayout *layNameProj = new QHBoxLayout;
  QLabel *nameProjLab = new QLabel(tr("Name :"));
  nameProjEdit = new QLineEdit();
  nameProjEdit->setMaxLength(100);
  nameProjEdit->setText(params->name);

  layNameProj->addWidget(nameProjLab);
  layNameProj->addWidget(nameProjEdit);

  QLabel *nbHolesLab = new QLabel(tr("number of holes :"));
  nbHoles = new QSpinBox();
  nbHoles->setRange(3,6);
  nbHoles->setValue(params->nbHoles);

  QLabel *patternLengthLab = new QLabel(tr("pattern length :"));
  patternLength = new QSpinBox();
  patternLength->setRange(2,100);
  patternLength->setValue(params->patternLength);

  QLabel *nbPatternsLab = new QLabel(tr("number of patterns :"));
  nbPatterns = new QSpinBox();
  nbPatterns->setRange(1,100);
  nbPatterns->setValue(params->nbPatterns);

  QHBoxLayout *laySpin = new QHBoxLayout;
  laySpin->addWidget(nbHolesLab);
  laySpin->addWidget(nbHoles);
  laySpin->addWidget(patternLengthLab);
  laySpin->addWidget(patternLength);
  laySpin->addWidget(nbPatternsLab);
  laySpin->addWidget(nbPatterns);

  // for now : desactivate changing the tree values
  nbHoles->setEnabled(false);
  patternLength->setEnabled(true);
  nbPatterns->setEnabled(true);

  QVBoxLayout *layAll = new QVBoxLayout;
  layAll->addLayout(layNameProj);
  layAll->addLayout(laySpin);

  setContent(layAll);  
}

void ParamsProjectDialog::checkBeforeAccept() {

  if (nameProjEdit->text().isEmpty()) {
    QMessageBox::critical(NULL,tr("Cannot set project parameters"),tr("Project name is missing."));
  }
  else if (patternLength->value() < nbHoles->value()) {
    QMessageBox::critical(NULL,tr("Cannot set project parameters"),tr("Pattern length is smaller than number of holes."));
  }
  else {
    params->setName(nameProjEdit->text());

    /* for now : cannot change the following parameters

    params->setNbHoles(nbHoles->value(),false);
    params->setNbPatterns(nbPatterns->value(),false);


    params->setPatternLength(patternLength->value(),false);
*/
    if ((int)(params->nbPatterns) != nbPatterns->value()) {
      control->setNbPatterns(nbPatterns->value());
    }

    if ((int)(params->patternLength) != patternLength->value()) {
      control->setPatternLength(patternLength->value());
    }
    accept();
  }
}
