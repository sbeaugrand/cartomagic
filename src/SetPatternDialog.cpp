/*-==============================================================-

file : SetPatternDialog.cpp

creation date : 08/02/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/

#include "SetPatternDialog.hpp"

SetPatternDialog::SetPatternDialog(ProjectParams *_params, DispatchControl *_control, QList<int> &_selIndex, QWidget *_parent) : BandDialog (tr("Set pick shape/rotation for pattern"),tr("Parameters"),_params,_control,_parent) {

  selIndex = _selIndex;

  QLabel *iconLeft = NULL;
  QLabel *iconRight = NULL;

  if (params->createMode == MODE_FROMBAND) {
    choiceBox = new QGroupBox(tr("Base pick orientation"));
    leftOri = new QRadioButton(tr("left"));
    rightOri = new QRadioButton(tr("right"));
  }
  else if (params->createMode == MODE_FROMMOVES) {
    choiceBox = new QGroupBox(tr("Mount of the first pick"));
    leftOri = new QRadioButton(tr("Z"));
    rightOri = new QRadioButton(tr("S"));
  }
  iconLeft = new QLabel("");
  iconLeft->setPixmap(QPixmap(":/icons/pick-left.png"));    
  iconRight = new QLabel("");
  iconRight->setPixmap(QPixmap(":/icons/pick-right.png"));
  rightOri->setChecked(true);

  QGridLayout *layLR = new QGridLayout;
  layLR->addWidget(rightOri,0,0);
  layLR->addWidget(iconRight,0,1);
  layLR->addWidget(leftOri,1,0);
  layLR->addWidget(iconLeft,1,1);

  choiceBox->setLayout(layLR);

  QGroupBox *classicBox = new QGroupBox(tr("Classic choices"));

  listClassics = new QComboBox();
  if (params->createMode == MODE_FROMBAND) {
    listClassics->addItem(tr("none"));
    listClassics->addItem(tr("first half"));
    listClassics->addItem(tr("second half"));
    listClassics->addItem(tr("half centered"));
    listClassics->addItem(tr("2/4 [with #1,2 not twisted]"));
    listClassics->addItem(tr("2/4 [with #1,2 twisted]"));
  }
  else if (params->createMode == MODE_FROMMOVES) {
    listClassics->addItem(tr("F for all"));
    listClassics->addItem(tr("B for first half"));
    listClassics->addItem(tr("B for second half"));
    listClassics->addItem(tr("B for half centered"));
    listClassics->addItem(tr("2B every 4 [with #1,2 F]"));
    listClassics->addItem(tr("2B every 4 [with #1,2 B]"));
  }

  QHBoxLayout * layClassic = new QHBoxLayout;
  layClassic->addWidget(listClassics);
  classicBox->setLayout(layClassic);

  QHBoxLayout *layUp = new QHBoxLayout;
  layUp->addWidget(choiceBox);
  layUp->addWidget(classicBox);

  quint32 nbRow = 1+ ((params->patternLength -1)/25);
  QGridLayout *layT = new QGridLayout;
  QLabel *numLab = new QLabel(tr("pick #"));
  QLabel *stateLab = NULL;
  if (params->createMode == MODE_FROMBAND) {
    stateLab = new QLabel(tr("twisted ?"));
  }
  else if (params->createMode == MODE_FROMMOVES) {
    stateLab = new QLabel(tr("B ?"));
  }
  layT->addWidget(stateLab,0,0);
  layT->addWidget(numLab,1,0);
  quint32 i,j;
  QCheckBox *c = NULL;
  QLabel *l;
  QString numPick;
  quint32 nbCheckPerRow;
  for(i=0;i<nbRow;i++) {
    if ((i+1) == nbRow) {
      nbCheckPerRow = params->patternLength % 25;
    }
    else {
      nbCheckPerRow = 25;
    }
    for(j=0;j<nbCheckPerRow;j++) {
      c= new QCheckBox();
      lstTwisted.append(c);
      numPick.setNum(25*i+j+1);
      l = new QLabel(numPick);
      layT->addWidget(c,2*i,j+1);
      layT->addWidget(l,2*i+1,j+1);
    }
  }

  QHBoxLayout *layDown = new QHBoxLayout;
  layDown->addLayout(layT);
  layDown->addStretch();

  QVBoxLayout *layBox = new QVBoxLayout;

  layBox->addLayout(layUp);
  layBox->addLayout(layDown);

  setContent(layBox);

  connect(listClassics,SIGNAL(currentIndexChanged(int)),this,SLOT(setClassicTwists(int)));
}


void SetPatternDialog::setClassicTwists(int index) {

  quint32 i;
  if (index == 0) {
    for(i=0;i<params->patternLength;i++) {
      lstTwisted.at(i)->setChecked(false);
    }
  }
  else if (index == 1) {
    for(i=0;i<params->patternLength/2;i++) {
      lstTwisted.at(i)->setChecked(true);
    }
    for(i=params->patternLength/2;i<params->patternLength;i++) {
      lstTwisted.at(i)->setChecked(false);
    }
  }
  else if (index == 2) {
    for(i=0;i<params->patternLength/2;i++) {
      lstTwisted.at(i)->setChecked(false);
    }
    for(i=params->patternLength/2;i<params->patternLength;i++) {
      lstTwisted.at(i)->setChecked(true);
    }
  }
  else if (index == 3) {
    for(i=0;i<params->patternLength;i++) {
      lstTwisted.at(i)->setChecked(false);
    }
    for(i=0;i<params->patternLength/2;i++) {
      lstTwisted.at((params->patternLength /4)+i)->setChecked(true);
    }
  }
  else if (index == 4) {
    for(i=0;i<params->patternLength;i++) {
      if ((i/2)%2 == 0) {
        lstTwisted.at(i)->setChecked(false);
      }
      else {
        lstTwisted.at(i)->setChecked(true);
      }
    }
  }
  else if (index == 5) {
    for(i=0;i<params->patternLength;i++) {
      if ((i/2)%2 == 0) {
        lstTwisted.at(i)->setChecked(true);
      }
      else {
        lstTwisted.at(i)->setChecked(false);
      }
    }
  }
}

void SetPatternDialog::checkBeforeAccept() {

  quint32 i;

  bool *twisted = new bool[params->patternLength];
  for(i=0;i<params->patternLength;i++) {
    if (lstTwisted.at(i)->isChecked()) {
      twisted[i] = true;
    }
    else {
      twisted[i] = false;
    }
  }
  quint8 baseShape = 0;
  if (leftOri->isChecked()) {
    baseShape = PARA_LEFT;
  }
  else {
    baseShape = PARA_RIGHT;
  }

  int nbSel = selIndex.size();
  int index;

  for(i=0;i<nbSel;i++) {
    index = selIndex.at(i);

    control->setPattern(index,baseShape,twisted);
  }

  delete [] twisted;

  accept();
}

