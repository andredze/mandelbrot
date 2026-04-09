DED_FLAGS = -D _DEBUG -ggdb3 -std=c++17 -Wall -Wextra -Weffc++ -Wsign-conversion 			 	 \
		   -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations 		   		 \
		   -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported  		   		 \
		    -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral 		 			 \
		   -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op 		   		 \
		   -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith  		 \
		   -Winit-self -Wredundant-decls -Wshadow -Wsign-promo		   		 					 \
		   -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn		   		 \
		   -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default 	 \
		   -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast 		 \
		   -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing  \
		   -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation 	 \
		   -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer 	 \
		   -Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE -Werror=vla -Wconversion

ASAN = -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,$\
	    float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,$\
	    null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,$\
	    undefined,unreachable,vla-bound,vptr

# ——————————————————————————————————————————————————————————————————————————————————————————

CXX = g++

CXXFLAGS = -march=native -g -fno-omit-frame-pointer

INCLUDES += -I include

SOURCES = src/main.cpp	 		  \
		  src/graphics.cpp 		  \
		  src/mandelbrot.cpp	  \
		  src/mandelbrot_test.cpp \

# ------------------------------------------------------------------ #

ifdef DEBUG
	CXXFLAGS += -D DEBUG $(DED_FLAGS) $(ASAN)
else
	CXXFLAGS += -DNDEBUG
endif

# ------------------------------------------------------------------ #

ifdef AVX
	CXXFLAGS += -D _AVX
else ifdef ARRAYS
	CXXFLAGS += -D _ARRAYS
else ifdef NAIVE
	CXXFLAGS += -D _NAIVE
endif

# ------------------------------------------------------------------ #

ifdef WITH_O3
	CXXFLAGS += -O3
else
	CXXFLAGS += -O2
endif

# ------------------------------------------------------------------ #

ifdef GRAPHICS
	CXXFLAGS += -D GRAPHICS
endif

# ------------------------------------------------------------------ #

OBJS = $(SOURCES:src/%.cpp=obj/%.o)

TARGET = run

LD_FLAGS = -lSDL2 -lSDL2_ttf

# ——————————————————————————————————————————————————————————————————————————————————————————

$(shell mkdir -p obj)

all: clean build

build: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(CXXFLAGS) $(LD_FLAGS)

obj/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

clean:
	rm -rf $(OBJS) $(TARGET)

.PHONY: all clean build

# ——————————————————————————————————————————————————————————————————————————————————————————