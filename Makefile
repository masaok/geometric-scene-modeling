OUTPUT := assignment3

OPTIONS := -lc++ -std=c++11

all: clean compile run

run: $(OUTPUT)
	./$(OUTPUT)

compile: $(OUTPUT).cpp
	gcc -framework OpenGL -framework GLUT $(OPTIONS) $(OUTPUT).cpp -o $(OUTPUT)

clean:
	rm -f $(OUTPUT)

stop:
	pkill $(OUTPUT)