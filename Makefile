GXX = gcc
CFLAGS = -pedantic -g -Wall -Wvla -Werror -Wno-error=unused-variable -lpthread -pthread
OFILES = cracker.o cracker_cmd.o producer.o consumer.o sha-256.o

all: cracker

cracker: $(OFILES)
	$(GXX) $(CFLAGS) cracker.o cracker_cmd.o producer.o consumer.o sha-256.o -o cracker

cracker.o: cracker.c cracker_cmd.h producer.h consumer.o sha-256.o global.h
	$(GXX) $(CFLAGS) cracker.c -c

cracker_cmd.o: cracker_cmd.c global.h
	$(GXX) $(CFLAGS) cracker_cmd.c -c

producer.o: producer.c producer.h global.h
	$(GXX) $(CFLAGS) producer.c -c

consumer.o: consumer.c consumer.h sha-256.h global.h
	$(GXX) $(CFLAGS) consumer.c -c

sha-256.o: sha-256.c sha-256.h
	$(GXX) $(CFLAGS) sha-256.c -c

clean:
	rm cracker *.o *~