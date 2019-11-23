#!/bin/bash
set -e
set -x

FILE=$1
W=$2
H=$3
shift

mkdir -p build
cd build
cmake ..
make

cd ..
cd output
mkdir -p png

../bin/generateCA "$@"

for i in *.pgm; do convert $i png/$i.png; done
exit 0

pdflatex -synctex=1 -interaction=nonstopmode -shell-escape ../cirles.tex 

for i in `seq 0 7`; do convert -density 300 cirles.pdf[$i] b$i.png; done
for i in `seq 8 15`; do convert -density 300 cirles.pdf[$i] f$i.png; done


mkdir -p result
#create num label from single state label
../commands.sh

#annotate the image label with text


mkdir -p png/merged


PS=`echo "(220*400)/500" | bc`
S=`echo "(225*400)/500" | bc`
for i in `seq 0 255`
do
	FONT=Noto-Sans-Mono-Regular ; 
	width=`identify -format %w result/$i.png`; 
	height=`identify -format %h result/$i.png`; 
	VAL=`echo "obase=16; $i" | bc`
	convert -font $FONT -pointsize $PS -geometry +0-2  -background black -fill white -gravity center -size ${width}x$S caption:"Rule 0x$VAL" result/$i.png   +swap -gravity south -composite  result/result_annotated$i.png
	montage -tile 1x2  png/file$i.pgm.png \( result/result_annotated$i.png -resize $W \) -geometry +0+3  png/merged/merged$i.png
done

FILES=`ls png/merged/merged* | sort  -t d -k 3 -n`
montage -tile 16x16 -geometry +10+20 $FILES $FILE





#Create pdf with multiple images  pdflatex -synctex=1 -interaction=nonstopmode -shell-escape cirles.tex
#convert each page to a single image a state of the rule
# for i in `seq 1 7`; do convert -density 300 cirles.pdf[$i] b$i.png; done
# for i in `seq 8 15`; do convert -density 300 cirles.pdf[$i] f$i.png; done
# create the numbers 
#convert  b0.png  f9.png  f10.png  f11.png  f12.png  f13.png  f14.png  f15.png +append result/254.png