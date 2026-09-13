/*-==============================================================-

file : GttExchanger.hpp

creation date : 21/02/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __GTTEXCHANGER_HPP__
#define __GTTEXCHANGER_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>
#include <QtXml>

#include "BandCst.hpp"
#include "ProjectParams.hpp"

#define NOERROR 0
#define ERR_GTTFILE 1
#define WARN_GTTFILE 2

#define ERR_GTTFILE_NOOPEN 1
#define ERR_GTTFILE_CORRUPTED 2
#define ERR_GTTFILE_BADFORMAT 4
#define ERR_GTTFILE_BADTYPE 8
#define ERR_GTTFILE_CANNOTIMPORT 16
#define ERR_GTTFILE_DIFFHOLES 32
#define ERR_GTTFILE_LIMNBHOLES 64

#define WARN_GTTFILE_PATTERNLENGTH 1
#define WARN_GTTFILE_ANGLOSAXON 2

using namespace std;
using namespace Qt;

class GttExchanger : public QObject {

  Q_OBJECT

public:
  GttExchanger(ProjectParams *_params);

  QList<QList<quint16> *> warps;  //each quint16 * will represent a serie of picks

public slots:
  int importFromGtt(QString fileName);
  int exportToGtt(QString fileName);
  void printShapes();
  QString getErrorString();
  QString getWarningString();
  void clearWarps();

private:
  ProjectParams *params;

  QDomDocument gttDom;
  QDomElement rootElement;

  quint8 patternLength;
  quint8 bandWidth;
  quint8 bandLength;
  quint8 nbHoles;
  QString name;

  int errors;
  int warnings;

  QList<quint8 *> cards; // each quint8* represents the hole colors of a card.
  QList<quint8> mountSZ;

  QHash<QString,QList<quint8> *> packs;
  QList<quint8 *> picks; // picks extracted from GTT file
  QList<QColor> palette;
  

  QDomNodeList cardList;
  QDomNodeList packList;
  QDomNodeList pickList;
  QDomNodeList paletteList;

  int setNbHoles();
  int setCards();
  int setPacks();
  int setPicks();
  int setPalette();
  void updateTwist();
  int setCardPick(int cardId, int pickId, const QString &move);

  void setWarpsShape();
  void setWarpsColorAndLettering();


  int guessPatternLength();
  bool weftsAreEqual(int pickId1, int pickId2);
  bool guessIfAngloSaxon();

  void clearCards();  
  void clearPacks();
  void clearPicks();

};

#endif // __GTTEXCHANGER_HPP__
