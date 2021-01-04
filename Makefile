CXX = clang++
CXXFLAGS = -Wall -Wextra -O3 -g -flto -MMD -MP -std=c++98
LDFLAGS = -g -flto
NAME = webserv

SOURCES_FILES = main.cpp Util.cpp \
	socket/Connection.cpp socket/Server.cpp \
	http/Headers.cpp http/Request.cpp http/Response.cpp http/Constants.cpp \
	handler/RequestHandler.cpp handler/errorResponse.cpp handler/fileResponse.cpp\
	handler/folderResponse.cpp handler/utilsHandler.cpp handler/fileUpload.cpp\
	${addprefix config/,\
		Config.cpp display_config.cpp Event_config.cpp Http_config.cpp \
		Location_config.cpp parsing.cpp Server_config.cpp \
	} \
	encodings/Encoding.cpp \
	encodings/Iso_to_utf8.cpp\
	language/Language.cpp
ALL_SOURCES_FILES = $(SOURCES_FILES) \
	encodings/DeflateEncoding.cpp \
	encodings/GZipEncoding.cpp \
	encodings/BrotliEncoding.cpp \
	language/PHPLanguage.cpp \
	language/LuaLanguage.cpp \
	language/PythonLanguage.cpp \
	language/JavaScriptLanguage.cpp \
	language/PerlLanguage.cpp

DIRECTORIES = socket http handler config encodings language

SOURCES = $(addprefix src/,$(SOURCES_FILES))
ALL_SOURCES = $(addprefix src/,$(ALL_SOURCES_FILES))

OBJECTS = $(addprefix objs/,$(SOURCES_FILES:.cpp=.o))
ALL_OBJECTS = $(addprefix objs/,$(ALL_SOURCES_FILES:.cpp=.o))

all: $(NAME)

-include $(addsuffix .d,$(ALL_OBJECTS))

# Zlib

USE_ZLIB = no
ifdef ENCODING_GZIP
  CXXFLAGS += -DENCODING_GZIP
  USE_ZLIB = yes
  OBJECTS += objs/encodings/GZipEncoding.o
endif
ifdef ENCODING_DEFLATE
  CXXFLAGS += -DENCODING_DEFLATE
  USE_ZLIB = yes
endif

ifeq ($(USE_ZLIB),yes)
  CXXFLAGS += -Izlib
  LDFLAGS += -lz
  OBJECTS += objs/encodings/DeflateEncoding.o
endif

# Brotli

ifdef ENCODING_BROTLI
  CXXFLAGS += $(shell pkg-config --cflags libbrotlienc) \
    $(shell pkg-config --cflags libbrotlidec) \
    -DENCODING_BROTLI
  LDFLAGS += $(shell pkg-config --libs libbrotlienc) \
    $(shell pkg-config --libs libbrotlidec)
  OBJECTS += objs/encodings/BrotliEncoding.o
endif

# PHP

ifdef LANGUAGE_PHP
  CXXFLAGS += $(shell php-config --includes) -DLANGUAGE_PHP
  LDFLAGS += $(shell php-config --ldflags) -lphp7
  OBJECTS += objs/language/PHPLanguage.o
endif

# Lua

ifdef LANGUAGE_LUA
  CXXFLAGS += $(shell pkg-config --cflags lua5.3) -DLANGUAGE_LUA
  LDFLAGS += $(shell pkg-config --libs lua5.3)
  OBJECTS += objs/language/LuaLanguage.o
endif

# Python

ifdef LANGUAGE_PYTHON
  CXXFLAGS += $(shell pkg-config --cflags python3) -DLANGUAGE_PYTHON
  LDFLAGS += $(shell pkg-config --libs python3)
  OBJECTS += objs/language/PythonLanguage.o
endif

# JavaScript

ifdef LANGUAGE_JAVASCRIPT
  CXXFLAGS += -DLANGUAGE_JAVASCRIPT -I$(HOME)/v8/include -DV8_COMPRESS_POINTERS
  LDFLAGS += -L$(HOME)/v8/out.gn/x64.release.sample/obj -lv8_monolith -pthread
  OBJECTS += objs/language/JavaScriptLanguage.o
endif

## V8 headers won't compile if cpp98! Using C++11 just for V8:
objs/language/JavaScriptLanguage.o: src/language/JavaScriptLanguage.cpp
	$(CXX) $(CXXFLAGS) -std=c++11 -Wno-unused-parameter -c $< -o $@ -MF $@.d

# Perl

ifdef LANGUAGE_PERL
  CXXFLAGS += -DLANGUAGE_PERL -I$(shell perl -MConfig -e 'print $$Config{archlib}')/CORE 
  LDFLAGS += -lperl -lm
  OBJECTS += objs/language/PerlLanguage.o
endif

prepare:
	mkdir -p objs $(addprefix objs/,$(DIRECTORIES))

$(NAME): prepare $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $(NAME)

objs/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ -MF $@.d

clean:
	rm -rf objs

fclean: clean
	rm -f $(NAME)

re: fclean $(NAME)

.PHONY: prepare all clean fclean re
