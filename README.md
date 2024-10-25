##### dat2geotif

convert n*m data to geotiff

compile:
```shell
    gcc dat2geotiff.c -ltiff -lgeotiff -lm -o dat2gtif
```

usage:
```shell
   (row first)    $dat2gtif -w [width] -l [length] -i [geotiff info] -d [data file] -o [output file]
   (column first) $dat2gtif -T -w [width] -l [length] -i [geotiff info] -d [data file] -o [output file]
```
the data file should have w*l lines


example:
```shell
    dat2gtif -w 1441 -l 721 -o test.tif -i geotiffinfo -d Model1_thick.dat
```
