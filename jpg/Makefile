#
# Makefile
#
# CC50
# Pset 5
#

all: copy resize whodunit

recover: Makefile recover.c
	gcc -ggdb -std=c99 -Wall -Werror -Wformat=0 -o recover recover.c

copy: bmp.h copy.c Makefile
	gcc -ggdb -std=c99 -Wall -Werror -Wformat=0 -o copy copy.c 

resize: bmp.h Makefile resize.c
	gcc -ggdb -std=c99 -Wall -Werror -Wformat=0 -o resize resize.c 

whodunit: bmp.h Makefile whodunit.c
	gcc -ggdb -std=c99 -Wall -Werror -Wformat=0 -o whodunit whodunit.c 

clean:
	rm -f *.o a.out copy core.* resize whodunit
