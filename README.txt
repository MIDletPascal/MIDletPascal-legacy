In this SVN repository you will find the different compiler versions (and it's RTL stubs) available under the following structure (also explained in the INHERITANCE.jpg file, useful for quick reference):




COMPILERS
---------

/MPC.2.0.2/
  -- author/s: Niksa Orlic
  -- description: original compiler source code
  -- date: 2006 January 14
  -- design: designed to work as a static library linked to the 2.0 IDE
  -- preverificator: yes
  -- project: /MPC.2.0.2/ProjectMobilePascal.dsp (Visual Studio 6)
  -- readme: /MPC.2.0.2/readme.txt (english)

/MPC.3.0.003/
  -- author/s: Niksa Orlic & Artem
  -- description: first enhacements to MPC.2.0.2; includes a new lexer, shl & shr operators, smart string concatenation
  -- date: 2009 October 10
  -- design: designed to work as an stand-alone application operated by the user via command-line
  -- preverificator: yes
  -- project: /MPC.3.0.003/mpc.vcproj (Visual C++ 8)
  -- readme: /MPC.3.0.003/readmerus.txt (russian)

/MPC.3.0.005/
  -- author/s: Niksa Orlic & Artem
  -- description: based on MPC.3.0.003 version; bytecode inlining
  -- date: 2009 October 14
  -- design: designed to work as an stand-alone application operated by the user via command-line
  -- preverificator: yes
  -- project: /MPC.3.0.005/mpc.vcproj (Visual C++ 8)
  -- readme: /MPC.3.0.005/readmerus.txt (russian)

/MPC.3.0.007/
  -- author/s: Niksa Orlic & Artem
  -- description: based on MPC.3.0.005 version; max array size up to 32767
  -- date: 2009 October 23
  -- design: designed to work as an stand-alone application operated by the user via command-line
  -- preverificator: yes
  -- project: /MPC.3.0.007/mpc.vcproj (Visual C++ 8)
  -- readme: /MPC.3.0.007/readmerus.txt (russian)

/MPC.3.0.009/
  -- author/s: Niksa Orlic & Artem
  -- description: based on MPC.3.0.007 version; includes {$R+/-} to enable/disable real numbers support, {$V+/-} to enable/disable internal bytecode preverification
  -- date: 2009 December 07
  -- design: designed to work as an stand-alone application operated by the user via command-line
  -- preverificator: yes
  -- project: /MPC.3.0.009/mpc.vcproj (Visual C++ 8)
  -- readme: /MPC.3.0.009/readmerus.txt (russian)

/MPC.3.0.009.SIMPLE/
  -- author/s: Niksa Orlic & Artem
  -- description: modified MPC.3.0.009 version; excludes the internal bytecode preverificator
  -- date: 2009 December 10
  -- design: designed to work as an stand-alone application operated by the user via command-line
  -- preverificator: no
  -- project: /MPC.3.0.009.SIMPLE/mpc.vcproj (Visual C++ 8)
  -- readme: /MPC.3.0.009.SIMPLE/readmerus.txt (russian)

/MPC.3.0.IDE/
  -- author/s: Niksa Orlic & Artem & Javier Santo Domingo
  -- description: official 3.0 compiler source code; based on MPC.3.0.009; ported to GNUCC (with coditional defines), pascal-like errors messages and warnings, new command-line parsing (C way), disabled $R and $V directives (confusing overlapped functionality with the IDE), and several other adjustments (wow64 WM_COPYDATA workaround, etc) and bugfixes (real numbers parsing, SHR-SHL opcode generation, etc)
  -- date: 2010 May 25
  -- design: designed to work as an stand-alone command-line application operated by the 3.0 IDE
  -- preverificator: yes
  -- project: /MPC.3.0.IDE/mp3CC.cbp (Code::Blocks 8 / GNUCC)
  -- readme: /MPC.3.0.IDE/readme.txt (english)

/MPC.3.0.010.SIMPLE/
  -- author/s: Niksa Orlic & Artem
  -- description: modified MPC.3.0.009.SIMPLE version; includes {$T+/-} to enable/disable the use of lowercase on current token
  -- date: 2010 June 28
  -- design: designed to work as an stand-alone application operated by the user via command-line
  -- preverificator: no
  -- project: /MPC.3.0.010.SIMPLE/mpc.vcproj (Visual C++ 8)
  -- readme: /MPC.3.0.010.SIMPLE/readmerus.txt (russian)

/MPC.3.0.0101/
  -- author/s: Niksa Orlic & Artem
  -- description: modified MPC.3.0.010.SIMPLE version; includes a fix for the presence of text after "end." giving only a warning, bugfix on the parser to avoid hangs by endless cycling, removed a comma from the construction "if then; else;" in "if then else;", etc
  -- date: 2010 July 01
  -- design: designed to work as an stand-alone application operated by the user via command-line
  -- preverificator: no
  -- project: /MPC.3.0.0101/mpc.vcproj (Visual C++ 8)
  -- readme: /MPC.3.0.0101/readmerus.txt (russian)

/MPC.3.1.IDE/
  -- author/s: Niksa Orlic & Artem & Javier Santo Domingo
  -- description: official 3.1 compiler source code; based on MPC.3.0.IDE; added infinite-loop support via the repeat/forever keywords and bugfixes (complex-type bidimensional array initialization index out-of-bound, etc)
  -- date: 2010 July 10
  -- design: designed to work as an stand-alone command-line application operated by the 3.1 IDE
  -- preverificator: yes
  -- project: /MPC.3.1.IDE/mp3CC.cbp (Code::Blocks 8 / GNUCC)
  -- readme: /MPC.3.1.IDE/readme.txt (english)

/MPC.3.0.0101.SIMPLE/
  -- author/s: Niksa Orlic & Artem
  -- description: modified MPC.3.0.0101 version; $C+/-/* canvas selection compiler directive, line number display on error reports, etc
  -- date: 2010 July 25
  -- design: designed to work as an stand-alone application operated by the user via command-line
  -- preverificator: no
  -- project: /MPC.3.0.0101.SIMPLE/mpc.vcproj (Visual C++ 8)
  -- readme: /MPC.3.0.0101.SIMPLE/readmerus.txt (russian)

/MPC.3.1.LINUX/
  -- author/s: Niksa Orlic & Artem & Javier Santo Domingo & Zoltán Várnagy
  -- description: 3.1 compiler source code for Linux (tested on PowerPC & x86); based on MPC.3.1.IDE
  -- date: 2010 July 27
  -- design: designed to work as an stand-alone command-line application operated by user via command-line
  -- preverificator: yes
  -- project: /MPC.3.1.LINUX/mp3CC.cbp (Code::Blocks 8 / GNUCC)
  -- readme: /MPC.3.1.LINUX/readme.txt (english)

/MPC.3.0.011.SIMPLE/
  -- author/s: Niksa Orlic & Artem
  -- description: modified MPC.3.0.0101.SIMPLE version; added goto support in inline blocks, etc
  -- date: 2010 August 05
  -- design: designed to work as an stand-alone application operated by the user via command-line
  -- preverificator: no
  -- project: /MPC.3.0.011.SIMPLE/mpc.vcproj (Visual C++ 8)
  -- readme: /MPC.3.0.011.SIMPLE/readmerus.txt (russian)

/MPC.3.2.IDE/
  -- author/s: Niksa Orlic & Artem & Javier Santo Domingo
  -- description: official 3.2 compiler source code; based on MPC.3.1.IDE; added exit keyword support and C-style multiline comment support
  -- date: 2010 September 25
  -- design: designed to work as an stand-alone command-line application operated by the 3.2 IDE
  -- preverificator: yes
  -- project: /MPC.3.2.IDE/mp3CC.cbp (Code::Blocks 8 / GNUCC)
  -- readme: /MPC.3.2.IDE/readme.txt (english)

/MPC.3.3.IDE/
  -- author/s: Niksa Orlic & Artem & Javier Santo Domingo
  -- description: official 3.3 compiler source code; based on MPC.3.2.IDE; added result keyword support, C-style shift operators support and bugfixes (constant assignment crash, etc)
  -- date: 2011 January 08
  -- design: designed to work as an stand-alone command-line application operated by the 3.3 IDE
  -- preverificator: yes
  -- project: /MPC.3.3.IDE/mp3CC.cbp (Code::Blocks 8 / GNUCC)
  -- readme: /MPC.3.3.IDE/readme.txt (english)

/MPC.3.4.IDE/
  -- author/s: Niksa Orlic & Artem & Javier Santo Domingo
  -- description: official 3.4 compiler source code; based on MPC.3.3.IDE; added Project Library Directory support via -p switch, imported the "ASM BLOCK" from the Artem's MPC.3.0.011.SIMPLE parser.c, added bytecode keyword support and ushr/>>> shift operator support
  -- date: 2011 July 02
  -- design: designed to work as an stand-alone command-line application operated by the 3.4 IDE
  -- preverificator: yes
  -- project: /MPC.3.4.IDE/mp3CC.cbp (Code::Blocks 8 / GNUCC)
  -- readme: /MPC.3.4.IDE/readme.txt (english)

/MPC.3.5.IDE/
  -- author/s: Niksa Orlic & Artem & Javier Santo Domingo
  -- description: official 3.5 compiler source code; based on MPC.3.4.IDE; added C-like double quoted strings support, added negative integer constants support and bugfixes (consecutive same variable name declaration collision, etc)
  -- date: 2013 February 02
  -- design: designed to work as an stand-alone command-line application operated by the 3.5 IDE
  -- preverificator: yes
  -- project: /MPC.3.5.IDE/mp3CC.cbp (Code::Blocks 8 / GNUCC)
  -- readme: /MPC.3.5.IDE/readme.txt (english)


RTL STUBS
---------


/MPS.2.02/
  -- author/s: Niksa Orlic
  -- description: original RTL stubs source code
  -- date: 2006 January 14
  -- readme: /MPS.2.02/readme.txt (english)

/MPS.3.0/
  -- author/s: Niksa Orlic & Javier Santo Domingo
  -- description: official 3.0 RTL stubs source code; based on MPS.2.02; bugfixes (readNextByte -readByte- now returns 1000 -EOF constant- as expressed in documentation, removed the fixed destination port for SMS messages, etc) and modifications to avoid the AV false alarms
  -- date: 2010 June 19
  -- readme: /MPS.3.0/readme.txt (english)

/MPS.3.1/
  -- author/s: Niksa Orlic & Javier Santo Domingo
  -- description: official 3.1 RTL stubs source code; based on MPS.3.0; Roar Lauritzsen's Real.java updated from version 1.07 to 1.13
  -- date: 2010 August 28
  -- readme: /MPS.3.1/readme.txt (english)



You will also find the important chronical events of the project listed in HISTORY.txt (since 2006).

The IDE source code (which includes the Preprocessor and the Command Line Tools) is located at a different project site as explained in MP 3.0 README.txt: http://sourceforge.net/projects/mp3ide/.


Enjoy,

Javier Santo Domingo
02.february.2013
