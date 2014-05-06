#!/bin/sh

for i in \
	01_opeEqu_sample  02_init_sample  03_append_sample \
	04_update_sample  05_erase_sample  06_index_sample \
	; do 
  echo $i
  ./$i
  echo ""
done
