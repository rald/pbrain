pb: pb.c
	gcc pb.c -o pb -lcurl -Wall -Wextra -pedantic

clean:
	rm pb
