CFLAGS := $(CFLAGS) -O3 -Wno-unused-result

# Constants
OBJECTS = array_utility.o bilinear_interpolation.o contrast_enhancement.o \
	edge_detection.o fourier_filter.o gradient_filter.o \
	histogram_equalization.o histograms.o kernel_filter.o \
	manipulate_hsi.o median_filter.o resize.o rotate.o lodepng.o

all:	process profile_rotate profile_median_filter profile_runner.sh

clean:
	rm -rf *.0 *.dSYM process

process: $(OBJECTS) process.o
	gcc $(CFLAGS) -o process process.o $(OBJECTS) -lm

array_utility.o: array_utility.c array_utility.h
bilinear_interpolation.o: bilinear_interpolation.c \
  bilinear_interpolation.h
contrast_enhancement.o: contrast_enhancement.c contrast_enhancement.h \
  histograms.h array_utility.h
edge_detection.o: edge_detection.c edge_detection.h array_utility.h \
  kernel_filter.h contrast_enhancement.h median_filter.h
fourier_filter.o: fourier_filter.c fourier_filter.h array_utility.h
gradient_filter.o: gradient_filter.c gradient_filter.h array_utility.h \
  kernel_filter.h
histogram_equalization.o: histogram_equalization.c \
  histogram_equalization.h array_utility.h
histograms.o: histograms.c histograms.h array_utility.h
kernel_filter.o: kernel_filter.c kernel_filter.h array_utility.h
lodepng.o: lodepng.c lodepng.h
manipulate_hsi.o: manipulate_hsi.c manipulate_hsi.h array_utility.h
median_filter.o: median_filter.c median_filter.h array_utility.h \
  kernel_filter.h
process.o: process.c array_utility.h rotate.h bilinear_interpolation.h \
  resize.h histograms.h contrast_enhancement.h manipulate_hsi.h \
  histogram_equalization.h kernel_filter.h median_filter.h \
  gradient_filter.h fourier_filter.h edge_detection.h lodepng.h
resize.o: resize.c resize.h bilinear_interpolation.h array_utility.h
rotate.o: rotate.c rotate.h bilinear_interpolation.h array_utility.h
profile_rotate.o: profile_rotate.c lodepng.h ppm.h rotate.h array_utility.h timer.h
profile_median_filter.o: profile_rotate.c lodepng.h ppm.h median_filter.h array_utility.h timer.h

profile_rotate: $(OBJECTS) profile_rotate.o
	gcc $(CFLAGS) -o profile_rotate profile_rotate.o $(OBJECTS) -lm

profile_median_filter: $(OBJECTS) profile_median_filter.o
	gcc $(CFLAGS) -o profile_median_filter profile_median_filter.o $(OBJECTS) -lm

profile_runner.sh: gen_runner.py
	python gen_runner.py > profile_runner.sh
	chmod +x profile_runner.sh
