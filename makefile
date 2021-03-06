#Windows 7:
#RM=del /y

#Windows 8.1:
#RM=del /S

OUTDIR=out
BINDIR=bin

all: $(OUTDIR) dll
	$(MAKE) -C TurtleGUI
	$(MAKE) -C TurtleMap
	$(MAKE) -C common
	$(MAKE) -C server
	$(MAKE) -C client

debug: export CXXFLAGS+=-g
debug: clean all

ifeq ($(OS),Windows_NT)
release: export CXXFLAGS+=-static-libgcc -static-libstdc++
endif
release: clean all package

#For use on my machine ONLY
package:
ifeq ($(OS),Windows_NT)
	rar a -r -ep Tortuga-win.rar $(OUTDIR)/*.exe  $(BINDIR)/*.dll
	rar a -r Tortuga-win.rar rsc/* copyright.txt instructions.txt
else ifeq ($(shell uname), Linux)
	tar -C $(OUTDIR) -zcvf Tortuga-linux.tar client server ../rsc ../copyright.txt ../instructions.txt
endif

dll: $(OUTDIR)
ifeq ($(OS),Windows_NT)
	xcopy /Y $(BINDIR)\\*.dll $(OUTDIR)
endif

$(OUTDIR):
	mkdir $(OUTDIR)

clean:
ifeq ($(OS),Windows_NT)
	del /S /Q *.o *.a *.exe $(OUTDIR)\*.dll
#	rmdir /S /Q $(OUTDIR)
else ifeq ($(shell uname), Linux)
	find . -type f -name '*.o' -exec rm -f -r -v {} \;
	find . -type f -name '*.a' -exec rm -f -r -v {} \;
#	rm $(OUTDIR)/* -f
	find . -empty -type d -delete
endif

rebuild: clean all
