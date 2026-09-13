#include "MovesSZView.hpp"

MovesSZView::MovesSZView(ProjectParams *_params, MovesModel *_model, QWidget *parent) : QWidget(parent) {
  
  params = _params;  
  model = _model;
  
  /* parameters to set the layout */

  QFont fontId("Arial",10);
  QFontMetrics fmId(fontId);
  idWidth = fmId.width("55");
  idHeight = fmId.height();

  QFont fontSZ("Arial",12);
  QFontMetrics fmSZ(fontSZ);
  szWidth = fmSZ.width("Z");
  szHeight = fmSZ.height();

  updateLayout();
  setBackgroundRole(QPalette::Base);
  setAutoFillBackground(true);
  setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

  cycleIcon = QImage(":/icons/pattern-cyclic.png").scaled(QSize(cycleSize,cycleSize));
  uncycleIcon = QImage(":/icons/pattern-uncyclic.png").scaled(QSize(cycleSize,cycleSize));

  connect(model,SIGNAL(moveChanged(int)),this,SLOT(updateMove(int)));

  connect(model,SIGNAL(allMovesChanged()),this,SLOT(updateView()));
}

void MovesSZView::updateLayout() {

  moveSize = params->moveWidth; // should be set to the same value than in MovesView
  marginUp  = 10;
  marginDown  = 10;
  marginLeft = 10;
  marginRight = 10;  

  gapToFirstSel = marginLeft+idWidth + 5; /* vertical gap in pixel between the left border and the first selector
                        it must be in adequation of the MovesView */

  gapSZToCycle = 5;

  cycleSize = moveSize -4;

  szStartX = gapToFirstSel;
  szStartY = marginUp+szHeight;

  cycleStartX = gapToFirstSel;
  cycleStartY = szStartY+gapSZToCycle;

  totalWidth = gapToFirstSel+(params->bandWidth)*moveSize+marginRight;
  totalHeight = cycleStartY+cycleSize+marginDown;
}

QSize MovesSZView::minimumSizeHint() const {
  return QSize(totalWidth,totalHeight);
}

QSize MovesSZView::sizeHint() const {
  return QSize(totalWidth,totalHeight);
}

void MovesSZView::updateView() {

  updateLayout();
  resize(sizeHint());
  update();
}

void MovesSZView::updateMove(int warpId) {

  update();
}

void MovesSZView::updateCyclic(int warpId) {

  update();
}

void MovesSZView::paintEvent(QPaintEvent *event) {
    
  QFont fontSZ("Arial",12);

  quint32 i;
  
  QPainter p(this);  
  QPen pen(Qt::black);
  pen.setWidth(1);
  p.setPen(pen);

  // paint S or Z mount
  p.setFont(fontSZ);

  for(i=0;i<(params->bandWidth);i++) {

    if ((params->createMode == MODE_FROMMOVES) || ( (params->createMode == MODE_FROMBAND) && (model->isVoidMove(i) == false) ) ) {
      if ((params->mount).at(i) == S_MOUNT) {
        p.drawText((moveSize-szWidth)/2+szStartX+i*moveSize,szStartY,"S");
      }
      else {
        p.drawText((moveSize-szWidth)/2+szStartX+i*moveSize,szStartY,"Z");
      }
    }
  }

  // paint cyclic or not cyclic
  if ((params->allowNoCyclic) && (params->createMode == MODE_FROMBAND)) {
    p.setBrush(Qt::white);

    pen.setWidth(1);
    p.setPen(pen);
    for(i=0;i<(params->bandWidth);i++) {

      if ((params->isWarpComplete).at(i) == true) {
        p.drawRect(cycleStartX+i*moveSize+1,cycleStartY,cycleSize+2, cycleSize+2);
        if ((params->cyclicCardPosition).at(i) == CYCLIC) {
          p.drawImage(cycleStartX+i*moveSize+((moveSize-cycleSize)/2),cycleStartY+((moveSize-cycleSize)/2),cycleIcon);
        }
        else {
          p.drawImage(cycleStartX+i*moveSize+((moveSize-cycleSize)/2),cycleStartY+((moveSize-cycleSize)/2),uncycleIcon);
        }
      }
    }
  }
}

void MovesSZView::mousePressEvent(QMouseEvent *event) {

  int x = event->x();
  int y = event->y();
  QPointF p(x,y);

  int warpId = (x-szStartX)/moveSize;
  x = (x-szStartX)%moveSize;

  if ((y>=(szStartY-szHeight))&&(y<=szStartY)) {
    if ( (params->createMode == MODE_FROMMOVES) || ((params->createMode == MODE_FROMBAND) && (model->isVoidMove(warpId) == false)) ) {
      emit mountClicked(warpId);
    }    
  }
  if ((y>=(cycleStartY))&&(y<=(cycleStartY+cycleSize+2))) {
    if ((params->createMode == MODE_FROMBAND) && (params->allowNoCyclic) && ((params->isWarpComplete).at(warpId) == true)) {
      emit cycleClicked(warpId);
    }
  }
}


