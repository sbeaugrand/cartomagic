#include "MovesLockersView.hpp"

MovesLockersView::MovesLockersView(ProjectParams *_params, MovesModel *_model, QWidget *parent) : QWidget(parent) {
  
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

  lockAllIcon = QImage(":/icons/locked-all-16x16.png").scaled(QSize(lockSize,lockSize));
  unlockAllIcon = QImage(":/icons/unlocked-all-16x16.png").scaled(QSize(lockSize,lockSize));

  setBackgroundRole(QPalette::Base);
  setAutoFillBackground(true);
  setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

  /*
  a lock changed
  */
  connect(model,SIGNAL(lockChanged(int)),this,SLOT(updateLockers(int)));

  connect(model,SIGNAL(allMovesChanged()), this, SLOT(updateView()));

}

void MovesLockersView::updateLayout() {

  moveWidth = params->moveWidth;
  marginUp = 10;
  marginDown = 10;
  marginLeft = 10;
  marginRight = 10;

  gapToFirstLock = marginLeft+5+idWidth;
  gapTotalLockToNumLock = 5;

  lockSize = moveWidth-6;

  totalLockStartX = gapToFirstLock;
  totalLockStartY = marginUp;

  numLockStartX = gapToFirstLock;
  numLockStartY  = marginUp+lockSize+gapTotalLockToNumLock;
  numLockEndY = marginUp+lockSize+gapTotalLockToNumLock+numHeight;

  totalWidth = gapToFirstLock+(params->bandWidth)*moveWidth+marginRight;
  totalHeight = numLockEndY+marginDown;

}


QSize MovesLockersView::minimumSizeHint() const {
  return QSize(totalWidth,totalHeight);
}

QSize MovesLockersView::sizeHint() const {
  return QSize(totalWidth,totalHeight);
}

void MovesLockersView::updateView() {

  // refresh the whole widget
  // need to resize it since it is not in a layout.
  updateLayout();
  resize(sizeHint());
  update();
}

void MovesLockersView::updateLockers(int warpId) {

  update();
}

void MovesLockersView::changeMoveWidth(int width) {

  moveWidth = width;
  update();
}

void MovesLockersView::paintEvent(QPaintEvent *event) {
  
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
    p.drawRect(totalLockStartX+i*moveWidth+2,totalLockStartY,moveWidth-4, moveWidth-4);
    lock = (model->totallyLockedMoves).at(i);
    if (lock) {
      p.drawImage(totalLockStartX+i*moveWidth+(moveWidth/2)-(lockSize/2),totalLockStartY+(moveWidth/2)-(lockSize/2),lockAllIcon);
    }
    else {
      p.drawImage(totalLockStartX+i*moveWidth+(moveWidth/2)-(lockSize/2),totalLockStartY+(moveWidth/2)-(lockSize/2),unlockAllIcon);
    }
  }

  // paint moves numbers
  QFont fontNum("Arial",7);
  p.setFont(fontNum);

  QString warpNumber;
  for(i=0;i<params->bandWidth;i++) {
    warpNumber.setNum(i+1);
    p.drawText(numLockStartX+i*moveWidth+(moveWidth-numWidth)/2,numLockEndY,warpNumber);
  }


}

void MovesLockersView::mousePressEvent(QMouseEvent *event) {

  int x = event->x();
  int y = event->y();

  QPointF p(x,y);
  int warpId;


  if ((x>=totalLockStartX) && (x<(totalLockStartX+moveWidth*(int)(params->bandWidth)))) {

    warpId = (x-totalLockStartX)/moveWidth;
    x = (x-totalLockStartX)%moveWidth;

    // if click on allLockers
    if ((y>=totalLockStartY)&& (y<=totalLockStartY+lockSize)) {
      if ((x>=(moveWidth/2)-(lockSize/2))&& (x<=(moveWidth/2)+(lockSize/2))) {
#ifdef DEBUG_PRINT
        cout << "warp n°" << warpId << " (un)locked" << endl;
#endif
        emit warpTotallyLocked(warpId);
      }
    }        
  }
}
