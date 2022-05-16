CC = gcc
FLAGS = -g -pthread -lrt -Wall
OBJS = functions.c header.h
MAIN = SystemManager.c
M_NODE = MobileNode.c
TARGET = offload_simulator
TARGET2 = mobile_node

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(FLAGS) $(MAIN) $(OBJS) -o $(TARGET)
	$(CC) $(FLAGS) $(M_NODE) $(OBJS) -o $(TARGET2)

clean:
		$(RM) $(TARGET)
		$(RM) $(TARGET2)

#gcc -g -pthread -lrt -Wall SystemManager.c functions.c Header.h -o offload_simulator
#gcc -g -pthread -lrt -Wall MobileNode.c functions.c Header.h -o mobile_node


