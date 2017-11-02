#  Makefile                                                      -*-makefile-*-
#  ----------------------------------------------------------------------------
#   Copyright (C) 2017 Dietmar Kuehl http://www.dietmar-kuehl.de         
#                                                                        
#   Permission is hereby granted, free of charge, to any person          
#   obtaining a copy of this software and associated documentation       
#   files (the "Software"), to deal in the Software without restriction, 
#   including without limitation the rights to use, copy, modify,        
#   merge, publish, distribute, sublicense, and/or sell copies of        
#   the Software, and to permit persons to whom the Software is          
#   furnished to do so, subject to the following conditions:             
#                                                                        
#   The above copyright notice and this permission notice shall be       
#   included in all copies or substantial portions of the Software.      
#                                                                        
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,      
#   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES      
#   OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND             
#   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT          
#   HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,         
#   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING         
#   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR        
#   OTHER DEALINGS IN THE SOFTWARE. 
#  ----------------------------------------------------------------------------

CPPFLAGS = -std=c++14
CXXFLAGS = -W -Wall -O3 -pthread
CC       = $(CXX)
LDFLAGS  = -pthread

NAME = partition

CXXFILES = \
	$(NAME).cpp \
	timer.cpp \

OFILES   = $(CXXFILES:%.cpp=%.o)

ifeq ($(COMPILER),gcc)
   DEPFLAGS = -M
endif

default: $(NAME)
	./$(NAME)

$(NAME): $(OFILES)
	$(CXX) $(LDFLAGS) -o $@ $(OFILES)

clean:
	$(RM) $(OFILES) $(NAME)
	$(RM) make.depend mkerr olderr

depend make.depend:
	$(CXX) $(DEPFLAGS) $(CPPFLAGS) $(CXXFILES) > make.depend

include make.depend
