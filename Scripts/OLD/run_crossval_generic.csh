#! /usr/local/bin/tcsh -f
# $1 = hostname/tag
# $2 = maxahead
# $3 = minint
# $4 = maxint
# $5 = numtimes
# $6 = BM limit
# $7 = ar param
#


combine_host $1 ~/$1.bin
bo_to_text ~/$1.bin > ~/$1.txt

crossval_generic ~/$1.txt $1 $2 $3 $4 $3 $4 $5 $6 AR $7 > ~/cross_val_ar.$1.out

rm ~/$1.bin ~/$1.txt
