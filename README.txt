In this SVN repository you will find the different compiler versions available under the following structure (useful for quick reference):


/MPC.2.0.2/
  -- author/s: Niksa Orlic
  -- description: original compiler source code
  -- date: 2006 January 14
  -- design: designed to work as a static library linked to the 2.0 IDE
  -- preverificator: yes
  -- project: /MPC.2.0.2/ProjectMobilePascal.dsp (Visual Studio 6)
  -- readme: /MPC.2.0.2/readme.txt (english)


/MPC.3.0.007/
  -- author/s: Niksa Orlic & Artem
  -- description: first enhacements to MPC.2.0.2; includes a new lexer, shl & shr operators, bytecode inlining, smart string concatenation, max array size up to 32767
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
  -- description: official 3.0 compiler source code; based on MPC.3.0.009; ported to GNUCC (with coditional defines), pascal-like errors messages and warnings, new command-line parsing (C way), disabled $R and $V directives (confusing overlapped functionality with the IDE), and several other adjustments (wow64 WM_COPYDATA workaround, etc) and bugfixes (real numbers parsing, etc)
  -- date: 2010 May 25
  -- design: designed to work as an stand-alone command-line application operated by the 3.0 IDE
  -- preverificator: yes
  -- project: /MPC.3.0.IDE/mp3CC.cbp (Code::Blocks 8 / GNUCC)
  -- readme: /MPC.3.0.IDE/readme.txt (english)


Enjoy,

Javier Santo Domingo
25.may.2010
