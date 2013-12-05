CC=g++
CFLAGS=-g -c -I/usr/include/opencv2 -Isrc
LDFLAGS=-L/usr/lib/x86_64-linux-gnu -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_features2d -lopencv_video

all: ParkAssistant utility

# Park Assistant compilation rules
ParkAssistant: main.o
	$(CC) $(LDFLAGS) src/libParkAssist.o src/main.o -o ./ParkAssistant

main.o: ParkAssistant.o
	$(CC) $(CFLAGS) src/main.cpp -o src/main.o

ParkAssistant.o:
	$(CC) $(CFLAGS) src/libParkAssist.cpp -o src/libParkAssist.o

# Utility compilation rules
utility: utility.o
	$(CC) $(LDFLAGS) src2/libutility1.o src2/utility1.o -o ./utility

utility.o:
	$(CC) $(CFLAGS) src2/libutility1.cpp -o src2/libutility1.o
	$(CC) $(CFLAGS) src2/utility1.cpp -o src2/utility1.o

# Cleaning rules
clean-parkassistant:
	rm -rf ParkAssistant
	rm -rf src/libParkAssist.o src/main.o

clean-utility:
	rm -rf utility
	rm -rf src2/libUtility1.o src2/utility1.o

clean: clean-utility clean-parkassistant

