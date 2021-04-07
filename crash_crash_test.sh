#!/bin/sh

a=0

while [ $a -lt 10000 ]
do
   ./crash 
   a=`expr $a + 1`
done