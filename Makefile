CXX = g++
RM = rm

CFLAGS = `pkg-config opencv --cflags --libs`
LIBS = `pkg-config opencv --cflags --libs`

TARGET = Tp

OBJS = camera.o main.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(CFLAGS) $(LIBS)
	
%.o: %.cpp
	$(CXX) -c $(CFLAGS) $<

clean: 
	$(RM) -f *.o $(TARGET)
