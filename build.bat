@echo off 
IF NOT EXIST ..\build mkdir ..\build 
pushd ..\build 
cl -nologo -MDd ..\code\simple_plugin.cpp -FC -Z7 -FmSimplePlugin.map /link -incremental:no -subsystem:console /PDB:SimplePlugin.pdb 
popd 
