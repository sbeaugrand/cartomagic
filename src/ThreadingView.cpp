#include "ThreadingView.hpp"

ThreadingView::ThreadingView(ProjectParams *_params, ThreadingModel *_model, QWidget *parent) : QWidget(parent) {
  
  params = _params;  
  model = _model;
  
  QFont fontNum("Arial",7);
  QFontMetrics fmNum(fontNum);
  numWidth = fmNum.width("55");
  numHeight = fmNum.height();

  QFont fontName("Arial",12);
  QFontMetrics fmName(fontName);
  nameWidth = fmName.width("B");
  nameHeight = fmName.height();

  QFont fontSZ("Arial",12);
  QFontMetrics fmSZ(fontSZ);
  zWidth = fmSZ.width("Z");
  zHeight = fmSZ.height();

  updateLayout();

  setBackgroundRole(QPalette::Base);
  setAutoFillBackground(true);
  setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

  connect(model,SIGNAL(threadingChanged(int)),this,SLOT(updateCard(int)));
  connect(model,SIGNAL(allThreadingsChanged()),this,SLOT(updateView()));  
}

void ThreadingView::updateLayout() {

  colorSize = params->threadWidth;
  marginUp = 10;
  marginDown = 10;
  marginLeft = 10;
  marginRight = 10;

  gapLetterToColor = 5;
  gapColorToSZ = 5;
  gapNumToColor = 5;
  gapToFirstColor = marginLeft+nameWidth+gapLetterToColor;

  numStartX = gapToFirstColor;
  numStartY = marginUp+numHeight;

  colorStartX = gapToFirstColor;
  colorStartY = numStartY+gapNumToColor;
  colorEndY = colorStartY+(params->nbHoles)*colorSize;

  letterStartX = marginLeft;
  letterStartY = colorStartY;

  szStartX = gapToFirstColor;
  szStartY = colorEndY+gapColorToSZ+zHeight;

  totalWidth = gapToFirstColor+(params->bandWidth)*colorSize+marginRight;
  totalHeight = szStartY+marginDown;
}

QSize ThreadingView::minimumSizeHint() const {
  return QSize(totalWidth,totalHeight);
}

QSize ThreadingView::sizeHint() const {
  return QSize(totalWidth,totalHeight);
}

void ThreadingView::updateView() {

  updateLayout();
  resize(sizeHint());
  update();
}

void ThreadingView::updateCard(int cardId) {

  update();
}


void ThreadingView::paintEvent(QPaintEvent *event) {
  
  QFont fontNum("Arial",7);
  QFont fontName("Arial",12);
  QFont fontSZ("Arial",12);

  quint8* lstHoles;
  quint8 holeColor;

  QBrush coloredPick(Qt::black);

  quint32 i,j;
  
  QPainter p(this);  
  QPen pen(Qt::black);
  pen.setWidth(2);
  p.setPen(pen);

  // paint holes names
  char firstHole = 'A';
  QString holeName("A");

  if (params->numberingOrder == DESCENDING) {
    firstHole += params->nbHoles-1;
    holeName = firstHole;
  }

  for(i=0;i<(params->nbHoles);i++) {
    p.setFont(fontName);
    p.drawText(letterStartX,letterStartY+(i+1)*colorSize-4,holeName);
    if (params->numberingOrder == ASCENDING) {
      firstHole += 1;
    }
    else {
      firstHole -= 1;
    }
    holeName = firstHole;
  }

  // paint warp numbers
  p.setFont(fontNum);

  QString warpNumber;
  for(i=0;i<params->bandWidth;i++) {
    warpNumber.setNum(i+1);
    p.drawText(numStartX+i*colorSize+(colorSize-numWidth)/2,numStartY,warpNumber);
  }
  // paint colored squares

  for(i=0;i<(params->bandWidth);i++) {

    if (params->createMode == MODE_FROMBAND) {
      if ((params->mount).at(i) == S_MOUNT) {
        lstHoles = (model->cardsInS).at(i);
      }
      else {
        lstHoles = (model->cardsInZ).at(i);
      }
    }
    else {
      lstHoles = (model->cardsForMoveMode).at(i);
    }

    for(j=0;j<(params->nbHoles);j++) {
      // if numberingType is anti-clockwise lstHoles is in the correct order
      if (params->numberingType == ANTICLOCKWISE) {

        // guntram like : anti-clockwise and from A to D numbering
        if (params->numberingOrder == ASCENDING) {
          holeColor = lstHoles[j];
        }
        // semi guntram : anti-clockwise but from D to A numbering
        else {
          holeColor = lstHoles[params->nbHoles-1-j];
        }
      }
      else {
        // not lik guntram but get the same
        if (params->numberingOrder == DESCENDING) {
          holeColor = lstHoles[j];
        }
        else {
          holeColor = lstHoles[params->nbHoles-1-j];
        }
      }
      if (holeColor == 0) {
        p.setBrush(Qt::Dense6Pattern);
      }
      else {        
        coloredPick.setColor((params->palette).at(holeColor-1));
        p.setBrush(coloredPick);
      }
      p.drawRect(colorStartX+i*colorSize,colorStartY+j*colorSize,colorSize,colorSize);
    }   
  }

  p.setFont(fontSZ);
  // paint S or Z mount
  for(i=0;i<(params->bandWidth);i++) {
    if ((params->isWarpComplete).at(i) == true) {
      if ((params->mount).at(i) == S_MOUNT) {
        p.drawText((colorSize-zWidth)/2+szStartX+i*colorSize,szStartY,"S");
      }
      else {
        p.drawText((colorSize-zWidth)/2+szStartX+i*colorSize,szStartY,"Z");
      }
    }
  }
}

void ThreadingView::mousePressEvent(QMouseEvent *event) {

  int cardId,holeId;
  int x = event->x();
  int y = event->y();
  bool rightButton = false;
  if(event->button() == Qt::RightButton) {
    rightButton = true;
  }

  cardId = 0;
  holeId = 0;

  if (params->createMode == MODE_FROMMOVES) {

    if ((x>=colorStartX) && (x<(colorStartX+colorSize*(int)(params->bandWidth))) && (y>=colorStartY) && (y <= colorEndY)) {

      cardId = (x-colorStartX)/colorSize;
      if (params->numberingOrder == ASCENDING) {
        holeId = (y-colorStartY)/colorSize;
      }
      else {
        holeId = (colorEndY-y)/colorSize;
      }

      //cout << "click on hole " << holeId << " of warp " << cardId << endl;

      emit holeClicked(cardId,holeId,rightButton);
    }
  }
}


