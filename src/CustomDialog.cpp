/*-==============================================================-

file : CustomDialog.cpp

creation date : 08/02/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#include "CustomDialog.hpp"

CustomDialog::CustomDialog(const QString &dialogTitle, const QString &boxTitle, QWidget *_parent) : QDialog(_parent) {

  setWindowTitle(dialogTitle);
  box = new QGroupBox(boxTitle);

  okButton = new QPushButton(tr("OK"));
  cancelButton = new QPushButton(tr("Cancel"));

  layBottom = new QHBoxLayout;
  layBottom->addStretch();
  layBottom->addWidget(okButton);
  layBottom->addWidget(cancelButton);

  connect(okButton,SIGNAL(clicked()),this, SLOT(checkBeforeAccept()));
  connect(cancelButton,SIGNAL(clicked()),this,SLOT(reject()));
}

void CustomDialog::checkBeforeAccept() { 
  accept();
}

bool CustomDialog::confirmAccept() {

  int ret = QMessageBox::question(NULL,tr("Confirmation"),tr("Are you sure ?"),QMessageBox::Ok | QMessageBox::Cancel,QMessageBox::Ok);
  if (ret == QMessageBox::Ok) return true;
  return false;
}

void CustomDialog::setContent(QVBoxLayout *boxLayout) {
  box->setLayout(boxLayout);

  QVBoxLayout *layAll = new QVBoxLayout;

  layAll->addWidget(box);
  layAll->addLayout(layBottom);

  setLayout(layAll);
}
