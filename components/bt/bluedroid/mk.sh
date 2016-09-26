#!/bin/bash

FIND_F=component.mk
MV_F=Makefile.backup

for mkf in `find . -name "$FIND_F"`
do
	echo $mkf
	mkf_n=`echo $mkf | sed -n "s/$FIND_F/$MV_F/p"`
	echo $mkf_n
	mv $mkf $mkf_n
done
