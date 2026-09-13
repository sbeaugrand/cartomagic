#include "BandView.hpp"

BandView::BandView(ProjectParams *_params, BandModel *_model, QWidget *parent) : QWidget(parent) {
  
  params = _params;  
  model = _model;
  
  QFont fontId("Arial",10);
  QFontMetrics fmId(fontId);
  idWidth = fmId.width("55");
  idHeight = fmId.height();

  QFont fontNum("Arial",7);
  QFontMetrics fmNum(fontNum);
  numWidth = fmNum.width("55");
  numHeight = fmNum.height();

  updateLayout();

  noPathes = true;
  updatePathes();

  setBackgroundRole(QPalette::Base);
  setAutoFillBackground(true);
  setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

  /*
  a single warp is touch or slection/lock area changed
  */
  connect(model,SIGNAL(warpChanged(int)),this,SLOT(updateWarp(int)));    
  connect(model,SIGNAL(letteringChanged(int)),this,SLOT(updateLettering(int)));  

  /*
  the whole band was modified for several reasons.
 */
  connect(model,SIGNAL(allWarpsChanged()),this,SLOT(updateView()));
}

BandView::~BandView() {
  clearPathes();
}

void BandView::updateLayout() {

  pickWidth = params->pickWidth;
  pickBaseLength = params->pickBaseLength;
  pickTotalLength = params->pickTotalLength;

  marginUp = 5;
  marginDown = 10;
  marginLeft = 10;
  marginRight = 10;

  gapIdToWarps = 5;
  gapToFirstWarp = marginLeft+idWidth+gapIdToWarps;

  idStartX = marginLeft;
  idEndY = marginUp+(params->bandLength-2)*pickBaseLength+pickTotalLength;

  warpsStartX = gapToFirstWarp;
  warpsStartY = marginUp;
  warpsEndY = marginUp+(params->bandLength-1)*pickBaseLength+pickTotalLength;

  totalWidth = gapToFirstWarp+(params->bandWidth)*pickWidth+marginRight;
  totalHeight = warpsEndY+marginDown;
}

void BandView::clearPathes() {

  QListIterator<QList<QPainterPath> *> iter(pathes);
  QList<QPainterPath> *lstPath;

  while(iter.hasNext()) {
    lstPath = iter.next();
    lstPath->clear();
    delete lstPath;
  }
  pathes.clear();
}

void BandView::updatePathes() {

  if (!noPathes) {
    clearPathes();
  }

  quint32 i;
  QList<QPainterPath> *lstPath;

  for(i=0;i<(params->bandWidth);i++) {

    lstPath = new QList<QPainterPath>();    
    pathes.append(lstPath);
    updatePath(i);   
  }
  noPathes = false;
}

void BandView::updatePath(int warpId) {

  QList<QPainterPath> *lstPath;

  QList<quint16> *lstPick;
  quint16 pick;
  int startLeft = warpsStartX;
  int startDown = warpsEndY;

  int i = warpId;
  quint32 j;

  lstPath = pathes.at(i);
  lstPath->clear();

  lstPick = (model->warps).at(i);

  /* special case for j=0 since it correspond to the first pick
     that figures the beginning of the band
    */
  QPainterPath p0;
  pick = lstPick->at(0);
  if ((PICK_SHAPE(pick) == PARA_RIGHT) || (PICK_SHAPE(pick) == TRI_RIGHT)) {
    p0.moveTo(startLeft+pickWidth*i,startDown);
    p0.lineTo(startLeft+pickWidth*i,startDown-pickBaseLength);
    p0.lineTo(startLeft+pickWidth*(i+1),startDown-pickTotalLength);
    p0.lineTo(startLeft+pickWidth*(i+1),startDown);
    p0.lineTo(startLeft+pickWidth*i,startDown);
    p0.closeSubpath();
  }
  else if ((PICK_SHAPE(pick) == PARA_LEFT) || (PICK_SHAPE(pick) == TRI_LEFT)) {
    p0.moveTo(startLeft+pickWidth*(i+1),startDown);
    p0.lineTo(startLeft+pickWidth*(i+1),startDown-pickBaseLength);
    p0.lineTo(startLeft+pickWidth*i,startDown-pickTotalLength);
    p0.lineTo(startLeft+pickWidth*i,startDown);
    p0.lineTo(startLeft+pickWidth*(i+1),startDown);
    p0.closeSubpath();
  }
  lstPath->append(p0);

  for(j=1;j<(params->bandLength);j++) {

    QPainterPath p;

    pick = lstPick->at(j);

    if (PICK_SHAPE(pick) == PARA_RIGHT) {
      p.moveTo(startLeft+pickWidth*i,startDown-j*pickBaseLength);
      p.lineTo(startLeft+pickWidth*i,startDown-(j+1)*pickBaseLength);
      p.lineTo(startLeft+pickWidth*(i+1),startDown-j*pickBaseLength - pickTotalLength);
      p.lineTo(startLeft+pickWidth*(i+1),startDown-j*pickBaseLength-(pickTotalLength-pickBaseLength));
      p.lineTo(startLeft+pickWidth*i,startDown-j*pickBaseLength);
      p.closeSubpath();
    }
    else if (PICK_SHAPE(pick) == TRI_RIGHT) {
      p.moveTo(startLeft+pickWidth*i,startDown-j*pickBaseLength);
      p.lineTo(startLeft+pickWidth*i,startDown-j*pickBaseLength-pickTotalLength);
      p.lineTo(startLeft+pickWidth*(i+1),startDown-j*pickBaseLength-(pickTotalLength/2));
      p.lineTo(startLeft+pickWidth*i,startDown-j*pickBaseLength);
      p.closeSubpath();
    }
    else if (PICK_SHAPE(pick) == PARA_LEFT) {
      p.moveTo(startLeft+pickWidth*(i+1),startDown-j*pickBaseLength);
      p.lineTo(startLeft+pickWidth*(i+1),startDown-(j+1)*pickBaseLength);
      p.lineTo(startLeft+pickWidth*i,startDown-j*pickBaseLength - pickTotalLength);
      p.lineTo(startLeft+pickWidth*i,startDown-j*pickBaseLength-(pickTotalLength-pickBaseLength));
      p.lineTo(startLeft+pickWidth*(i+1),startDown-j*pickBaseLength);
      p.closeSubpath();
    }
    else if (PICK_SHAPE(pick) == TRI_LEFT) {
      p.moveTo(startLeft+pickWidth*(i+1),startDown-j*pickBaseLength);
      p.lineTo(startLeft+pickWidth*(i+1),startDown-j*pickBaseLength-pickTotalLength);
      p.lineTo(startLeft+pickWidth*i,startDown-j*pickBaseLength-(pickTotalLength/2));
      p.lineTo(startLeft+pickWidth*(i+1),startDown-j*pickBaseLength);
      p.closeSubpath();
    }
    lstPath->append(p);
  }
}

QSize BandView::minimumSizeHint() const {
  return QSize(totalWidth,totalHeight);
}

QSize BandView::sizeHint() const {
  return QSize(totalWidth,totalHeight);
}

void BandView::updateView() {

  params->mustBeSaved = true;
  updateLayout();
  // update all pathes
  updatePathes();
  // refresh the whole widget
  // need to resize it since it is not in a layout.
  resize(sizeHint());

  update();
}

void BandView::updateWarp(int warpId) {

  params->mustBeSaved = true;  
  updatePath(warpId);
  update(warpsStartX-2+warpId*pickWidth,warpsStartY,pickWidth+4,warpsEndY-warpsStartY);
}

void BandView::updateLettering(int warpId) {

  update(warpsStartX-2+warpId*pickWidth,warpsStartY,pickWidth+4,warpsEndY-warpsStartY);
}

void BandView::changePickBaseLength(int length) {

  pickBaseLength = length;
  updateLayout();
  updatePathes();
  update();
}

void BandView::changePickTotalLength(int length) {

  pickTotalLength = length;
  updateLayout();
  updatePathes();
  update();
}

void BandView::changePickWidth(int width) {

  pickWidth = width;
  updateLayout();
  updatePathes();
  update();
}

void BandView::paintEvent(QPaintEvent *event) {
  
  QList<QPainterPath> *lstPath;
  QPainterPath path;
  QList<quint16> *lstPick;
  quint16 pick;

  QBrush coloredPick(Qt::black);

  quint32 i,j;
  
  QPainter p(this);  
  QPen pen(Qt::black);
  pen.setWidth(1);
  p.setPen(pen);

  QString holeTxt;

  // paint pick numbers  
  QFont fontId("Arial",10);
  p.setFont(fontId);

  QString pickNumber;
  int yStartPos = idEndY;
  for(i=0;i<params->nbPatterns;i++) {
    for(j=0;j<params->patternLength;j++) {
      pickNumber.setNum(j+1);
      p.drawText(idStartX,yStartPos-(pickBaseLength-numHeight)/2,pickNumber);
      yStartPos -= pickBaseLength;
    }
  }
  // paint pathes
  for(i=0;i<(params->bandWidth);i++) {
    lstPath = pathes.at(i);
    lstPick = (model->warps).at(i);

    for(j=0;j<(params->bandLength);j++) {

      path = lstPath->at(j);
      pick = lstPick->at(j);
      if (PICK_COLOR(pick) == VOID ) {
        p.setBrush(Qt::Dense6Pattern);
      }
      else {        
        coloredPick.setColor((params->palette).at(PICK_COLOR(pick)-1));
        p.setBrush(coloredPick);
      }
      p.drawPath(path);
    }   
  }

  QFont fontHole("Arial",7);
  QFontMetrics fmHole(fontHole);
  int holeWidth = fmHole.width("A");
  int holeHeight = fmHole.height();
  p.setFont(fontHole);
  char firstHole = 'A';
  QString holeName("A");
  quint16 holeNum;

  // paint hole letter
  for(i=0;i<(params->bandWidth);i++) {

    if (params->isWarpHoleNumber.at(i)) {
      yStartPos = idEndY;
      lstPick = (model->warps).at(i);

      for(j=1;j<(params->bandLength);j++) {

        pick = lstPick->at(j);
        holeNum = PICK_HOLE(pick);
        holeName = firstHole+holeNum;
        p.drawText(warpsStartX+i*pickWidth + (pickWidth-holeWidth)/2,yStartPos-(pickTotalLength-numHeight)/2,holeName);
        yStartPos -= pickBaseLength;
      }
    }
  }
}

void BandView::mousePressEvent(QMouseEvent *event) {

  // take account event only if in FROM_BAND mode

  if (params->createMode == MODE_FROMBAND) {
    int x = event->x();
    int y = event->y();

    bool rightButton = false;
    if(event->button() == Qt::RightButton) {
      rightButton = true;
    }
    QPointF p(x,y);
    int warpId;


    if ((x>=(int)warpsStartX) && (x<(int)(warpsStartX+pickWidth*params->bandWidth))) {

      warpId = (x-warpsStartX)/pickWidth;
      x = (x-warpsStartX)%pickWidth;


      // click on picks
      if ((y>=(int)warpsStartY)&& (y<=(int)warpsEndY)) {
        int pickId = -1;

        if (warpId < (int)(params->bandWidth)) {
          quint32 i=1; // ignore click on pick n° 0
          QList<QPainterPath> *lstPath = pathes.at(warpId);
          QPainterPath path;
          bool found = false;

          while ((i<(params->bandLength)) && (!found)) {
            path = lstPath->at(i);
            if (path.contains(p)) {
              found = true;
              pickId = i;
            }
            else {
              i++;
            }
          }

          if (found) {
            if (!(model->totallyLockedWarps).at(warpId)) {
              emit pickClicked(warpId,pickId,rightButton);
            }
          }
        }
      }
    }
  }
}
