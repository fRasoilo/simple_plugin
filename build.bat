@echo off 
IF NOT EXIST ..\build mkdir ..\build 
pushd ..\build 
cl -nologo -MDd ..\code\simple_plugin.cpp -FC -Z7 -FmSimplePlugin.map /link -incremental:no -subsystem:console /PDB:SimplePlugin.pdb 
cl -LD -nologo -MDd ..\code\plugin_example.cpp -FC -Z7 -Fmplugin_example.map /link  -incremental:no -subsystem:console /PDB:plugin_example.%RANDOM%.pdb 
cl -LD -nologo -MDd ..\code\second_plugin.cpp -FC -Z7 -Fmsecond_plugin.map /link  -incremental:no -subsystem:console /PDB:second_plugin.%RANDOM%.pdb 

popd 
