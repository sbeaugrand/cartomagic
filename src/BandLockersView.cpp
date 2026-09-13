#include "BandLockersView.hpp"

BandLockersView::BandLockersView(ProjectParams *_params, BandModel *_model, QWidget *parent) : QWidget(parent) {
  
  params = _params;  
  model = _model;
  
  /* parameters to set the layout */  
  QFont fontId("Arial",10);
  QFontMetrics fmId(fontId);
  idWidth = fmId.width("55");
  idHeight = fmId.height();

  QFont fontNum("Arial",7);
  QFontMetrics fmNum(fontNum);
  numWidth = fmNum.width("55");
  numHeight = fmNum.height();

  updateLayout();

  lockIcon = QImage(":/icons/locked.png").scaled(QSize(lockSize,lockSize));
  unlockIcon = QImage(":/icons/unlocked.png").scaled(QSize(lockSize,lockSize));
  lockAllIcon = QImage(":/icons/locked-all-16x16.png").scaled(QSize(lockSize,lockSize));
  unlockAllIcon = QImage(":/icons/unlocked-all-16x16.png").scaled(QSize(lockSize,lockSize));
  twistIcon = QImage(":/icons/warp-twist.png").scaled(QSize(pickWidth,pickWidth));

  setBackgroundRole(QPalette::Base);
  setAutoFillBackground(true);
  setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

  /*
  a lock changed
  */
  connect(model,SIGNAL(lockChanged(int)),this,SLOT(updateLockers(int)));

  connect(model,SIGNAL(allWarpsChanged()),this,SLOT(updateView()));


}

void BandLockersView::updateLayout() {

  pickWidth = params->pickWidth;
  marginUp = 10;
  marginDown = 10;
  marginLeft = 10;
  marginRight = 10;
  gapToFirstLock = 5+idWidth;
  gapTotalLockToShapeLock = 5;
  gapShapeLockToNumLock = 5;

  lockSize = pickWidth-6;

  totalLockStartX = marginLeft+gapToFirstLock;
  totalLockStartY = marginUp;

  shapeLockStartX = marginLeft+gapToFirstLock;
  shapeLockStartY = marginUp+lockSize+gapTotalLockToShapeLock;

  numLockStartX = marginLeft+gapToFirstLock;
  numLockStartY  = marginUp+lockSize+gapTotalLockToShapeLock+lockSize+gapShapeLockToNumLock;
  numLockEndY = marginUp+lockSize+gapTotalLockToShapeLock+lockSize+gapShapeLockToNumLock+numHeight;

  totalWidth = marginLeft+gapToFirstLock+(params->bandWidth)*pickWidth+marginRight;
  totalHeight = numLockEndY+marginDown;

}

QSize BandLockersView::minimumSizeHint() const {
  return QSize(totalWidth,totalHeight);
}

QSize BandLockersView::sizeHint() const {
  return QSize(totalWidth,totalHeight);
}

void BandLockersView::updateView() {

  // refresh the whole widget
  // need to resize it since it is not in a layout.
  updateLayout();
  resize(sizeHint());
  update();
}

void BandLockersView::updateLockers(int warpId) {

  update();
}

void BandLockersView::changePickWidth(int width) {

  pickWidth = width;
  update();
}

void BandLockersView::paintEvent(QPaintEvent *event) {
  
  quint32 i;
  
  QPainter p(this);  
  QPen pen(Qt::black);
  pen.setWidth(2);
  p.setPen(pen);  

  // paint allLockers
  p.setBrush(Qt::darkGray);
  bool lock;
  for(i=0;i<(params->bandWidth);i++) {
    pen.setWidth(1);
    p.setPen(pen);
    p.drawRect(totalLockStartX+i*pickWidth+2,totalLockStartY,pickWidth-4, pickWidth-4);
    lock = (model->totallyLockedWarps).at(i);
    if (lock) {
      p.drawImage(totalLockStartX+i*pickWidth+(pickWidth/2)-(lockSize/2),totalLockStartY+(pickWidth/2)-(lockSize/2),lockAllIcon);
    }
    else {
      p.drawImage(totalLockStartX+i*pickWidth+(pickWidth/2)-(lockSize/2),totalLockStartY+(pickWidth/2)-(lockSize/2),unlockAllIcon);
    }
  }

  // paint shapeLockers
  p.drawImage(shapeLockStartX-pickWidth,shapeLockStartY,twistIcon);
  p.setBrush(Qt::darkGray);  
  for(i=0;i<(params->bandWidth);i++) {
    pen.setWidth(1);
    p.setPen(pen);
    p.drawRect(shapeLockStartX+i*pickWidth+2,shapeLockStartY,pickWidth-4, pickWidth-4);
    lock = (model->shapeLockedWarps).at(i);
    if (lock) {
      p.drawImage(shapeLockStartX+i*pickWidth+(pickWidth/2)-(lockSize/2),shapeLockStartY+(pickWidth/2)-(lockSize/2),lockIcon);
    }
    else {
      p.drawImage(shapeLockStartX+i*pickWidth+(pickWidth/2)-(lockSize/2),shapeLockStartY+(pickWidth/2)-(lockSize/2),unlockIcon);
    }
  }
  // paint warp numbers
  QFont fontNum("Arial",7);
  p.setFont(fontNum);

  QString warpNumber;
  for(i=0;i<params->bandWidth;i++) {
    warpNumber.setNum(i+1);
    p.drawText(numLockStartX+i*pickWidth+(pickWidth-numWidth)/2,numLockEndY,warpNumber);
  }


}

void BandLockersView::mousePressEvent(QMouseEvent *event) {

  int x = event->x();
  int y = event->y();

  QPointF p(x,y);
  int warpId;


  if ((x>=(int)totalLockStartX) && (x<(int)(totalLockStartX+pickWidth*params->bandWidth))) {

    warpId = (x-totalLockStartX)/pickWidth;
    x = (x-totalLockStartX)%pickWidth;

    // if click on allLockers
    if ((y>=totalLockStartY)&& (y<=totalLockStartY+lockSize)) {
      if ((x>=(pickWidth/2)-(lockSize/2))&& (x<=(pickWidth/2)+(lockSize/2))) {
#ifdef DEBUG_PRINT
        cout << "warp n°" << warpId << " (un)locked" << endl;
#endif
        emit warpTotallyLocked(warpId);
      }
    }
    // if click on shapeLockers
    else if ((y>=shapeLockStartY)&& (y<=shapeLockStartY+lockSize)) {
      if ((x>=(pickWidth/2)-(lockSize/2))&& (x<=(pickWidth/2)+(lockSize/2))) {
#ifdef DEBUG_PRINT
        cout << "warp n°" << warpId << " (un)locked" << endl;
#endif
        emit warpShapeLocked(warpId);
      }
    }
  }
}
