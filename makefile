CXX = g++ -std=c++11

src = $(wildcard *.cpp)
head = $(wildcard *.h)
obj = $(patsubst %.cpp, %.o, $(wildcard demo*.cpp)) 
target = $(patsubst %.cpp, %, $(wildcard demo*.cpp))  #目标文件是所有demo*的 
rely = credis.cpp redis_connect_pool.cpp


INCS = -I./hiredis #使用自己的静态库编译
LIBS = -L./hiredis -lhiredis 

CFLAGS += -g 

all:$(target) $(relay)

$(target):%:%.cpp $(rely)  	#用模式规则对每个独立的目标进行编译
	$(CXX) $(INCS) $(CFLAGS) $^ -o $@ $(LIBS)

all:$(relay)

$(realay):%:%:.o 
		$(CXX) $(INC) $(CFLAGS) $^ -o $@ $(LIBS)

.PHONY:all clean
clean:
	@echo "clean all .o and exe: "
	rm -rf  $(obj) $(target) ./file/*.txt
