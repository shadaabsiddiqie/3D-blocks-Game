all: sample2D

sample2D: Sample_GL3_2D.cpp glad.c
	g++ -g -o  sample3D sample2.cpp glad.c -lGL -lglfw -ldl

clean:
	rm sample2D
