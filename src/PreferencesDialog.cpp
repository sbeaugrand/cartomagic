/*-==============================================================-

file : PreferencesDialog.cpp

creation date : 08/02/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description :

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#include "PreferencesDialog.hpp"
#include "SetSeparatorDialog.hpp"

PreferencesDialog::PreferencesDialog(ProjectParams *_params, DispatchControl *_control, QWidget *parent) : QDialog(parent) {

  params = _params;
  control = _control;

  setWindowTitle(tr("Options"));
  okButton = new QPushButton(tr("OK"));
  cancelButton = new QPushButton(tr("Cancel"));

  QHBoxLayout *layBottom = new QHBoxLayout;
  layBottom->addStretch();
  layBottom->addWidget(okButton);
  layBottom->addWidget(cancelButton);

  tabs = new QTabWidget();

  // create band options
  optBand = new QWidget();
  pickWidth = new QSpinBox();
  pickWidth->setRange(20,40);
  pickWidth->setValue(params->pickWidth);
  pickBaseLength = new QSpinBox();
  pickBaseLength->setRange(15,40);
  pickBaseLength->setValue(params->pickBaseLength);
  pickTotalLength = new QSpinBox();
  pickTotalLength->setRange(20,80);
  pickTotalLength->setValue(params->pickTotalLength);
  QLabel *labPickWidth = new QLabel(tr("Pick width"));
  QLabel *labPickBaseLength = new QLabel(tr("Pick base length"));
  QLabel *labPickTotalLength = new QLabel(tr("Pick total length"));  

  QVBoxLayout *layC1 = new QVBoxLayout;
  QVBoxLayout *layC2 = new QVBoxLayout;
  QHBoxLayout *layC3 = new QHBoxLayout;
  layC1->addWidget(labPickWidth);
  layC1->addWidget(labPickBaseLength);
  layC1->addWidget(labPickTotalLength);
  layC2->addWidget(pickWidth);
  layC2->addWidget(pickBaseLength);
  layC2->addWidget(pickTotalLength);
  layC3->addStretch();

  QHBoxLayout *layL1 = new QHBoxLayout;
  layL1->addLayout(layC1);
  layL1->addLayout(layC2);
  layL1->addLayout(layC3);
  QGroupBox *boxBand = new QGroupBox(tr("Band"));
  boxBand->setLayout(layL1);

  moveWidth = new QSpinBox();
  moveWidth->setRange(16,40);
  moveWidth->setSingleStep(2);
  moveWidth->setValue(params->moveWidth);
  QLabel *labMoveWidth = new QLabel(tr("Move size"));
  QHBoxLayout *layL2 = new QHBoxLayout;
  layL2->addWidget(labMoveWidth);
  layL2->addWidget(moveWidth);
  layL2->addStretch();
  QGroupBox *boxMoves = new QGroupBox(tr("Rotations"));
  boxMoves->setLayout(layL2);

  threadWidth = new QSpinBox();
  threadWidth->setRange(16,40);
  threadWidth->setSingleStep(2);
  threadWidth->setValue(params->moveWidth);
  QLabel *labthreadWidth = new QLabel(tr("Thread size"));
  QHBoxLayout *layL3 = new QHBoxLayout;
  layL3->addWidget(labthreadWidth);
  layL3->addWidget(threadWidth);
  layL3->addStretch();
  QGroupBox *boxThreads = new QGroupBox(tr("Threads"));
  boxThreads->setLayout(layL3);

  QVBoxLayout *layBand = new QVBoxLayout;
  layBand->addWidget(boxBand);  
  layBand->addWidget(boxMoves);
  layBand->addWidget(boxThreads);

  optBand->setLayout(layBand);

  // create band options
  optPrint = new QWidget();  
  printVertSep = new QCheckBox(tr("Print vertical separators"));
  printVertSep->setChecked(params->printVertSep);
  QHBoxLayout *layL4 = new QHBoxLayout;
  layL4->addWidget(printVertSep);
  layL4->addStretch();

  advSepBut = new QPushButton(tr("Setting separators"));
  QHBoxLayout *layL5 = new QHBoxLayout;
  layL5->addStretch();
  layL5->addWidget(advSepBut);

  QVBoxLayout *layOptPrint = new QVBoxLayout;
  layOptPrint->addLayout(layL4);
  layOptPrint->addLayout(layL5);  
  layOptPrint->addStretch();

  optPrint->setLayout(layOptPrint);

  tabs->addTab(optBand,tr("Visual aspect"));
  tabs->addTab(optPrint,tr("Printing"));

  QVBoxLayout *layAll = new QVBoxLayout;

  layAll->addWidget(tabs);
  layAll->addLayout(layBottom);

  connect(okButton,SIGNAL(clicked()),this,SLOT(checkBeforeAccept()));
  connect(cancelButton,SIGNAL(clicked()),this,SLOT(reject()));
  connect(advSepBut,SIGNAL(clicked()),this,SLOT(setVerticalSepLocation()));

  setLayout(layAll);

}

void PreferencesDialog::setVerticalSepLocation() {

  BandDialog *dialog = new SetSeparatorDialog(params,control);
  dialog->exec();
}

void PreferencesDialog::checkBeforeAccept() {

  bool changePickAppearence = false;
  bool changeMoveAppearence = false;
  bool changeThreadAppearence = false;
  if (params->pickWidth != pickWidth->value()) {
    changePickAppearence = true;
    params->pickWidth = pickWidth->value();
  }
  if (params->pickBaseLength != pickBaseLength->value()) {
    changePickAppearence = true;
    params->pickBaseLength = pickBaseLength->value();
  }
  if (params->pickTotalLength != pickTotalLength->value()) {
    changePickAppearence = true;
    params->pickTotalLength = pickTotalLength->value();
  }
  if (params->moveWidth != moveWidth->value()) {
    changeMoveAppearence = true;
    params->moveWidth = moveWidth->value();
  }
  if (params->threadWidth != threadWidth->value()) {
    changeThreadAppearence = true;
    params->threadWidth = threadWidth->value();
  }
  if (changePickAppearence) {
    control->repaintBand();
  }
  if (changeMoveAppearence) {
    control->repaintMoves();
  }
  if (changeThreadAppearence) {
    control->repaintThreading();
  }  

  if (printVertSep->isChecked()) {
    params->printVertSep = true;
  }
  else {
    params->printVertSep = false;
  }

  accept();
}
