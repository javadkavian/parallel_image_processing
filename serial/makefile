PATH_SRC  = ./src
PATH_INC  = $(PATH_SRC)/include
PATH_OBJ  = ./obj
#-------------------------------
CXX      =  g++
CXXFLAGS += -g
CPPFLAGS += -std=c++11 -I$(PATH_INC)
#----------------------------------------
OUTPUT = ImageFilters.out
#----------------------------------------
vpath %.cpp $(PATH_SRC)
vpath %.hpp $(PATH_INC)
#----------------------------------------
OBJS = main.o bmp.o
#-----------------------------------------
all: $(PATH_OBJ) $(OUTPUT)

$(OUTPUT) : $(addprefix $(PATH_OBJ)/, $(OBJS))
		$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^

$(PATH_OBJ)/main.o: main.cpp bmp.hpp globals.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(PATH_OBJ)/bmp.o: bmp.cpp bmp.hpp globals.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

#--------------------------------------------------
$(PATH_OBJ):  ; mkdir -p $@

.PHONY: all clean run

clean:
	rm -rf $(PATH_OBJ) $(OUTPUT) > /dev/null 2>&1