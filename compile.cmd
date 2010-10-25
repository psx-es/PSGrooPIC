del *.hex	/f /q/s

ccsc +FH +Y9 -L -A -E -M -P -J -D +GFW301="true" main.c
ccsc +FH +Y9 -L -A -E -M -P -J -D +GFW310="true" main.c
ccsc +FH +Y9 -L -A -E -M -P -J -D +GFW315="true" main.c
ccsc +FH +Y9 -L -A -E -M -P -J -D +GFW341="true" main.c
ccsc +FH +Y9 -L -A -E -M -P -J -D +GFW301="true" +GWBOOTLOADER="true" main.c
ccsc +FH +Y9 -L -A -E -M -P -J -D +GFW310="true" +GWBOOTLOADER="true" main.c
ccsc +FH +Y9 -L -A -E -M -P -J -D +GFW315="true" +GWBOOTLOADER="true" main.c
ccsc +FH +Y9 -L -A -E -M -P -J -D +GFW341="true" +GWBOOTLOADER="true" main.c

#del *.err	/f /q /s
del *.esym	/f /q /s

pause
