For building this stubs, just call the java compiler and then the preverifier in the regular way.

For example, assumbing you have both in the path, the WTK is in C:\WTK25, and that you have a subdirectory called "bin" to hold the preverified class output, for building the SM class you can do:

javac -g:none -classpath c:\WTK25\lib\midpapi10.jar;c:\WTK25\lib\cldcapi10.jar;c:\WTK25\lib\wma11.jar -source 1.3 -target 1.1 SM.java
preverify1.1 -nofp -nofinalize -nonative -classpath c:\WTK25\lib\midpapi10.jar;c:\WTK25\lib\cldcapi10.jar;c:\WTK25\lib\wma11.jar -d "bin" "SM"

Enjoy,
Javier Santo Domingo
