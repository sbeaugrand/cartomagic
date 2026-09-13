/*-==============================================================-

file : BandDialog.cpp

creation date : 08/02/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/

#include "BandDialog.hpp"

BandDialog::BandDialog(const QString &dialogTitle, const QString &boxTitle, ProjectParams *_params, DispatchControl *_control, QWidget *_parent) : CustomDialog (dialogTitle,boxTitle,_parent) {
  params = _params;
  control = _control;
}
