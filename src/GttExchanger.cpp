#include "GttExchanger.hpp"

GttExchanger::GttExchanger(ProjectParams *_params) : QObject() {

  params = _params;
  errors = 0;
  warnings = 0;
}

void GttExchanger::clearCards() {
  int i;
  quint8 *tab;
  for(i=0;i<bandWidth;i++) {
    tab = cards.at(i);
    delete [] tab;
  }
  cards.clear();
}

void GttExchanger::clearPicks() {
  int i;
  quint8 *tab;
  for(i=0;i<bandWidth;i++) {
    tab = picks.at(i);
    delete [] tab;
  }
  picks.clear();
}


void GttExchanger::clearWarps() {  

  /* beware : just clear the upper list and not the inner lists
     sinces they are used by the rest of application */

  warps.clear();
}

void GttExchanger::clearPacks() {

  QHashIterator<QString,QList<quint8> *> iter(packs);
  QList<quint8> *lst;
  while (iter.hasNext()) {
    iter.next();
    lst = iter.value();
    lst->clear();;
    delete lst;
  }
  packs.clear();

}

QString GttExchanger::getErrorString() {

  if(errors & ERR_GTTFILE_NOOPEN) {
    return tr("Cannot open file. Check if it exists or if you have sufficient permissions.");
  }
  else if (errors & ERR_GTTFILE_CORRUPTED) {
    return tr("The file cannot be analyzed because its structure is not correct. It may be corrupted.");
  }
  else if (errors & ERR_GTTFILE_BADTYPE) {
    return tr("The file does not contain a threaded-in pattern.");
  }
  else if (errors & ERR_GTTFILE_BADFORMAT) {
    return tr("An element in the file has an incorrect value. Try to load then save it in the last version of GTT.");
  }
  else if (errors & ERR_GTTFILE_LIMNBHOLES) {
    return tr("The pattern uses less than 3 or more than 6 holes per cards. Cartomagic cannot deal with such a case.");
  }
  else if (errors & ERR_GTTFILE_DIFFHOLES) {
    return tr("Some cards have different numbers of holes. Cartomagic cannot deal with such a case.");
  }

  return tr("Undefined error");
}

QString GttExchanger::getWarningString() {

  if(warnings & WARN_GTTFILE_ANGLOSAXON) {
    return tr("The pattern seems to be in anglo-saxon style. The pattern length has been set up to %1 but the result may be not accurate.").arg(patternLength);
  }
  else if(warnings & WARN_GTTFILE_PATTERNLENGTH) {
    return tr("Impossible to find the pattern length. It has been set up to %1 but the result may be not accurate. Try to load the pattern in GTT and correct it to have the same sequence twice.").arg(patternLength);
  }
  return tr("Undefined warning");
}


void GttExchanger::setWarpsShape() {

  int i,j;
  quint8 pick = 0;
  quint8 currPickWarp = 0;
  quint8 lastPickWarp = 0;
  quint8 *lst;
  QList<quint16> *lstPick;
  int id;
  // set pick n°0 and n°1
  for(i=0;i<bandWidth;i++) {
    lst = picks.at(i);
    lstPick = warps.at(i);
    lstPick->clear();
    pick = lst[0];
    if ((pick == SF) || (pick == SI) || (pick == STI) || (pick == ZB) || (pick == STB) || (pick == ZTF)) {
      lstPick->append((PARA_RIGHT << SHAPE_STEP)); // for pick 0
      lstPick->append((PARA_RIGHT << SHAPE_STEP)); // for pick 1
    }
    else if ((pick == ZF) || (pick == ZI) || (pick == ZTI) || (pick == SB) || (pick == ZTB) || (pick == STF)) {
      lstPick->append((PARA_LEFT << SHAPE_STEP)); // for pick 0
      lstPick->append((PARA_LEFT << SHAPE_STEP)); // for pick 1
    }
  }

  for(i=2;i<bandLength;i++) {
    id = (i-1)%patternLength;
    for(j=0;j<bandWidth;j++) {
      lst = picks.at(j);
      lstPick = warps.at(j);
      pick = lst[id];
      lastPickWarp = lstPick->at(i-1);
      if (PICK_MOVE(pick) == IDLE) {
        currPickWarp = lastPickWarp;
      }
      else  if ((pick == SF) || (pick == ZB) || (pick == STB) || (pick == ZTF)) {
        currPickWarp = (PARA_RIGHT << SHAPE_STEP);
      }
      else if ((pick == ZF) || (pick == SB) || (pick == ZTB) || (pick == STF)) {
        currPickWarp = (PARA_LEFT << SHAPE_STEP);
      }

      if (currPickWarp != lastPickWarp) {
        if (lastPickWarp == (PARA_RIGHT << SHAPE_STEP)) {
          lstPick->replace(i-1,(TRI_RIGHT << SHAPE_STEP));
        }
        else if (lastPickWarp == (PARA_LEFT << SHAPE_STEP)) {
          lstPick->replace(i-1,(TRI_LEFT << SHAPE_STEP));
        }
#ifdef DEBUG
        else {
          cerr << "setWarpsShape : invalid last pick shape" << endl;
        }
#endif
      }
      lstPick->append(currPickWarp);
    }
  }
}

void GttExchanger::printShapes() {
  int i,j;
  QList<quint16> *lst;
  quint8 move;
  quint8 color;

  for(i=0;i<bandWidth;i++) {
    lst = warps.at(i);
    cout << "warp n°" << i << " : ";
    for(j=0;j<bandLength;j++) {
      move = PICK_SHAPE(lst->at(j));
      color = PICK_COLOR(lst->at(j));
      if (move == PARA_LEFT) {
        cout << "PL";
      }
      else if (move == PARA_RIGHT) {
        cout << "PR";
      }
      else if (move == TRI_LEFT) {
        cout << "TL";
      }
      else if (move == TRI_RIGHT) {
        cout << "TR";
      }
      cout << " [" << (int)color << "],";

    }
    cout << endl;
  }
}

void GttExchanger::setWarpsColorAndLettering() {

  int i,j;
  quint16 pickWarp;
  quint8 move;
  int id;
  quint8 frontHole = 0;
  quint8 backHole = nbHoles-1;
  quint8 lastWovenHole = 0;
  quint8 wovenHole = 0;

  bool isTwisted = false;
  quint8 * colors;
  quint8 firstMount = S_MOUNT;

  QList<quint16> *lstPick;
  quint8 *lst;

  for(i=0;i<bandWidth;i++) {
    isTwisted = false;
    lstPick = warps.at(i);
    lst = picks.at(i);
    colors = cards.at(i);
    frontHole = 0;
    backHole = nbHoles-1;
    lastWovenHole = 0;

    for(j=1;j<bandLength;j++) {
      id = (j-1)%patternLength;
      pickWarp = lstPick->at(j);

      move = lst[id];
      if (j==1) {
        firstMount = PICK_MOUNT(move);
      }

#ifdef DEBUG_PRINT
      cout << "[" << (int)frontHole << "," << (int)backHole << "]->";

      if (PICK_MOUNT(move) == S_MOUNT) cout << "S";
      else cout << "Z";
      if (PICK_TWIST(move) == TWIST) cout << "T";
      if (PICK_MOVE(move) == FORWARD) cout << "F->";
      else if (PICK_MOVE(move) == BACKWARD) cout << "B->";
      else cout << "I->";
#endif

      if (PICK_TWIST(move) == NO_TWIST) {

        // if counter-clockwise numbering
        if (!isTwisted) {
          if (PICK_MOVE(move) == BACKWARD) {
            frontHole = (frontHole+1)%nbHoles;
            backHole = (backHole+1)%nbHoles;
          }
          else if (PICK_MOVE(move) == FORWARD) {
            frontHole = (frontHole-1+nbHoles)%nbHoles;
            backHole = (backHole-1+nbHoles)%nbHoles;
          }
        }
        // else if clockwise numbering
        else if (isTwisted) {
          if (PICK_MOVE(move) == FORWARD) {
            frontHole = (frontHole+1)%nbHoles;
            backHole = (backHole+1)%nbHoles;
          }
          else if (PICK_MOVE(move) == BACKWARD) {
            frontHole = (frontHole-1+nbHoles)%nbHoles;
            backHole = (backHole-1+nbHoles)%nbHoles;
          }
        }
      }
      // else a twist on current pick
      else if (PICK_TWIST(move) == TWIST) {

        // if counter-clockwise numbering
        if (!isTwisted) {
          if (PICK_MOVE(move) == BACKWARD) {
            frontHole = (backHole-1+nbHoles)%nbHoles;
            //backHole = backHole;
          }
          else if (PICK_MOVE(move) == FORWARD) {
            //frontHole = frontHole;
            backHole = (frontHole+1)%nbHoles;
          }
          isTwisted = true;
        }
        // else if clockwise numbering
        else if (isTwisted) {
          if (PICK_MOVE(move) == BACKWARD) {
            frontHole = (backHole+1)%nbHoles;
            //backHole = backHole;
          }
          else if (PICK_MOVE(move) == FORWARD) {
            //frontHole = frontHole;
            backHole = (frontHole-1+nbHoles)%nbHoles;
          }
          isTwisted = false;
        }
      }

      if (PICK_MOVE(move) == BACKWARD) {
        wovenHole = backHole;
        lastWovenHole = backHole;
      }
      else if (PICK_MOVE(move) == FORWARD) {
        wovenHole = frontHole;
        lastWovenHole = frontHole;
      }
      else if (PICK_MOVE(move) == IDLE) {
        wovenHole =  lastWovenHole;
      }
#ifdef DEBUG_PRINT
      cout << "[" << (int)frontHole << "," << (int)backHole << "] :" << "WH=" << (int)wovenHole << ",WC=" << (int)(colors[wovenHole]) << " | ";
#endif
      if (firstMount == S_MOUNT) {
        pickWarp = pickWarp | colors[wovenHole] | (wovenHole << HOLE_STEP);
      }
      else {
        /* NOTE : since cartomagic will immediately guess the moves/threading for S mount, the hole
           letters for Z mount in GTT must be swapped in order to compute them as if they where in S
           */
        pickWarp = pickWarp | colors[wovenHole] | ((nbHoles - 1 - wovenHole) << HOLE_STEP);
      }
      lstPick->replace(j,pickWarp);
    }
#ifdef DEBUG_PRINT
	cout << endl;
#endif
  }
}


bool GttExchanger::weftsAreEqual(int pickId1, int pickId2) {
#ifdef DEBUG_PRINT
  cout << "testing weft " << pickId1 << " and " << pickId2 << endl;
#endif
  quint8 *lst;
  int i;
  for(i=0;i<bandWidth;i++) {
    lst = picks.at(i);
    if (lst[pickId1] != lst[pickId2]) return false;
  }
  return true;
}

int GttExchanger::guessPatternLength() {

  int i,j,k;
  bool good = true;
  /* try to guess from a pattern length of nbHoles
     since thie is the minimal
     */
  for(i=nbHoles;i<=bandLength/2;i++) {

    j = i;
    good = true;
    while (((j+i) <= bandLength) && (good)) {
      for(k=0;(k<i)&&(good);k++) {
        // test if weft j+k is the same than weft k
        good = weftsAreEqual(k,j+k);
      }
      if (good) {
        j += i;
      }
    }
    if (good) {
      return i;
    }
  }
  warnings = warnings | WARN_GTTFILE_PATTERNLENGTH;
  return bandLength;
}

bool GttExchanger::guessIfAngloSaxon() {

  // just have to test if there idle every two warps
  bool isIdles1 = true;
  bool isIdles2 = true;

  int i;
  quint8 *lst;

  // test for idle on even warps of pick n°0
  for(i=0;(i<bandWidth)&&(isIdles1 == true);i+=2) {
    lst = picks.at(i);
    if (PICK_MOVE(lst[0]) != IDLE) isIdles1 = false;
  }
  // test for idle on odd warps of pick n°0
  for(i=1;(i<bandWidth)&&(isIdles2 == true);i+=2) {
    lst = picks.at(i);
    if (PICK_MOVE(lst[0]) != IDLE) isIdles2 = false;
  }
  if ((!isIdles1) && (!isIdles2)) {
#ifdef DEBUG_PRINT
    cout << "no idle on even/odd warps of picks n°0" << endl;
#endif
    return false;
  }
  int start;
  if (isIdles1) {
    start = 1;
  }
  else {
    start = 0;
  }

  isIdles1 = true;
  // test for idle on even warps of pick n°0
  for(i=start;(i<bandWidth)&&(isIdles1 == true);i+=2) {
    lst = picks.at(i);
    if (PICK_MOVE(lst[1]) != IDLE) isIdles1 = false;
  }
  if (!isIdles1) {
#ifdef DEBUG_PRINT
    cout << "no idle on even/odd warps of picks n°1" << endl;
#endif
    return false;
  }
  return true;
}

int GttExchanger::setPalette() {

  QDomNode node;
  QDomElement element;
  QDomElement actionList;
  QString txt;
  int nbColor;
  int id;
  int rgb,r,g,b;
  int i;
  bool ok;

  palette.clear();
  for(i=0;i<PALETTE_SIZE;i++) {
    palette.append(Qt::white);
  }
  node = paletteList.at(0);
  element = node.toElement();
  txt = element.attribute("Size","UNDEFINED");
  if (txt == "UNDEFINED") {
    errors = errors | ERR_GTTFILE_BADFORMAT;
    return ERR_GTTFILE;
  }
  nbColor = txt.toInt(&ok);
  if (!ok) {
    errors = errors | ERR_GTTFILE_BADFORMAT;
    return ERR_GTTFILE;
  }
  node = element.firstChild();
  for(i=0;i<nbColor;i++) {
    element = node.toElement();
    txt = element.attribute("Index","UNDEFINED");
    if (txt == "UNDEFINED") {
      errors = errors | ERR_GTTFILE_BADFORMAT;
      return ERR_GTTFILE;
    }
    id = txt.toInt(&ok);
    if ((!ok) || (id <1) || (id > nbColor)) {
      errors = errors | ERR_GTTFILE_BADFORMAT;
      return ERR_GTTFILE;
    }
    id -= 1;
    txt = node.firstChild().toText().data();
    rgb = txt.toInt(&ok);
    if ((ok) && (id < PALETTE_SIZE)) {
      r = rgb & 255;
      g = (rgb >> 8) & 255;
      b = (rgb >> 16) & 255;
      palette.replace(id,QColor(r,g,b));
    }
    node = node.nextSibling();
  }

  return NOERROR;
}

int GttExchanger::setPicks() {

  int ret;
  QDomNode node;
  QDomElement element,actElt;
  QDomElement actionList;
  QString txt;
  QString typeStr;
  QString targetStr;
  QString targetIdStr;
  int cardId;  
  QString dirStr;
  QString distStr;
  int pickId;
  quint8 *lst;
  int i,j,k;
  bool ok;
  QDomNode action;
  int nbActions;
  QList<quint8> *lstCards;

  for(i=0;i<bandLength;i++) {
    node = pickList.at(i);
    element = node.toElement();
    txt = element.attribute("Index","UNDEFINED");
    if (txt == "UNDEFINED") {
      errors = errors | ERR_GTTFILE_BADFORMAT;
      return ERR_GTTFILE;
    }

    pickId = txt.toInt(&ok);
    if ((!ok) || (pickId < 0) || (pickId > bandLength-1)) {
      errors = errors | ERR_GTTFILE_BADFORMAT;
      return ERR_GTTFILE;
    }

    actionList = element.firstChildElement("Actions");
    if (actionList.isNull()) return ERR_GTTFILE_BADFORMAT;
    txt = actionList.attribute("Count","UNDEFINED");
    if (txt == "UNDEFINED") {
      errors = errors | ERR_GTTFILE_BADFORMAT;
      return ERR_GTTFILE;
    }
    nbActions = txt.toInt(&ok);
    // CAUTION : there may be more than bandWidth actions since there may be twist action !
    if ((!ok) || (nbActions < 1) ) {
      errors = errors | ERR_GTTFILE_BADFORMAT;
      return ERR_GTTFILE;
    }

#ifdef DEBUG_PRINT    
    cout << "pick n°" << pickId << " : " << nbActions << " actions" << endl;
#endif
    action = actionList.firstChild();

    for(j=0;j<nbActions;j++) {

      actElt = action.toElement();
      if (actElt.isNull()) {
        errors = errors | ERR_GTTFILE_BADFORMAT;
        return ERR_GTTFILE;
      }
      typeStr = actElt.attribute("Type","UNDEFINED");
      if (typeStr == "UNDEFINED") {
        errors = errors | ERR_GTTFILE_BADFORMAT;
        return ERR_GTTFILE;
      }
      targetStr = actElt.attribute("Target","UNDEFINED");
      if (targetStr == "UNDEFINED") {
        errors = errors | ERR_GTTFILE_BADFORMAT;
        return ERR_GTTFILE;
      }
      targetIdStr = actElt.attribute("TargetID","UNDEFINED");
      if (targetIdStr == "UNDEFINED") {
        errors = errors | ERR_GTTFILE_BADFORMAT;
        return ERR_GTTFILE;
      }
      dirStr = actElt.attribute("Dir","UNDEFINED");
      if (dirStr == "UNDEFINED") {
        errors = errors | ERR_GTTFILE_BADFORMAT;
        return ERR_GTTFILE;
      }
      if (typeStr == "Turn") {
        distStr = actElt.attribute("Dist","UNDEFINED");
        if (distStr == "UNDEFINED") {
          errors = errors | ERR_GTTFILE_BADFORMAT;
          return ERR_GTTFILE;
        }
      }

      // first analyse card or pack mouvements
      if (typeStr == "Turn") {

        if (targetStr == "Card") {
          cardId = targetIdStr.toInt(&ok);
          if ((!ok) || (cardId < 1) || (cardId > bandWidth)) {
            errors = errors | ERR_GTTFILE_BADFORMAT;
            return ERR_GTTFILE;
          }
          cardId -=1;
          ret = setCardPick(cardId,pickId,dirStr);
          if (ret != NOERROR) return ret;
        }
        else if (targetStr == "Pack") {
          if (!packs.contains(targetIdStr)) {
            errors = errors | ERR_GTTFILE_BADFORMAT;
            return ERR_GTTFILE;
          }
          lstCards = packs.value(targetIdStr);
#ifdef DEBUG_PRINT
          cout << "action on pack " << qPrintable(targetIdStr) << endl;
#endif

          for(k=0;k<lstCards->size();k++) {
            cardId = lstCards->at(k) -1;

            ret = setCardPick(cardId,pickId,dirStr);
            if (ret != NOERROR) return ret;
          }
        }
        else {
          errors = errors | ERR_GTTFILE_BADFORMAT;
          return ERR_GTTFILE;
        }
      }
      else if (typeStr == "Twist") {

        if (targetStr == "Card") {
          cardId = targetIdStr.toInt(&ok);
          if ((!ok) || (cardId < 1) || (cardId > bandWidth)) {
            errors = errors | ERR_GTTFILE_BADFORMAT;
            return ERR_GTTFILE;
          }
          cardId -=1;
          lst = picks.at(cardId);
          lst[pickId] = lst[pickId] | (TWIST << 1);
        }
        else if (targetStr == "Pack") {
          if (!packs.contains(targetIdStr)) {
            errors = errors | ERR_GTTFILE_BADFORMAT;
            return ERR_GTTFILE;
          }
          lstCards = packs.value(targetIdStr);

          for(k=0;k<lstCards->size();k++) {
            cardId = lstCards->at(k) -1;
            lst = picks.at(cardId);
            lst[pickId] = lst[pickId] | (TWIST << 1);
          }
        }
        else {
          errors = errors | ERR_GTTFILE_BADFORMAT;
          return ERR_GTTFILE;
        }
      }

      action = action.nextSibling();
    }
  }

  return NOERROR;
}

int GttExchanger::setCardPick(int cardId, int pickId, const QString &move) {

  quint8 * lst;
  quint8 pick;
  quint8 twist;
  lst = picks.at(cardId);
  pick = lst[pickId];
  if (PICK_TWIST(pick) == TWIST) {
    twist = (TWIST << 1);
  }
  else {
    twist = (NO_TWIST << 1);
  }

  if (move == "F") {
    if (mountSZ.at(cardId) == S_MOUNT) {
      lst[pickId] = SF | twist;
    }
    else {
      lst[pickId] = ZF | twist;
    }
  }
  else if (move == "B") {
    if (mountSZ.at(cardId) == S_MOUNT) {
      lst[pickId] = SB | twist;
    }
    else {
      lst[pickId] = ZB | twist;
    }
  }
  else if (move == "I") {
    if (mountSZ.at(cardId) == S_MOUNT) {
      lst[pickId] = SI | twist;
    }
    else {
      lst[pickId] = ZI | twist;
    }
  }
  else {
    errors = errors | ERR_GTTFILE_BADFORMAT;
    return ERR_GTTFILE;
  }

  return NOERROR;
}

void GttExchanger::updateTwist() {
  quint8* lst;
  quint8 pick;
  bool isTwisted = false;

  int i,j;
  for(i=0;i<bandWidth;i++) {
    lst = picks.at(i);
    isTwisted = false;

    for(j=0; j<bandLength;j++) {
      pick = lst[j];
      // since twist is done before turning, check twist before
      if (PICK_TWIST(pick) == TWIST) {
        isTwisted = !isTwisted;
      }
      if (isTwisted) {
        // change current pick mount
        pick = pick ^ 1;
		lst[j] = pick;
      }
    }
  }
}

int GttExchanger::setPacks() {

  QDomNode node;
  QDomElement element;
  QDomElement idList;
  QString txt;
  QString idStr;
  quint8 id;
  QList<quint8> *lst;
  int i,j;
  bool ok;

  for(i=0;i<packList.size();i++) {
    node = packList.at(i);
    element = node.toElement();
    if (element.isNull()) {
      errors = errors | ERR_GTTFILE_BADFORMAT;
      return ERR_GTTFILE;
    }
    txt = element.attribute("Name","UNDEFINED");
    if (txt == "UNDEFINED") {
      errors = errors | ERR_GTTFILE_BADFORMAT;
      return ERR_GTTFILE;
    }
    lst = new QList<quint8>;
    packs.insert(txt,lst);

    idList = element.firstChildElement("Cards");
    if (idList.isNull()) {
      errors = errors | ERR_GTTFILE_BADFORMAT;
      return ERR_GTTFILE;
    }
    txt = idList.firstChild().toText().data();
    if (txt != "") {
      QStringList ids = txt.split(",");
      for(j=0;j<ids.size();j++) {
        idStr = ids.at(j);
        id = idStr.toInt(&ok);
        if ((!ok) || (id < 1)) {
          errors = errors | ERR_GTTFILE_BADFORMAT;
          return ERR_GTTFILE;
        }
        // NB : it is possible that some packs are not well defined : cards ids > bandwidth
        if (id <= bandWidth) {
          lst->append(id);
        }
      }
    }
  }

  return NOERROR;
}

int GttExchanger::setCards() {

  QDomNode node;
  QDomElement element;
  QDomElement holes;
  QDomElement mount;
  QDomNode colorNode;
  quint8 *cardCol;
  QString txt;
  int idColor;
  int numCard;
  bool ok;

  int i,j;

  for(i=0;i<bandWidth;i++) {
    node = cardList.at(i);
    element = node.toElement();
    if (element.isNull()) {
      errors = errors | ERR_GTTFILE_BADFORMAT;
      return ERR_GTTFILE;
    }
    txt = element.attribute("Number","UNDEFINED");
    if (txt == "UNDEFINED") {
      errors = errors | ERR_GTTFILE_BADFORMAT;
      return ERR_GTTFILE;
    }
    numCard = txt.toInt(&ok);
    if ((!ok) || (numCard < 1) || (numCard > bandWidth)) {
      errors = errors | ERR_GTTFILE_BADFORMAT;
      return ERR_GTTFILE;
    }
    numCard -= 1; // to reset into 0 -> bandWidth-1
    cardCol = cards.at(numCard);

    holes = element.firstChildElement("Holes");
    mount = element.firstChildElement("Threading");

    if ((holes.isNull()) || (mount.isNull())) {
      errors = errors | ERR_GTTFILE_BADFORMAT;
      return ERR_GTTFILE;
    }

    colorNode = holes.firstChild();
    for(j=0;j<nbHoles;j++) {
      txt = colorNode.firstChild().toText().data();
      idColor = txt.toInt(&ok);
      if (!ok) {
        errors = errors | ERR_GTTFILE_BADFORMAT;
        return ERR_GTTFILE;
      }
      cardCol[j] = (quint8)(idColor+1);
      colorNode = colorNode.nextSibling();
    }

    txt = mount.firstChild().toText().data();
    if (txt == "S") {
      mountSZ.replace(numCard,S_MOUNT);
    }
    else if (txt == "Z") {
      mountSZ.replace(numCard,Z_MOUNT);
    }
    else {
      errors = errors | ERR_GTTFILE_BADFORMAT;
      return ERR_GTTFILE;
    }
  }
  return NOERROR;
}

int GttExchanger::setNbHoles() {

  QDomNode node;
  QDomElement element;
  bool ok;

  nbHoles = 0;
  quint32 i;
  for(i=0;i<bandWidth;i++) {

    node = cardList.item(i);
    element = node.toElement();
    if (element.isNull()) {
      errors = errors | ERR_GTTFILE_BADFORMAT;
      return ERR_GTTFILE;
    }
    QString nbH = element.attribute("Holes","UNDEFINED");
    if (nbH == "UNDEFINED") {
      errors = errors | ERR_GTTFILE_BADFORMAT;
      return ERR_GTTFILE;
    }
    quint8 val = nbH.toInt(&ok);
    if (!ok) {
      errors = errors | ERR_GTTFILE_BADFORMAT;
      return ERR_GTTFILE;
    }
    if (nbHoles == 0) {
      nbHoles = val;
    }
    else {
      if (val != nbHoles) {
        errors = errors | ERR_GTTFILE_DIFFHOLES;
        return ERR_GTTFILE;
      }
    }
  }
  if ((nbHoles <3) || (nbHoles>6)) {
    errors = errors | ERR_GTTFILE_LIMNBHOLES;
    return ERR_GTTFILE;
  }
  return NOERROR;
}

int GttExchanger::importFromGtt(QString fileName) {

  int ret;
  int i;

  gttDom = QDomDocument("gtt-conf");

  QFile file(fileName);

  if (!file.open(QIODevice::ReadOnly)) {
    errors = errors | ERR_GTTFILE_NOOPEN;
    return ERR_GTTFILE;
  }

  QString errorMsg;
  if (!gttDom.setContent(&file,&errorMsg)) {
    file.close();
    errors = errors | ERR_GTTFILE_CORRUPTED;
    return ERR_GTTFILE;
  }

  file.close();

  /* analysing the DOM */
  QString tagName;
  QDomElement element;
  QDomNode node;  

  /* searching for root element <TWData> */
  rootElement = gttDom.documentElement();
  tagName = rootElement.tagName();

  if (tagName != "TWData") {
#ifdef DEBUG_PRINT
    QString msg = "bad root tag";
    cerr << qPrintable(msg) << endl;
#endif
    errors = errors | ERR_GTTFILE_BADFORMAT;
    return ERR_GTTFILE;
  }

  QDomNodeList patternNode = rootElement.elementsByTagName("Pattern");
  if (patternNode.size() != 1) {
#ifdef DEBUG_PRINT
    QString msg = "no or too many <Pattern> tag";
    cerr << qPrintable(msg) << endl;
#endif
    errors = errors | ERR_GTTFILE_BADFORMAT;
    return ERR_GTTFILE;
  }
  node = patternNode.at(0);
  element = node.toElement();
  if (element.isNull()) {

#ifdef DEBUG_PRINT
    QString msg = "<Pattern> : Cannot convert node to element";
    cerr << qPrintable(msg) << endl;
#endif
    errors = errors | ERR_GTTFILE_BADFORMAT;
    return ERR_GTTFILE;
  }
  QString patternType = element.attribute("Type","UNDEFINED");
  if ((patternType != "Threaded") && (patternType != "Threaded-in")) {
#ifdef DEBUG_PRINT
    QString msg = "<Pattern> : Not a Threaded pattern file";
    cerr << qPrintable(msg) << endl;
#endif
    errors = errors | ERR_GTTFILE_BADTYPE;
    return ERR_GTTFILE;
  }

  /* extracting palette node */
  paletteList  = rootElement.elementsByTagName("Palette");
  if (paletteList.size() != 1) {
    errors = errors | ERR_GTTFILE_BADFORMAT;
    return ERR_GTTFILE;
  }

  /* extracting all cards nodes */
  cardList  = rootElement.elementsByTagName("Card");
  bandWidth = cardList.size();
  if (bandWidth == 0) {
    errors = errors | ERR_GTTFILE_BADFORMAT;
    return ERR_GTTFILE;
  }

  /* extracting all packs node */
  packList = rootElement.elementsByTagName("Pack");

  /* extracting all picks node */
  pickList = rootElement.elementsByTagName("Pick");
  bandLength = pickList.size();

  /* set palette */
  ret = setPalette();
  if (ret != NOERROR) return ret;

  /* set number of holes */
  ret = setNbHoles();
  if (ret != NOERROR) return ret;

  /* set cards */
  quint8 *tab = NULL;
  for(i=0;i<bandWidth;i++) {
    tab = new quint8[nbHoles];
    cards.append(tab);
    mountSZ.append(S_MOUNT);
  }

#ifdef DEBUG_PRINT
  cout << "setting cards" << endl;
#endif
  ret = setCards();
  if (ret != NOERROR) {
    clearCards();
    return ret;
  }

#ifdef DEBUG_PRINT
  cout << "setting packs" << endl;
#endif
  ret = setPacks();
  if (ret != NOERROR) {
    clearCards();
    clearPacks();
    return ret;
  }

#ifdef DEBUG_PRINT
  cout << "setting picks" << endl;
#endif
  for(i=0;i<bandWidth;i++) {
    tab = new quint8[bandLength];
    if (mountSZ.at(i) == S_MOUNT) {
      memset(tab,SI,bandLength);
    }
    else {
      memset(tab,ZI,bandLength);
    }
    picks.append(tab);
  }

  ret = setPicks();
  if (ret != NOERROR) {    
    clearCards();
    clearPacks();
    clearPicks();
    return ret;
  }
  updateTwist();

  bool anglo = false;
  anglo = guessIfAngloSaxon();

  if (!anglo) {
    ret = guessPatternLength();
#ifdef DEBUG_PRINT
    cout << "pattern seems to be " << ret << " long" << endl;
#endif    
  }
  else {    
    ret = 2*nbHoles;
    warnings = warnings | WARN_GTTFILE_ANGLOSAXON;
#ifdef DEBUG_PRINT
    cout << "pattern seems to be anglo-saxon style. set pattern to" << ret << " long" << endl;
#endif
  }
  patternLength = ret;
  bandLength = 1+3*patternLength;

  QList<quint16> *lst;
  for(i=0;i<bandWidth;i++) {
    lst = new QList<quint16>();
    warps.append(lst);
  }

  setWarpsShape();
  setWarpsColorAndLettering();

  clearCards();
  clearPacks();
  clearPicks();

  params->clear();

  QFileInfo fi(fileName);
  params->name = fi.baseName();
  params->palette = palette;
  params->setNbHoles(nbHoles);
  params->setBandWidth(bandWidth);
  params->setPatternLength(patternLength);

  if (warnings != 0) {
    return WARN_GTTFILE;
  }
  else {
    return NOERROR;
  }
}


int GttExchanger::exportToGtt(QString fileName) {

  return NOERROR;
}
