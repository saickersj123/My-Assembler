<!-- Improved compatibility of back to top link: See: https://github.com/othneildrew/Best-README-Template/pull/73 -->
<a id="readme-top"></a>
<!--
*** Thanks for checking out the Best-README-Template. If you have a suggestion
*** that would make this better, please fork the repo and create a pull request
*** or simply open an issue with the tag "enhancement".
*** Don't forget to give the project a star!
*** Thanks again! Now go create something AMAZING! :D
-->



<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->

[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]

<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/saickersj123/Assembler-2023">
    <img src="images/logo.png" alt="Logo" width="80" height="80">
  </a>

<h3 align="center">My Assembler</h3>

  <p align="center">
    SIC/XE Assembler implementation
  </p>
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#contact">Contact</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project

System Programming Project
SIC/XE Assembler implementation

<p align="right">(<a href="#readme-top">back to top</a>)</p>



### Built With

[![C][C]][MingW-url]


<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- GETTING STARTED -->
## Getting Started

### Prerequisites

* Put your SIC/XE instructions on inst.data
* Instructions Sample
  ```sh
  ADD	   	3	18
  ADDF	3	58
  ADDR	2	90
  AND 	3	40
  BYTE	0	0B
  CLEAR	2	B4
  COMP	3	28
  COMPF	3	88
  COMPR	2	A0
  CSECT	0	0A
  DIV	    3	24
  DIVF	3	64
  DIVR	2	9C
  END	    0	0E
  EQU	    0	09
  EXTDEF	0	02
  EXTREF	0	03
  FIX	    1	C4
  FLOAT	1	C0
  HIO	    1	F4
  J	    3	3C
  JEQ	    3	30
  JGT	    3	34
  JLT	    3	38
  JSUB	3	48
  LDA	    3	00
  LDB	    3	68
  LDCH	3	50
  LDF	    3	70
  LDL	    3	08
  LDS	    3	6C
  LDT	    3	74
  LDX	    3	04
  LPS	    3	D0
  LTORG	0	07
  MUL	    3	20
  MULF	3	60
  MULR	2	98
  NORM	1	C8
  OR	    3	44
  RD	    3	D8
  RESB	0	06
  RESW	0	05
  RMO	    2	AC
  RSUB	3	4C
  SHIFTL	2	A4
  SHIFTR	2	A8
  SIO	    1	F0
  SSK	    3	EC
  STA	    3	0C
  START	0	01
  STB	    3	78
  STCH	3	54
  STF	    3	80
  STI	    3	D4
  STL	    3	14
  STS	    3	7C
  STSW	3	E8
  STT 	3	84
  STX	    3	10
  SUB	    3	1C
  SUBF	3	5C
  SUBR	2	94
  SVC 	2	B0
  TD	    3	E0
  TIO	    1	F8
  TIX	    3	2C
  TIXR	2	B8
  WD	    3	DC
  WORD	0	0D
  ```
* Put your SIX/XE inputs on input.data
* Inputs Sample
  ```sh
  COPY	START	0	
	EXTDEF	BUFFER,BUFEND,LENGTH
	EXTREF	RDREC,WRREC
  FIRST	STL	RETADR	
  CLOOP  	+JSUB	RDREC	
	LDA	LENGTH	
	COMP	#0
	JEQ	ENDFIL	
	+JSUB	WRREC	
	J	CLOOP	
  ENDFIL	LDA	=C'EOF'	
	STA	BUFFER
	LDA	#3	
	STA	LENGTH
	+JSUB	WRREC	
	J	@RETADR	
  RETADR	RESW	1
  LENGTH	RESW	1	
	LTORG
  BUFFER	RESB	4096	
  BUFEND	EQU	*
  MAXLEN	EQU	BUFEND-BUFFER	
  RDREC	CSECT
  .
  .	SUBROUTINE TO READ RECORD INTO BUFFER
  .	
	EXTREF	BUFFER,LENGTH,BUFEND
	CLEAR	X	
	CLEAR	A	
	CLEAR	S	
	LDT	MAXLEN
  RLOOP	TD	INPUT	
	JEQ	RLOOP	
	RD	INPUT	
	COMPR	A,S	
	JEQ	EXIT	
	+STCH	BUFFER,X	
	TIXR	T	
	JLT	RLOOP	
  EXIT	+STX	LENGTH	
	RSUB		
  INPUT	BYTE	X'F1'	
  MAXLEN	WORD	BUFEND-BUFFER
  WRREC	CSECT
  .
  .	SUBROUTINE TO WRITE RECORD FROM BUFFER
  .
	EXTREF	LENGTH,BUFFER
	CLEAR	X	
	+LDT	LENGTH
  WLOOP	TD	=X'05'	
	JEQ	WLOOP	
	+LDCH	BUFFER,X	
	WD	=X'05'	
	TIXR	T	
	JLT	WLOOP	
	RSUB		
	END	FIRST
  ``

### Installation

1. Install GNU Compiler
   https://www.mingw-w64.org/downloads/
2. Change directory
   ```sh
    cd src
   ```
3. Compile the main source
   ```sh
    gcc my_assembler_main.c
   ```
4. Execute the program
   ```sh
   ./a.exe
   ```

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- USAGE EXAMPLES -->
## Usage

This project can be used to understand the process of a computer system comprehend and convert natural languages into machine codes.

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTACT -->
## Contact

Sangjin Park - saickersj123@gmail.com

Project Link: [https://github.com/saickersj123/Assembler-2023](https://github.com/saickersj123/Assembler-2023)

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/saickersj123/Assembler-2023.svg?style=for-the-badge
[contributors-url]: https://github.com/saickersj123/Assembler-2023/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/saickersj123/Assembler-2023.svg?style=for-the-badge
[forks-url]: https://github.com/saickersj123/Assembler-2023/network/members
[stars-shield]: https://img.shields.io/github/stars/saickersj123/Assembler-2023.svg?style=for-the-badge
[stars-url]: https://github.com/saickersj123/Assembler-2023/stargazers
[issues-shield]: https://img.shields.io/github/issues/saickersj123/Assembler-2023.svg?style=for-the-badge
[issues-url]: https://github.com/saickersj123/Assembler-2023/issues
[C]: https://img.shields.io/badge/GCC-000000?style=for-the-badge&logo=c&logoColor=A8B9CC
[MingW-url]: https://sourceforge.net/projects/mingw/