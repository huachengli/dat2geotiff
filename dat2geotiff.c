/*
 * 21/6/2024
 * convert ascii list data to geotiff.
 * example for libgeotiff
 */

#include <tiff.h>
#include <tiffio.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <geotiff/xtiffio.h>  /* for TIFF */
#include <geotiff/geotiffio.h> /* for GeoTIFF */
#include <string.h>
#include <unistd.h>
#include <assert.h>

int main(int argc, char* argv[]) {
    char in_file_name[1024] = "Model1_thick.dat";
    char out_name[1024] = "a.tif";
    char in_geo_name[1024] = ""; // unused
    char in_info_name[1024] = "geotiffinfo";
    const char * proj4_string = "+proj=eqc +lat_0=0.000000000 +lon_0=0.000000000 +lat_ts=0.000000000 +x_0=0.000 +y_0=0.000 +a=1737400.000 +b=1737400.000 +units=m";

    // some GEOTIFFTAG
    uint32_t spp = 1;
    uint32_t bps = 32;
    uint32_t width = 1441;
    uint32_t length = 721;

    // parser input
    int c; opterr = 0;
    while ((c = getopt (argc, argv, "w:l:d:i:o:")) != -1)
    {
        switch(c)
        {
            case 'w':
                width = atoi(optarg);
                break;
            case 'l':
                length = atoi(optarg);
                break;
            case 'd':
                strncpy(in_file_name,optarg,1024);
                break;
            case 'i':
                strncpy(in_info_name,optarg,1024);
                break;
            case 'o':
                strncpy(out_name,optarg,1024);
                break;
            case '?':
                fprintf (stderr,"Unknown option character `\\x%x'.\n",optopt);
                return 1;
            default:
                abort();
        }
    }


    // calculate the x/y scale
    double out_scale[3] = {1737400.0*M_PI*2.0/1441,1737400.0*M_PI*1.0/721,0.0};

    FILE * in = fopen(in_file_name,"r");
    TIFF * out = XTIFFOpen(out_name,"w");
    GTIF * out_gtif = NULL;
    TIFF * in_geo = NULL;
    if(strlen(in_geo_name) > 0)
    {
        in_geo = XTIFFOpen(in_geo_name,"r");
        assert(NULL != in_geo);
    }

    assert(NULL != in);
    assert(NULL != out);
    // assert(NULL != out_gtif);

    if(strlen(in_info_name) > 0)
    {
        out_gtif = GTIFNew(out);
        FILE* in_info = fopen(in_info_name,"r");
        assert(NULL != in_info);
        if(!GTIFImport(out_gtif,0,in_info))
        {
            fprintf(stderr,"Failure in GTIFImport\n");
            exit (-1);
        }
        fclose(in_info);
    }

    TIFFSetField(out,TIFFTAG_BITSPERSAMPLE,bps);
    TIFFSetField(out,TIFFTAG_SAMPLESPERPIXEL,spp);
    TIFFSetField(out,TIFFTAG_IMAGELENGTH,length);
    TIFFSetField(out,TIFFTAG_IMAGEWIDTH,width);
    TIFFSetField(out,TIFFTAG_COMPRESSION,COMPRESSION_NONE);
    TIFFSetField(out, TIFFTAG_FILLORDER, FILLORDER_LSB2MSB);
    TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, 1);
    TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(out,TIFFTAG_SAMPLEFORMAT,SAMPLEFORMAT_INT);
    TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    TIFFSetField(out,TIFFTAG_GEOPIXELSCALE,3,out_scale);

    // process geo related paras
    /*
    // there some error : with +proj=eqc
    if(!GTIFSetFromProj4(out_gtif,proj4_string))
    {
        fprintf(stderr,"Error GTIFSetFromProj4\n");
    }
    */

    GTIFWriteKeys(out_gtif);
    GTIFFree(out_gtif);

    tmsize_t scanlinesize = TIFFScanlineSize(out);
    unsigned char *obuf = _TIFFmalloc(scanlinesize);
    int32_t * ibuf = (int32_t *)obuf;
    for(size_t k=0;k<length;++k)
    {
        //read data from dat file
        for(size_t j=0;j<width;++j)
        {
            double _local_data;
            fscanf(in,"%lf",&_local_data);
            ibuf[(j+width/2)%width] = (int32_t) round(_local_data*1000.0);
        }
        TIFFWriteScanline(out,obuf,k,0);
    }
    TIFFWriteDirectory(out); // update IFD in tif
    _TIFFfree(obuf);
    if(NULL != in_geo) XTIFFClose(in_geo);
    fclose(in);
    XTIFFClose(out);
    return 0;
}
