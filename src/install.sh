#!/bin/sh

####################################
# Carto'magic installation script
###################################

if ! [ -f ./cartomagic ]; then
  echo "Impossible de trouver l'executable dans le répertoire courant. Arret"
  exit 1
fi
if ! [ -d ./locales ]; then
  echo "Impossible de trouver les fichiers de langue dans le répertoire courant. Arret"
  exit 1
fi

if ! [ -d /usr/local/bin ]; then
  mkdir -p /usr/local/bin 
fi

if ! [ -d /usr/local/share/cartomagic ]; then
  mkdir -p /usr/local/share/cartomagic 
else
  rm -rf /usr/local/share/cartomagic
  mkdir -p /usr/local/share/cartomagic 
fi

if [ -f /usr/local/bin/cartomagic ]; then
  echo "Suppression de la version actuellement installée"
  rm -f /usr/local/bin/cartomagic
fi
echo "Installation :"
echo "binaries"
cp cartomagic /usr/local/bin 
echo "locales"
cp -r locales /usr/local/share/cartomagic/
echo "examples"
cp -r examples /usr/local/share/cartomagic/
echo "done."
