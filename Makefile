EDSDK_DIR		:=	../edsdk-windows

INCLUDE_DIRS	:=	$(EDSDK_DIR)/EDSDK/Header
LIB_DIRS		:=	$(EDSDK_DIR)/EDSDK_64/Library


INCLUDE_DIRS	:=	$(addprefix -I,$(INCLUDE_DIRS))
LIB_DIRS		:=	$(addprefix -L,$(LIB_DIRS))

LIBS			:=	$(EDSDK_DIR)/EDSDK_64/Library/EDSDK.lib



out.exe: *.cpp
	g++ $^ $(INCLUDE_DIRS) $(LIB_DIRS) $(LIBS) -o $@

clean:
	rm -f out.exe

.PHONY: clean run
