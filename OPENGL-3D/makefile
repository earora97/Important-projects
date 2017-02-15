all: sample2D

sample2D: mycode.cpp coor.cpp
	g++ -g -o sample2D mycode.cpp coor.cpp -lao -lm -lmpg123 -lglfw -lGLEW -lGL -ldl

clean:
	rm sample2D
