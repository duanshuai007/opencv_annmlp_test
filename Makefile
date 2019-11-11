TARGET := main
OBJ := image.o train.o predict.o

CFLAG := -Wall -std=c++11

OPENCV_LIBS := $(shell pkg-config --libs --cflags opencv)

ROOTDIR := -I$(shell pwd)

all: $(TARGET)
	@echo "make all"
$(TARGET): % : %.cpp $(OBJ)
	@#the $(OPENCV_LIBS)  必须放在最后的位置，否则会链接出错
	@echo "make target"
	@g++ $(CFLAG) $(INCLUDE) $^ -o $@  $(OPENCV_LIBS) 

$(OBJ) : %.o : %.cpp
	@echo "compile $@"
	@g++ ${CFLAG} $(ROOTDIR) $(OPENCV_LIBS) -c $^ -o $@

clean:
	@rm -rf *.o ${TARGET} a.out
