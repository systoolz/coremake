# coremake
COREMAKE - **CORE'97** and **CORE'98** intros remake.


## I. About

This is **C.O.R.E** (Challenge of Reverse Engineering) intros [CORE'97](https://www.pouet.net/prod.php?which=84291) and [CORE'98](https://www.pouet.net/prod.php?which=70685) remake. This intros reverse engineered and remaked solely for fun and nostalgia. Most of the code disassembled from intros, but expect some slightly differences here and there compared to original behaviour. Greetings to everyone at CORE who did original intros almost 30 years ago!

**COREMAKE.EXE** - compiled COREMAKE intros with Borland Turbo C 2.01 on Compact Model, run this file with any commandline argument to start CORE'98 version (for example without double quotes: "COREMAKE.EXE 8"). This is MS-DOS application so DOS or VM like [DOSBox](https://www.dosbox.com/) required to run. For DOSBox increase cycles or animation will be too slow!

**CORE.NFO** - optional minimal sized file to check CRC function, this file will work with original intros too.


## II. Differences

Changelog or differences between this and original intros:
- rewritten in ANSI C ([Borland Turbo C 2.01](https://web.archive.org/web/20060614091700/http://bdn.borland.com/article/20841)) with full source codes available (no more "Runtime error 200" at startup and also should use less memory)
- merged both CORE'97 and CORE'98 intros into single file (specify any commandline argument to run CORE'98 version)
- all graphics resources converted to greyscale Targa image format
- logo7 shifted horizontally for rotation around skull center (not symmetrical)
- add character "8" to CORE'97 version of font1 file to support both intros (in CORE'98 version of font1 for some reason character "1" was corrupted)
- replaced "\xFA" characters with printable "*" in credit lines
- restored original font2 and made it proportional width except of "*" character (CORE corrupted few characters when reduced font size and made it monospace)
- add special data files with info for both font file images
- all resources moved to an overlay archive and can be optionally packed with LZX (Microsoft "COMPRESS.EXE", as alternative you can use [mscompress](https://gnuwin32.sourceforge.net/packages/mscompress.htm))
- executable file can be optionally packed with [aPACK](https://web.archive.org/web/20230601045543/https://ibsensoftware.com/download.html)

Original CORE intros was created in Borland Pascal and affected with infamous "Runtime error 200" issue but can't be easily fixed since executable files packed with WWPACK (use "[UNP](https://bencastricum.nl/unp/) t filename.exe" to unpack and [BP7FiX](http://prgmizer.chat.ru/bp7fix.htm) to fix after that).


## III. Assets

Origins of identified resources and code used in CORE'97/'98 intros:

- both intros code: by DrRhui [1997 & 1998]
  (as it stated at the ending screen)

- files created by CORE:
  (logo8.tga) CORE logo: author unknown [1998]
  (ansi1.scr) opening text screen: by rad! [1997]
  (ansi2.scr) closing text screen: by rad! [1997]

- rotation and zooming code: aliaspas.pas [1996-04-09]
  > Scaling, Rotation, Antialiasing prototyper. revision 3.
  This program was written by Lewis A. Sellers (Minimalist)
  of The Minimalist Group, and the MOSOCI Grail Project in
  1995-1996 Anno Dominus.

  (cite from "alias.txt" in "aliaspas.pas" code archive [^1])

- (logo7.tga) skull image: death.bmp [1994-11-20]
  > From an old medical book, and subequentially edited,
  filtered, and pixel-edited for effect.

  (cite from "alias.txt" in "aliaspas.pas" code archive [^1])


- (font1.tga) font1: source unknown [before 1998]
  metallic-looking font
  source unknown, author(s) unknown
  please contact if you know the origin or have the original file

- (font2.tga) font2: fontset2.pcx [1993-07-13]
  from the "PCXUtils" - by Patch, Avalanche coder
  graphic by Typhoon (as it states in "PCXUtils.nfo")
  in original file this is a light blue font [^2]

- (advhsc.tpu) HSC/PLAYER V 1.8 FOR TPAS 7 [1994-11-04]
  by Access / Antares / ADV (C) 1994
  (CORE used this to play music [^3])

- (hscobj.bin) HSC player code: hscobj.bin [1994-05-24]
  HSCOBJ 1.4 (C)93/94 by Chicken/ECR
  (linked with previous file [^4])

- (live2.hsc) music file: live2.hsc [1994-05-24]
  (c) by Blackbird/S!P
  (according to "HSCTRACK.DOC" from "HSC-Tracker V1.5" [^4])

- Borland-Pascal 7.0 Runtime Libary Update - Release 1.6 [1994-08-29]
  > Original library code is Copyright (C) 1983,92 Borland International
  New / additional library code is Copyright (C) 1988-1994 Norbert Juffa

  (CORE probably used this for faster and precise floating point arithmetic [^5])

To play .HSC music files in Winamp you'll need "in_adlib.dll" plugin - [AdPlug for Winamp](https://adplug.github.io/) (HSC Adlib Composer / HSC-Tracker).


## IV. License

COREMAKE code (*.C and *.H files) licensed under the Apache License, Version 2.0 (the "License") - see LICENSE file.

> [!NOTE]
> ***Be aware that all other assets (animation code, music, graphics, etc.) are property of their respective owners!***

[^1]: [X2 Support Group Game Development Archives](http://ftp.lanet.lv/ftp/mirror/x2ftp/msdos/programming/demosrc/00index.html) ([alias.zip](http://ftp.lanet.lv/ftp/mirror/x2ftp/msdos/programming/demosrc/alias.zip))

[^2]: [X2 Support Group Game Development Archives](http://ftp.lanet.lv/ftp/mirror/x2ftp/msdos/programming/grutils/00index.html) ([pcxutils.lzh](http://ftp.lanet.lv/ftp/mirror/x2ftp/msdos/programming/grutils/pcxutils.lzh))

[^3]: [X2 Support Group Game Development Archives](http://ftp.lanet.lv/ftp/mirror/x2ftp/msdos/programming/mxlibs/00index.html) ([advhsc18.zip](http://ftp.lanet.lv/ftp/mirror/x2ftp/msdos/programming/mxlibs/advhsc18.zip))

[^4]: [AdPlug - AdLib file formats reference library](https://adplug.github.io/library/entry/HSC.html) ([HSC.zip](https://adplug.github.io/library/files/HSC.zip))

[^5]: [University of Vaasa MS-DOS FTP archives](http://ftp.lip6.fr/pub/pc/garbo/pc/turbopa7/) ([bpl70n16.zip](http://ftp.lip6.fr/pub/pc/garbo/pc/turbopa7/bpl70n16.zip))
