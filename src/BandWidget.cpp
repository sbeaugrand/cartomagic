/*-==============================================================-
  
file : BandWidget.cpp

creation date : 30/01/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#include <QHBoxLayout>
#include <QScrollBar>
#include <QColorDialog>
#include "BandWidget.hpp"

BandWidget::BandWidget(ProjectParams *_params, QWidget *parent) : QWidget(parent) {
  
  params = _params;
  
  model = new BandModel(params);
  view = new BandView(params,model);
  lockers = new BandLockersView(params,model);
  selectors = new BandSelectorsView(params,model);
  control = new BandControl(params,model,view,lockers,selectors);
  
  createWidget();
  createConnection();
}

BandWidget::~BandWidget() {
  delete [] lstColors;
  model->deleteLater();
  control->deleteLater();
}

void BandWidget::createWidget() {
  
  int i;

  warnLettering = new QLabel("");
  warnLettering->setPixmap(QPixmap(":/icons/warning.png").scaled(QSize(20,20)));
  warnLettering->setVisible(false);
  labLettering = new QLabel(tr("Letterings for void warp moves may be inaccurate"));
  labLettering->setVisible(false);
  QHBoxLayout *layWarn = new QHBoxLayout();
  layWarn->addWidget(warnLettering);
  layWarn->addWidget(labLettering);
  layWarn->addStretch();
  layWarn->setSizeConstraint(QLayout::SetFixedSize);

  scrollBand = new QScrollArea;
  scrollBand->setBackgroundRole(QPalette::Dark);
  scrollBand->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  scrollBand->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
  QSize s = view->sizeHint();


  if (s.width() < 500) {
    scrollBand->setMinimumWidth(s.width()+25);
  }
  else {
    scrollBand->setMinimumWidth(s.width()/2);
  }
  //scrollBand->setMaximumWidth(s.width()+25);
  scrollBand->setWidget(view);

  scrollLockers = new QScrollArea; 
  scrollLockers->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scrollLockers->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scrollLockers->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Fixed);
  scrollLockers->setFixedHeight(lockers->sizeHint().height()+4);
  scrollLockers->setWidget(lockers);

  scrollSelectors = new QScrollArea;
  scrollSelectors->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scrollSelectors->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scrollSelectors->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Fixed);
  scrollSelectors->setFixedHeight(selectors->sizeHint().height()+4);
  scrollSelectors->setWidget(selectors);

  bandBox = new QGroupBox(tr("Band"));
  QVBoxLayout *layView = new QVBoxLayout();

  layView->addWidget(scrollLockers);
  layView->addWidget(scrollBand);
  layView->addWidget(scrollSelectors);
  layView->addLayout(layWarn);
  bandBox->setLayout(layView);

  modeBox = new QGroupBox(tr("Change picks"));
  QHBoxLayout *layColorMode = new QHBoxLayout();
  QVBoxLayout *layMode = new QVBoxLayout();
  
  radioColor = new QRadioButton(tr("color"));
  radioColor->setChecked(true);
  currentColor = new QLabel("");
  
  QPixmap pxCurCol(QSize(16,16));
  pxCurCol.fill(Qt::white);
  currentColor->setPixmap(pxCurCol);

  QHBoxLayout *layColor = new QHBoxLayout();
  layColor->addWidget(radioColor);
  layColor->addWidget(currentColor);

  radioTwist = new QRadioButton(tr("twist"));
  layMode->addLayout(layColor);
  layMode->addWidget(radioTwist);
  modeBox->setLayout(layMode);
  
  paletteBox = new QGroupBox(tr("Palette"));
  QVBoxLayout *layPal = new QVBoxLayout();
  lstColors = new QPushButton*[16]; // max 16 colors in palette
  for(i=0;i<16;i++) {
    lstColors[i] = new QPushButton("");
    lstColors[i]->setFlat(true);
    
    QPixmap pxCol(QSize(50,16));
    pxCol.fill((params->palette).at(i));
    QIcon icon(pxCol);
    (lstColors[i])->setIcon(icon);
    (lstColors[i])->setIconSize(QSize(50,16));
    layPal->addWidget(lstColors[i]);
  }
  paletteBox->setLayout(layPal);
  
  
  QVBoxLayout *layRight = new QVBoxLayout();
  layRight->addWidget(modeBox);
  layRight->addWidget(paletteBox);
  layRight->addStretch();
  

  QHBoxLayout *layAll = new QHBoxLayout();
  layAll->addWidget(bandBox);
  //layAll->addLayout(layView);
  layAll->addLayout(layRight);
  
  setLayout(layAll);
}

void BandWidget::createConnection() {

  connect(radioColor,SIGNAL(clicked()),this,SLOT(setMode()));
  connect(radioTwist,SIGNAL(clicked()),this,SLOT(setMode()));

  connect(lstColors[0],SIGNAL(clicked()),this,SLOT(chooseColor1()));
  connect(lstColors[1],SIGNAL(clicked()),this,SLOT(chooseColor2()));
  connect(lstColors[2],SIGNAL(clicked()),this,SLOT(chooseColor3()));
  connect(lstColors[3],SIGNAL(clicked()),this,SLOT(chooseColor4()));
  connect(lstColors[4],SIGNAL(clicked()),this,SLOT(chooseColor5()));
  connect(lstColors[5],SIGNAL(clicked()),this,SLOT(chooseColor6()));
  connect(lstColors[6],SIGNAL(clicked()),this,SLOT(chooseColor7()));
  connect(lstColors[7],SIGNAL(clicked()),this,SLOT(chooseColor8()));
  connect(lstColors[8],SIGNAL(clicked()),this,SLOT(chooseColor9()));
  connect(lstColors[9],SIGNAL(clicked()),this,SLOT(chooseColor10()));
  connect(lstColors[10],SIGNAL(clicked()),this,SLOT(chooseColor11()));
  connect(lstColors[11],SIGNAL(clicked()),this,SLOT(chooseColor12()));
  connect(lstColors[12],SIGNAL(clicked()),this,SLOT(chooseColor13()));
  connect(lstColors[13],SIGNAL(clicked()),this,SLOT(chooseColor14()));
  connect(lstColors[14],SIGNAL(clicked()),this,SLOT(chooseColor15()));
  connect(lstColors[15],SIGNAL(clicked()),this,SLOT(chooseColor16()));

  lstColors[0]->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(lstColors[0],SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(changeColor1(QPoint)));
  lstColors[1]->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(lstColors[1],SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(changeColor2(QPoint)));
  lstColors[2]->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(lstColors[2],SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(changeColor3(QPoint)));
  lstColors[3]->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(lstColors[3],SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(changeColor4(QPoint)));
  lstColors[4]->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(lstColors[4],SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(changeColor5(QPoint)));
  lstColors[5]->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(lstColors[5],SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(changeColor6(QPoint)));
  lstColors[6]->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(lstColors[6],SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(changeColor7(QPoint)));
  lstColors[7]->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(lstColors[7],SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(changeColor8(QPoint)));
  lstColors[8]->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(lstColors[8],SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(changeColor9(QPoint)));
  lstColors[9]->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(lstColors[9],SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(changeColor10(QPoint)));
  lstColors[10]->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(lstColors[10],SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(changeColor11(QPoint)));
  lstColors[11]->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(lstColors[11],SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(changeColor12(QPoint)));
  lstColors[12]->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(lstColors[12],SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(changeColor13(QPoint)));
  lstColors[13]->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(lstColors[13],SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(changeColor14(QPoint)));
  lstColors[14]->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(lstColors[14],SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(changeColor15(QPoint)));
  lstColors[15]->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(lstColors[15],SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(changeColor16(QPoint)));  

  connect(params,SIGNAL(bandWidthChanged()),this,SLOT(resizeScrollArea()));

  connect(scrollBand->horizontalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(updateScrollAreas));
}

void BandWidget::updateForNewCreationMode() {
  if (params->createMode == MODE_FROMBAND) {
    scrollSelectors->setVisible(true);
    scrollLockers->setVisible(true);
    connect(scrollBand->horizontalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(updateScrollAreas(int)));
  }
  else {
    disconnect(scrollBand->horizontalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(updateScrollAreas(int)));
    scrollSelectors->setVisible(false);
    scrollLockers->setVisible(false);    
  }
}

void BandWidget::resizeScrollArea() {

  QSize s = view->sizeHint();
  if (s.width() < 500) {
    scrollBand->setMinimumWidth(s.width()+25);
  }
  else {
    scrollBand->setMinimumWidth(s.width()/2);
  }
  //scrollBand->setMaximumWidth(s.width()+25);

}

void BandWidget::updatePalette(int colorId) {
  QPixmap pxCol(QSize(50,16));
  pxCol.fill((params->palette).at(colorId));
  QIcon icon(pxCol);
  (lstColors[colorId])->setIcon(icon);
  (lstColors[colorId])->setIconSize(QSize(50,16));
}

void BandWidget::zoomIn() {
  
}

void BandWidget::zoomOut() {
  
}

void BandWidget::setMode() {

  if (radioColor->isChecked()) {
    control->setMode(MODE_COLOR);
  }
  else {
    control->setMode(MODE_TWIST);
  }
}

void BandWidget::chooseColor(quint8 numCol) {
  radioColor->click();
  QPixmap px(QSize(16,16));
  px.fill((params->palette).at(numCol));
  currentColor->setPixmap(px);
  emit currentColorChanged(numCol);
}


void BandWidget::chooseColor1() {
  chooseColor(0);
}
void BandWidget::chooseColor2() {  
  chooseColor(1);
}
void BandWidget::chooseColor3() {
  chooseColor(2);
}
void BandWidget::chooseColor4() {
  chooseColor(3);
}
void BandWidget::chooseColor5() {
  chooseColor(4);
}
void BandWidget::chooseColor6() {
  chooseColor(5);
}
void BandWidget::chooseColor7() {
  chooseColor(6);
}
void BandWidget::chooseColor8() {
  chooseColor(7);
}
void BandWidget::chooseColor9() {
  chooseColor(8);
}
void BandWidget::chooseColor10() {
  chooseColor(9);
}
void BandWidget::chooseColor11() {
  chooseColor(10);
}
void BandWidget::chooseColor12() {
  chooseColor(11);
}
void BandWidget::chooseColor13() {
  chooseColor(12);
}
void BandWidget::chooseColor14() {
  chooseColor(13);
}
void BandWidget::chooseColor15() {
  chooseColor(14);
}
void BandWidget::chooseColor16() {
  chooseColor(15);
}

void BandWidget::changeColor(quint8 numCol) {

  QColor col = QColorDialog::getColor();

  if (params->currColor == numCol) {
    QPixmap px(QSize(16,16));
    px.fill(col);
    currentColor->setPixmap(px);
  }
  emit paletteChanged(numCol,col);
}
void BandWidget::changeColor1(QPoint p) {
  changeColor(0);
}
void BandWidget::changeColor2(QPoint p) {
  changeColor(1);
}
void BandWidget::changeColor3(QPoint p) {
  changeColor(2);
}
void BandWidget::changeColor4(QPoint p) {
  changeColor(3);
}
void BandWidget::changeColor5(QPoint p) {
  changeColor(4);
}
void BandWidget::changeColor6(QPoint p) {
  changeColor(5);
}
void BandWidget::changeColor7(QPoint p) {
  changeColor(6);
}
void BandWidget::changeColor8(QPoint p) {
  changeColor(7);
}
void BandWidget::changeColor9(QPoint p) {
  changeColor(8);
}
void BandWidget::changeColor10(QPoint p) {
  changeColor(9);
}
void BandWidget::changeColor11(QPoint p) {
  changeColor(10);
}
void BandWidget::changeColor12(QPoint p) {
  changeColor(11);
}
void BandWidget::changeColor13(QPoint p) {
  changeColor(12);
}
void BandWidget::changeColor14(QPoint p) {
  changeColor(13);
}
void BandWidget::changeColor15(QPoint p) {
  changeColor(14);
}
void BandWidget::changeColor16(QPoint p) {
  changeColor(15);
}


void BandWidget::updateScrollAreas(int dx) {

  int maxBand = scrollBand->horizontalScrollBar()->maximum();
  int maxLock = scrollLockers->horizontalScrollBar()->maximum();
  int maxSel = scrollLockers->horizontalScrollBar()->maximum();
  if (maxLock != maxBand) {
    scrollLockers->horizontalScrollBar()->setMaximum(maxBand);
  }
  if (maxSel != maxBand) {
    scrollSelectors->horizontalScrollBar()->setMaximum(maxBand);
  }
  scrollLockers->horizontalScrollBar()->setValue(dx);  
  scrollSelectors->horizontalScrollBar()->setValue(dx);
}
