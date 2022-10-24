make: main.c
	gcc main.c -o main -lallegro -lallegro_image -lallegro_audio -lallegro_acodec

run:
	gcc main.c -o main -lallegro -lallegro_image -lallegro_audio -lallegro_acodec
	./main

clean:
	rm main