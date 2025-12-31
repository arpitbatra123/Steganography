
LIBTOOL_PREFIX := $(shell brew --prefix libtool)

output: encode-image.cpp decode-image.cpp
	mkdir -p ./outputs
	c++ encode-image.cpp -o ./outputs/encode-image  -Wall `GraphicsMagick++-config --cppflags --cxxflags --ldflags --libs` -L$(LIBTOOL_PREFIX)/lib
	c++ decode-image.cpp -o ./outputs/decode-image  -Wall `GraphicsMagick++-config --cppflags --cxxflags --ldflags --libs` -L$(LIBTOOL_PREFIX)/lib
	c++ encode-file.cpp -o ./outputs/encode-file -Wall `GraphicsMagick++-config --cppflags --cxxflags --ldflags --libs` -L$(LIBTOOL_PREFIX)/lib
	c++ decode-file.cpp -o ./outputs/decode-file  -Wall `GraphicsMagick++-config --cppflags --cxxflags --ldflags --libs` -L$(LIBTOOL_PREFIX)/lib
        
clean:
	rm -rf ./outputs
