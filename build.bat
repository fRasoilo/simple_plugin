@echo off 
IF NOT EXIST ..\build mkdir ..\build 
pushd ..\build 
cl -nologo -MDd ..\code\simple_plugin.cpp -FC -Z7 -FmSimplePlugin.map /link -incremental:no -subsystem:console /PDB:SimplePlugin.pdb 
cl -LD -nologo -MDd ..\code\sample_plugin.cpp -FC -Z7 -Fmsample_plugin.map /link  -incremental:no -subsystem:console /PDB:sample_plugin.%RANDOM%.pdb 
cl -LD -nologo -MDd ..\code\second_plugin.cpp -FC -Z7 -Fmsecond_plugin.map /link  -incremental:no -subsystem:console /PDB:second_plugin.%RANDOM%.pdb 
rem cl -LD -nologo -MDd ..\code\third_plugin.cpp -FC -Z7 -Fmthird_plugin.map /link  -incremental:no -subsystem:console /PDB:third_plugin.%RANDOM%.pdb 
rem cl -LD -nologo -MDd ..\code\fourth_plugin.cpp -FC -Z7 -Fmfourth_plugin.map /link  -incremental:no -subsystem:console /PDB:fourth_plugin.%RANDOM%.pdb 


popd 
