#!/bin/bash
gcc -w -fpermissive generate_base_cs.c MO445.c -o ../generate_base_cs -lm
gcc -w -fpermissive generate_base_fourier.c MO445.c -o ../generate_base_fourier -lm
gcc -w -fpermissive generate_base_momentsinvariant.c MO445.c -o ../generate_base_momentsinvariant -lm
gcc -w -fpermissive generate_base_ss.c MO445.c -o ../generate_base_ss -lm
gcc -w -fpermissive generate_base_ts.c MO445.c -o ../generate_base_ts -lm
gcc -w -fpermissive -Iift/include/ generate_base_bas.c -o ../generate_base_bas -lm
gcc -w -fpermissive -Iift/include/ generate_base_bic.c -o ../generate_base_bic -lm
gcc -w -fpermissive -Iift/include/ generate_base_msfractal.c -o ../generate_base_msfractal -lm
gcc -w -fpermissive -Iift/include/ generate_base_sid.c -lm -o ../generate_base_sid
gcc -w -fpermissive -Iift/include/ generate_base_sms.c -o ../generate_base_sms -lm
gcc -w -fpermissive -Iift/include/ generate_base_tsdiz.c -o ../generate_base_tsdiz -lm

cd acc/source
make
cd app
gcc -L ../lib/ -I ../include/ generate_base_acc.c -l colordescriptors -lm -o ../../../../generate_base_acc
cd ../../../

cd colorbitmap/source/
make
cd app
gcc -L ../lib/ -I ../include/ generate_base_lucolor.c -l colordescriptors -lm -o ../../../../generate_base_lucolor
cd ../../../

cd csd/source/
make
cd app
gcc -L ../lib/ -I ../include/ generate_base_csd.c -l colordescriptors -lm -o ../../../../generate_base_csd
cd ../../../

cd gch/source/
make
cd app
gcc -L ../lib/ -I ../include/ generate_base_gch.c -l colordescriptors -lm -o ../../../../generate_base_gch
cd ../../../

cd htd/
make
cd app
gcc -L ../lib/ -I ../include/ generate_base_htd.c -l colordescriptors -lm -o ../../../generate_base_htd
cd ../../

cd jac/source/
make
cd app
gcc -L ../lib/ -I ../include/ generate_base_jac.c -l colordescriptors -lm -o ../../../../generate_base_jac
cd ../../../

cd las/source/
make
cd app
gcc -L ../lib/ -I ../include/ generate_base_las.c -l descriptors -lm -o ../../../../generate_base_las
cd ../../../

cd lbp/source/
make
cd app
gcc -L ../lib/ -I ../include/ generate_base_lbp.c -l texturedescriptors -lm -o ../../../../generate_base_lbp
cd ../../../

cd sasi/source/
make
cd app
gcc -L ../lib/ -I ../include/ generate_base_sasi.c -l texturedescriptors -lm -o ../../../../generate_base_sasi

echo FINISHED...

