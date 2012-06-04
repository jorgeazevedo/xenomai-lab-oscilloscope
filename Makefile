###### CONFIGURATION ######

### List of applications to be build
EXECUTABLE=oscilloscope

BLOCK_PROJECT_FOLDER=src/

all : oscilloscope 
.PHONY : all
     
oscilloscope :
	cd $(BLOCK_PROJECT_FOLDER); qmake && make;

clean::
	cd $(BLOCK_PROJECT_FOLDER); qmake && make clean;
cleanall::
	cd $(BLOCK_PROJECT_FOLDER); qmake && make clean;

