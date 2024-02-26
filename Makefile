CC ?= g++
TVMPATH = ${TVM}
VTAPATH = ${TVMPATH}/3rdparty/vta-hw
CFLAGS = -Wall -O3 -std=c++11 -I/usr/include -I${VTAPATH}/src/pynq
LDFLAGS = -L/usr/lib -L/opt/python3.6/lib/python3.6/site-packages/pynq/lib/
LIBS = -l:libcma.so -lstdc++ -pthread
INCLUDE_DIR = ${VTAPATH}/include
DRIVER_DIR = ${VTAPATH}/src/pynq

VPATH = $(DRIVER_DIR):$(TESTLIB_DIR)
SOURCES = pynq_driver.cc lstm.cc
OBJECTS = pynq_driver.o lstm.o lstm_test.o
EXECUTABLE = lstm

# Include VTA config
VTA_CONFIG = python ${VTAPATH}/config/vta_config.py
CFLAGS += `${VTA_CONFIG} --cflags`
LDFLAGS += `${VTA_CONFIG} --ldflags`
VTA_TARGET := $(shell ${VTA_CONFIG} --target)

# Include bitstream
VTA_PROGRAM = python3 ${TVMPATH}/vta/python/vta/program_bitstream.py
VTA_BIT = "./hardware/bitstream/vta.bit"

# Define flags
CFLAGS += -I $(INCLUDE_DIR) -DNO_SIM

# All Target
all: vtainstall $(EXECUTABLE)

%.o: %.cc $(SOURCES)
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

vtainstall:
	${VTA_PROGRAM} ${VTA_TARGET} ${VTA_BIT}
clean:
	rm -rf *.o $(EXECUTABLE)
