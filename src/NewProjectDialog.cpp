#include <QLabel>
#include <QMessageBox>
#include "NewProjectDialog.hpp"

NewProjectDialog::NewProjectDialog(ProjectParams *_params, QWidget *parent) : CustomDialog(tr("Create a new project"),tr("Parameters"), parent) {

  params = _params;

  QHBoxLayout *layNameProj = new QHBoxLayout;
  QLabel *nameProjLab = new QLabel(tr("Name :"));
  nameProjEdit = new QLineEdit();
  nameProjEdit->setMaxLength(100);
  nameProjEdit->setText("");

  layNameProj->addWidget(nameProjLab);
  layNameProj->addWidget(nameProjEdit);

  QHBoxLayout *laySpin = new QHBoxLayout;
  QLabel *nbCardsLab = new QLabel(tr("number of cards :"));
  nbCards = new QSpinBox();
  nbCards->setRange(1,100);
  nbCards->setValue(16);

  QLabel *nbHolesLab = new QLabel(tr("number of holes :"));
  nbHoles = new QSpinBox();
  nbHoles->setRange(3,6);
  nbHoles->setValue(4);

  laySpin->addWidget(nbCardsLab);
  laySpin->addWidget(nbCards);
  laySpin->addWidget(nbHolesLab);
  laySpin->addWidget(nbHoles);

  QHBoxLayout *layPat = new QHBoxLayout;
  QLabel *patternLengthLab = new QLabel(tr("pattern length :"));
  QLabel *nbPatternLab = new QLabel(tr("number of patterns :"));
  patternLength = new QSpinBox();
  patternLength->setRange(2,100);
  patternLength->setValue(8);
  nbPattern = new QSpinBox();
  nbPattern->setRange(1,4);
  nbPattern->setValue(3);

  layPat->addWidget(patternLengthLab);
  layPat->addWidget(patternLength);
  layPat->addWidget(nbPatternLab);
  layPat->addWidget(nbPattern);

  QVBoxLayout *layAll = new QVBoxLayout;
  layAll->addLayout(layNameProj);
  layAll->addLayout(laySpin);
  layAll->addLayout(layPat);

  setContent(layAll);  
}

void NewProjectDialog::checkBeforeAccept() {

  bool allOk = true;
  int ret;
  if (nameProjEdit->text().isEmpty()) {
    QMessageBox::critical(NULL,tr("Cannot create project"),tr("Project name is missing."));
    allOk = false;
  }
  else if (patternLength->value() < nbHoles->value()) {
    QMessageBox::critical(NULL,tr("Cannot create project"),tr("Pattern length is smaller than number of holes."));
    allOk = false;
  }
  else if ((patternLength->value() >= 52) && (nbPattern->value() > 1)) {
    ret = QMessageBox::question(NULL,tr("Confirming number of patterns"),tr("Taking account of pattern length, the number of patterns is too high to comfortably create the band. Do you confirm its value ?"),QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
    if (ret == QMessageBox::Cancel) {
      allOk = false;
    }
  }
  else if ((patternLength->value() >= 26) && (nbPattern->value() > 2)) {
    ret = QMessageBox::question(NULL,tr("Confirming number of patterns"),tr("Taking account of pattern length, the number of patterns is too high to comfortably create the band. Do you confirm its value ?"),QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
    if (ret == QMessageBox::Cancel) {
      allOk = false;
    }
  }


  if (allOk) {
    params->setName(nameProjEdit->text());
    params->setNbHoles(nbHoles->value());
    params->setBandWidth(nbCards->value());
    params->setNbPatterns(nbPattern->value(),false);
    /* NB : this should be the last thing to do in order
       to have guesser update() called correctly
       */
    params->setPatternLength(patternLength->value());

    accept();
  }
}
