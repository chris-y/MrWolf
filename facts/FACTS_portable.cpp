
/*** PE/CPP/base ***/
/* PortablE target module for C++ */ 



#define _CRT_SECURE_NO_DEPRECATE 1	//silence depreciated warnings of Visual C++
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <string.h>

#define NULLA NULL
#define NULLS NULL
#define NULLL NULL
#define EMPTY (void)0
#define TRUE -1
#define FALSE 0
#define QuadChara(a, b, c, d) ((a << 24) | (b << 16) | (c << 8) | d)
typedef signed char BOOLEAN;	//enum BOOLEAN {FALSE=0, TRUE=-1};
class eException {} eException;

void* FastNew(long size, BOOLEAN noClear);
void* FastDispose(void* mem, long size);

int    main_argc;
char** main_argv;

const long ALL=(long) -1;
class object;
class class_base;
class function;

/* non-native code */

//possible cast functions
//PROC Bool(value)  IS value <> FALSE
//PROC Byte(value)  IS (IF value>=0 THEN (value AND $7F) ELSE -(value AND $7F))!!BYTE
//PROC Char(value)  IS (value AND $FF)!!CHAR
//PROC Int(value)   IS (IF value>=0 THEN (value AND $7FFF) ELSE -(value AND $7FFF))!!INT
//PROC Long(value)  IS value!!LONG
//PROC Quad(value)  IS value!!QUAD
//PROC Float(value) IS value!!FLOAT
//PROC Ptr(value)   IS value!!PTR


class object {
public:
	void* operator new(size_t size) {
		return FastNew(size, FALSE);
	}
	void operator delete(void* mem) {
		FastDispose(mem, -999);
		return;
	}
};

class class_base: public object {
public:
	BOOLEAN notCalledDestructor;
public:
	class_base() {notCalledDestructor=1;}
	virtual ~class_base() {if(notCalledDestructor) {end_class(); notCalledDestructor=0;}}
	virtual void end_class() ;
	virtual long InfoClassType() ;
	virtual BOOLEAN IsSameClassTypeAs(long type) ;
	virtual BOOLEAN IsOfClassType(long parent) ;
};
const long TYPEOF_class_base = (long) "class_base";

class function: public class_base {
public:
	virtual ~function() {if(notCalledDestructor) {end_class(); notCalledDestructor=0;}}
	virtual void new_function() ;
	virtual BOOLEAN IsOfClassType(long parent) ;
	virtual long InfoClassType() ;
};
const long TYPEOF_function = (long) "function";

char* pe_TargetLanguage=NULL;

/* system globals */

long exception;
char* exceptionInfo=NULL;

char quadStr[5];
float retFloat2; long ret2; float retFloat3; long ret3; float retFloat4; long ret4; float retFloat5; long ret5;
void PrintL(char* fmtString, long* args2=NULLL) ;
short Inp(void* fileHandle);
long FileLength(char* path);
BOOLEAN StrCmp(char* first, char* second, long len=ALL, long firstOffset=0, long secondOffset=0);
BOOLEAN StrCmpNoCase(char* first, char* second, long len=ALL, long firstOffset=0, long secondOffset=0);
long Val(char* string, long* addrRead=NULLA) ;
long InStr(char* haystack, char* needle, long startPos=0);
char* UpperStr(char* string);
char* LowerStr(char* string);
void AstrCopy(void* destination, char* source, long destSize);
char* QuadToStr(long value) ;
void* NewR(long size, BOOLEAN noClear=FALSE);
void CleanUp(long returnValue=0);
void* FastNew(long size, BOOLEAN noClear=FALSE) ;
void* FastDispose(void* mem, long size) ;
BOOLEAN FastVerify(BOOLEAN quiet=FALSE) ;
long Rnd(long max) ;
long Mod(long a, long b) ;
long Pow(long a, long b) ;
float RealVal(char* string);
void Throw(long a, char* b=NULLA);
void Raise(long a);
void PrintException();
void new_base() ;
short Inp(void* fileHandle) {
	short char2;
	char2 = getc((FILE*)fileHandle );
	if( char2 == EOF  ) { char2 = -1;}
	return char2;
} 
long FileLength(char* path) {
	long size;
	
	FILE* stream = fopen(path ,"rb");
	if( stream== NULL) {
		size = -1;
	} else {
		fseek(stream, 0, SEEK_END);
		size = ftell(stream);
		fclose(stream);
	}
	return size;
} 

BOOLEAN StrCmp(char* first, char* second, long len, long firstOffset, long secondOffset) {
//IS 0 = (IF len=ALL THEN NATIVE {strcmp(} firstOffset*SIZEOF CHAR + first {,} secondOffset*SIZEOF CHAR + second {)} ENDNATIVE) ELSE (0 = NATIVE {strncmp(} firstOffset*SIZEOF CHAR + first {,} secondOffset*SIZEOF CHAR + second {,} len {)} ENDNATIVE))
	BOOLEAN match;
	if( len == ALL) {
		match = - (strcmp((char*) ((long) (firstOffset*sizeof( char ))+ first ),(char*) ((long) (secondOffset*sizeof( char ))+ second ))== 0) 	;//!!BYTE
	} else {
		match = - (strncmp((char*) ((long) (firstOffset*sizeof( char ))+ first ),(char*) ((long) (secondOffset*sizeof( char ))+ second ),len )== 0) 	;//!!BYTE
	}
	return match;
} 

BOOLEAN StrCmpNoCase(char* first, char* second, long len, long firstOffset, long secondOffset) {
//IS 0 = (IF len=ALL THEN NATIVE {strcasecmp(} firstOffset*SIZEOF CHAR + first {,} secondOffset*SIZEOF CHAR + second {)} ENDNATIVE) ELSE (0 = NATIVE {strncasecmp(} firstOffset*SIZEOF CHAR + first {,} secondOffset*SIZEOF CHAR + second {,} len {)} ENDNATIVE))
	BOOLEAN match;
	if( len == ALL) {
		match = - (strcasecmp((char*) ((long) (firstOffset*sizeof( char ))+ first ),(char*) ((long) (secondOffset*sizeof( char ))+ second ))== 0) 	;//!!BYTE
	} else {
		match = - (strncasecmp((char*) ((long) (firstOffset*sizeof( char ))+ first ),(char*) ((long) (secondOffset*sizeof( char ))+ second ),len )== 0) 	;//!!BYTE
	}
	return match;
} 

long Val(char* string, long* addrRead)  {
	long value, read2;
	char* str=NULL; long i;
	char* final=NULL; signed char base, isNegative;
	long temp_QUAD;
try {
	temp_QUAD = exception ;
	exception = 0 ;
	
	//find start of number (skip any spaces & tabs)
	/*i := 0
	WHILE (string[i]=" ") OR (string[i]="\t")
		i++
	ENDWHILE*/
	i = strspn(string ," \t");
	str = (char*) (string + (long) (i * sizeof( char)));
	
	//determine sign & base of number (and skip their symbols)
	if( (isNegative = - (str[0] == '-')  )) { str++;}
	
	if(      str[0] == '\000') { base =  0	;//string is empty
	} else if( str[0] == '%' ) { base =  2 ; str++;
	} else if( str[0] == '$' ) { base = 16 ; str++;
	} else {                  base = 10;
	}
	
	//interpret value
	if( base == 0) {
		value = 0;
		read2  = 0;
	} else {
		errno = 0;
		if( base == 10) {
			value = strtol(str ,&final ,base );
		} else {
			value = strtoul(str ,&final ,base );
		}
		if( errno== 0) {
			read2  = (long) ((long) (char*) (final - (long) string)  / sizeof( char ));
		} else {
			value = 0;
			read2  = 0;
		}
	}
	
	if( addrRead ) { addrRead[0] = (long) read2 ;}
	if( isNegative ) { value = -value;}
} catch(...) {}
	ret2 = read2 ;
	if (exception!=0) {throw eException;} else {EMPTY;};
	exception = temp_QUAD ;
	return value ;
} 

long InStr(char* haystack, char* needle, long startPos) {
	long foundPos;
	char* start=NULL; char* found=NULL;
	
	start = (char*) (haystack + (long) (startPos * sizeof( char)));
	
	found = strstr(start ,needle );
	if( found) {
		foundPos = (long) ((long) (char*) (found - (long) haystack)  / sizeof( char));
	} else {
		foundPos = -1;
	}
	return foundPos;
} 
char* UpperStr(char* string) {
	long i; char chara;
	i = 0;
	while( chara = string[i]) {
		if( - (chara >= 'a')  & - (chara <= 'z')  ) { string[i] = chara + 'A' - 'a';}
		i++;
	}
	return string;
} 
char* LowerStr(char* string) {
	long i; char chara;
	i = 0;
	while( chara = string[i]) {
		if( - (chara >= 'A')  & - (chara <= 'Z')  ) { string[i] = chara + 'a' - 'A';}
		i++;
	}
	return string;
} 
void AstrCopy(void* destination, char* source, long destSize) {
	strncpy((char*) destination ,source ,destSize-1 );
	((char*) destination )[destSize - 1 ] = 0;
	return ;
}
//OstrCmp()
//OstrCmpNoCase()

char* QuadToStr(long value)  {
	char* string=NULL;
	quadStr[0] = (char) (( 32>(value >> 24 & 0xFF ))? (long) 32 : value >> 24 & 0xFF);
	quadStr[1] = (char) (( 32>(value >> 16 & 0xFF ))? (long) 32 : value >> 16 & 0xFF);
	quadStr[2] = (char) (( 32>(value >>  8 & 0xFF ))? (long) 32 : value >>  8 & 0xFF);
	quadStr[3] = (char) (( 32>(value >>  0 & 0xFF ))? (long) 32 : value >>  0 & 0xFF);
	quadStr[4] = 0;
	
	string = quadStr;
	return string ;
} 
void* NewR(long size, BOOLEAN noClear) {
	void* mem=NULL;
	mem = ( noClear )? malloc(size ): memset(malloc(size ), 0,size );
	if( mem == NULL ) { Raise(QuadChara(0,'M','E','M'));}
	return mem;
} 
void CleanUp(long returnValue) {
	Throw(-1, (char*) returnValue)	;//use reserved exception -1 for CleanUp()
	return ;
}
long Rnd(long max)  {
	long num;
	if( max >= 0) {
		num = ((rand() ) % (max ));
	} else {
		srand(abs(max ));
		num = 0;
	}
	return num ;
} 
long Mod(long a, long b)  {
	long c, d;
	long temp_QUAD;
try {
	temp_QUAD = exception ;
	exception = 0 ;
	d = a / b;
	c = a - d * b;
} catch(...) {}
	ret2 = d ;
	if (exception!=0) {throw eException;} else {EMPTY;};
	exception = temp_QUAD ;
	return c ;
} 
/*->ldiv() simply does not work on too many compilers
PROC Mod(a, b) RETURNS c, d
	{ldiv_t temp = ldiv(} a {,} b {)}
	c := {temp.rem}!!VALUE
	d := {temp.quot}!!VALUE
ENDPROC*/
long Pow(long a, long b)  {
	long c;
	c = 1;
	while( b > 0) {
		c = c * a;
		b--;
	}
	return c ;
} 
float RealVal(char* string) {
	float value; long read2;
	char* final=NULL;
	long temp_QUAD;
try {
	temp_QUAD = exception ;
	exception = 0 ;
	
	errno = 0;
	value = strtod(string ,&final );
	if( errno== 0) {
		read2  = (long) ((long) (char*) (final - (long) string)  / sizeof( char));
	} else {
		value = 0.0;
		read2  = 0;
	}
} catch(...) {}
	ret2 = read2;
	if (exception!=0) {throw eException;} else {EMPTY;};
	exception = temp_QUAD ;
	return value;
} 

//Does not work in StormC: IS (exception := a) BUT (exceptionInfo := b) BUT {throw new Exception()}
void Throw(long a, char* b) {
	exception     = a;
	exceptionInfo = b;
	throw eException;
	return ;
}
void Raise(long a) {
	exception = a;
	throw eException;
	return ;
}

void PrintException() {
	if( - (exception != 0)  & - (exception != -1)) {
		if( exceptionInfo) {
			printf("EXCEPTION: \"%s\"; %s.\n" ,(long) QuadToStr(exception) ,(long) exceptionInfo ,0 ,0 ,0 ,0 ,0 ,0 );
		} else {
			printf("EXCEPTION: \"%s\".\n" ,(long) QuadToStr(exception) ,0 ,0 ,0 ,0 ,0 ,0 ,0 );
		}
	}
	return ;
}
void new_base()  {
	pe_TargetLanguage = (char*) "CPP";
	return ;
}
void class_base::end_class()  {
	EMPTY;
}
long class_base::InfoClassType()  {
	return TYPEOF_class_base;
}
BOOLEAN class_base::IsSameClassTypeAs(long type)  {
	return - (type == this->InfoClassType());
}
BOOLEAN class_base::IsOfClassType(long parent)  {
	return - (parent == TYPEOF_class_base);
}
void function::new_function()  {
	EMPTY;
}
BOOLEAN function::IsOfClassType(long parent)  {
	return ( parent == TYPEOF_function)? TRUE : this ->class_base:: IsOfClassType(parent );
}
long function::InfoClassType()  {
	return TYPEOF_function;
}

/*** target/PE/base ***/
/* PortablE target module for C++ AmigaOS */ 

/* system constants */

const short OLDFILE=1005;
const short NEWFILE=1006;

char* pe_TargetOS=NULL;
void* SetStdIn(void* fileHandle) ;
void* SetStdOut(void* fileHandle) ;
void new_base2() ;
/* stdin & stdout */

void* SetStdIn(void* fileHandle)  {
	void* oldstdin=NULL;
	oldstdin = (void*) stdin;
	
	#ifdef __NEWLIB_H__
		stdin = (FILE*) fileHandle;
	#else
		__iob[0] = (iob*) fileHandle ;
	#endif
	;
	return oldstdin ;
} 

void* SetStdOut(void* fileHandle)  {
	void* oldstdout=NULL;
	oldstdout = (void*) stdout;
	
	#ifdef __NEWLIB_H__
		stdout = (FILE*) fileHandle;
	#else
		__iob[1] = (iob*) fileHandle ;
	#endif
	;
	return oldstdout ;
} 
void new_base2()  {
	pe_TargetOS = (char*) "AmigaOS4";
	return ;
}

/*** PE/EndianShared ***/
/* PE/EndianShared.e 27-04-2010
   Endian-swapping routines shared by all targets.
*/ 
long long SwapEndianBIGVALUE(long long in);
/*PROC SwapEndianLONG(in:LONG)
	DEF out:LONG
	DEF p1:LONG, p2:LONG, p3:LONG, p4:LONG
	p1 := in SHL 24 AND $FF000000
	p2 := in SHL  8 AND $00FF0000
	p3 := in SHR  8 AND $0000FF00
	p4 := in SHR 24 AND $000000FF
	out := p1 OR p2 OR p3 OR p4
ENDPROC out*/

long long SwapEndianBIGVALUE(long long in) {
	long long out;
	long long* bigValue=NULL; long buffer2[2]; long temp2;
	
	bigValue = (long long*) buffer2 ;
	bigValue[0] = in;
	
	temp2      = (long) (buffer2[0] << 24 & (long) 0xFF000000  | buffer2[0] <<  8 & 0x00FF0000  | buffer2[0] >>  8 & 0x0000FF00  | buffer2[0] >> 24 & 0x000000FF  );
	buffer2[0] = (long) (buffer2[1] << 24 & (long) 0xFF000000  | buffer2[1] <<  8 & 0x00FF0000  | buffer2[1] >>  8 & 0x0000FF00  | buffer2[1] >> 24 & 0x000000FF  );
	buffer2[1] = temp2;
	
	out = bigValue[0];
	return out;
} 

/*** PE/EString_partial ***/
/* PE/EString_partial.e 13-01-11
   A re-implementation of AmigaE's E-string functions.
   
   By Christopher S Handley:
   10-03-02 - Started coding it, to replace the existing AmigaE functions.
   19-03-02 - Mostly completed.
   14-07-06 - Ported to PortablE.
   27-07-06 - Updated to use the STRING type.
   27-10-07 - Fixed Next() & Link() bugs.
   19-04-09 - StrJoin() added earlier but not documented until now.
   09-10-09 - Added check to prevent source & destination being the same.  Problem reported by Matthias Rustler.
   03-05-10 - Declared the "missing" procedures as prototypes.  Added StringFL().
   13-01-11 - RealF() decimalPlaces is no-longer a BYTE.
*/ /* Emulated procedures:
NewString(maxLen) RETURNS eString:STRING
DisposeString(eString:STRING) RETURNS NILS
StrCopy( eString:STRING, string:ARRAY OF CHAR, len=ALL, pos=0) RETURNS eString:STRING
StrAdd(  eString:STRING, string:ARRAY OF CHAR, len=ALL, pos=0) RETURNS eString:STRING
StrJoin(s1=NILA:ARRAY OF CHAR, s2=NILA:ARRAY OF CHAR, s3=NILA:ARRAY OF CHAR, s4=NILA:ARRAY OF CHAR, s5=NILA:ARRAY OF CHAR, s6=NILA:ARRAY OF CHAR, s7=NILA:ARRAY OF CHAR, s8=NILA:ARRAY OF CHAR, s9=NILA:ARRAY OF CHAR, s10=NILA:ARRAY OF CHAR, s11=NILA:ARRAY OF CHAR) RETURNS newString:STRING
EstrLen( eString:STRING) RETURNS len:VALUE
StrMax(  eString:STRING) RETURNS max:VALUE
RightStr(eString:STRING, eString2:ARRAY OF CHAR, n) RETURNS eString:STRING
MidStr(  eString:STRING, string:ARRAY OF CHAR, pos, len=ALL) RETURNS eString:STRING
SetStr(  eString:STRING, newLen)
Link(    complex:STRING, tail:OWNS STRING) RETURNS complex:STRING
Next(    complex:STRING) RETURNS tail:STRING
Forward( complex:STRING, num) RETURNS tail:STRING

On-purposely missing procedures:
ReadStr(fileHandle:PTR, eString:STRING) RETURNS fail:BOOL
StringF( eString:STRING, fmtString:ARRAY OF CHAR, ...)             RETURNS eString:STRING, len
StringFL(eString:STRING, fmtString:ARRAY OF CHAR, args=NILL:ILIST) RETURNS eString:STRING, len
RealF(   eString:STRING, value:FLOAT, decimalPlaces=8) RETURNS eString:STRING
*/
class pEString;


class pEString: public object {
public:
	long length;                 	//length of actual string (excluding terminating zero)
	long size;                  	//max length of string    (including terminating zero)
	pEString* next;	//points to next string header, not the actual string
};
char* NewString(long maxLen);
char* DisposeString(char* eString);
char* StrCopy(char* eString, char* string, long len=ALL, long pos=0);
char* StrAdd(char* eString, char* string, long len=ALL, long pos=0);
char* StrJoin(char* s1=NULLA, char* s2=NULLA, char* s3=NULLA, char* s4=NULLA, char* s5=NULLA, char* s6=NULLA, char* s7=NULLA, char* s8=NULLA, char* s9=NULLA, char* s10=NULLA, char* s11=NULLA, char* s12=NULLA, char* s13=NULLA, char* s14=NULLA, char* s15=NULLA, char* s16=NULLA, char* s17=NULLA, char* s18=NULLA, char* s19=NULLA);
long EstrLen(char* eString);
long StrMax(char* eString);
char* RightStr(char* eString, char* eString2, long n);
char* MidStr(char* eString, char* string, long pos, long len=ALL);
void SetStr(char* eString, long newLen);
BOOLEAN ReadStr(void* fileHandle, char* eString) ;
char* StringF(char* eString, char* fmtString, long arg1=0, long arg2=0, long arg3=0, long arg4=0, long arg5=0, long arg6=0, long arg7=0, long arg8=0) ;
char* StringFL(char* eString, char* fmtString, long* args2=NULLL) ;
char* RealF(char* eString, float value, long decimalPlaces=8) ;
char* Link(char* complex, char* tail);
char* Next(char* complex);
char* Forward(char* complex, long num);


char* NewString(long maxLen) {
	char* eString=NULL;
	pEString* pEString2=NULL;	
	long sizeOfEString;
	
	//use check
	if( maxLen < 0 ) { Throw(QuadChara(0,'E','P','U'), (char*) "EString; NewString(); maxLen<0");}
	
	//allocate eString
	sizeOfEString = (maxLen + 1 )* sizeof( char)  + sizeof( pEString);
	pEString2 = (pEString*) FastNew(sizeOfEString, TRUE);
	
	//init
	pEString2->length = 0;
	pEString2->size   = maxLen + 1;
	pEString2->next   = (pEString*) NULL;
	
	//retrieve string after header
	eString = (char*) ((char*) pEString2 + (long) sizeof( pEString ));
	
	//zero-terminate empty string
	eString[0] = '\000';
	return eString;
} 

char* DisposeString(char* eString) {
	pEString* pEString2=NULL;
	pEString* next=NULL;
	
	if( eString) {
		//retrieve string header
		pEString2 = (pEString*) (eString - (long) sizeof( pEString ));
		
		//loop through all strings in linked list
		do {
			//store any string tail
			next = pEString2->next;
			
			//dealloc string
			pEString2 = (pEString*) FastDispose(pEString2, pEString2->size * sizeof( char)  + sizeof( pEString));
			
			//move to tail
			pEString2 = next;
		} while(( (void*) pEString2 == NULL)==0);
	}
	return NULLS;
} 

char* StrCopy(char* eString, char* string, long len, long pos) {
	pEString* pEString2=NULL;
	
	//use check
	if( eString == NULLS ) { Throw(QuadChara(0,'E','P','U'), (char*) "EString; StrCopy(); eString=NILS");}
	if(  (void*) string == NULLA ) { Throw(QuadChara(0,'E','P','U'), (char*) "EString; StrCopy(); string=NILA");}
	if( eString == string ) { Throw(QuadChara(0,'E','P','U'), (char*) "EString; StrCopy(); source & destination must not be the same");}
	if( - (len < 0)  & - (len != ALL)  ) { Throw(QuadChara(0,'E','P','U'), (char*) "EString; StrCopy(); len<0");}
	if( pos < 0) { Throw(QuadChara(0,'E','P','U'), (char*) "EString; StrCopy(); pos<0");}
	
	//retrieve string header
	pEString2 = (pEString*) (eString - (long) sizeof( pEString ));
	
	//empty e-string before appending to it
	pEString2->length = 0;
	eString[0] = '\000';
	
	StrAdd(eString, string, len, pos);
	return eString;
} 

char* StrAdd(char* eString, char* string, long len, long pos) {
	pEString* pEString2=NULL;
	long readIndex, maxReadIndex;
	long writeIndex, maxWriteIndex;
	
	//use check
	if( eString == NULLS ) { Throw(QuadChara(0,'E','P','U'), (char*) "EString; StrAdd(); eString=NILS");}
	if(  (void*) string == NULLA ) { Throw(QuadChara(0,'E','P','U'), (char*) "EString; StrAdd(); string=NILA");}
	if( eString == string ) { Throw(QuadChara(0,'E','P','U'), (char*) "EString; StrAdd(); source & destination must not be the same");}
	if( - (len < 0)  & - (len != ALL)  ) { Throw(QuadChara(0,'E','P','U'), (char*) "EString; StrAdd(); len<0");}
	if( pos < 0) { Throw(QuadChara(0,'E','P','U'), (char*) "EString; StrAdd(); pos<0");}
	
	//retrieve string header
	pEString2 = (pEString*) (eString - (long) sizeof( pEString ));
	
	//calc end of string reading from & writing to (inc zero termination)
	maxReadIndex  = pos + (( len==ALL )? pEString2->size - 1 : len);
	maxWriteIndex = pEString2->size - 1;
	
	//copy all characters that will fit
	readIndex  = pos;
	writeIndex = pEString2->length	;//start writing past end of string
	while( - (string[readIndex] != 0)  & - (writeIndex < maxWriteIndex)  & - (readIndex < maxReadIndex)) {
		eString[writeIndex] = string[readIndex];
		
		writeIndex++;
		readIndex++;
	}
	
	//update string's stored length
	pEString2->length = writeIndex;
	eString[writeIndex] = '\000';
	return eString;
} 

char* StrJoin(char* s1, char* s2, char* s3, char* s4, char* s5, char* s6, char* s7, char* s8, char* s9, char* s10, char* s11, char* s12, char* s13, char* s14, char* s15, char* s16, char* s17, char* s18, char* s19) {
	char* newString=NULL;
	long len;
	
	len = 0;
	if( s1 ) { len = len + strlen(s1 );}
	if( s2 ) { len = len + strlen(s2 );}
	if( s3 ) { len = len + strlen(s3 );}
	if( s4 ) { len = len + strlen(s4 );}
	if( s5 ) { len = len + strlen(s5 );}
	if( s6 ) { len = len + strlen(s6 );}
	if( s7 ) { len = len + strlen(s7 );}
	if( s8 ) { len = len + strlen(s8 );}
	if( s9 ) { len = len + strlen(s9 );}
	if( s10 ) { len = len + strlen(s10 );}
	if( s11 ) { len = len + strlen(s11 );}
	if( s12 ) { len = len + strlen(s12 );}
	if( s13 ) { len = len + strlen(s13 );}
	if( s14 ) { len = len + strlen(s14 );}
	if( s15 ) { len = len + strlen(s15 );}
	if( s16 ) { len = len + strlen(s16 );}
	if( s17 ) { len = len + strlen(s17 );}
	if( s18 ) { len = len + strlen(s18 );}
	if( s19 ) { len = len + strlen(s19 );}
	
	newString= NewString(len);
	if( s1 ) { StrAdd(newString, s1);}
	if( s2 ) { StrAdd(newString, s2);}
	if( s3 ) { StrAdd(newString, s3);}
	if( s4 ) { StrAdd(newString, s4);}
	if( s5 ) { StrAdd(newString, s5);}
	if( s6 ) { StrAdd(newString, s6);}
	if( s7 ) { StrAdd(newString, s7);}
	if( s8 ) { StrAdd(newString, s8);}
	if( s9 ) { StrAdd(newString, s9);}
	if( s10 ) { StrAdd(newString, s10);}
	if( s11 ) { StrAdd(newString, s11);}
	if( s12 ) { StrAdd(newString, s12);}
	if( s13 ) { StrAdd(newString, s13);}
	if( s14 ) { StrAdd(newString, s14);}
	if( s15 ) { StrAdd(newString, s15);}
	if( s16 ) { StrAdd(newString, s16);}
	if( s17 ) { StrAdd(newString, s17);}
	if( s18 ) { StrAdd(newString, s18);}
	if( s19 ) { StrAdd(newString, s19);}
	return newString;
} 

long EstrLen(char* eString) {
	long len;
	pEString* pEString2=NULL;
	
	//use check
	if( eString == NULLS ) { Throw(QuadChara(0,'E','P','U'), (char*) "EString; EstrLen(); eString=NILS");}
	
	//retrieve string header
	pEString2 = (pEString*) (eString - (long) sizeof( pEString ));
	len = pEString2->length;
	return len;
} 

long StrMax(char* eString) {
	long max;
	pEString* pEString2=NULL;
	
	//use check
	if( eString == NULLS ) { Throw(QuadChara(0,'E','P','U'), (char*) "EString; StrMax(); eString=NILS");}
	
	//retrieve string header
	pEString2 = (pEString*) (eString - (long) sizeof( pEString ));
	max = pEString2->size - 1;
	return max;
} 

char* RightStr(char* eString, char* eString2, long n) {
	pEString* pEString2=NULL;
	char* readString=NULL;
	
	//use check
	if( eString  == NULLS ) { Throw(QuadChara(0,'E','P','U'), (char*) "EString; RightStr(); eString=NILS");}
	if( eString2 == NULLS ) { Throw(QuadChara(0,'E','P','U'), (char*) "EString; RightStr(); eString2=NILS");}
//	if( eString == eString2 ) { Throw(QuadChara(0,'E','P','U'), (char*) "EString; RightStr(); source & destination must not be the same");}
	if( n < 0           ) { Throw(QuadChara(0,'E','P','U'), (char*) "EString; RightStr(); n<0");}
	
	//retrieve string header
	pEString2 = (pEString*) (eString2 - (long) sizeof( pEString ));
	
	//restrict n to sensible range
	if( n > pEString2->length ) { n = pEString2->length;}
	
	//move to start of n characters
	readString = (char*) (eString2 + (long) ((pEString2->length - n) * sizeof( char)) );
	
	//use strCopy procedure
	StrCopy(eString, readString, n);
	return eString;
} 

char* MidStr(char* eString, char* string, long pos, long len) {
	long index; char* readString=NULL;
	
	//use check
	if( eString == NULLS ) { Throw(QuadChara(0,'E','P','U'), (char*) "EString; MidStr(); eString=NILS");}
	if(  (void*) string == NULLA ) { Throw(QuadChara(0,'E','P','U'), (char*) "EString; MidStr(); string=NILA");}
	if( eString == string ) { Throw(QuadChara(0,'E','P','U'), (char*) "EString; MidStr(); source & destination must not be the same");}
	if( pos < 0        ) { Throw(QuadChara(0,'E','P','U'), (char*) "EString; MidStr(); pos<0");}
	if( - (len < 0)  & - (len != ALL)  ) { Throw(QuadChara(0,'E','P','U'), (char*) "EString; MidStr(); len<0");}
	
	//find correct start position SAFELY (which is more than AmigaE does!)
	index = 0;
	while( - (string[index] != 0)  & - (pos > 0)) {
		index++;
		pos--;
	}
	
	readString = (char*) (string + (long) (index * sizeof( char)) );
	
	//copy specified part of string
	StrCopy(eString, readString, len);
	return eString;
} 

void SetStr(char* eString, long newLen) {
	pEString* pEString2=NULL;
	
	//use check
	if( eString == NULLS ) { Throw(QuadChara(0,'E','P','U'), (char*) "EString; SetStr(); eString=NILS");}
	if( newLen < 0     ) { Throw(QuadChara(0,'E','P','U'), (char*) "EString; SetStr(); newLen<0");}
	
	//retrieve string header
	pEString2 = (pEString*) (eString - (long) sizeof( pEString ));
	
	//additional use check
	if( newLen >= pEString2->size ) { Throw(QuadChara(0,'E','P','U'), (char*) "EString; SetStr(); newLen exceeds string size");}
	
	//set length
	pEString2->length = newLen;
	eString[newLen] = '\000';
	return ;
}

char* Link(char* complex, char* tail) {
	pEString* pEString2=NULL;
	
	//use check
	if( complex == NULLS ) { Throw(QuadChara(0,'E','P','U'), (char*) "EString; Link(); complex=NILS");}
	
	//retrieve string header
	pEString2 = (pEString*) (complex - (long) sizeof( pEString ));
	
	//store tail's header
	pEString2->next = (pEString*) (( tail )? (void*) (tail - (long) sizeof( pEString )): NULL);
	return complex;
} 

char* Next(char* complex) {
	char* tail=NULL;
	pEString* pEString2=NULL;
	
	if( complex == NULLS ) {
		tail= NULLS;
		goto finally;
	}
	
	//retrieve string header
	pEString2 = (pEString*) (complex - (long) sizeof( pEString ));
	
	//return tail with hidden header
	pEString2 = pEString2->next;
	if( (void*) pEString2 == NULL ) {
		tail = NULLS ;
	} else { 
		tail = (char*) ((char*) pEString2 + (long) sizeof( pEString ));
	}
finally: 0; 
	return tail;
} 

char* Forward(char* complex, long num) {
	char* tail=NULL;
	pEString* pEString2=NULL;
	
	//use check
	if( num < 0        ) { Throw(QuadChara(0,'E','P','U'), (char*) "EString; Forward(); num<0");}
	
	if( complex == NULLS ) {
		tail= NULLS;
		goto finally;
	}
	
	//retrieve string header
	pEString2 = (pEString*) (complex - (long) sizeof( pEString ));
	
	while( - ((void*) pEString2 != NULL)  & - (num > 0)) {
		pEString2 = pEString2->next;
		num--;
	}
	
	//retrieve string after header
	if( (void*) pEString2 == NULL ) {
		tail = NULLS ;
	} else { 
		tail = (char*) ((char*) pEString2 + (long) sizeof( pEString ));
	}
finally: 0; 
	return tail;
} 

/*** PE/pThreadNode_prototypes ***/
/* prototypes of pThreadNode */ 
class threadNode;

class threadNode: public object {
public:
	long threadID;
	threadNode* next;	//circular list
};
threadNode* FindThreadNode(threadNode** head2, long sizeOfThreadNode) ;
void EndAllThreadNodes(threadNode** head2) ;

/*** PE/pSemaphores_prototypes ***/
/* prototypes of pSemaphores */ 
void* NewSemaphore() ;
void* DisposeSemaphore(void* sem2) ;
void SemLock(void* sem2) ;
void SemUnlock(void* sem2) ;
BOOLEAN SemTryLock(void* sem2) ;

/*** PE/Char ***/
/* PE/Char.e 06-06-08
*/ 

/*** PE/OstrCmp ***/



//Replacement for AmigaE's OstrCmp(), which (possibly erratically) seems to incorrectly think these two characters are the same:
// "A" = $41 = 065 = %01000001
// "«" = $AB = 171 = %10101011

signed char OstrCmp(char* string1, char* string2, long max=ALL, long string1Offset=0, long string2Offset=0);
signed char OstrCmpNoCase(char* string1, char* string2, long max=ALL, long string1Offset=0, long string2Offset=0);

signed char OstrCmp(char* string1, char* string2, long max, long string1Offset, long string2Offset) {
	signed char sign;
	short order; char char1; long index;
	
	string1 = (char*) ((long) (string1Offset*sizeof( char ))+ string1);
	string2 = (char*) ((long) (string2Offset*sizeof( char ))+ string2);
	
	index = 0;
	if( - (index < max)  | - (max==ALL)) {
		do {
			char1 = string1[index];
			order = string2[index] - char1		;//sign indicates order
			
			index++;
		} while(( - (order!=0)  | - (char1==0)  | - (index >= max)  & - (max!=ALL) 	)==0);//char1=0 catches case where both strings are same length
	} else {
		order = 0;
	}
	
	sign = ( order==0 )? 0 : ( order<0 )? -1 : 1;
	return sign;
} 

//This is like OstrCmp() but it does not care about letter case
signed char OstrCmpNoCase(char* string1, char* string2, long max, long string1Offset, long string2Offset) {
	short order; char char1, char2; long index;
	
	string1 = (char*) ((long) (string1Offset*sizeof( char ))+ string1);
	string2 = (char*) ((long) (string2Offset*sizeof( char ))+ string2);
	
	index = 0;
	if( - (index < max)  | - (max==ALL)) {
		do {
			char1=string1[index];
			char2=string2[index];
			
			if( - (char1>='a')  & - (char1<='z')  ) { char1 = char1 - 'a' + 'A';}
			if( - (char2>='a')  & - (char2<='z')  ) { char2 = char2 - 'a' + 'A';}
			
			order=char2 - char1	;//sign indicates order
			
			index++;
		} while(( - (order!=0)  | - (char1==0)  | - (index >= max)  & - (max!=ALL) 	)==0);//char1=0 catches case where both strings are same length
	} else {
		order = 0;
	}
	return ( order==0 )? 0 : ( order<0 )? -1 : 1;
} 

/*** PE/EndianBig ***/
/* PE/EndianBig.e 27-04-10
   For use on big-endian machines.
*/ 

/*** PE/CPP/EString ***/
/* PortablE target module that completes EStrings */ /* missing E-string functions */
char* appendDecimal(char* eString, long value, long minWidth);

char* StringF(char* eString, char* fmtString, long arg1, long arg2, long arg3, long arg4, long arg5, long arg6, long arg7, long arg8)  {
	long max, len;
	long temp_QUAD;
try {
	temp_QUAD = exception ;
	exception = 0 ;
	
	max = StrMax(eString);
	len = snprintf(eString ,max+1 ,fmtString ,arg1 ,arg2 ,arg3 ,arg4 ,arg5 ,arg6 ,arg7 ,arg8 );
	len = ( len<max )? len : max;
	SetStr(eString, len);
} catch(...) {}
	ret2 = len;
	if (exception!=0) {throw eException;} else {EMPTY;};
	exception = temp_QUAD ;
	return eString;
} 

BOOLEAN ReadStr(void* fileHandle, char* eString)  {
	BOOLEAN fail; 
	fail = - (fgets(eString ,StrMax(eString)+1 , (FILE*) fileHandle )== NULL);
	SetStr(eString, strlen(eString ));
	return fail ;
} 

/*->snprintf() simply does not work on too many compilers, for floating-point
PROC RealF(eString:STRING, value:FLOAT, decimalPlaces=8) REPLACEMENT
	DEF max, len
	
	max := StrMax(eString)
	len := NATIVE {snprintf(} eString {,} max+1 {, "%.*f",} decimalPlaces {,} value {)} ENDNATIVE !!VALUE
	SetStr(eString, Min(len, max))
ENDPROC eString
*/


char* RealF(char* eString, float value, long decimalPlaces)  {
	float integer; long nextDecimalPlaces;
	char* temp_ARRAY_OF_CHAR=NULL;
	
	if( value>0 ) {
		temp_ARRAY_OF_CHAR = (char*) "";
	} else { 
		temp_ARRAY_OF_CHAR = (char*) "-";
	}
	StrCopy(eString, temp_ARRAY_OF_CHAR );
	value = fabs(value );
	
	integer = floor(value );
	appendDecimal(eString, (long) integer, 0);
	
	if( decimalPlaces > 0) {
		StrAdd(eString, (char*) ".");
		
		value = value - integer;
		do {
			nextDecimalPlaces = ( decimalPlaces<9 )? decimalPlaces : (long) 9;
			decimalPlaces = decimalPlaces - nextDecimalPlaces;
			
			value = value * (float) Pow(10, nextDecimalPlaces);
			integer = floor(value );
			appendDecimal(eString, (long) integer, nextDecimalPlaces);
			value = value - integer;
		} while(( decimalPlaces <= 0)==0);
	}
	return eString;
} 


char* appendDecimal(char* eString, long value, long minWidth) {
	BOOLEAN isNegative; char temp2[12]; signed char pos; long digit;
	
	//use check
	if( (void*) eString == NULLA ) { Throw(QuadChara(0,'E','P','U'), (char*) "PE/CPP/EString; appendDecimal(); eString=NILA");}
	
	//force value to be positive
	if( value >= 0) {
		isNegative = FALSE;
	} else {
		isNegative = TRUE;
		value = 0 - value		;//make value positive
	}
	
	//end string with terminating zero to make it valid
	pos = 11;
	temp2[pos] = '\000';
	
	//write string (from end of string) representing value as decimal digits
	if( value == 0) {
		pos--;
		temp2[pos] = '0';
		minWidth--;
	} else {
		do {
			//extract right-most digit then remove from value
			digit= Mod(value, 10);
			value = ret2 ;
			
			//write digit as character
			pos--;
			temp2[pos] = (char) ('0' + digit );
			minWidth--;
		} while(( value == 0)==0);
		
		//prepend minus sign if was negative
		if( isNegative) {
			pos--;
			temp2[pos] = '-';
		}
	}
	
	//prepend required 0 digits
	while( minWidth > 0) {
		pos--;
		temp2[pos] = '0';
		minWidth--;
	}
	
	//now append string representation to target E-string
	StrAdd(eString, temp2, ALL, pos);
	return eString;
} 

/*** PE/FastMem ***/
/* PE/FastMem.e 20-12-10
	An incredibly fast memory allocator, with O(1) performance.


Copyright (c) 2009,2010 Christopher Steven Handley ( http://cshandley.co.uk/email )
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* The source code must not be modified after it has been translated or converted
away from the PortablE programming language.  For clarification, the intention
is that all development of the source code must be done using the PortablE
programming language.

* Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/ /*
	By Christopher S Handley.
	16-08-09 - Started (TLSF took 1 day to implement).
	30-08-09 - Completed.
	12-09-09 - Switched from ThreadNode to pSemaphores.
	21-08-10 - Added FastVerify() to help track memory corruption.
	06-12-10 - Cleaned-up for release.
	20-12-10 - Added memory tracing recording.
*//*
This allocator is based upon the TLSF algorithm, but adapted to work on a
per-program basis (rather than the per-OS basis that it was intended for).  It
adds space (aka "pools") as needed, starting off at 64KB, and gradually
increasing up to 16MB.  Pools are removed when they become empty.

Unfortunately the TLSF algorithm has an overhead of 8 bytes per block, which is
unacceptable for small allocations, plus it's O(1) constant time allocation is
still quite large.  So on top of it there is a "Slab" allocator (loosly based
upon the real Slab allocator & also upon AmigaE's FastNew algorithm), which for
blocks <= 256 bytes will pre-allocate 16 blocks of identical size in a single
slab.  The downside is that blocks > 256 bytes now have an overhead of 12
bytes per block.  But the upside is that blocks <= 256 bytes only have an
overhead of 2.875 bytes per block, and their O(1) constant allocation time is
small.
*/


//### #define DEBUG DEBUG_
//### #define RECORDTRACE


const BOOLEAN DEBUG=FALSE ;
const BOOLEAN DISABLE_AUTOPOOLS=FALSE;
const BOOLEAN DISABLE_SLABS=FALSE;


/*---------------------------------------------------------------------------------*/	//TLSF allocator

const signed char SL_BITWIDTH=5	;//must be between 1 & 32 inclusive, but reasonable values are 4 or 5
const long MINPOOLSIZE=65536	;//64KB
const long MAXPOOLSIZE=16777216	;//16MB


const signed char SL_SIZE=1 << SL_BITWIDTH;
class secondLevel;
class arena;
class pool;


const signed char SIZEMASK_ISLASTBLOCK=0x1/*01*/;
const signed char SIZEMASK_ISFREEBLOCK=0x2/*10*/;
const signed char SIZEMASK_SIZEINBYTES=~ 0x3/*11*/;

const signed char SIZEROUNDUP=~ SIZEMASK_SIZEINBYTES;
class block;
class usedBlock;
class freeBlock;


const signed char ALIGN_SIZE=sizeof( long	);//must be a power of 2, and >= 4
const signed char ALIGN_SIZE_M1=ALIGN_SIZE - 1;


/*---------------------------------------------------------------------------------*/	//Slab allocator layer

const short MAX_SLABBLOCKSIZE=((2 + 256) + ALIGN_SIZE_M1 & ~ ALIGN_SIZE_M1)				;//must be a multiple of 4, and <= 504 (which is "127*ALIGN_SIZE - AlignRoundUp(SIZEOF slabBlock)", for slabIndex to fit within a BYTE)
class slabArena;
class slab;
const signed char SIZEOF_slab=10;
class slabBlock;
const signed char SIZEOF_slabBlock=2;

class secondLevel: public object {
public:
	long freeBitmap;		//a set bit indicates one or more freeBlocks
	freeBlock* freeLists[SL_SIZE];
};


class arena: public object {
public:
	long fl_freeBitmap;	//a set bit indicates one or more freeBlocks in the corresponding secondLevel
	secondLevel fl_array[32];		//this could be smaller for pools under 2GB, but would save very little space (perhaps 1-2KB)
	pool* head_arena;
	long nextPoolSize;
};

class pool: public object {
public:
	pool* next;	//straight list, terminated by NIL
	pool* prev;	//(ditto)
	long sizeInBytes;			//total size of this object, including the block(s) after it
};
class block: public object {
public:
	long size;	//the overall size in bytes, rounded up to a multiple of 4, with the lowest two bits used to record whether it ISLASTBLOCK and/or ISFREEBLOCK
			//use it to calculate the nextBlock := block + (size AND SIZEMASK_SIZEINBYTES)
	block* prevBlock;		//straight list, terminated by NIL
};


class usedBlock: public block {
	//size
	//prevBlock:PTR TO block
};

class freeBlock: public block {
public:
	//size
	//prevBlock:PTR TO block
	freeBlock* nextFree;	//straight list, terminated by NIL
	freeBlock* prevFree;	//(ditto)
};

class slabArena: public arena {
public:
	slab* freeSlabsArray[MAX_SLABBLOCKSIZE/ALIGN_SIZE+1];
};

class slab: public object {
public:
	slab* prev;	//straight list, terminated by NIL
	slab* next;	//(ditto)
	short freeBitmap;
};

class slabBlock: public object {
public:
	 signed char slabIndex;		//a value of -1 indicates this is not part of a Slab
	signed char blockIndex;
};


/*---------------------------------------------------------------------------------*/	//memory tracing functionality (only valid for C/C++)



/*---------------------------------------------------------------------------------*/	//FastNew wrapper

slabArena* arena_fastmem=NULL;
void* sem=NULL;
void throw_fastmem(long except, char* exceptInfo=NULLA);
signed char largestBitPosition(long number) ;
signed char smallestBitPosition(long number) ;
arena* newArena(long sizeOf_arena) ;
void endArena(arena* arena2);
long indexesForAdd(long sizeInBytes) ;
long indexesForSearch(long sizeInBytes) ;
freeBlock* findFreeInArena(arena* arena2, long sizeInBytes) ;
usedBlock* removeFreeFromArena(arena* arena2, freeBlock* freeBlock2, long freeFI, long freeSI, secondLevel* sl) ;
usedBlock* removeFreeFromArenaSimple(arena* arena2, freeBlock* freeBlock2) ;
void addFreeToArena(arena* arena2, usedBlock* usedBlock2);
usedBlock* splitBlock(usedBlock* usedBlock2, long sizeInBytes, long blockSize) ;
usedBlock* mergeBothBlocks(usedBlock* firstBlock, usedBlock* secondBlock) ;
void addPoolToArena(arena* arena2, long sizeInBytes);
void removePoolFromArena(arena* arena2, pool* pool2);
void* allocFromArena(arena* arena2, long sizeInBytes) ;
void deallocFromArena(arena* arena2, void* block2);
BOOLEAN reportTestFailure(long testID, long object2, char* item, long actual, long expected) ;
BOOLEAN testArena(arena* arena2, long testID=0) ;
BOOLEAN testPool(pool* pool2, long testID) ;
slabArena* newSlabArena() ;
void endSlabArena(slabArena* arena2);
void* allocFromSlabArena(slabArena* arena2, long sizeInBytes) ;
void deallocFromSlabArena(slabArena* arena2, void* block2);
BOOLEAN testSlabArena(slabArena* arena2, long testID=0) ;
void new_fastmem();
void end_fastmem();







void throw_fastmem(long except, char* exceptInfo) {
	if( DEBUG) {
		if( except) {
			if( exceptInfo) {
				printf("ERROR:  \"%c%c%c%c\"; %s.\n" ,( 32>(except >> 24 & 0xFF ))? (long) 32 : except >> 24 & 0xFF ,( 32>(except >> 16 & 0xFF ))? (long) 32 : except >> 16 & 0xFF ,( 32>(except >> 8 & 0xFF ))? (long) 32 : except >> 8 & 0xFF ,( 32>(except >> 0 & 0xFF ))? (long) 32 : except >> 0 & 0xFF ,(long) exceptInfo ,0 ,0 ,0 );
			} else {
				printf("ERROR:  \"%c%c%c%c\".\n" ,( 32>(except >> 24 & 0xFF ))? (long) 32 : except >> 24 & 0xFF ,( 32>(except >> 16 & 0xFF ))? (long) 32 : except >> 16 & 0xFF ,( 32>(except >> 8 & 0xFF ))? (long) 32 : except >> 8 & 0xFF ,( 32>(except >> 0 & 0xFF ))? (long) 32 : except >> 0 & 0xFF ,0 ,0 ,0 ,0 );
			}
		}
	}
	
	Throw(except, exceptInfo);
	return ;
}

//return the position of the largest bit set in the number
//NOTE:  the smallest position is 0, and the largest is 31.
//NOTE:  if no bits are set (i.e. number=0) then -1 is returned.
//NOTE:  it requires no more than 6 comparisons to compute the answer.
signed char largestBitPosition(long number)  {
	signed char position;
	if( number & (long) 0xFFFF0000) {
		if( number & (long) 0xFF000000) {
			if( number & (long) 0xF0000000) {
				if( number & (long) 0xC0000000) {
					if( number & (long) 0x80000000) {
						position = 31;
					} else {       //($40000000)
						position = 30;
					}
				} else {       //($30000000)
					if( number & 0x20000000) {
						position = 29;
					} else {       //($10000000)
						position = 28;
					}
				}
			} else {       //($0F000000)
				if( number & 0x0C000000) {
					if( number & 0x08000000) {
						position = 27;
					} else {       //($04000000)
						position = 26;
					}
				} else {       //($03000000)
					if( number & 0x02000000) {
						position = 25;
					} else {       //($01000000)
						position = 24;
					}
				}
			}
		} else {       //($00FF0000)
			if( number & 0x00F00000) {
				if( number & 0x00C00000) {
					if( number & 0x00800000) {
						position = 23;
					} else {       //($00400000)
						position = 22;
					}
				} else {       //($00300000)
					if( number & 0x00200000) {
						position = 21;
					} else {       //($00100000)
						position = 20;
					}
				}
			} else {       //($000F0000)
				if( number & 0x000C0000) {
					if( number & 0x00080000) {
						position = 19;
					} else {       //($00040000)
						position = 18;
					}
				} else {       //($00030000)
					if( number & 0x00020000) {
						position = 17;
					} else {       //($00010000)
						position = 16;
					}
				}
			}
		}
		
	} else if( number & 0x0000FFFF) {
		if( number & 0x0000FF00) {
			if( number & 0x0000F000) {
				if( number & 0x0000C000) {
					if( number & 0x00008000) {
						position = 15;
					} else {       //($00004000)
						position = 14;
					}
				} else {       //($00003000)
					if( number & 0x00002000) {
						position = 13;
					} else {       //($00001000)
						position = 12;
					}
				}
			} else {       //($00000F00)
				if( number & 0x00000C00) {
					if( number & 0x00000800) {
						position = 11;
					} else {       //($00000400)
						position = 10;
					}
				} else {       //($00000300)
					if( number & 0x00000200) {
						position = 9;
					} else {       //($00000100)
						position = 8;
					}
				}
			}
		} else {       //($000000FF)
			if( number & 0x000000F0) {
				if( number & 0x000000C0) {
					if( number & 0x00000080) {
						position = 7;
					} else {       //($00000040)
						position = 6;
					}
				} else {       //($00000030)
					if( number & 0x00000020) {
						position = 5;
					} else {       //($00000010)
						position = 4;
					}
				}
			} else {       //($0000000F)
				if( number & 0x0000000C) {
					if( number & 0x00000008) {
						position = 3;
					} else {       //($00000004)
						position = 2;
					}
				} else {       //($00000003)
					if( number & 0x00000002) {
						position = 1;
					} else {       //($00000001)
						position = 0;
					}
				}
			}
		}
		
	} else { //IF number = 0
		//(no bits are set)
		position = -1;
	}
	return position ;
} 

//return the position of the smallest bit set in the number
//NOTE:  the smallest position is 0, and the largest is 31.
//NOTE:  if no bits are set (i.e. number=0) then -1 is returned.
//NOTE:  it requires no more than 6 comparisons to compute the answer.
signed char smallestBitPosition(long number)  {
	signed char position;
	if( number & 0x0000FFFF) {
		if( number & 0x000000FF) {
			if( number & 0x0000000F) {
				if( number & 0x00000003) {
					if( number & 0x00000001) {
						position = 0;
					} else {       //($00000002)
						position = 1;
					}
				} else {       //($0000000C)
					if( number & 0x00000004) {
						position = 2;
					} else {       //($00000008)
						position = 3;
					}
				}
			} else {       //($000000F0)
				if( number & 0x00000030) {
					if( number & 0x00000010) {
						position = 4;
					} else {       //($00000020)
						position = 5;
					}
				} else {       //($000000C0)
					if( number & 0x00000040) {
						position = 6;
					} else {       //($00000080)
						position = 7;
					}
				}
			}
		} else {       //($0000FF00)
			if( number & 0x00000F00) {
				if( number & 0x00000300) {
					if( number & 0x00000100) {
						position = 8;
					} else {       //($00000200)
						position = 9;
					}
				} else {       //($00000C00)
					if( number & 0x00000400) {
						position = 10;
					} else {       //($00000800)
						position = 11;
					}
				}
			} else {       //($0000F000)
				if( number & 0x00003000) {
					if( number & 0x00001000) {
						position = 12;
					} else {       //($00002000)
						position = 13;
					}
				} else {       //($0000C000)
					if( number & 0x00004000) {
						position = 14;
					} else {       //($00008000)
						position = 15;
					}
				}
			}
		}
		
	} else if( number & (long) 0xFFFF0000) {
		if( number & 0x00FF0000) {
			if( number & 0x000F0000) {
				if( number & 0x00030000) {
					if( number & 0x00010000) {
						position = 16;
					} else {       //($00020000)
						position = 17;
					}
				} else {       //($000C0000)
					if( number & 0x00040000) {
						position = 18;
					} else {       //($00080000)
						position = 19;
					}
				}
			} else {       //($00F00000)
				if( number & 0x00300000) {
					if( number & 0x00100000) {
						position = 20;
					} else {       //($00200000)
						position = 21;
					}
				} else {       //($00C00000)
					if( number & 0x00400000) {
						position = 22;
					} else {       //($00800000)
						position = 23;
					}
				}
			}
		} else {       //($FF000000)
			if( number & 0x0F000000) {
				if( number & 0x03000000) {
					if( number & 0x01000000) {
						position = 24;
					} else {       //($02000000)
						position = 25;
					}
				} else {       //($0C000000)
					if( number & 0x04000000) {
						position = 26;
					} else {       //($08000000)
						position = 27;
					}
				}
			} else {       //($F0000000)
				if( number & 0x30000000) {
					if( number & 0x10000000) {
						position = 28;
					} else {       //($20000000)
						position = 29;
					}
				} else {       //($C0000000)
					if( number & 0x40000000) {
						position = 30;
					} else {       //($80000000)
						position = 31;
					}
				}
			}
		}
		
	} else { //IF number = 0
		//(no bits are set)
		position = -1;
	}
	return position ;
} 
arena* newArena(long sizeOf_arena)  {
	arena* arena2=NULL;
	arena2 = (arena*) NewR(sizeOf_arena);
	//arena.fl_freeBitmap
	//arena.fl_array
	arena2->head_arena = (pool*) NULL;
	arena2->nextPoolSize = 0;
	return arena2 ;
} 

void endArena(arena* arena2) {
	pool* pool2=NULL; pool* next=NULL;
	
	pool2 = arena2->head_arena;
	while( pool2) {
		next = pool2->next;
		free((void*) pool2  );
		NULLA;
		pool2 = next;
	}
	
	free(arena2 );
	NULLA;
	return ;
}




//returns the indexes that should be used to store a free block of the specified size
long indexesForAdd(long sizeInBytes)  {
	long firstIndex, secondIndex;
	long temp_QUAD;
try {
	temp_QUAD = exception ;
	exception = 0 ;
	firstIndex  = largestBitPosition(sizeInBytes);
	secondIndex = (( firstIndex - SL_BITWIDTH >= 0 )? sizeInBytes >> firstIndex - SL_BITWIDTH : sizeInBytes << labs(firstIndex - SL_BITWIDTH ))& ~ (1 << SL_BITWIDTH);
} catch(...) {}
	ret2 = secondIndex ;
	if (exception!=0) {throw eException;} else {EMPTY;};
	exception = temp_QUAD ;
	return firstIndex ;
} 

//returns the indexes that should be used to search for a free block that is >= the specified size
//NOTE: The blocks will be larger than the requested size, by a whole Second Level, unless sizeInBytes is at the bottom of a Second Level range.
//      This guarantees that any block returned will be large enough.
long indexesForSearch(long sizeInBytes)  {
	long firstIndex, secondIndex; 
	ret2 = secondIndex ;
	return indexesForAdd(sizeInBytes + (1 << largestBitPosition(sizeInBytes) - SL_BITWIDTH)- 1);
}

freeBlock* findFreeInArena(arena* arena2, long sizeInBytes)  {
	freeBlock* freeBlock2=NULL; long freeFI, freeSI; secondLevel* sl=NULL;
	long firstIndex, secondIndex, suitableBitmap;
	long temp_QUAD;
try {
	temp_QUAD = exception ;
	exception = 0 ;
	
	firstIndex= indexesForSearch(sizeInBytes);
	secondIndex = ret2 ;
	sl = & arena2->fl_array[firstIndex];
	suitableBitmap = sl->freeBitmap & (BOOLEAN) 0xFFFFFFFF << secondIndex;
	if( suitableBitmap != 0) {
		//(current FL index has one or more suitable SL matches) so find the smallest match
		freeSI = smallestBitPosition(suitableBitmap);
		freeFI = firstIndex;
		//sl := arena.fl_array[freeFI]
//Print('/n# findFreeInArena: Suitable match on closest FI=/d, with SI=/d/n', freeFI, freeSI)
	} else {
		//(no suitable matches on current FL) so find the next largest FL that is not full
//Print('/n# findFreeInArena: No suitable match on current FI=/d, SI=/d, so looking for free new one/n', firstIndex, secondIndex)
		suitableBitmap = arena2->fl_freeBitmap & (BOOLEAN) 0xFFFFFFFF << firstIndex + 1;
//Print('# findFreeInArena: FL suitableBitmap=$/h, complete bitmap=$/h, min index allowed by mask=/d/n', suitableBitmap, arena.fl_freeBitmap, firstIndex + 1)

		if( suitableBitmap == 0 ) {

			freeBlock2 = (freeBlock*) NULL;

			freeFI = -1;

			freeSI = -1;

			sl = (secondLevel*) NULL;

			goto finally;

		}
		freeFI = smallestBitPosition(suitableBitmap);
//Print('# findFreeInArena: smallest free FI=/d/n', freeFI)

		
		sl = & arena2->fl_array[freeFI];
		freeSI = smallestBitPosition(sl->freeBitmap);
//Print('# findFreeInArena: smallest free SI=/d (from bitmap $/h)/n', freeSI, sl.freeBitmap)
//Print('# findFreeInArena: freeBlock=/d/n', sl.freeLists[freeSI] + SIZEOF_usedBlock)
	}
	
	freeBlock2 = sl->freeLists[freeSI];
	if( (void*) freeBlock2 == NULL ) { throw_fastmem(QuadChara(0,'B','U','G'), (char*) "PE/FastMem; findFreeInArena(); freeBlock=NIL");}
finally: 0; 
} catch(...) {}
	ret4 = (long) sl ;
	ret3 = freeSI ;
	ret2 = freeFI ;
	if (exception!=0) {throw eException;} else {EMPTY;};
	exception = temp_QUAD ;
	return freeBlock2 ;
} 

usedBlock* removeFreeFromArena(arena* arena2, freeBlock* freeBlock2, long freeFI, long freeSI, secondLevel* sl)  {
	usedBlock* usedBlock2=NULL;
	long debugFI, debugSI;
	BOOLEAN noMoreFreeSI;
	
	//freeFI, freeSI := indexesForAdd(freeBlock.size AND SIZEMASK_SIZEINBYTES)
	if( DEBUG) {
		debugFI= indexesForAdd(freeBlock2->size & SIZEMASK_SIZEINBYTES);
		debugSI = ret2 ;
		if( - (freeFI != debugFI)  | - (freeSI != debugSI)) {
			printf("# freeBlock.size=%ld=$%lx, freeFI=%ld, freeSI=%ld, debugFI=%ld, debugSI=%ld\n" ,freeBlock2->size ,freeBlock2->size ,freeFI ,debugFI ,freeSI ,debugSI ,0 ,0 );
			//     # freeBlock.size=276=$114, freeFI=9, freeSI=25, debugFI=8, debugSI=2
		}
		if( freeFI != debugFI ) { throw_fastmem(QuadChara(0,'B','U','G'), (char*) "PE/FastMem; removeFreeFromArena(); incorrect freeFI - please use FastVerify() to find memory corruption");}
		if( freeSI != debugSI ) { throw_fastmem(QuadChara(0,'B','U','G'), (char*) "PE/FastMem; removeFreeFromArena(); incorrect freeSI - please use FastVerify() to find memory corruption");}
	}
	
	//sl := arena.fl_array[freeFI]
	if( DEBUG ) { if( sl != & arena2->fl_array[freeFI] ) { throw_fastmem(QuadChara(0,'B','U','G'), (char*) "PE/FastMem; removeFreeFromArena(); incorrect sl");}}
	
	//turn freeBlock into a usedBlock
	noMoreFreeSI = FALSE;
	if( (void*) freeBlock2->nextFree != NULL) {
		freeBlock2->nextFree->prevFree = freeBlock2->prevFree;
	}
	if( (void*) freeBlock2->prevFree != NULL) {
		freeBlock2->prevFree->nextFree = freeBlock2->nextFree;
	} else {
		//(this is the head of the list)
		if( DEBUG ) { if( sl->freeLists[freeSI] != freeBlock2 ) { throw_fastmem(QuadChara(0,'B','U','G'), (char*) "PE/FastMem; removeFreeFromArena(); freeBlock.prevFree=NIL")	;}}//was not really head of the list
		
		sl->freeLists[freeSI] = freeBlock2->nextFree;
		if( (void*) freeBlock2->nextFree == NULL) {
			noMoreFreeSI = TRUE;
		} else {
			freeBlock2->nextFree->prevFree = (freeBlock*) NULL;
		}
	}
	freeBlock2->size = freeBlock2->size & ~ SIZEMASK_ISFREEBLOCK;
	usedBlock2 = (usedBlock*) (block*) freeBlock2 ;
	
	//update bitmaps
	if( noMoreFreeSI) {
		//(no more freeBlocks) so clear bitmap
		sl->freeBitmap = sl->freeBitmap & ~ (1 << freeSI);
		if( sl->freeBitmap == 0) {
			//(no freeBlocks in entire secondLevel) so clear bitmap
			arena2->fl_freeBitmap = arena2->fl_freeBitmap & ~ (1 << freeFI);
		}
	}
	return usedBlock2 ;
} 

usedBlock* removeFreeFromArenaSimple(arena* arena2, freeBlock* freeBlock2)  {
	usedBlock* usedBlock2=NULL;
	long freeFI, freeSI; secondLevel* sl=NULL;
	
	freeFI= indexesForAdd(freeBlock2->size & SIZEMASK_SIZEINBYTES);
	freeSI = ret2 ;
	sl = & arena2->fl_array[freeFI];
	usedBlock2 = removeFreeFromArena(arena2, freeBlock2, freeFI, freeSI, sl);
	return usedBlock2 ;
} 

void addFreeToArena(arena* arena2, usedBlock* usedBlock2) {
	freeBlock* freeBlock2=NULL; long freeFI, freeSI; secondLevel* sl=NULL;
	
	//turn usedBlock into a freeBlock
	usedBlock2->size = usedBlock2->size | SIZEMASK_ISFREEBLOCK;
	freeBlock2 = (freeBlock*) (block*) usedBlock2 ;
	
	//find where to put the freeBlock
	freeFI= indexesForAdd(freeBlock2->size & SIZEMASK_SIZEINBYTES);
	freeSI = ret2 ;
	sl = & arena2->fl_array[freeFI];
//Print('# added free block /d using FI=/d, SI=/d/n', freeBlock + SIZEOF_usedBlock, freeFI, freeSI)
	
	//add freeBlock to it's freeList
	freeBlock2->nextFree = sl->freeLists[freeSI];
	freeBlock2->prevFree = (freeBlock*) NULL;
	
	if( freeBlock2->nextFree ) { freeBlock2->nextFree->prevFree = freeBlock2;}
	sl->freeLists[freeSI] = freeBlock2;
	
	//update bitmaps
	sl->freeBitmap = sl->freeBitmap | 1 << freeSI;
	arena2->fl_freeBitmap = arena2->fl_freeBitmap | 1 << freeFI;
	return ;
}

usedBlock* splitBlock(usedBlock* usedBlock2, long sizeInBytes, long blockSize)  {
	usedBlock* remUsedBlock=NULL;
	block* nextBlock=NULL;
	
	/*IF DEBUG THEN*/ if( sizeInBytes & SIZEROUNDUP ) { throw_fastmem(QuadChara(0,'B','U','G'), (char*) "PE/FastMem; splitBlock(); sizeInBytes is not a multiple of 4");}
	
	//blockSize := usedBlock.size AND SIZEMASK_SIZEINBYTES
	if( DEBUG ) { if( blockSize != (usedBlock2->size & SIZEMASK_SIZEINBYTES) ) { throw_fastmem(QuadChara(0,'B','U','G'), (char*) "PE/FastMem; splitBlock(); incorrect blockSize");}}
	
	if( DEBUG ) { if( blockSize & SIZEROUNDUP ) { throw_fastmem(QuadChara(0,'B','U','G'), (char*) "PE/FastMem; splitBlock(); blockSize is not a multiple of 4");}}
	
	
	remUsedBlock = (usedBlock*) ((char*) usedBlock2 + (long) sizeInBytes);
	remUsedBlock->size = blockSize - sizeInBytes ; if( usedBlock2->size & SIZEMASK_ISLASTBLOCK ) { remUsedBlock->size = remUsedBlock->size | SIZEMASK_ISLASTBLOCK;}
	remUsedBlock->prevBlock = usedBlock2;
	
	usedBlock2->size = sizeInBytes	;//AND NOT (SIZEMASK_ISFREEBLOCK OR SIZEMASK_ISLASTBLOCK)
	//usedBlock.prevBlock
	
	
	if( (remUsedBlock->size & SIZEMASK_ISLASTBLOCK )== 0) {
		nextBlock = (usedBlock*) ((char*) usedBlock2 + (long) blockSize);
		nextBlock->prevBlock = remUsedBlock;
	}
	return remUsedBlock ;
} 

usedBlock* mergeBothBlocks(usedBlock* firstBlock, usedBlock* secondBlock)  {
	usedBlock* mergedBlock=NULL;
	block* nextBlock=NULL;
	
	if( DEBUG ) { if( (block*) firstBlock != secondBlock->prevBlock ) { throw_fastmem(QuadChara(0,'B','U','G'), (char*) "PE/FastMem; mergeBothBlocks(); blocks not adjacent");}}
	
	
	//firstBlock.prevBlock
	firstBlock->size = (firstBlock->size & SIZEMASK_SIZEINBYTES) + (secondBlock->size & SIZEMASK_SIZEINBYTES)		;//AND NOT SIZEMASK_ISFREEBLOCK
	if( secondBlock->size & SIZEMASK_ISLASTBLOCK) {
		firstBlock->size = firstBlock->size | SIZEMASK_ISLASTBLOCK;
	} else {
		nextBlock = (usedBlock*) ((char*) secondBlock + (long) (secondBlock->size & SIZEMASK_SIZEINBYTES));
		nextBlock->prevBlock = firstBlock;
	}
	
	mergedBlock = firstBlock;
	return mergedBlock ;
} 


void addPoolToArena(arena* arena2, long sizeInBytes) {
	long poolSize; pool* pool2=NULL; usedBlock* usedBlock2=NULL;
	
	sizeInBytes = sizeInBytes  + SIZEROUNDUP & ~ SIZEROUNDUP 	;
	
	//create & add pool object
	poolSize = (sizeof( pool)  + ALIGN_SIZE_M1 & ~ ALIGN_SIZE_M1)	 + sizeInBytes;
	pool2 = (pool*) memset(malloc(poolSize ), 0,poolSize );
	if( (void*) pool2 == NULL ) { goto finally;}
	//pool.next
	//pool.prev
	pool2->sizeInBytes = poolSize;
	
	pool2->next = arena2->head_arena;
	pool2->prev = (pool*) NULL;
	if( arena2->head_arena ) { arena2->head_arena->prev = pool2;}
	arena2->head_arena = pool2;
	
	//create & add free block inside pool
	usedBlock2 = (usedBlock*) ((char*) (void* ) pool2 + (long) (sizeof( pool)  + ALIGN_SIZE_M1 & ~ ALIGN_SIZE_M1)	);
	usedBlock2->size      = sizeInBytes | SIZEMASK_ISLASTBLOCK;
	usedBlock2->prevBlock = (block*) NULL;
	
	addFreeToArena(arena2, usedBlock2);
finally: 0; 
	return ;
//Print('# allocated pool /d to hold /d bytes/n', pool, sizeInBytes)
}

void removePoolFromArena(arena* arena2, pool* pool2) {
	pool* prevPool=NULL; pool* nextPool=NULL; block* firstBlock=NULL;
	
//Print('# deallocated pool /d/n', pool)
	
	//use check
	firstBlock = (block*) ((char*) (void* ) pool2 + (long) (sizeof( pool)  + ALIGN_SIZE_M1 & ~ ALIGN_SIZE_M1)	);
	if( DEBUG) {
		if( (firstBlock->size & (SIZEMASK_ISLASTBLOCK | SIZEMASK_ISFREEBLOCK) )!= (SIZEMASK_ISLASTBLOCK | SIZEMASK_ISFREEBLOCK) ) { throw_fastmem(QuadChara(0,'E','M','U'), (char*) "PE/FastMem; removePoolFromArena(); pool is not completely free");}
		
		if( (void*) firstBlock->prevBlock != NULL ) { throw_fastmem(QuadChara(0,'B','U','G'), (char*) "PE/FastMem; removePoolFromArena(); prevBlock<>NIL");}
	}
	
	//perform request
	removeFreeFromArenaSimple(arena2, (freeBlock*) firstBlock);
	
	prevPool = pool2->prev;
	nextPool = pool2->next;
	
	if( prevPool) {
		prevPool->next = nextPool;
	} else {
		if( DEBUG ) { if( arena2->head_arena != pool2 ) { throw_fastmem(QuadChara(0,'B','U','G'), (char*) "PE/FastMem; removePoolFromArena(); pool was unexpectedly not head of list");}}
		arena2->head_arena = nextPool;
	}
	
	if( nextPool) {
		nextPool->prev = prevPool;
	}
	
	free((void*) pool2  );
	NULLA;
	return ;
}


void* allocFromArena(arena* arena2, long sizeInBytes)  {
	void* block2=NULL; long actualSize;
	freeBlock* freeBlock2=NULL; long freeFI, freeSI; secondLevel* sl=NULL;
	usedBlock* usedBlock2=NULL; long blockSize; usedBlock* remUsedBlock=NULL;
	long nextPoolSize, poolFI, poolSI;
	long temp_QUAD;
try {
	temp_QUAD = exception ;
	exception = 0 ;
	
	//find free block
	sizeInBytes = (( (sizeof( usedBlock)  + ALIGN_SIZE_M1 & ~ ALIGN_SIZE_M1)		 + sizeInBytes>sizeof( freeBlock) )? (sizeof( usedBlock)  + ALIGN_SIZE_M1 & ~ ALIGN_SIZE_M1)		 + sizeInBytes : sizeof( freeBlock)) + SIZEROUNDUP & ~ SIZEROUNDUP 	;
	
	freeBlock2= findFreeInArena(arena2, sizeInBytes);
	freeFI= ret2 ;
	freeSI= ret3 ;
	sl = (secondLevel*) ret4 ;
	if( (void*) freeBlock2 == NULL) {
		//(no pools are free) so add a suitable new pool
		if( DISABLE_AUTOPOOLS == FALSE) {
			nextPoolSize = ( arena2->nextPoolSize>8 * sizeInBytes )? arena2->nextPoolSize : 8 * sizeInBytes;
			if( nextPoolSize > MAXPOOLSIZE) {
				//(ideal pool size is too large) so use minimum size that will allow findFreeInArena() to succeed
				poolFI= indexesForSearch(sizeInBytes  + SIZEROUNDUP & ~ SIZEROUNDUP 	);
				poolSI = ret2 ;
				nextPoolSize = (1 << SL_BITWIDTH | poolSI )<< poolFI - SL_BITWIDTH;
				
			} else if( nextPoolSize < MINPOOLSIZE) {
				//(this arena has not been initialised) so use the minimum pool size
				nextPoolSize = MINPOOLSIZE;
			}
			addPoolToArena(arena2, nextPoolSize)		;//this may not succeed, if there is insufficient free system memory
			
			//store next pool size
			arena2->nextPoolSize = ( 2 * nextPoolSize<MAXPOOLSIZE )? 2 * nextPoolSize : MAXPOOLSIZE;
			
			//retry allocation
			freeBlock2= findFreeInArena(arena2, sizeInBytes);
			freeFI= ret2 ;
			freeSI= ret3 ;
			sl = (secondLevel*) ret4 ;
		}
		if( (void*) freeBlock2 == NULL ) {
			block2 = NULLA;
			actualSize = 0;
			goto finally;
		}
	}
	
	//remove block from it's free list
	usedBlock2 = removeFreeFromArena(arena2, freeBlock2, freeFI, freeSI, sl);
	
	//handle splitting of block
	blockSize = usedBlock2->size & SIZEMASK_SIZEINBYTES;
	if( blockSize - sizeInBytes >= sizeof( freeBlock )+ 1) {
		//(block can be split) so split it
		remUsedBlock = splitBlock(usedBlock2, sizeInBytes, blockSize);
//Print('# split block, remaining block=/d size=/d/n', remUsedBlock + SIZEOF_usedBlock, remUsedBlock.size AND SIZEMASK_SIZEINBYTES - SIZEOF_usedBlock)
		addFreeToArena(arena2, remUsedBlock);
		
		blockSize = sizeInBytes;
	}
	
	block2 = (void*) ((char*) (void* ) usedBlock2 + (long) (sizeof( usedBlock)  + ALIGN_SIZE_M1 & ~ ALIGN_SIZE_M1)		);
	actualSize = blockSize  - (sizeof( usedBlock)  + ALIGN_SIZE_M1 & ~ ALIGN_SIZE_M1)		;
	if( DEBUG ) { if( block2 != (void*) ((long) ((char*) block2  + (long) ALIGN_SIZE_M1 )& (long) ~ ALIGN_SIZE_M1) 	 ) { throw_fastmem(QuadChara(0,'E','M','U'), (char*) "PE/FastMem; allocFromArena(); block is not aligned");}}
finally: 0; 
} catch(...) {}
	ret2 = actualSize ;
	if (exception!=0) {throw eException;} else {EMPTY;};
	exception = temp_QUAD ;
	return block2 ;
} 

void deallocFromArena(arena* arena2, void* block2) {
	usedBlock* usedBlock2=NULL;
	block* prevBlock=NULL; usedBlock* prevUsedBlock=NULL;
	block* nextBlock=NULL; usedBlock* nextUsedBlock=NULL;
	pool* pool2=NULL;
	
	usedBlock2 = (usedBlock*) ((char*) (void* ) block2 - (long) (sizeof( usedBlock)  + ALIGN_SIZE_M1 & ~ ALIGN_SIZE_M1)		);
	
	//attempt to merge usedBlock with any adjacent freeBlocks
	prevBlock = usedBlock2->prevBlock;
	nextBlock = (usedBlock*) ((char*) usedBlock2 + (long) (usedBlock2->size & SIZEMASK_SIZEINBYTES));
	
	if( prevBlock) {
		if( prevBlock->size & SIZEMASK_ISFREEBLOCK) {
			prevUsedBlock = removeFreeFromArenaSimple(arena2, (freeBlock*) prevBlock);
			usedBlock2 = mergeBothBlocks(prevUsedBlock, usedBlock2);
		}
	}
	
	if( (usedBlock2->size & SIZEMASK_ISLASTBLOCK )== 0) {
		if( nextBlock->size & SIZEMASK_ISFREEBLOCK) {
			nextUsedBlock = removeFreeFromArenaSimple(arena2, (freeBlock*) nextBlock);
			usedBlock2 = mergeBothBlocks(usedBlock2, nextUsedBlock);
		}
	}
	
	//finally add usedBlock to the the free list
	addFreeToArena(arena2, usedBlock2);
	
	//check if (now free) block is the entire pool, and if so then deallocate it
	if( DISABLE_AUTOPOOLS == FALSE) {
		if( (void*) usedBlock2->prevBlock == NULL) {
			if( usedBlock2->size & SIZEMASK_ISLASTBLOCK) {
				pool2 = (pool*) ((char*) (void* ) usedBlock2 - (long) (sizeof( pool)  + ALIGN_SIZE_M1 & ~ ALIGN_SIZE_M1)	);
				removePoolFromArena(arena2, pool2);
			}
		}
	}
	return ;
}


BOOLEAN reportTestFailure(long testID, long object2, char* item, long actual, long expected)  {
	BOOLEAN success;
	printf("# FAILED test ID %ld of object %ld, as %s was %ld not %ld\n" ,testID ,object2 ,(long) item ,actual ,expected ,0 ,0 ,0 );
	success = FALSE;
	return success ;
} 

BOOLEAN testArena(arena* arena2, long testID)  {
	BOOLEAN success;
	pool* pool2=NULL; pool* prevPool=NULL;
	
	success = TRUE;
	
	prevPool = (pool*) NULL;
	pool2 = arena2->head_arena;
	while( pool2) {
		if( pool2->prev != prevPool ) { success = reportTestFailure(testID, (long) pool2, (char*) "pool.prev", (long) pool2->prev, (long) prevPool);}
		if( testPool(pool2, testID) == FALSE ) { success = FALSE;}
		
		prevPool = pool2;
		pool2 = pool2->next;
	if( success == FALSE) break;
	} 
	return success ;
} 

BOOLEAN testPool(pool* pool2, long testID)  {
	BOOLEAN success;
	void* pastEndOfPool=NULL; block* block2=NULL; block* prevBlock=NULL; block* nextBlock=NULL;
	
	success = TRUE;
	pastEndOfPool = (void*) ((char*) (void* ) pool2 + (long) pool2->sizeInBytes);
	
	block2     = (block*) NULL;
	nextBlock = (block*) ((char*) (void* ) pool2 + (long) (sizeof( pool)  + ALIGN_SIZE_M1 & ~ ALIGN_SIZE_M1)	);
	do {
		prevBlock = block2;
		block2 = nextBlock;
		
		if( prevBlock ) { if( (prevBlock->size & SIZEMASK_ISLASTBLOCK )!= 0 ) { success = reportTestFailure(testID, (long) ((char*) prevBlock + (long) (sizeof( usedBlock)  + ALIGN_SIZE_M1 & ~ ALIGN_SIZE_M1)		), (char*) "block.size AND SIZEMASK_ISLASTBLOCK", SIZEMASK_ISLASTBLOCK, 0);}}
		if( block2->prevBlock != prevBlock ) { success = reportTestFailure(testID, (long) ((char*) block2 + (long) (sizeof( usedBlock)  + ALIGN_SIZE_M1 & ~ ALIGN_SIZE_M1)		), (char*) "block.prevBlock", (long) (( block2->prevBlock )? (block*) ((char*) block2->prevBlock + (long) (sizeof( usedBlock)  + ALIGN_SIZE_M1 & ~ ALIGN_SIZE_M1)		 ): (block*) (long) 0), (long) (( prevBlock )? (block*) ((char*) prevBlock + (long) (sizeof( usedBlock)  + ALIGN_SIZE_M1 & ~ ALIGN_SIZE_M1)		 ): (block*) (long) 0));}
		
		nextBlock = (block*) ((char*) block2 + (long) (block2->size & SIZEMASK_SIZEINBYTES));
		//IF success = FALSE THEN Print('# block=/d, size=/d, prevBlock=/d (expected /d), nextBlock=/d/n', block, block.size AND SIZEMASK_SIZEINBYTES, block.prevBlock, prevBlock, nextBlock)
	} while(( - ((long) nextBlock >= (long) pastEndOfPool)  | - (success == FALSE))==0);
	
	if( success) {
		if( (block2->size & SIZEMASK_ISLASTBLOCK )== 0 ) { success = reportTestFailure(testID, (long) ((char*) block2 + (long) (sizeof( usedBlock)  + ALIGN_SIZE_M1 & ~ ALIGN_SIZE_M1)		), (char*) "block.size AND SIZEMASK_ISLASTBLOCK", 0, SIZEMASK_ISLASTBLOCK);}
		if( (void*) nextBlock != pastEndOfPool ) { success = reportTestFailure(testID, (long) ((char*) block2 + (long) (sizeof( usedBlock)  + ALIGN_SIZE_M1 & ~ ALIGN_SIZE_M1)		), (char*) "block + (block.size AND SIZEMASK_SIZEINBYTES)", (long) nextBlock, (long) pastEndOfPool);}
	}
	return success ;
} 


slabArena* newSlabArena()  {
	slabArena* arena2=NULL;
	arena2 = (slabArena*) newArena(sizeof( slabArena));
	return arena2 ;
} 

void endSlabArena(slabArena* arena2) {
	endArena(arena2);
	return ;
}


void* allocFromSlabArena(slabArena* arena2, long sizeInBytes)  {
	void* block2=NULL; long actualSize;
	signed char blockIndex; slabBlock* slabBlock2=NULL; long slabBlockSize;
	signed char slabIndex; slab* slab2=NULL;
	long temp_QUAD;
try {
	temp_QUAD = exception ;
	exception = 0 ;
	
	slabBlockSize = SIZEOF_slabBlock + sizeInBytes  + ALIGN_SIZE_M1 & ~ ALIGN_SIZE_M1 	;
	
	if( - (slabBlockSize > MAX_SLABBLOCKSIZE)  | DISABLE_SLABS) {
		//(too big for a slab) so allocate directly from the Arena, and mark it that this is the case
		block2= allocFromArena(arena2, (SIZEOF_slabBlock  + ALIGN_SIZE_M1 & ~ ALIGN_SIZE_M1)	 + sizeInBytes);
		actualSize = ret2 ;
		if( block2 == NULL ) {
			block2 = NULLA;
			actualSize = 0;
			goto finally;
		}
		block2 = (void*) ((char*) (void* ) block2 + (long) (SIZEOF_slabBlock  + ALIGN_SIZE_M1 & ~ ALIGN_SIZE_M1)	);
		actualSize = actualSize - (SIZEOF_slabBlock  + ALIGN_SIZE_M1 & ~ ALIGN_SIZE_M1)	;
		
		slabBlock2 = (slabBlock*) ((char*) (void* ) block2 - (long) SIZEOF_slabBlock);
		slabBlock2->slabIndex = -1;
	} else {
		//(must be allocated from a slab) so first check if there are any suitable slabs with free space
		slabIndex = (signed char) (slabBlockSize / ALIGN_SIZE );
		if( DEBUG ) { if( - (slabIndex < 0)  | - (slabIndex > MAX_SLABBLOCKSIZE/ALIGN_SIZE)  ) { throw_fastmem(QuadChara(0,'B','U','G'), (char*) "PE/FastMem; allocFromSlabArena(); slabIndex out of bounds");}}
		slab2 = arena2->freeSlabsArray[slabIndex];
		if( (void*) slab2 == NULL) {
			//(no non-full slabs) so create a new slab
//Print('# allocating new slab, slabBlockSize=/d, slabIndex=/d/n', slabBlockSize, slabIndex)
			slab2 = (slab*) allocFromArena(arena2, SIZEOF_slab + 16 * slabBlockSize)		;//16 due to slab.freeBitmap being an INT
			if( (void*) slab2 == NULL ) {
				block2 = NULLA;
				actualSize = 0;
				goto finally;
			}
			slab2->prev = (slab*) NULL;
			slab2->next = (slab*) NULL		;//since arena.freeSlabsArray[slabIndex] = NIL
			slab2->freeBitmap = -1	;//$FFFF
			
			arena2->freeSlabsArray[slabIndex] = slab2;
			
			blockIndex = 0		;//arbitrarily pick any (free) block
		} else {
			//(found a non-full slab) so arbitrarily pick any free block
			blockIndex = largestBitPosition(slab2->freeBitmap & 0xFFFF);
//Print('# using existing slab, slabIndex=/d, blockIndex=/d/n', slabIndex, blockIndex)
		}
		if( DEBUG ) { if( - (blockIndex < 0)  | - (blockIndex > 15)  ) { throw_fastmem(QuadChara(0,'B','U','G'), (char*) "PE/FastMem; allocFromSlabArena(); blockIndex out of bounds");}}
		
		//retrieve free block from slab
		slabBlock2 = (slabBlock*) ((char*) (void*) ((char*) (void* ) slab2 + (long) SIZEOF_slab )+ (long) (blockIndex * slabBlockSize));
		
		block2    = (void*) ((char*) (void* ) slabBlock2 + (long) SIZEOF_slabBlock);
		actualSize = slabBlockSize - SIZEOF_slabBlock;
		
		//mark block as no-longer free in the slab
		slabBlock2-> slabIndex =  slabIndex;
		slabBlock2->blockIndex = blockIndex;
		
		slab2->freeBitmap = (short) (slab2->freeBitmap & ~ (1 << blockIndex));
		if( slab2->freeBitmap == 0) {
			//(slab is full) so remove it from list of free slabs
			if( DEBUG ) { if( (void*) slab2->prev != NULL                        ) { throw_fastmem(QuadChara(0,'B','U','G'), (char*) "PE/FastMem; allocFromSlabArena(); slab not head (1)");}}
			if( DEBUG ) { if( arena2->freeSlabsArray[slabIndex] != slab2 ) { throw_fastmem(QuadChara(0,'B','U','G'), (char*) "PE/FastMem; allocFromSlabArena(); slab not head (2)");}}
			
			arena2->freeSlabsArray[slabIndex] = slab2->next;
			if( slab2->next ) { slab2->next->prev = (slab*) NULL;}
		}
	}
	
	if( DEBUG ) { if( block2 != (void*) ((long) ((char*) block2  + (long) ALIGN_SIZE_M1 )& (long) ~ ALIGN_SIZE_M1) 	 ) { throw_fastmem(QuadChara(0,'E','M','U'), (char*) "PE/FastMem; allocFromSlabArena(); block is not aligned");}}
finally: 0; 
} catch(...) {}
	ret2 = actualSize ;
	if (exception!=0) {throw eException;} else {EMPTY;};
	exception = temp_QUAD ;
	return block2 ;
} 

void deallocFromSlabArena(slabArena* arena2, void* block2) {
	signed char blockIndex; slabBlock* slabBlock2=NULL; long slabBlockSize;
	signed char slabIndex; slab* slab2=NULL;
	
	slabBlock2 = (slabBlock*) ((char*) (void* ) block2 - (long) SIZEOF_slabBlock);
	
	if( slabBlock2->slabIndex == -1) {
		//(block was allocated directly from the Arena)
		block2 = (void*) ((char*) (void* ) block2 - (long) (SIZEOF_slabBlock  + ALIGN_SIZE_M1 & ~ ALIGN_SIZE_M1)	);
		deallocFromArena(arena2, block2);
	} else {
		if( DISABLE_SLABS ) { throw_fastmem(QuadChara(0,'B','U','G'), (char*) "PE/FastMem; deallocFromSlabArena(); slabIndex<>-1");}
		
		//calculate slab of slabBlock
		 slabIndex = slabBlock2-> slabIndex;
		blockIndex = slabBlock2->blockIndex;
		if( DEBUG ) { if( - (slabIndex < 0)  | - (slabIndex > MAX_SLABBLOCKSIZE/ALIGN_SIZE)  ) { throw_fastmem(QuadChara(0,'B','U','G'), (char*) "PE/FastMem; deallocFromSlabArena(); slabIndex out of bounds");}}
		if( DEBUG ) { if( - (blockIndex < 0)  | - (blockIndex > 15)                              ) { throw_fastmem(QuadChara(0,'B','U','G'), (char*) "PE/FastMem; deallocFromSlabArena(); blockIndex out of bounds");}}
		
		slabBlockSize = slabIndex * ALIGN_SIZE;
		slab2 = (slab*) ((char*) (void*) ((char*) (void* ) slabBlock2 - (long) SIZEOF_slab )- (long) (blockIndex * slabBlockSize));
		
		//mark block as free in the slab
		if( slab2->freeBitmap == 0) {
			//(slab is no-longer full) so add it to the list of free slabs
			slab2->prev = (slab*) NULL;
			slab2->next = arena2->freeSlabsArray[slabIndex];
			
			arena2->freeSlabsArray[slabIndex] = slab2;
			if( slab2->next ) { slab2->next->prev = slab2;}
		}
		
		slab2->freeBitmap = (short) (slab2->freeBitmap | 1 << blockIndex);
		if( (slab2->freeBitmap & 0xFFFF )== 0xFFFF) {
			//(slab is completely free) so remove it from list of free slabs, then deallocate it
			if( (void*) slab2->prev == NULL) {
				if( DEBUG ) { if( arena2->freeSlabsArray[slabIndex] != slab2 ) { throw_fastmem(QuadChara(0,'B','U','G'), (char*) "PE/FastMem; deallocFromSlabArena(); slab not head");}}
				arena2->freeSlabsArray[slabIndex] = slab2->next;
				if( slab2->next ) { slab2->next->prev = (slab*) NULL;}
			} else {
				slab2->prev->next = slab2->next;
				if( slab2->next ) { slab2->next->prev = slab2->prev;}
			}
			
			deallocFromArena(arena2, slab2);
		}
	}
	return ;
}

BOOLEAN testSlabArena(slabArena* arena2, long testID)  {
	BOOLEAN success;
	long slabIndex; slab* slab2=NULL; slab* prevSlab=NULL;
	
	success = testArena(arena2, testID);
	if( success == FALSE ) { goto finally;}
	
	for( slabIndex = 0 ; slabIndex <=(long) MAX_SLABBLOCKSIZE/ALIGN_SIZE; slabIndex ++) {
		prevSlab = (slab*) NULL;
		slab2 = arena2->freeSlabsArray[slabIndex];
		while( slab2) {
			if( slab2->prev != prevSlab ) { success = reportTestFailure(testID, (long) slab2, (char*) "slab.prev", (long) slab2->prev, (long) prevSlab);}
			//IF testSlab(slab, testID) = FALSE THEN success := FALSE
			
			prevSlab = slab2;
			slab2 = slab2->next;
		if( success == FALSE) break;
		} 
	}
finally: 0; 
	return success ;
} 



void new_fastmem() {
	sem = NewSemaphore();
	arena_fastmem = newSlabArena();
	return ;
	
	
}

void end_fastmem() {
	
	
	endSlabArena(arena_fastmem);
	sem = DisposeSemaphore(sem);
	return ;
}


void* FastNew(long size, BOOLEAN noClear)  {
	void* mem=NULL; 
	if( size <= 0 ) { Raise(QuadChara(0,'M','E','M'));}
	
	SemLock(sem);
	mem = allocFromSlabArena(arena_fastmem, size);
	SemUnlock(sem);
	
	if( noClear == FALSE ) { memset(mem , 0,size );}
	return mem ;
	
	
} 

void* FastDispose(void* mem, long size)  {
	if( mem) {
		if( - (size <= 0)  & - (size != -999)  ) { throw_fastmem(QuadChara(0,'M','E','M'), (char*) "FastDispose(); size<=0");}
	
		
		
		SemLock(sem);
		deallocFromSlabArena(arena_fastmem, mem);
		SemUnlock(sem);
	}
	return NULLA;
} 

BOOLEAN FastVerify(BOOLEAN quiet)  {
	BOOLEAN failed; 
	failed =  FALSE ;
	return failed ;
} 

/*** PE/CPP/EList ***/
/* PE/CPP/EList.e 25-05-11
   A re-implementation of AmigaE's E-list functions.
   
   By Christopher S Handley:
   27-07-06 - Started coding it for PortablE.
   30-07-06 - Completed.
   14-08-06 - Updated to use the ILIST type.
   07-01-08 - Fixed DisposeList() bug.
   18-08-09 - Rewrote to store everything in it's array, rather than using an object as a header.
   06-02-11 - Added PrintL() & StringFL() functions (limited to lists of up to 8 items).
   25-05-11 - Improved PrintL() & StringFL() to handle lists of up to 20 items.
*/ /* Emulated procedures:
NewList(maxLen) RETURNS list:LIST
DisposeList(list:LIST) RETURNS NILL
ListCopy(list: LIST, other:ILIST, len=ALL) RETURNS list:LIST
ListAdd( list: LIST, other:ILIST, len=ALL) RETURNS list:LIST
ListCmp( list:ILIST, other:ILIST, len=ALL) RETURNS match:BOOL
ListMax( list: LIST) RETURNS max:VALUE
ListLen( list:ILIST) RETURNS len:VALUE
ListItem(list:ILIST, index) RETURNS value
SetList( list: LIST, newLen)

PrintL(fmtString:ARRAY OF CHAR, args=NILL:ILIST)
*/



const BOOLEAN INDEX_LENGTH=-1;
const signed char INDEX_SIZE=-2;

const signed char ITEM_SIZE=sizeof( long);
const signed char HEADER_SIZE=2			;//PortablE is hard-coded to use a value of 2 for static ILISTs
const signed char HEADER_OFFSET=HEADER_SIZE * ITEM_SIZE;
long* InitList(long* array, long maxLen) ;
long* NewList_elist(long maxLen) ;
long* DisposeList(long* list);
long* ListCopy(long* list, long* other, long len=ALL);
long* ListAdd(long* list, long* other, long len=ALL);
BOOLEAN ListCmp(long* list, long* other, long len=ALL) ;
long ListMax(long* list) ;
long ListLen(long* list) ;
long ListItem(long* list, long index) ;
void SetList(long* list, long newLen);
char* StringF2(char* eString, char* fmtString, long arg1=0, long arg2=0, long arg3=0, long arg4=0, long arg5=0, long arg6=0, long arg7=0, long arg8=0, long arg9=0, long arg10=0, long arg11=0, long arg12=0, long arg13=0, long arg14=0, long arg15=0, long arg16=0, long arg17=0, long arg18=0, long arg19=0, long arg20=0) ;



long* InitList(long* array, long maxLen)  {
	long* list=NULL;
	list = (long*) ((char*) array + (long) HEADER_OFFSET );
	list[INDEX_LENGTH] = 0;
	list[INDEX_SIZE]   = maxLen;
	return list ;
} 

long* NewList_elist(long maxLen)  {
	long* list=NULL;
	//use check
	if( maxLen < 0 ) { Throw(QuadChara(0,'E','P','U'), (char*) "EList; NewList(); maxLen<0");}
	
	//allocate e-list
	list = (long*) ((char*) FastNew((maxLen + HEADER_SIZE )* ITEM_SIZE, TRUE) + (long) HEADER_OFFSET );
	
	//init
	list[INDEX_LENGTH] = 0;
	list[INDEX_SIZE]   = maxLen;
	return list ;
} 

long* DisposeList(long* list) {
	if( list ) { FastDispose((long*) ((char*) list - (long) HEADER_OFFSET), -999)	;}//could use "list[INDEX_SIZE] + HEADER_SIZE * ITEM_SIZE" instead of -999
	return NULLL;
} 

long* ListCopy(long* list, long* other, long len) {
	//use check
	if(  list == NULLL ) { Throw(QuadChara(0,'E','P','U'), (char*) "EList; ListCopy(); list=NILL");}
	if( other == NULLL ) { Throw(QuadChara(0,'E','P','U'), (char*) "EList; ListCopy(); other=NILL");}
	if( - (len < 0)  & - (len != ALL)  ) { Throw(QuadChara(0,'E','P','U'), (char*) "EList; ListCopy(); len<0");}
	
	//empty e-list before appending to it
	list[INDEX_LENGTH] = 0;
	ListAdd(list, other, len);
	return list;
} 

long* ListAdd(long* list, long* other, long len) {
	long readIndex, maxReadIndex;
	long writeIndex, maxWriteIndex;
	
	//use check
	if(  list == NULLL ) { Throw(QuadChara(0,'E','P','U'), (char*) "EList; ListAdd(); list=NILL");}
	if( other == NULLL ) { Throw(QuadChara(0,'E','P','U'), (char*) "EList; ListAdd(); other=NILL");}
	if( - (len < 0)  & - (len != ALL)  ) { Throw(QuadChara(0,'E','P','U'), (char*) "EList; ListAdd(); len<0");}
	
	//calc end of list reading from & writing to
	maxReadIndex  = ( len==ALL )? other[INDEX_SIZE] : ( len<other[INDEX_SIZE] )? len : other[INDEX_SIZE];
	maxWriteIndex = list[INDEX_SIZE];
	
	//copy all characters that will fit
	readIndex  = 0;
	writeIndex = list[INDEX_LENGTH]		;//start writing past end of list
	while( - (writeIndex < maxWriteIndex)  & - (readIndex < maxReadIndex)) {
		list[writeIndex] = other[readIndex];
		
		writeIndex++;
		readIndex++;
	}
	
	//update list's stored length
	list[INDEX_LENGTH] = writeIndex;
	return list;
} 

BOOLEAN ListCmp(long* list, long* other, long len)  {
	BOOLEAN match;
	long index, maxIndex;
	
	//use check
	if(  list == NULLL ) { Throw(QuadChara(0,'E','P','U'), (char*) "EList; ListCmp(); list=NILL");}
	if( other == NULLL ) { Throw(QuadChara(0,'E','P','U'), (char*) "EList; ListCmp(); other=NILL");}
	if( - (len < 0)  & - (len != ALL)  ) { Throw(QuadChara(0,'E','P','U'), (char*) "EList; ListCmp(); len<0");}
	
	if( list[INDEX_LENGTH] != other[INDEX_LENGTH]) {
		match = FALSE;
	} else {
		//calc where should stop comparison
		maxIndex = ( list[INDEX_SIZE]<other[INDEX_SIZE] )? list[INDEX_SIZE] : other[INDEX_SIZE];
		if( len != ALL ) { maxIndex = ( len<maxIndex )? len : maxIndex;}
		
		//compare all characters
		match = TRUE;
		index = 0;
		while( index < maxIndex) {
			if( list[index] != other[index] ) { match = FALSE;}
			
			index++;
		if( match == FALSE) break;
		} 
	}
	return match ;
} 

long ListMax(long* list)  {
	long max;
	//use check
	if( list == NULLL ) { Throw(QuadChara(0,'E','P','U'), (char*) "EList; ListMax(); list=NILL");}
	
	max = list[INDEX_SIZE];
	return max ;
} 

long ListLen(long* list)  {
	long len;
	//use check
	if( list == NULLL ) { Throw(QuadChara(0,'E','P','U'), (char*) "EList; ListLen(); list=NILL");}
	
	len = list[INDEX_LENGTH];
	return len ;
} 

long ListItem(long* list, long index)  {
	long value;
	//use check
	if( list == NULLL ) { Throw(QuadChara(0,'E','P','U'), (char*) "EList; ListLen(); list=NILL");}
	
	//additional use check
	if( - (index < 0)  | - (index >= list[INDEX_LENGTH])  ) { Throw(QuadChara(0,'E','P','U'), (char*) "EList; ListLen(); index exceeds list bounds");}
	
	value = list[index];
	return value ;
} 

void SetList(long* list, long newLen) {
	//use check
	if( list == NULLL ) { Throw(QuadChara(0,'E','P','U'), (char*) "EList; SetList(); list=NILL");}
	if( newLen < 0  ) { Throw(QuadChara(0,'E','P','U'), (char*) "EList; SetList(); newLen<0");}
	
	//additional use check
	if( newLen > list[INDEX_SIZE] ) { Throw(QuadChara(0,'E','P','U'), (char*) "EList; SetList(); newLen exceeds list size");}
	
	//set length
	list[INDEX_LENGTH] = newLen;
	return ;
}


void PrintL(char* fmtString, long* args2)  {
	long alen;
	alen = ( args2 )? ListLen(args2) : (long) 0;
	if( alen > 20 ) { Throw(QuadChara(0,'E','P','U'), (char*) "PrintL(); args has too many items");}
	printf(fmtString ,( alen >= 1 )? args2[0] : (long) 0 ,( alen >= 2 )? args2[1] : (long) 0 ,( alen >= 3 )? args2[2] : (long) 0 ,( alen >= 4 )? args2[3] : (long) 0 ,( alen >= 5 )? args2[4] : (long) 0 ,( alen >= 6 )? args2[5] : (long) 0 ,( alen >= 7 )? args2[6] : (long) 0 ,( alen >= 8 )? args2[7] : (long) 0 ,( alen >= 9 )? args2[8] : (long) 0 ,( alen >= 10 )? args2[9] : (long) 0 ,( alen >= 11 )? args2[10] : (long) 0 ,( alen >= 12 )? args2[11] : (long) 0 ,( alen >= 13 )? args2[12] : (long) 0 ,( alen >= 14 )? args2[13] : (long) 0 ,( alen >= 15 )? args2[14] : (long) 0 ,( alen >= 16 )? args2[15] : (long) 0 ,( alen >= 17 )? args2[16] : (long) 0 ,( alen >= 18 )? args2[17] : (long) 0 ,( alen >= 19 )? args2[18] : (long) 0 ,( alen >= 20 )? args2[19] : (long) 0 );
	return ;
}

char* StringFL(char* eString, char* fmtString, long* args2)  {
	long alen, len;
	long temp_QUAD;
try {
	temp_QUAD = exception ;
	exception = 0 ;
	alen = ( args2 )? ListLen(args2) : (long) 0;
	if( alen > 20 ) { Throw(QuadChara(0,'E','P','U'), (char*) "StringFL(); args has too many items");}
	eString= StringF2(eString, fmtString, ( alen >= 1 )? args2[0] : (long) 0, ( alen >= 2 )? args2[1] : (long) 0, ( alen >= 3 )? args2[2] : (long) 0, ( alen >= 4 )? args2[3] : (long) 0, ( alen >= 5 )? args2[4] : (long) 0, ( alen >= 6 )? args2[5] : (long) 0, ( alen >= 7 )? args2[6] : (long) 0, ( alen >= 8 )? args2[7] : (long) 0, ( alen >= 9 )? args2[8] : (long) 0, ( alen >= 10 )? args2[9] : (long) 0, ( alen >= 11 )? args2[10] : (long) 0, ( alen >= 12 )? args2[11] : (long) 0, ( alen >= 13 )? args2[12] : (long) 0, ( alen >= 14 )? args2[13] : (long) 0, ( alen >= 15 )? args2[14] : (long) 0, ( alen >= 16 )? args2[15] : (long) 0, ( alen >= 17 )? args2[16] : (long) 0, ( alen >= 18 )? args2[17] : (long) 0, ( alen >= 19 )? args2[18] : (long) 0, ( alen >= 20 )? args2[19] : (long) 0);
	len = ret2 ;
} catch(...) {}
	ret2 = len;
	if (exception!=0) {throw eException;} else {EMPTY;};
	exception = temp_QUAD ;
	return eString;
} 


char* StringF2(char* eString, char* fmtString, long arg1, long arg2, long arg3, long arg4, long arg5, long arg6, long arg7, long arg8, long arg9, long arg10, long arg11, long arg12, long arg13, long arg14, long arg15, long arg16, long arg17, long arg18, long arg19, long arg20)  {
//REPLACEMENT
	long max, len;
	long temp_QUAD;
try {
	temp_QUAD = exception ;
	exception = 0 ;
	
	max = StrMax(eString);
	len = snprintf(eString ,max+1 ,fmtString ,arg1 ,arg2 ,arg3 ,arg4 ,arg5 ,arg6 ,arg7 ,arg8 ,arg9 ,arg10 ,arg11 ,arg12 ,arg13 ,arg14 ,arg15 ,arg16 ,arg17 ,arg18 ,arg19 ,arg20 );
	len = ( len<max )? len : max;
	SetStr(eString, len);
} catch(...) {}
	ret2 = len;
	if (exception!=0) {throw eException;} else {EMPTY;};
	exception = temp_QUAD ;
	return eString;
} 

/*** target/amitcp/netinet/tcp_timer ***/



#include <netinet/tcp_timer.h>

/*** targetShared/CPP/pCallback ***/
/* PortablE callback kludge for C++ */ 
long call0many (void* func) ;
long call1many (void* func, long p1) ;
long call2many (void* func, long p1, long p2) ;
long call3many (void* func, long p1, long p2, long p3) ;
long call4many (void* func, long p1, long p2, long p3, long p4) ;
long call5many (void* func, long p1, long p2, long p3, long p4, long p5) ;
long call6many (void* func, long p1, long p2, long p3, long p4, long p5, long p6) ;
long call0many (void* func)  {
	return ((long (*)(void))func )();
}
long call1many (void* func, long p1)  {
	return ((long (*)(long))func )(p1 );
}
long call2many (void* func, long p1, long p2)  {
	return ((long (*)(long,long))func )(p1 ,p2 );
}
long call3many (void* func, long p1, long p2, long p3)  {
	return ((long (*)(long,long,long))func )(p1 ,p2 ,p3 );
}
long call4many (void* func, long p1, long p2, long p3, long p4)  {
	return ((long (*)(long,long,long,long))func )(p1 ,p2 ,p3 ,p4 );
}
long call5many (void* func, long p1, long p2, long p3, long p4, long p5)  {
	return ((long (*)(long,long,long,long,long))func )(p1 ,p2 ,p3 ,p4 ,p5 );
}
long call6many (void* func, long p1, long p2, long p3, long p4, long p5, long p6)  {
	return ((long (*)(long,long,long,long,long,long))func )(p1 ,p2 ,p3 ,p4 ,p5 ,p6 );
}

/*** target/rexx/errors ***/
/* $Id: errors.h,v 1.12 2005/11/10 15:39:42 hjfrieden Exp $ */ 

#include <rexx/errors.h>

/*** target/amiga_compiler ***/
/* $Id: amiga_compiler.h,v 1.18 2005/11/18 16:19:50 obarthel Exp $ */ /* NOTE: Only GCC PPC constants have been kept. */

#include <amiga_compiler.h>

/*** target/graphics/rpattr ***/
/* $Id: rpattr.h,v 1.12 2005/11/10 15:36:43 hjfrieden Exp $ */ 

#include <graphics/rpattr.h>

/*** target/graphics/display ***/
/* $Id: display.h,v 1.12 2005/11/10 15:36:43 hjfrieden Exp $ */ 

#include <graphics/display.h>

/*** target/graphics/collide ***/
/* $Id: collide.h,v 1.12 2005/11/10 15:36:43 hjfrieden Exp $ */ 

#include <graphics/collide.h>

/*** target/graphics/coerce ***/
/* $Id: coerce.h,v 1.11 2005/11/10 15:36:43 hjfrieden Exp $ */ 

#include <graphics/coerce.h>

/*** target/exec/strings ***/


const signed char NL=10;
      const signed char DEL=0x7F;
      const signed char BS=8;
      const signed char BELL=7;
      const signed char CR=13;
      const signed char LF=10;
      const BOOLEAN EOS=0;

/*** target/exec/initializers ***/
/* $Id: initializers.h,v 1.14 2005/11/10 15:33:07 hjfrieden Exp $ */ 

#include <exec/initializers.h>

/*** target/exec/errors ***/
/* $Id: errors.h,v 1.14 2005/11/10 15:33:07 hjfrieden Exp $ */ 

#include <exec/errors.h>

const BOOLEAN ERR_OPENDEVICE=-1	;//IOERR_OPENFAIL

/*** target/exec/alerts ***/
/* $Id: alerts.h,v 1.14 2005/11/10 15:33:07 hjfrieden Exp $ */ 

#include <exec/alerts.h>

/*** target/dos/dos_lib ***/


const signed char RESERVE=4;
      const signed char VSIZE=6;

/*** target/dos/doshunks ***/
/* $Id: doshunks.h,v 1.11 2005/11/10 15:32:20 hjfrieden Exp $ */ 

#include <dos/doshunks.h>

const signed char EXT_COMMONDEF=4;

/*** target/amitcp/sys/netinclude_types ***/



#include <sys/netinclude_types.h>

/*** target/amitcp/sys/errno ***/



#include <sys/errno.h>
/*
 * $Id: errno.h,v 1.6 2007-08-26 12:30:26 obarthel Exp $
 *
 * :ts=8
 *
 * 'Roadshow' -- Amiga TCP/IP stack
 * Copyright © 2001-2007 by Olaf Barthel.
 * All Rights Reserved.
 *
 * Amiga specific TCP/IP 'C' header files;
 * Freely Distributable
 */

/*
 * Copyright (c) 1982, 1986, 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)errno.h	8.5 (Berkeley) 1/21/94
 */

//{int errno=0;}
#define __NO_NETINCLUDE_ERRNO

/*** target/resources/battclock ***/
/* $Id: battclock.h,v 1.10 2005/11/10 15:39:42 hjfrieden Exp $ */ 

#include <resources/battclock.h>
char* battclockname=NULL;
void new_battclock() ;
void new_battclock()  {
	battclockname = (char*) "battclock.resource";
	return ;
}

/*** PE/pThreadNode_dummy ***/
/* dummy implementation of pThreadNode, for single-threaded programs */ 

threadNode* FindThreadNode(threadNode** head2, long sizeOfThreadNode)  {
	threadNode* match=NULL; 
	match = head2[0];
	if( (void*) match == NULL) {
		head2[0] = match = (threadNode*) NewR(sizeOfThreadNode);
	}
	return match ;
} 

void EndAllThreadNodes(threadNode** head2)  {
	free(head2[0] );
	head2[0] = (threadNode*) NULLA;
	return ;
}

/*** PE/pSemaphores_dummy ***/
/* dummy implementation of pSemaphores, for single-threaded programs */ 

void* NewSemaphore()  {
	void* sem2=NULL; 
	sem2 = NULL;
	return sem2 ;
} 

void* DisposeSemaphore(void* sem2)  {
	void* nil=NULL; 
	sem2 = NULL;
	nil = NULL;
	return nil ;
} 

void SemLock(void* sem2)  {
	sem2 = NULL;
	return ;
}

void SemUnlock(void* sem2)  {
	sem2 = NULL;
	return ;
}

BOOLEAN SemTryLock(void* sem2)  {
	BOOLEAN success; 
	sem2 = NULL;
	success = TRUE;
	return success ;
} 

/*** files:/old/projects_aug2011/facts/facts_const ***/





const BOOLEAN MSG_TRANS_BY=0;
const signed char MSG_IN_DST=1;
const signed char MSG_TZ=2;
const signed char MSG_ERR_MEM=3;
const signed char MSG_ERR_NOHOST=4;
const signed char MSG_ERR_NOTCP=5;
const signed char MSG_ERR_DEV=6;
const signed char MSG_DST_SYNCNOW=7;
const signed char MENU_SPECIAL_SHOWTIME=8;
const signed char MENU_SETTINGS_ONERROR=9;
const signed char MENU_SETTINGS_TRYNEXT=10;
const signed char MENU_SETTINGS_NEXTSERVICE=11;
const signed char MSG_ERR_AREXX=12;
const signed char MSG_ERR_NOBATT=13;
const signed char MSG_GUI_NOTCP=14;
const signed char MSG_LOOKUP=15;
const signed char MSG_DNS_HOST_NOT_FOUND=16;
const signed char MSG_DNS_TRY_AGAIN=17;
const signed char MSG_DNS_NO_RECOVERY=18;
const signed char MSG_DNS_NO_DATA=19;
const signed char MSG_DNS_DEFAULT=20;
const signed char MSG_GUI_CONNECT=21;
const signed char MSG_CONNECT=22;
const signed char MSG_RECEIVING_DATA=23;
const signed char MSG_SENDING_REQUEST=24;
const signed char MSG_TIMED_OUT=25;
const signed char MSG_ALARM=26;
const signed char MSG_ALARM_INFO=27;
const signed char MSG_DIFF_MATCH=28;
const signed char MSG_DIFF_SLOW=29;
const signed char MSG_DIFF_FAST=30;
const signed char MSG_GUI_BATTCLOCK=31;
const signed char LAB_GADG_SERVER=32;
const signed char LAB_GADG_CHECK=33;
const signed char LAB_GADG_SAVE=34;
const signed char LAB_GADG_AMIGA=35;
const signed char LAB_GADG_DIFF=36;
const signed char LAB_GADG_TZNAME=37;
const signed char LAB_GADG_TZ=38;
const signed char LAB_GADG_DST=39;
const signed char LAB_GADG_TIMESERVER=40;
const signed char LAB_GADG_TIMEINT=41;
const signed char LAB_GADG_DEFPORT=42;
const signed char MSG_QUIT=43;
const signed char MSG_YES=44;
const signed char MSG_NO=45;
const signed char CX_DESCRIPTION=46;
const signed char MSG_ERR_WRITING=47;
const signed char MENU_PROJECT=48;
const signed char MENU_PROJECT_SERVER_INFO=49;
const signed char MENU_PROJECT_VERSION=50;
const signed char MENU_PROJECT_ABOUT=51;
const signed char MENU_PROJECT_HIDE=52;
const signed char MENU_PROJECT_QUIT=53;
const signed char MENU_SETTINGS=54;
const signed char MENU_SETTINGS_WRITE_TIME_TO=55;
const signed char MENU_SETTINGS_WRITE_SYSTEM=56;
const signed char MENU_SETTINGS_WRITE_BATTCLOCK=57;
const signed char MENU_SETTINGS_AUTODST=58;
const signed char MENU_SETTINGS_AUTODST_OFF=59;
const signed char MENU_SETTINGS_AUTODST_CUSTOM=60;
const signed char MENU_SETTINGS_AUTODST_QUIET=61;
const signed char MENU_SETTINGS_AUTODST_EMULATE=62;
const signed char MENU_SETTINGS_IGNORETZ=63;
const signed char MENU_SETTINGS_SERVICE=64;
const signed char MENU_SETTINGS_OFFSET=65;
const signed char MENU_SETTINGS_CHECK_ON_STARTUP=66;
const signed char MENU_SETTINGS_ANINC=67;
const signed char MENU_SETTINGS_COMPATIBILITY=68;
const signed char MENU_SETTINGS_COMPAT_MODE=69;
const signed char MENU_SETTINGS_COMPAT_BASE=70;
const signed char MENU_SETTINGS_COMPAT_BATTCLOCK=71;
const signed char MENU_SETTINGS_APPMENU=72;
const signed char MENU_SETTINGS_EXTRAHALF=73;
const signed char MENU_SETTINGS_SNAPSHOT=74;
const signed char MENU_SETTINGS_SAVE=75;
const signed char MENU_SPECIAL=76;
const signed char MENU_SPECIAL_RESET=77;
const signed char MENU_SPECIAL_READ=78;
const signed char MENU_SPECIAL_DOWNLOAD=79;
const signed char MENU_SPECIAL_BOREDOM=80;
const signed char MSG_EH=81;
const signed char MSG_DST_OUT=82;
const signed char MSG_DST_IN=83;
const signed char MSG_INFO_IP=84;
const signed char MSG_INFO_STRATUM=85;
const signed char MSG_INFO_REFERENCE=86;
const signed char MSG_INFO_POLL=87;
const signed char MSG_INFO_PRECISION=88;
const signed char MSG_INFO_ROOT_DELAY=89;
const signed char MSG_INFO_ROOT_DISP=90;
const signed char MSG_INFO_LEAP_ADDED=91;
const signed char MSG_INFO_LEAP_REMOVED=92;
const signed char MSG_INFO_ALARM=93;
const signed char MSG_INFO_USE_SNTP=94;
const signed char MSG_OK=95;
const signed char MSG_INFO_PUBLISHED=96;
const signed char MSG_ERR_STRATUMONE=97;
const signed char MSG_ERR_STRATUMTWO=98;
const signed char MSG_ASK_DOWNLOAD=99;
const signed char MSG_STRATUMONE_DATA=100;
const signed char MSG_STRATUMTWO_DATA=101;
const signed char MSG_INFO_NOINFO=102;
const signed char MSG_RECEIVING=103;
const signed char MSG_ERR_STRATUM_LATEST=104;
const signed char MSG_ERR_STRATUM_OTHER=105;
const signed char MSG_RECEIVED=106;
const signed char MSG_CANCEL=107;
const signed char MSG_ASK_DOWNLOAD_NEWER=108;
const signed char MSG_MOTD_NOT_AVAILABLE=109;
const signed char MSG_NEWER_NOT_AVAILABLE=110;
const signed char MSG_VERSION_CHECK=111;
const signed char MSG_NEWER_AVAILABLE=112;
const signed char MSG_ERR_CONN_DEFAULT=113;
const signed char MSG_ERR_DOWNLOADING_STRATUMONE=114;
const signed char MSG_ERR_DOWNLOADING_STRATUMTWO=115;
const signed char MSG_ERR_CONN_EBADF=116;
const signed char MSG_ERR_CONN_EADDRNOTAVAIL=117;
const signed char MSG_ERR_CONN_EAFNOSUPPORT=118;
const signed char MSG_ERR_CONN_EISCONN=119;
const signed char MSG_ERR_CONN_ETIMEDOUT=120;
const signed char MSG_ERR_CONN_ECONNREFUSED=121;
const signed char MSG_ERR_CONN_ENETUNREACH=122;
const signed char MSG_ERR_CONN_EADDRINUSE=123;
const signed char MSG_ERR_CONN_EINPROGRESS=124;
const signed char MSG_ERR_CONN_EALREADY=125;
const signed char APP_MENU=126;
const signed char MENU_SETTINGS_AUTODST_LONGTZ=127;
const short MSG_LEAPSEC=128;
const short MENU_SETTINGS_LEAPSEC=129;
const short MENU_SETTINGS_COMPAT_CLKDAEMON=130;
const short MSG_ICON_NOTIFY=131;
const short MSG_ICON_NOTIFY_OPTS=132;
const short MENU_SETTINGS_TIMEENV=133;
const short MSG_GUESSED=134;

/*** target/exec/types ***/
/* $Id: types.h,v 1.27 2005/11/10 15:33:07 hjfrieden Exp $ */ 

#include <exec/types.h>

/*** target/amitcp/netinet/udp ***/




#include <netinet/udp.h>

/*** target/amitcp/netinet/tcp ***/




#include <netinet/tcp.h>
void set_tcphdr_off(struct tcphdr* t, signed char off);
void set_tcphdr_x2(struct tcphdr* t, signed char x2);

void set_tcphdr_off(struct tcphdr* t, signed char off) {
  t->th_off = off;
	return ;
}

void set_tcphdr_x2(struct tcphdr* t, signed char x2) {
  t->th_x2 = x2;
	return ;
}

/*** target/amitcp/net/radix ***/




#include <net/radix.h>

/*** target/amitcp/sys/uio ***/




#include <sys/uio.h>

/*** target/prefs/printergfx ***/
/* placeholder module */ 

#include <prefs/printergfx.h>

/*** target/prefs/printertxt ***/
/* placeholder module */ 

#include <prefs/printertxt.h>

/*** target/hardware/dmabits ***/
/* placeholder module */ 

#include <hardware/dmabits.h>

/*** target/hardware/custom ***/
/* placeholder module */ 

#include <hardware/custom.h>

/*** target/hardware/blit ***/
/* placeholder module */ 

#include <hardware/blit.h>

/*** target/graphics/sprite ***/
/* $Id: sprite.h,v 1.12 2005/11/10 15:36:43 hjfrieden Exp $ */ 

#include <graphics/sprite.h>

/*** target/exec/resident ***/
/* $Id: resident.h,v 1.13 2005/11/10 15:33:07 hjfrieden Exp $ */ 

#include <exec/resident.h>

/*** target/exec/avl ***/
/* $Id: avl.h,v 1.11 2005/11/10 15:33:07 hjfrieden Exp $ */ 

#include <exec/avl.h>

/*** target/dos/stdio ***/
/* $Id: stdio.h,v 1.11 2005/11/10 15:32:20 hjfrieden Exp $ */ 

#include <dos/stdio.h>

/*** target/utility/tagitem ***/
/* $Id: tagitem.h,v 1.12 2005/11/10 15:39:42 hjfrieden Exp $ */ 

#include <utility/tagitem.h>

/*** target/utility/random ***/
/* $Id: random.h,v 1.10 2005/11/10 15:39:42 hjfrieden Exp $ */ 

#include <utility/random.h>

/*** target/utility/name ***/
/* $Id: name.h,v 1.12 2005/11/10 15:39:42 hjfrieden Exp $ */ 

#include <utility/name.h>

/*** target/utility/message_digest ***/
/* $Id: message_digest.h,v 1.9 2005/11/10 15:39:42 hjfrieden Exp $ */ 

#include <utility/message_digest.h>

/*** target/utility/data_structures ***/
/* $Id: data_structures.h,v 1.10 2005/11/10 15:39:42 hjfrieden Exp $ */ 

#include <utility/data_structures.h>

/*** target/exec/nodes ***/
/* $Id: nodes.h,v 1.13 2005/11/10 15:33:07 hjfrieden Exp $ */ 

#include <exec/nodes.h>

/*** target/dos/errors ***/
/* $Id: errors.h,v 1.9 2005/11/10 15:32:20 hjfrieden Exp $ */ 

#include <dos/errors.h>

/*** target/utility/date ***/
/* $Id: date.h,v 1.12 2005/11/10 15:39:42 hjfrieden Exp $ */ 

#include <utility/date.h>

/*** target/intuition/icclass ***/
/* $Id: icclass.h,v 1.10 2005/11/10 15:39:40 hjfrieden Exp $ */ 

#include <intuition/icclass.h>

/*** target/intuition/bitmapshare ***/
/* $Id: bitmapshare.h,v 1.15 2005/11/10 15:39:40 hjfrieden Exp $ */ 

#include <intuition/bitmapshare.h>

/*** target/graphics/videocontrol ***/
/* $Id: videocontrol.h,v 1.12 2005/11/10 15:36:43 hjfrieden Exp $ */ 

#include <graphics/videocontrol.h>

/*** target/exec/exectags ***/
/* $Id: exectags.h,v 1.24 2005/11/10 15:33:07 hjfrieden Exp $ */ 

#include <exec/exectags.h>

/*** target/exec/emulation ***/
/* $Id: emulation.h,v 1.14 2005/11/10 15:33:07 hjfrieden Exp $ */ 

#include <exec/emulation.h>

/*** target/graphics/gfx ***/
/* $Id: gfx.h,v 1.13 2005/11/10 15:36:43 hjfrieden Exp $ */ 

#include <graphics/gfx.h>

/*** target/utility/pack ***/
/* $Id: pack.h,v 1.13 2005/11/10 15:39:42 hjfrieden Exp $ */ 

#include <utility/pack.h>

/*** target/dos/dostags ***/
/* $Id: dostags.h,v 1.73 2005/12/30 04:57:48 cwenzel Exp $ */ 

#include <dos/dostags.h>

/*** target/graphics/graphint ***/
/* $Id: graphint.h,v 1.12 2005/11/10 15:36:43 hjfrieden Exp $ */ 

#include <graphics/graphint.h>

/*** target/dos/var ***/
/* $Id: var.h,v 1.20 2005/11/20 04:19:45 cwenzel Exp $ */ 

#include <dos/var.h>

/*** target/dos/rdargs ***/
/* $Id: rdargs.h,v 1.15 2006/01/04 09:23:08 cwenzel Exp $ */ 

#include <dos/rdargs.h>

/*** target/exec/lists ***/
/* $Id: lists.h,v 1.13 2005/11/10 15:33:07 hjfrieden Exp $ */ 

#include <exec/lists.h>

/*** target/utility/hooks ***/
/* $Id: hooks.h,v 1.12 2005/11/10 15:39:42 hjfrieden Exp $ */ 

#include <utility/hooks.h>

/*** target/graphics/scale ***/
/* $Id: scale.h,v 1.12 2005/11/10 15:36:43 hjfrieden Exp $ */ 

#include <graphics/scale.h>

/*** target/graphics/regions ***/
/* $Id: regions.h,v 1.12 2005/11/10 15:36:43 hjfrieden Exp $ */ 

#include <graphics/regions.h>

/*** target/libraries/keymap ***/
/* $VER: keymap.h 53.11 (31.1.2010) */ 

#include <libraries/keymap.h>

/*** target/exec/interrupts ***/
/* $Id: interrupts.h,v 1.22 2005/11/10 15:33:07 hjfrieden Exp $ */ 

#include <exec/interrupts.h>

const long SF_SAR=0x8000;
const signed char SIH_QUEUES=5;
const short SF_SINT=0x2000;
const short SF_TQE=0x4000;

/*** target/exec/libraries ***/
/* $Id: libraries.h,v 1.14 2005/11/10 15:33:07 hjfrieden Exp $ */ 

#include <exec/libraries.h>

/*** target/exec/ports ***/
/* $Id: ports.h,v 1.14 2005/11/10 15:33:07 hjfrieden Exp $ */ 

#include <exec/ports.h>

const signed char MP_SOFTINT=16;

/*** target/dos/exall ***/
/* $Id: exall.h,v 1.13 2005/11/10 15:32:20 hjfrieden Exp $ */ 

#include <dos/exall.h>

/*** target/exec/memory ***/
/* $Id: memory.h,v 1.18 2005/11/10 15:33:07 hjfrieden Exp $ */ 

#include <exec/memory.h>

/*** target/devices/keymap ***/
/* $VER: keymap.h 53.17 (31.1.2010) */ 

#include <devices/keymap.h>

/*** target/utility/utility ***/
/* $Id: utility.h,v 1.11 2005/11/10 15:39:42 hjfrieden Exp $ */ 

#include <utility/utility.h>
char* utilityname=NULL;
void new_utility() ;
void new_utility()  {
	utilityname = (char*) "utility.library";
	return ;
}

/*** target/libraries/identify ***/
/* $VER: identify.h 13.0 (11.09.2001) */ 

#include <libraries/identify.h>
char* identifyname=NULL;
void new_identify() ;
void new_identify()  {
	identifyname = (char*) "identify.library";
	return ;
}

/*** target/rexx/storage ***/
/* $Id: storage.h,v 1.12 2005/11/10 15:39:42 hjfrieden Exp $ */ 

#include <rexx/storage.h>

/*** target/exec/devices ***/
/* $Id: devices.h,v 1.13 2005/11/10 15:33:07 hjfrieden Exp $ */ 

#include <exec/devices.h>

/*** target/exec/tasks ***/
/* $Id: tasks.h,v 1.20 2005/11/10 15:33:07 hjfrieden Exp $ */ 

#include <exec/tasks.h>
class etask;

const signed char CHILD_NOTNEW=1;
const signed char CHILD_NOTFOUND=2;
const signed char CHILD_EXITED=3;
const signed char CHILD_ACTIVE=4;

const long SYS_TRAPALLOC=0x8000;
const long SYS_SIGALLOC=0xFFFF;

//no such object!
class etask: public object {
public:
	struct Message mn;
	struct Task* parent;
	long uniqueid;
	struct MinList children;
	short trapalloc;
	short trapable;
	long result1;
	long result2;
	struct MsgPort taskmsgport;
};

/*** target/dos/dos ***/
/* $Id: dos.h,v 1.48 2006/02/08 06:07:09 cwenzel Exp $ */ 

#include <dos/dos.h>
char* dosname=NULL;
void new_dos() ;
void new_dos()  {
	dosname = (char*) "dos.library";
	return ;
}

/*** target/rexx/rexxio ***/
/* $Id: rexxio.h,v 1.12 2005/11/10 15:39:42 hjfrieden Exp $ */ 

#include <rexx/rexxio.h>

/*** target/rexx/rxslib ***/
/* $Id: rxslib.h,v 1.12 2005/11/10 15:39:42 hjfrieden Exp $ */ 

#include <rexx/rxslib.h>
char* rxsname=NULL;
char* rxsdir=NULL;
char* rxstname=NULL;
void new_rxslib() ;
void new_rxslib()  {
	rxsname = (char*) "rexxsyslib.library";
	rxsdir = (char*) "REXX";
	rxstname = (char*) "ARexx";
	return ;
}

/*** target/exec/io ***/
/* $Id: io.h,v 1.15 2005/11/10 15:33:07 hjfrieden Exp $ */ 

#include <exec/io.h>

/*** target/exec/semaphores ***/
/* $Id: semaphores.h,v 1.13 2005/11/10 15:33:07 hjfrieden Exp $ */ 

#include <exec/semaphores.h>

const signed char SM_LOCKMSG=16;

/*** target/dos/notify ***/
/* $Id: notify.h,v 1.18 2005/11/10 15:32:20 hjfrieden Exp $ */ 

#include <dos/notify.h>

/*** target/dos/record ***/
/* $Id: record.h,v 1.11 2005/11/10 15:32:20 hjfrieden Exp $ */ 

#include <dos/record.h>

/*** target/dos/path ***/
/* $Id: path.h,v 1.9 2005/11/10 15:32:20 hjfrieden Exp $ */ 

#include <dos/path.h>

/*** target/dos/mount ***/
/* $Id: mount.h,v 1.29 2005/11/10 15:32:20 hjfrieden Exp $ */ 

#include <dos/mount.h>

/*** target/dos/filehandler ***/
/* $Id: filehandler.h,v 1.16 2005/11/10 15:32:20 hjfrieden Exp $ */ 

#include <dos/filehandler.h>

/*** target/dos/anchorpath ***/
/* $Id: anchorpath.h,v 1.15 2005/11/10 15:32:19 hjfrieden Exp $ */ 

#include <dos/anchorpath.h>

/*** target/dos/datetime ***/
/* $Id: datetime.h,v 1.13 2005/11/10 15:32:20 hjfrieden Exp $ */ 

#include <dos/datetime.h>

/*** target/workbench/startup ***/
/* $Id: startup.h,v 1.11 2005/11/10 15:39:42 hjfrieden Exp $ */ 

#include <workbench/startup.h>

/*** target/devices/serial ***/
/* $Id: serial.h,v 1.13 2005/11/10 15:31:33 hjfrieden Exp $ */ 


#define DEVICES_SERIAL_H_OBSOLETE
#include <devices/serial.h>

char* serialname=NULL;
void new_serial() ;
void new_serial()  {
	serialname = (char*) "serial.device";
	return ;
}

/*** target/devices/parallel ***/
/* $Id: parallel.h,v 1.12 2005/11/10 15:31:33 hjfrieden Exp $ */ 

#include <devices/parallel.h>
char* parallelname=NULL;
void new_parallel() ;
void new_parallel()  {
	parallelname = (char*) "parallel.device";
	return ;
}

/*** target/devices/clipboard ***/
/* $Id: clipboard.h,v 1.12 2005/11/10 15:31:33 hjfrieden Exp $ */ 

#include <devices/clipboard.h>

/*** target/exec/interfaces ***/
/* $Id: interfaces.h,v 1.15 2005/11/10 15:33:07 hjfrieden Exp $ */ 

#include <exec/interfaces.h>

/*** target/devices/timer ***/
/* $Id: timer.h,v 1.17 2005/11/10 15:31:33 hjfrieden Exp $ */ /*{
//#define __USE_OLD_TIMEVAL__
#define timeval TimeVal
#define timerequest TimeRequest
#define tv_secs Seconds
#define tv_micro Microseconds
#define tr_node Request
#define tr_time Time

#include <devices/timer.h>
}
NATIVE {TimeVal} OBJECT
NATIVE {TimeRequest} OBJECT
NATIVE {Seconds} DEF
NATIVE {Microseconds} DEF
->NATIVE {Request} DEF
NATIVE {Time} DEF

NATIVE {timeval} OBJECT timeval
    {tv_secs}	secs	:ULONG
    {tv_micro}	micro	:ULONG
ENDOBJECT
NATIVE {timerequest} OBJECT timerequest
    {tr_node}	io	:io
    {tr_time}	time	:timeval
ENDOBJECT
*/

#include <devices/timer.h>
char* timername=NULL;
void new_timer() ;
void new_timer()  {
	timername = (char*) "timer.device";
	return ;
}

/*** target/dos/dosasl ***/
/* $Id: dosasl.h,v 1.26 2005/11/10 15:32:20 hjfrieden Exp $ */ 

#include <dos/dosasl.h>

/*** target/libraries/iffparse ***/
/* $Id: iffparse.h,v 1.10 2005/11/10 15:39:41 hjfrieden Exp $ */ 

#include <libraries/iffparse.h>

/*** target/exec/execbase ***/
/* $Id: execbase.h,v 1.22 2005/11/10 15:33:07 hjfrieden Exp $ */ 

#include <exec/execbase.h>

/*** target/libraries/locale ***/
/* $Id: locale.h,v 1.23 2005/11/10 15:39:41 hjfrieden Exp $ */ 

#include <libraries/locale.h>

/*** target/intuition/notify ***/
/* $Id: notify.h,v 1.11 2005/11/10 15:39:41 hjfrieden Exp $ */ 

#include <intuition/notify.h>

/*** target/intuition/preferences ***/
/* $Id: preferences.h,v 1.13 2005/12/18 13:54:10 dwuerkner Exp $ */ 

#include <intuition/preferences.h>

/*** target/graphics/gfxnodes ***/
/* $Id: gfxnodes.h,v 1.17 2005/11/10 15:36:43 hjfrieden Exp $ */ 

#include <graphics/gfxnodes.h>

/*** target/graphics/copper ***/
/* $Id: copper.h,v 1.12 2005/11/10 15:36:43 hjfrieden Exp $ */ 

#include <graphics/copper.h>

/*** target/graphics/gels ***/
/* $Id: gels.h,v 1.16 2005/11/10 15:36:43 hjfrieden Exp $ */ 

#include <graphics/gels.h>

/*** target/graphics/layers ***/
/* $Id: layers.h,v 1.19 2005/11/10 15:36:43 hjfrieden Exp $ */ 

#include <graphics/layers.h>

/*** target/graphics/rastport ***/
/* $Id: rastport.h,v 1.16 2005/11/10 15:36:43 hjfrieden Exp $ */ 

#include <graphics/rastport.h>

/*** target/graphics/clip ***/
/* $Id: clip.h,v 1.16 2005/11/10 15:36:43 hjfrieden Exp $ */ 

#include <graphics/clip.h>

/*** target/graphics/text ***/
/* $Id: text.h,v 1.19 2005/11/10 15:36:43 hjfrieden Exp $ */ 

#include <graphics/text.h>

/*** target/datatypes/datatypes ***/
/* $Id: datatypes.h,v 1.14 2005/11/10 15:31:11 hjfrieden Exp $ */ 

#include <datatypes/datatypes.h>

/*** target/exec ***/
/* $Id: exec_protos.h,v 1.9 2006/01/07 10:35:13 dwuerkner Exp $ */ 

#include <proto/exec.h>

/*** target/amitcp/sys/socket ***/





#include <sys/socket.h>

/*** target/graphics/monitor ***/
/* $Id: monitor.h,v 1.17 2005/11/10 15:36:43 hjfrieden Exp $ */ 

#include <graphics/monitor.h>
char* default_monitor_name=NULL;
char* ntsc_monitor_name=NULL;
char* pal_monitor_name=NULL;
char* vga_monitor_name=NULL;
char* vga70_monitor_name=NULL;
void new_monitor() ;
void new_monitor()  {
	default_monitor_name = (char*) "default.monitor";
	ntsc_monitor_name = (char*) "ntsc.monitor";
	pal_monitor_name = (char*) "pal.monitor";
	vga_monitor_name = (char*) "vga.monitor";
	vga70_monitor_name = (char*) "vga70.monitor";
	return ;
}

/*** targetShared/CPP/AmigaOS/CtrlC ***/





/*** targetShared/CPP/AmigaOS/pStack ***/




long FreeStack() ;
long StackSize() ;

long FreeStack()  {
	long bytes;
	struct Task* task=NULL; long size;
	task = IExec->FindTask((char*) NULLA );
	size = (long) ((char*) (void*) task->tc_SPUpper - (long) task->tc_SPLower);
	
	bytes = (long) ((char*) (&bytes )- (long) task->tc_SPLower);
	if( - (bytes < 0)  | - (bytes > size)) {
		bytes = (long) ((char*) (void*) task->tc_SPReg - (long) task->tc_SPLower);
		if( - (bytes < 0)  | - (bytes > size)) {
			bytes = size;
		}
	}
	return bytes ;
} 

long StackSize()  {
	long bytes;
	struct Task* task=NULL;
	task = IExec->FindTask((char*) NULLA );
	bytes = (long) ((char*) (void*) task->tc_SPUpper - (long) task->tc_SPLower);
	return bytes ;
} 

/*** targetShared/AmigaOS/exec ***/




void* NewM(long size, long flags);

void* NewM(long size, long flags) {
	void* mem=NULL;
	
	if( flags & (MEMF_CHIP | MEMF_FAST | MEMF_PUBLIC | MEMF_LOCAL | MEMF_24BITDMA)) {
		printf("NewM() emulation was passed an unsupported flag\n" ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 );
		Raise(QuadChara(0,'M','E','M'));
	}
	
	mem = memset(malloc(size ), 0,size );
	if( mem == NULL ) { Raise(QuadChara(0,'M','E','M'));}
	return mem;
} 

/*** target/timer ***/
/* $Id: timer_protos.h,v 1.8 2005/11/10 15:30:32 hjfrieden Exp $ */ 


#include <proto/timer.h>

struct Device* TimerBase = NULL;
struct TimerIFace* ITimer = NULL;

const int ITimer_size = 20;
struct IORequest* ITimer_ioRequest[ITimer_size];
int ITimer_count = 0;

signed char OpenDevice_timer(char* devName, ULONG unit, struct IORequest* ioRequest, ULONG flags) ;
void CloseDevice_timer(struct IORequest* ioRequest) ;

signed char OpenDevice_timer(char* devName, ULONG unit, struct IORequest* ioRequest, ULONG flags)  {
	signed char ret;
	ret = IExec->OpenDevice(devName ,unit ,ioRequest ,flags );
	
	if(ret==0 && strcasecmp(devName, TIMERNAME)==0) {
		//get global interface for "timer.device"
		if (ITimer == NULL) {
			ITimer = (struct TimerIFace *) IExec->GetInterface((struct Library *)ioRequest->io_Device, "main", 1, NULL);
		}
		
		//add ioRequest to list
		if (ITimer_count >= ITimer_size) {
			Throw(QuadChara(0,'B','U','G'), (char*) "OpenDevice(\"timer.device\") called too many times for OS4 wrapper to handle");
		}
		ITimer_ioRequest[ITimer_count] =ioRequest;
		ITimer_count++;
	}
	;
	return ret;
} 

void CloseDevice_timer(struct IORequest* ioRequest)  {
	
	int i; bool found = false;
	
	//see if this ioRequest matches any used to open "timer.device"
	for(i=0; i<ITimer_count; i++) {
		if(ITimer_ioRequest[i] ==ioRequest) {
			found = true;
			break;
		}
	}
	
	if(found) {
		//remove ioRequest from list
		ITimer_count--;
		ITimer_ioRequest[i] = ITimer_ioRequest[ITimer_count];
		
		//drop interface for "timer.device"
		if (ITimer_count == 0) {
			IExec->DropInterface((struct Interface *) ITimer);
			ITimer = NULL;
		}
	}
	;
	IExec->CloseDevice(ioRequest );
	return ;
}

/*** target/other/battclock ***/
/* $Id: battclock_protos.h,v 1.7 2005/11/10 15:30:32 hjfrieden Exp $ */ 


#include <proto/battclock.h>

struct Library *BattClockBase = NULL;
struct BattClockIFace* IBattClock = NULL;

APTR OpenResource_battclock(char* resName) ;

APTR OpenResource_battclock(char* resName)  {
	APTR ret=NULL;
	ret = IExec->OpenResource(resName );
	
	if(ret!=NULL && strcasecmp(resName, BATTCLOCKNAME)==0) {
		if (IBattClock == NULL) {
			//get global interface for "battclock.resource"
			IBattClock = (struct BattClockIFace *) IExec->GetInterface( (struct Library *) ret, "main", 1, NULL);
		}
	}
	;
	return ret;
} 

/*** target/amitcp/net/if_arp ***/




#include <net/if_arp.h>

/*** target/amitcp/netinet/in ***/




#include <netinet/in.h>

/*** target/graphics/displayinfo ***/
/* $Id: displayinfo.h,v 1.13 2005/11/10 15:36:43 hjfrieden Exp $ */ 

#include <graphics/displayinfo.h>

/*** target/PEalias/exec ***/
/* alias module AND patches OpenLibrary() for OS4 interfaces */ 
class libNode;



class libNode: public object {
public:
	char* libName;				//library name specified during initialisation
	struct Library* library;					//actual opened library
	struct Interface** ifaceAddress;	//address of library's interface variable
	long count;
	
	libNode* next;
};

libNode* head=NULL;
void end_exec();
libNode* findLibraryNameNode(char* libName) ;
libNode* findLibraryNode(struct Library* library) ;
void InitLibrary(char* libName, struct Interface** ifaceAddress);
struct Library* OpenLibrary_exec2(char* libName, ULONG version) ;
void CloseLibrary_exec2(struct Library* library) ;

//clean-up all library nodes created by InitLibrary() calls
void end_exec() {
	libNode* node=NULL; libNode* next=NULL;
	libNode* temp_PTR_TO_libNode=NULL; libNode* temp_PTR_TO_libNode2=NULL; libNode* temp_PTR_TO_libNode3=NULL;
	
	temp_PTR_TO_libNode = head;
	head= (libNode*) NULL;
	node = temp_PTR_TO_libNode ;
	while( node) {
		temp_PTR_TO_libNode2 = node->next;
		node->next= (libNode*) NULL;
		next = temp_PTR_TO_libNode2 ;
		delete node; node=NULL; 
		temp_PTR_TO_libNode3 = next;
		next= (libNode*) NULL;
		node = temp_PTR_TO_libNode3 ;
	}
	return ;
}

//helper procedure
libNode* findLibraryNameNode(char* libName)  {
	libNode* found=NULL;
	libNode* node=NULL;
	
	found = (libNode*) NULL;
	node = head;
	while( node) {
		if( strcasecmp(node->libName ,libName )== 0 ) { found = node;}
		
		node = node->next;
	if( found) break;
	} 
	return found ;
} 

//helper procedure
libNode* findLibraryNode(struct Library* library)  {
	libNode* found=NULL;
	libNode* node=NULL;
	
	found = (libNode*) NULL;
	node = head;
	while( node) {
		if( node->library == library ) { found = node;}
		
		node = node->next;
	if( found) break;
	} 
	return found ;
} 




//this should be called once per library, from within the relevant module's new() procedure
void InitLibrary(char* libName, struct Interface** ifaceAddress) {
	libNode* node=NULL;
	libNode* temp_PTR_TO_libNode=NULL; libNode* temp_PTR_TO_libNode2=NULL;
	
	node = new libNode;
	node->libName = libName;
	node->library = (struct Library*) NULL;
	node->ifaceAddress = ifaceAddress;
	node->count   = 0;
	temp_PTR_TO_libNode = head;
	head= (libNode*) NULL;
	node->next    = temp_PTR_TO_libNode ;
	
	node->ifaceAddress[0] = (struct Interface*) NULL		;//kludge for utility.library & any other interfaces that may be automatically opened by newlib/clib2
	
	temp_PTR_TO_libNode2 = node;
	node= (libNode*) NULL;
	head = temp_PTR_TO_libNode2 ;
	return ;
}

struct Library* OpenLibrary_exec2(char* libName, ULONG version)  {
	struct Library* ret=NULL; 
	libNode* node=NULL;
	
	if( (ret = IExec->OpenLibrary(libName ,version ))) {
		//find relevant library node
		node = findLibraryNameNode(libName);
		if( (void*) node == NULL ) { Throw(QuadChara(0,'B','U','G'), (char*) "target/PEalias/exec; OpenLibrary(); InitLibrary() has not been called for supplied library");}
		
		//handle OS4 library interface
		node->count = node->count + 1;
		if( (void*) node->ifaceAddress[0] == NULL) {
			//get global interface
			node->ifaceAddress[0] = IExec->GetInterface(ret , "main", 1, NULL);
			node->library = ret;
			
		} else if( node->library != ret) {
			Throw(QuadChara(0,'B','U','G'), (char*) "target/PEalias/exec; OpenLibrary(); SUPER call unexpectedly returned a different library compared to a previous call");
		}
	}
	return ret ;
} 

void CloseLibrary_exec2(struct Library* library)  {
	libNode* node=NULL;
	
	//find relevant library node
	node = findLibraryNode(library);
	
	//handle OS4 library interface
	if( (void*) node == NULL) {
		//Throw("BUG", 'target/PEalias/exec; CloseLibrary(); InitLibrary() has not been called for supplied library')
	} else {
		node->count = node->count - 1;
		
		//drop global interface
		if( node->count == 0) {
			IExec->DropInterface(node->ifaceAddress[0] );
			node->library         = (struct Library*) NULL;
			node->ifaceAddress[0] = (struct Interface*) NULL;
		}
	}
	
	IExec->CloseLibrary(library );
	return ;
}

/*** target/amitcp/net/if ***/





#include <net/if.h>

/*** target/amitcp/netinet/ip ***/




#include <netinet/ip.h>
void set_ip_v(struct ip* i, signed char v);
void set_ip_hl(struct ip* i, signed char hl);
void set_ip_timestamp_oflw(struct ip_timestamp* i, signed char oflw);
void set_ip_timestamp_flg(struct ip_timestamp* i, signed char flg);

void set_ip_v(struct ip* i, signed char v) {
  i->ip_v = v;
	return ;
}

void set_ip_hl(struct ip* i, signed char hl) {
  i->ip_hl = hl;
	return ;
}

void set_ip_timestamp_oflw(struct ip_timestamp* i, signed char oflw) {
  i->ipt_oflw = oflw;
	return ;
}

void set_ip_timestamp_flg(struct ip_timestamp* i, signed char flg) {
  i->ipt_flg = flg;
	return ;
}

/*** target/amitcp/netdb ***/




#include <netdb.h>

/*** target/graphics/modeid ***/
/* $Id: modeid.h,v 1.13 2005/11/10 15:36:43 hjfrieden Exp $ */ 

#include <graphics/modeid.h>

/*** target/graphics/view ***/
/* $Id: view.h,v 1.14 2005/11/10 15:36:43 hjfrieden Exp $ */ 

#include <graphics/view.h>

/*** target/rexxsyslib ***/
/* $Id: rexxsyslib_protos.h,v 1.11 2005/11/10 15:30:32 hjfrieden Exp $ */ 


#include <proto/rexxsyslib.h>

struct Library* RexxSysBase = NULL;
struct RexxSysIFace* IRexxSys = NULL;

void new_rexxsyslib();

void new_rexxsyslib() {
	InitLibrary((char*) "rexxsyslib.library", (struct Interface **) &IRexxSys);
	return ;
}

/*** target/identify ***/
/* $VER: identify_protos.h 11.0 (23.4.99) */ 


#include <proto/identify.h>

struct Library* IdentifyBase = NULL;
struct IdentifyIFace* IIdentify = NULL;

void new_identify2();

void new_identify2() {
	InitLibrary((char*) "identify.library", (struct Interface **) &IIdentify);
	return ;
}

/*** target/locale ***/
/* $Id: locale_protos.h,v 1.12 2005/11/10 15:30:32 hjfrieden Exp $ */ 


#include <proto/locale.h>

struct Library* LocaleBase = NULL;
struct LocaleIFace* ILocale = NULL;

void new_locale();

void new_locale() {
	InitLibrary((char*) "locale.library", (struct Interface **) &ILocale);
	return ;
}

/*** target/utility ***/
/* $Id: utility_protos.h,v 1.10 2005/11/10 15:30:32 hjfrieden Exp $ */ 


#include <proto/utility.h>

#ifndef __NEWLIB_H__
struct Library* UtilityBase = NULL;
struct UtilityIFace* IUtility = NULL;
#endif

void new_utility2();

void new_utility2() {
	InitLibrary((char*) "utility.library", (struct Interface **) &IUtility);
	return ;
}

/*** target/amitcp/net/route ***/




#include <net/route.h>

/*** target/amitcp/sys/mbuf ***/




#include <sys/mbuf.h>

/*** target/amitcp/netinet/ip_icmp ***/





#include <netinet/ip_icmp.h>

/*** target/graphics/gfxbase ***/
/* $Id: gfxbase.h,v 1.18 2005/11/10 15:36:43 hjfrieden Exp $ */ 

#include <graphics/gfxbase.h>
char* graphicsname=NULL;
void new_gfxbase() ;
void new_gfxbase()  {
	graphicsname = (char*) "graphics.library";
	return ;
}

/*** targetShared/AmigaOS/utility ***/
/* Portable implementations of certain functions in the 'utility' module.
*/ 
long SdivMod32(long dividend, long divisor);
ULONG UdivMod32(ULONG dividend, ULONG divisor);

long SdivMod32(long dividend, long divisor) {
	long rem, quot;
	long temp_QUAD;
try {
	temp_QUAD = exception ;
	exception = 0 ;
	quot = dividend / divisor;
	rem  = dividend - quot * divisor;
} catch(...) {}
	ret2 = rem;
	if (exception!=0) {throw eException;} else {EMPTY;};
	exception = temp_QUAD ;
	return quot;
} 

ULONG UdivMod32(ULONG dividend, ULONG divisor) {
	ULONG rem; ULONG quot;
	long temp_QUAD;
try {
	temp_QUAD = exception ;
	exception = 0 ;
	quot = (ULONG) (long) ((long) dividend / (long) divisor);
	rem  = (ULONG) (long) ((long) dividend - (long) (ULONG) (long) ((long) quot * (long) divisor));
} catch(...) {}
	ret2 = (long) rem;
	if (exception!=0) {throw eException;} else {EMPTY;};
	exception = temp_QUAD ;
	return quot;
} 

/*** target/amitcp/libraries/bsdsocket ***/





#include <libraries/bsdsocket.h>
char* network_controller_port_name=NULL;
void new_bsdsocket() ;
void new_bsdsocket()  {
	network_controller_port_name = (char*) "TCP/IP Control";
	return ;
}

/*** target/graphics ***/
/* $Id: graphics_protos.h,v 1.13 2005/11/10 15:30:32 hjfrieden Exp $ */ 


#include <proto/graphics.h>

struct Library* GfxBase = NULL;
struct GraphicsIFace* IGraphics = NULL;

void new_graphics();
void end_graphics();

//automatic opening of graphics library
void new_graphics() {
	GfxBase = IExec->OpenLibrary("graphics.library" ,(ULONG) (long) 39 );
	if( (void*) GfxBase==NULL ) { CleanUp(RETURN_ERROR);}
	
	IGraphics = (struct GraphicsIFace *) IExec->GetInterface((struct Library *)GfxBase, "main", 1, NULL);
	return ;
}

//automatic closing of graphics library
void end_graphics() {
	IExec->DropInterface((struct Interface *) IGraphics);
	IExec->CloseLibrary(GfxBase );
	return ;
}

/*** target/intuition/screens ***/
/* $Id: screens.h,v 1.16 2005/11/10 15:39:41 hjfrieden Exp $ */ 

#include <intuition/screens.h>

/*** tools/arexx ***/
/* History:
03-03-2009: Disabled "rexxsysbase:=mes.libbase" as it appears to be in error, and is not supported by AROS either.  AROS issue reported by Chris Young.
12-11-2008: Added auto-opening of rexxsyslib library, as Chris Young says it is done by his copy of this AmigaE module.
11-11-2008: Released as part of PortablE r3 beta1.
*/ 
void new_arexx();
void end_arexx();
struct MsgPort* rx_OpenPort(char* portname);
void rx_ClosePort(struct MsgPort* port2);
struct RexxMsg* rx_GetMsg(struct MsgPort* port2);
void rx_ReplyMsg(struct RexxMsg* mes, long rc=0, char* resultstring=NULLA);
void rx_HandleAll(void* interpret_proc, char* portname);

void new_arexx() {
	RexxSysBase = OpenLibrary_exec2(rxsname, (ULONG) (long) 39);
	if( (void*) RexxSysBase==NULL ) { CleanUp(RETURN_ERROR);}
	return ;
}

void end_arexx() {
	CloseLibrary_exec2(RexxSysBase);
	return ;
}


struct MsgPort* rx_OpenPort(char* portname) {
  struct MsgPort* port2=NULL; signed char sig; long exc; struct Node* ln=NULL;
  long temp_QUAD;
try {
	temp_QUAD = exception ;
	exception = 0 ;
  port2 = new struct MsgPort;
  exc=0;
  IExec->Forbid();
  if( IExec->FindPort(portname )) {
    exc=QuadChara('D','O','U','B');
  } else {
    port2->mp_SigTask=(APTR) (long) IExec->FindTask((char*) NULLA );
    port2->mp_Flags=(UBYTE) (long) PA_SIGNAL;
    ln = (struct Node*) port2 ;
    ln->ln_Name=portname;
    ln->ln_Type=(UBYTE) (long) NT_MSGPORT;
    if( (long) (sig=IExec->AllocSignal(-1 ))==(long) NULL) {
      exc=QuadChara(0,'S','I','G');
    } else {
      port2->mp_SigBit=(UBYTE) (long) sig ;
      IExec->AddPort(port2 );
    }
  }
  IExec->Permit();
  if( exc ) { Raise(exc);}
} catch(...) {}
	ret2 = ( sig >= 0 )? 1 << sig : 1 >> labs(sig );
	if (exception!=0) {throw eException;} else {EMPTY;};
	exception = temp_QUAD ;
	return port2;
} 

void rx_ClosePort(struct MsgPort* port2) {
  if( port2) {
    IExec->FreeSignal((long) port2->mp_SigBit );
    IExec->RemPort(port2 );
    delete port2; port2=NULL; 
  }
	return ;
}

struct RexxMsg* rx_GetMsg(struct MsgPort* port2) {
  struct RexxMsg* mes=NULL; char* arg2=NULL;
  long temp_QUAD;
try {
	temp_QUAD = exception ;
	exception = 0 ;
  if( (mes=(struct RexxMsg*) IExec->GetMsg(port2 ))) {
    //rexxsysbase:=mes.libbase
    arg2 = (char*) mes->rm_Args[0] ;
  } else {
    arg2 = (char*) NULLA;
  }
} catch(...) {}
	ret2 = (long) arg2;
	if (exception!=0) {throw eException;} else {EMPTY;};
	exception = temp_QUAD ;
	return mes;
} 

void rx_ReplyMsg(struct RexxMsg* mes, long rc, char* resultstring) {
	char* temp_ARRAY_OF_CHAR=NULL;
  mes->rm_Result1=rc;
  mes->rm_Result2=(long) NULL;
  if( mes->rm_Action & RXFF_RESULT & - (rc==0)  & - (resultstring!=NULL)) {
    temp_ARRAY_OF_CHAR = IRexxSys->CreateArgstring(resultstring ,(ULONG) strlen(resultstring ) );
    if( NULL== temp_ARRAY_OF_CHAR ) { Raise(QuadChara(0,'M','E','M') );}
    mes->rm_Result2=(long) temp_ARRAY_OF_CHAR ;
  }
  IExec->ReplyMsg((struct Message*) mes  );
	return ;
}

void rx_HandleAll(void* interpret_proc, char* portname) {
  struct MsgPort* port2=NULL; long sig, quit; struct RexxMsg* mes=NULL; long s, rc; char* rs=NULL; long rsv;
  long temp_QUAD;
try {
	temp_QUAD = exception ;
	exception = 0 ;
  port2=(struct MsgPort*) NULL;
  quit=FALSE;
  port2=rx_OpenPort(portname);
  sig= ret2 ;
  do {
    IExec->Wait((ULONG) sig );
    do {
      mes=rx_GetMsg(port2);
      s= ret2 ;
      if( mes ) {
        quit=call1many(interpret_proc, s) ;
        rc= ret2 ;
        rsv= ret3 ; rs=(char*) rsv;
        rx_ReplyMsg(mes,rc,rs);
      }
    } while(( - ((void*) mes==NULL)  | - (quit==TRUE))==0);
  } while(( quit)==0);
  Raise(0);
} catch(...) {}
  rx_ClosePort(port2);
	if (exception!=0) {throw eException;} else {EMPTY;};
	exception = temp_QUAD ;
	return ;
}

/*** targetShared/CPP/AmigaOS/graphics ***/





struct RastPort* stdrast=NULL;
void Plot(long x, long y, long colour=1);
void Line(long x1, long y1, long x2, long y2, long colour=1);
void Box(long x1, long y1, long x2, long y2, long colour=1);
void Colour(long foreground, long background=0);
long TextF(long x, long y, char* fmtString, long arg1=0, long arg2=0, long arg3=0, long arg4=0, long arg5=0, long arg6=0, long arg7=0, long arg8=0);
struct RastPort* SetStdRast(struct RastPort* rast);
void SetTopaz(short size=8);

void Plot(long x, long y, long colour) {
	if( stdrast) {
		IGraphics->SetAPen(stdrast ,(ULONG) colour );
		IGraphics->WritePixel(stdrast ,x ,y );
	}
	return ;
}

void Line(long x1, long y1, long x2, long y2, long colour) {
	if( stdrast) {
		IGraphics->SetAPen(stdrast ,(ULONG) colour );
		IGraphics->Move(stdrast ,(short) x1  ,(short) y1  );
		IGraphics->Draw(stdrast ,x2 ,y2 );
	}
	return ;
}

void Box(long x1, long y1, long x2, long y2, long colour) {
	long xmin, ymin, xmax, ymax;
	if( stdrast) {
		IGraphics->SetAPen(stdrast ,(ULONG) colour );
		xmin = ( x1<x2 )? x1 : x2;
		xmax = ( x1>x2 )? x1 : x2;
		ymin = ( y1<y2 )? y1 : y2;
		ymax = ( y1>y2 )? y1 : y2;
		IGraphics->RectFill(stdrast ,xmin ,ymin ,xmax ,ymax );
	}
	return ;
}

void Colour(long foreground, long background) {
	if( stdrast) {
		IGraphics->SetAPen(stdrast ,(ULONG) foreground );
		IGraphics->SetBPen(stdrast ,(ULONG) background );
	}
	return ;
}

long TextF(long x, long y, char* fmtString, long arg1, long arg2, long arg3, long arg4, long arg5, long arg6, long arg7, long arg8) {
	long length;
	char* string=NULL;
	
	if( stdrast) {
		string= NewString(strlen(fmtString )*2 + 100 );
		StringF(string, fmtString, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
		
		length = EstrLen(string);
		IGraphics->Move(stdrast ,(short) x  ,(short) y  );
		IGraphics->Text(stdrast ,string ,(ULONG) length );
		
		string= DisposeString(string);
	}
	return length;
} 

struct RastPort* SetStdRast(struct RastPort* rast) {
	struct RastPort* oldstdrast=NULL;
	oldstdrast = stdrast;
	stdrast = rast;
	return oldstdrast;
} 

void SetTopaz(short size) {
	struct TextFont* font=NULL;
	struct TextAttr temp_ARRAY_OF_textattr[1 ];
	temp_ARRAY_OF_textattr [0].ta_Name = (char*) "topaz.font";
	temp_ARRAY_OF_textattr [0].ta_Style = (UBYTE) (long) FS_NORMAL;
	temp_ARRAY_OF_textattr [0].ta_Flags = (UBYTE) (long) (FPF_PROPORTIONAL | FPF_DESIGNED);
	
	if( stdrast) {
		temp_ARRAY_OF_textattr [0].ta_YSize = (UWORD) (long) size;
		font = IGraphics->OpenFont((struct TextAttr*) temp_ARRAY_OF_textattr  );
		IGraphics->SetFont(stdrast ,font );
		//CloseFont(font)
	}
	return ;
}

/*** target/graphics/gfxmacros ***/
/* $Id: gfxmacros.h,v 1.15 2005/11/10 15:36:43 hjfrieden Exp $ */ 

#include <graphics/gfxmacros.h>

/*** target/devices/inputevent ***/
/* $Id: inputevent.h,v 1.16 2005/11/10 15:31:33 hjfrieden Exp $ */ 

#include <devices/inputevent.h>

/*** target/libraries/commodities ***/
/* $Id: commodities.h,v 1.14 2006/02/20 10:23:24 sfalke Exp $ */ 

#include <libraries/commodities.h>

/*** target/intuition/intuition ***/
/* $Id: intuition.h,v 1.34 2005/12/23 19:38:25 mtantignone Exp $ */ 

#include <intuition/intuition.h>

/*** target/commodities ***/
/* $Id: commodities_protos.h,v 1.7 2005/11/10 15:30:32 hjfrieden Exp $ */ 


#include <proto/commodities.h>

struct Library* CxBase = NULL;
struct CommoditiesIFace* ICommodities = NULL;

void new_commodities();

void new_commodities() {
	InitLibrary((char*) "commodities.library", (struct Interface **) &ICommodities);
	return ;
}

/*** target/devices/printer ***/
/* $Id: printer.h,v 1.13 2005/11/21 10:59:04 dwuerkner Exp $ */ 

#include <devices/printer.h>

/*** target/intuition/pointerclass ***/
/* $Id: pointerclass.h,v 1.10 2005/11/10 15:39:41 hjfrieden Exp $ */ 

#include <intuition/pointerclass.h>

/*** target/intuition/plugins ***/
/* $Id: plugins.h,v 1.10 2005/11/10 15:39:41 hjfrieden Exp $ */ 

#include <intuition/plugins.h>

/*** target/intuition/intuitionbase ***/
/* $Id: intuitionbase.h,v 1.10 2005/11/10 15:39:41 hjfrieden Exp $ */ 

#include <intuition/intuitionbase.h>

/*** target/intuition/gui ***/
/* $Id: gui.h,v 1.28 2006/01/13 18:42:39 mtantignone Exp $ */ 

#include <intuition/gui.h>

/*** target/intuition/cghooks ***/
/* $Id: cghooks.h,v 1.11 2005/11/10 15:39:40 hjfrieden Exp $ */ 

#include <intuition/cghooks.h>

/*** target/intuition/iobsolete ***/
/* $Id: iobsolete.h,v 1.12 2005/11/10 15:39:41 hjfrieden Exp $ */ 

#include <intuition/iobsolete.h>

/*** target/workbench/workbench ***/
/* $Id: workbench.h,v 1.16 2005/11/10 15:39:42 hjfrieden Exp $ */ 

#include <workbench/workbench.h>
char* workbench_name=NULL;
void new_workbench() ;
void new_workbench()  {
	workbench_name = (char*) "workbench.library";
	return ;
}

/*** target/libraries/gadtools ***/
/* $Id: gadtools.h,v 1.14 2005/11/10 15:39:41 hjfrieden Exp $ */ 

#include <libraries/gadtools.h>

/*** target/dos/dosextens ***/
/* $Id: dosextens.h,v 1.170 2006/02/20 02:09:07 cwenzel Exp $ */ 

#include <dos/dosextens.h>

/*** target/intuition/sghooks ***/
/* $Id: sghooks.h,v 1.11 2005/11/10 15:39:41 hjfrieden Exp $ */ 

#include <intuition/sghooks.h>

/*** target/intuition/classusr ***/
/* $Id: classusr.h,v 1.14 2005/11/10 15:39:40 hjfrieden Exp $ */ 

#include <intuition/classusr.h>
char* rootclass=NULL;
char* imageclass=NULL;
char* frameiclass=NULL;
char* sysiclass=NULL;
char* fillrectclass=NULL;
char* gadgetclass=NULL;
char* propgclass=NULL;
char* strgclass=NULL;
char* buttongclass=NULL;
char* frbuttonclass=NULL;
char* groupgclass=NULL;
char* scrollergclass=NULL;
char* icclass=NULL;
char* modelclass=NULL;
char* itexticlass=NULL;
char* pointerclass=NULL;
void new_classusr() ;
void new_classusr()  {
	rootclass = (char*) "rootclass";
	imageclass = (char*) "imageclass";
	frameiclass = (char*) "frameiclass";
	sysiclass = (char*) "sysiclass";
	fillrectclass = (char*) "fillrectclass";
	gadgetclass = (char*) "gadgetclass";
	propgclass = (char*) "propgclass";
	strgclass = (char*) "strgclass";
	buttongclass = (char*) "buttongclass";
	frbuttonclass = (char*) "frbuttonclass";
	groupgclass = (char*) "groupgclass";
	scrollergclass = (char*) "scrollergclass";
	icclass = (char*) "icclass";
	modelclass = (char*) "modelclass";
	itexticlass = (char*) "itexticlass";
	pointerclass = (char*) "pointerclass";
	return ;
}

/*** target/intuition/imageclass ***/
/* $Id: imageclass.h,v 1.16 2005/11/10 15:39:40 hjfrieden Exp $ */ 

#include <intuition/imageclass.h>

/*** target/intuition/gadgetclass ***/
/* $Id: gadgetclass.h,v 1.21 2005/12/23 19:38:25 mtantignone Exp $ */ 

#include <intuition/gadgetclass.h>

/*** target/gadtools ***/
/* $Id: gadtools_protos.h,v 1.8 2005/11/10 15:30:32 hjfrieden Exp $ */ 


#include <proto/gadtools.h>

struct Library* GadToolsBase = NULL;
struct GadToolsIFace* IGadTools = NULL;

void new_gadtools();

void new_gadtools() {
	InitLibrary((char*) "gadtools.library", (struct Interface **) &IGadTools);
	return ;
}

/*** target/devices/prtbase ***/
/* $Id: prtbase.h,v 1.15 2005/12/08 07:18:57 dwuerkner Exp $ */ 

#include <devices/prtbase.h>

/*** target/dos ***/
/* $Id: dos_protos.h,v 1.108 2006/02/05 16:27:44 cwenzel Exp $ */ 


#include <proto/dos.h>

//#ifndef __NEWLIB_H__
//struct Library* DOSBase = NULL;
//struct DOSIFace* IDOS = NULL;
//#endif

void new_dos2();
void end_dos();

//automatic opening of dos library
void new_dos2() {
	DOSBase = IExec->OpenLibrary("dos.library" ,(ULONG) (long) 39 );
	if( (void*) DOSBase==NULL ) { CleanUp(RETURN_ERROR);}
	
	IDOS = (struct DOSIFace *) IExec->GetInterface((struct Library *)DOSBase, "main", 1, NULL);
	return ;
}

//automatic closing of dos library
void end_dos() {
	IExec->DropInterface((struct Interface *) IDOS);
	IExec->CloseLibrary(DOSBase );
	return ;
}

/*** target/amitcp/socket ***/






#include <proto/bsdsocket.h>

struct Library* SocketBase = NULL;
struct SocketIFace* ISocket = NULL;

void new_socket();

void new_socket() {
	InitLibrary((char*) "bsdsocket.library", (struct Interface **) &ISocket);
	return ;
}

/*** target/intuition/classes ***/
/* $Id: classes.h,v 1.11 2005/11/16 11:21:01 srupprecht Exp $ */ 

#include <intuition/classes.h>

/*** target/intuition/sysiclass ***/
/* $Id: sysiclass.h,v 1.12 2005/11/10 15:39:41 hjfrieden Exp $ */ 

#include <intuition/sysiclass.h>

/*** target/datatypes/datatypesclass ***/
/* $Id: datatypesclass.h,v 1.13 2005/11/10 15:31:11 hjfrieden Exp $ */ 

#include <datatypes/datatypesclass.h>
char* datatypesclass=NULL;
void new_datatypesclass() ;
void new_datatypesclass()  {
	datatypesclass = (char*) "datatypesclass";
	return ;
}

/*** targetShared/CPP/AmigaOS/dos ***/






char* arg=NULL;


char* argString=NULLS;
void new_dos3();
void end_dos2();
struct Process* CreateNewProc_dos2(struct TagItem* tags) ;


void new_dos3() {
	char* args2=NULL; long len;
	
	if( (args2 = IDOS->GetArgStr())) {
		len = strlen(args2 );
		argString= NewString(len);
		
		StrCopy(argString, args2);
		if( len > 0) {
			if( argString[len-1] == '\n' ) { SetStr(argString, len-1);}
		}
	}
	
	arg = argString;
	return ;
}

void end_dos2() {
	argString= DisposeString(argString);
	return ;
}

struct Process* CreateNewProc_dos2(struct TagItem* tags)  {
	struct Process* proc=NULL; 
	struct TagItem temp_ARRAY_OF_tagitem[3 ];
	temp_ARRAY_OF_tagitem [0].ti_Tag = (Tag) NP_Child;
	temp_ARRAY_OF_tagitem [0].ti_Data = (ULONG) (long) TRUE;
	temp_ARRAY_OF_tagitem [1].ti_Tag = (Tag) NP_NotifyOnDeathSigTask;
	temp_ARRAY_OF_tagitem [1].ti_Data = (ULONG) (long) IExec->FindTask((char*) NULLA );
	temp_ARRAY_OF_tagitem [2].ti_Tag = (Tag) (long) (( tags )? TAG_MORE : TAG_END);
	temp_ARRAY_OF_tagitem [2].ti_Data = (ULONG) (long) tags;
	proc = IDOS->CreateNewProc(temp_ARRAY_OF_tagitem  );
	goto finally;
finally: 0; 
	return proc ;
} 

/*** target/intuition ***/
/* $Id: intuition_protos.h,v 1.21 2005/12/12 10:31:50 dwuerkner Exp $ */ 


#include <proto/intuition.h>

struct Library* IntuitionBase = NULL;
struct IntuitionIFace* IIntuition = NULL;

void new_intuition();
void end_intuition();

//automatic opening of intuition library
void new_intuition() {
	IntuitionBase = IExec->OpenLibrary("intuition.library" ,(ULONG) (long) 39 );
	if( (void*) IntuitionBase==NULL ) { CleanUp(RETURN_ERROR);}
	
	IIntuition = (struct IntuitionIFace *) IExec->GetInterface((struct Library *)IntuitionBase, "main", 1, NULL);
	return ;
}

//automatic closing of intuition library
void end_intuition() {
	IExec->DropInterface((struct Interface *) IIntuition);
	IExec->CloseLibrary(IntuitionBase );
	return ;
}

/*** target/datatypes/pictureclass ***/
/* $Id: pictureclass.h,v 1.12 2005/11/10 15:31:11 hjfrieden Exp $ */ 

#include <datatypes/pictureclass.h>
char* picturedtclass=NULL;
void new_pictureclass() ;
void new_pictureclass()  {
	picturedtclass = (char*) "picture.datatype";
	return ;
}

/*** targetShared/CPP/AmigaOS/intuition ***/





const signed char GADGETSIZE=120;



long code; long qual; APTR iaddr=NULL;
struct TextAttr tempGlobal_ARRAY_OF_textattr[1 ];
void SetColour(struct Screen* screen, UBYTE colourreg, UBYTE r, UBYTE g, UBYTE b);
struct Window* OpenW(long x, long y, long width, long height, long idcmp, long wflags, char* title, struct Screen* screen, long sflags, struct Gadget* gadgets, struct TagItem* taglist=NULLA);
void CloseW(struct Window* wptr);
struct Screen* OpenS(long width, long height, long depth, long sflags, char* title, struct TagItem* taglist=NULLA);
void CloseS(struct Screen* sptr);
void* Gadget_intuition(void* buffer2, void* glist2, long id, long flags, long x, long y, long width, char* string);
long Mouse();
BOOLEAN LeftMouse(struct Window* win2) ;
void WaitLeftMouse(struct Window* win2);
long MouseX(struct Window* win2) ;
long MouseY(struct Window* win2) ;
long WaitIMessage(struct Window* win2);
long MsgCode() ;
long MsgQualifier() ;
APTR MsgIaddr() ;
void new_intuition2() ;

void SetColour(struct Screen* screen, UBYTE colourreg, UBYTE r, UBYTE g, UBYTE b) {
	IGraphics->SetRGB32(& screen->ViewPort ,(ULONG) (long) colourreg ,(ULONG) ((long) r << 32-8) ,(ULONG) ((long) g << 32-8) ,(ULONG) ((long) b << 32-8) );
	return ;
	//or SetRGB4(screen.viewport, colourreg, r, g, b)
}


struct Window* OpenW(long x, long y, long width, long height, long idcmp, long wflags, char* title, struct Screen* screen, long sflags, struct Gadget* gadgets, struct TagItem* taglist) {
	struct Window* wptr=NULL;
	struct TagItem temp_ARRAY_OF_tagitem[12 ];
	temp_ARRAY_OF_tagitem [0].ti_Tag = (Tag) WA_Left;
	temp_ARRAY_OF_tagitem [1].ti_Tag = (Tag) WA_Top;
	temp_ARRAY_OF_tagitem [2].ti_Tag = (Tag) WA_Width;
	temp_ARRAY_OF_tagitem [3].ti_Tag = (Tag) WA_Height;
	temp_ARRAY_OF_tagitem [4].ti_Tag = (Tag) WA_DetailPen;
	temp_ARRAY_OF_tagitem [4].ti_Data = (ULONG) (long) 0xFF ;
	temp_ARRAY_OF_tagitem [5].ti_Tag = (Tag) WA_BlockPen;
	temp_ARRAY_OF_tagitem [5].ti_Data = (ULONG) (long) 0xFF ;
	temp_ARRAY_OF_tagitem [6].ti_Tag = (Tag) WA_IDCMP;
	temp_ARRAY_OF_tagitem [7].ti_Tag = (Tag) WA_Flags;
	temp_ARRAY_OF_tagitem [9].ti_Tag = (Tag) WA_Title;
	temp_ARRAY_OF_tagitem [0].ti_Data = (ULONG) x;
	temp_ARRAY_OF_tagitem [1].ti_Data = (ULONG) y;
	temp_ARRAY_OF_tagitem [2].ti_Data = (ULONG) width;
	temp_ARRAY_OF_tagitem [3].ti_Data = (ULONG) height;
	temp_ARRAY_OF_tagitem [6].ti_Data = (ULONG) idcmp;
	temp_ARRAY_OF_tagitem [7].ti_Data = (ULONG) wflags;
	temp_ARRAY_OF_tagitem [8].ti_Tag = (Tag) (( gadgets )? WA_Gadgets : (long) TAG_SKIP);
	temp_ARRAY_OF_tagitem [8].ti_Data = (ULONG) (long) gadgets;
	temp_ARRAY_OF_tagitem [9].ti_Data = (ULONG) (long) title;
	temp_ARRAY_OF_tagitem [10].ti_Tag = (Tag) (( screen )? WA_CustomScreen : (long) TAG_SKIP);
	temp_ARRAY_OF_tagitem [10].ti_Data = (ULONG) (long) screen;
	temp_ARRAY_OF_tagitem [11].ti_Tag = (Tag) (long) (( taglist )? TAG_MORE : TAG_DONE);
	temp_ARRAY_OF_tagitem [11].ti_Data = (ULONG) (long) taglist;
	wptr = IIntuition->OpenWindowTagList((struct NewWindow*) NULL ,temp_ARRAY_OF_tagitem  );
	/*unused*/ sflags = 0;
	stdrast = wptr->RPort;
	return wptr;
} 

void CloseW(struct Window* wptr) {
	if( wptr) {
		if( wptr->RPort == stdrast ) { stdrast = (struct RastPort*) NULL;}
		IIntuition->CloseWindow(wptr );
	}
	return ;
}

struct Screen* OpenS(long width, long height, long depth, long sflags, char* title, struct TagItem* taglist) {
	struct Screen* sptr=NULL;
	long modeID;
	struct TagItem temp_ARRAY_OF_tagitem[4 ]; struct TagItem temp_ARRAY_OF_tagitem2[11 ];
	temp_ARRAY_OF_tagitem [0].ti_Tag = (Tag) BIDTAG_NominalWidth;
	temp_ARRAY_OF_tagitem [1].ti_Tag = (Tag) BIDTAG_NominalHeight;
	temp_ARRAY_OF_tagitem [2].ti_Tag = (Tag) BIDTAG_Depth;
	temp_ARRAY_OF_tagitem [3].ti_Tag = (Tag) (long) TAG_END;
	temp_ARRAY_OF_tagitem [3].ti_Data = (ULONG) (long) 0;
	temp_ARRAY_OF_tagitem2 [0].ti_Tag = (Tag) SA_Left;
	temp_ARRAY_OF_tagitem2 [0].ti_Data = (ULONG) (long) 0;
	temp_ARRAY_OF_tagitem2 [1].ti_Tag = (Tag) SA_Top;
	temp_ARRAY_OF_tagitem2 [1].ti_Data = (ULONG) (long) 0;
	temp_ARRAY_OF_tagitem2 [2].ti_Tag = (Tag) SA_Width;
	temp_ARRAY_OF_tagitem2 [3].ti_Tag = (Tag) SA_Height;
	temp_ARRAY_OF_tagitem2 [4].ti_Tag = (Tag) SA_Depth;
	temp_ARRAY_OF_tagitem2 [5].ti_Tag = (Tag) SA_DetailPen;
	temp_ARRAY_OF_tagitem2 [5].ti_Data = (ULONG) (long) DETAILPEN;
	temp_ARRAY_OF_tagitem2 [6].ti_Tag = (Tag) SA_BlockPen;
	temp_ARRAY_OF_tagitem2 [6].ti_Data = (ULONG) (long) BLOCKPEN;
	temp_ARRAY_OF_tagitem2 [7].ti_Tag = (Tag) SA_Type;
	temp_ARRAY_OF_tagitem2 [7].ti_Data = (ULONG) (long) (CUSTOMSCREEN | SHOWTITLE);
	temp_ARRAY_OF_tagitem2 [8].ti_Tag = (Tag) SA_Title;
	
	temp_ARRAY_OF_tagitem [0].ti_Data = (ULONG) width;
	temp_ARRAY_OF_tagitem [1].ti_Data = (ULONG) height;
	temp_ARRAY_OF_tagitem [2].ti_Data = (ULONG) depth;
	modeID = (long) IGraphics->BestModeIDA(temp_ARRAY_OF_tagitem  );
	
	/*newscreen.viewmodes := sflags*/	sflags = 0;
	
	temp_ARRAY_OF_tagitem2 [2].ti_Data = (ULONG) width;
	temp_ARRAY_OF_tagitem2 [3].ti_Data = (ULONG) height;
	temp_ARRAY_OF_tagitem2 [4].ti_Data = (ULONG) depth;
	temp_ARRAY_OF_tagitem2 [8].ti_Data = (ULONG) (long) title;
	temp_ARRAY_OF_tagitem2 [9].ti_Tag = (Tag) (( modeID == INVALID_ID )? (long) TAG_IGNORE : SA_DisplayID);
	temp_ARRAY_OF_tagitem2 [9].ti_Data = (ULONG) modeID;
	temp_ARRAY_OF_tagitem2 [10].ti_Tag = (Tag) (long) (( taglist )? TAG_MORE : TAG_DONE);
	temp_ARRAY_OF_tagitem2 [10].ti_Data = (ULONG) (long) taglist;
	sptr = IIntuition->OpenScreenTagList((struct NewScreen*) NULL ,temp_ARRAY_OF_tagitem2  );
	stdrast = & sptr->RastPort;
	return sptr;
} 

void CloseS(struct Screen* sptr) {
	if( sptr) {
		if( & sptr->RastPort == stdrast ) { stdrast = (struct RastPort*) NULL;}
		-IIntuition->CloseScreen(sptr );
	}
	return ;
}

void* Gadget_intuition(void* buffer2, void* glist2, long id, long flags, long x, long y, long width, char* string) {
	void* nextbuffer=NULL;
	struct Gadget* gadget=NULL; struct Border* border=NULL; struct IntuiText* intuitext=NULL; short* borderxy=NULL;
	short height; struct Gadget* firstgadget=NULL;
	signed char* zero=NULL; long i;
	
	nextbuffer = buffer2;
	height = 12;
	
	//clear memory (probably not necessary, but does no harm)
	zero = (signed char*) nextbuffer;
	for( i = 0 ; i <=(long) sizeof( struct Gadget )+ sizeof( struct Border )+ sizeof( struct IntuiText )- 1; i ++) {
		zero[i] = 0;
	}
	
	//init Gadget
	gadget = (struct Gadget*) nextbuffer ;
	gadget->NextGadget = (struct Gadget*) NULL;
	gadget->LeftEdge = (short) x ;
	gadget->TopEdge  = (short) y ;
	gadget->Width    = (short) width ;
	gadget->Height   = height;
	gadget->Flags      = (UWORD) (long) (GFLG_GADGHCOMP | (( flags & 2 )? GFLG_SELECTED : (short) 0));
	gadget->Activation = (UWORD) (long) (GACT_RELVERIFY | (( flags & 1 )? GACT_TOGGLESELECT : (short) 0));
	gadget->GadgetType = (UWORD) (long) GTYP_BOOLGADGET;
	//gadget.gadgetrender
	gadget->SelectRender = (APTR) (long) NULL;
	//gadget.gadgettext
	gadget->MutualExclude = 0;
	gadget->SpecialInfo   = (APTR) (long) NULL;
	gadget->GadgetID = (UWORD) (long) 0;
	gadget->UserData = (APTR) id ;
	nextbuffer = (void*) ((char*) nextbuffer + (long) sizeof( struct Gadget));
	
	//init Border
	border = (struct Border*) nextbuffer ;
	border->LeftEdge = 0;
	border->TopEdge  = 0;
	border->FrontPen = (UBYTE) (long) 1;
	border->BackPen  = (UBYTE) (long) 0;
	border->DrawMode = (UBYTE) (long) 0;
	border->Count = 5;
	//border.xy
	border->NextBorder = (struct Border*) NULL;
	nextbuffer = (void*) ((char*) nextbuffer + (long) sizeof( struct Border));
	
	//init IntuiText
	intuitext = (struct IntuiText*) nextbuffer ;
	intuitext->FrontPen = (UBYTE) (long) 1;
	intuitext->BackPen  = (UBYTE) (long) 0;
	intuitext->DrawMode = (UBYTE) (long) 1;
	intuitext->LeftEdge = (short) ((width - 8 * strlen(string )) / 2 );
	intuitext->TopEdge  = 2;
	intuitext->ITextFont= (struct TextAttr*) tempGlobal_ARRAY_OF_textattr ;
	intuitext->IText    = string;
	intuitext->NextText = (struct IntuiText*) NULL;
	nextbuffer = (void*) ((char*) nextbuffer + (long) sizeof( struct IntuiText));
	
	//init BorderXY
	borderxy = (short*) nextbuffer ;
	borderxy[0] = 0               ; borderxy[1] = 0;
	borderxy[2] = 0               ; borderxy[3] = height - 1;
	borderxy[4] = (short ) (width - 1 ); borderxy[5] = height - 1;
	borderxy[6] = (short ) (width - 1 ); borderxy[7] = 0;
	borderxy[8] = 0               ; borderxy[9] = 0;
	nextbuffer = (void*) ((char*) nextbuffer + (long) (10 * sizeof( short)));
	
	//link objects to each other
	gadget->GadgetRender = (APTR) (long) border;
	gadget->GadgetText   = intuitext;
	border->XY = borderxy;
	
	//add Gadget to list
	if( glist2) {
		firstgadget = (struct Gadget*) glist2;
		gadget->NextGadget      = firstgadget->NextGadget;
		firstgadget->NextGadget = gadget;
	}
	
	if( (long) ((char*) nextbuffer - (long) buffer2) > (long) GADGETSIZE ) { Throw(QuadChara(0,'B','U','G'), (char*) "Gadget(); GADGETSIZE is too small");}
	nextbuffer = (void*) ((char*) buffer2 + (long) GADGETSIZE);
	return nextbuffer;
} 

long Mouse() {
	long code2;
	code2 = 0;
	if( (*(signed char*) 0xBFE001  )&   64 ) { code2 = code2 | 1;}
	if( (*(short*) 0xDFF016  )& 1024 ) { code2 = code2 | 2;}
	if( (*(short*) 0xDFF016  )&  256 ) { code2 = code2 | 4;}
	return code2;
} 

BOOLEAN LeftMouse(struct Window* win2)  {
	BOOLEAN button;
	long idcmp; struct IntuiMessage* msg=NULL;
	
	
		IIntuition->GetWindowAttr(win2 ,(ULONG) WA_IDCMP ,(APTR) (long) & idcmp ,(ULONG) (long) sizeof( long) );
	
	-IIntuition->ModifyIDCMP(win2 ,(ULONG) (IDCMP_MOUSEBUTTONS | idcmp) );
	
	msg = (struct IntuiMessage*) IExec->GetMsg(win2->UserPort );
	if( msg) {
		button = - ((long) msg->Class == (long) IDCMP_MOUSEBUTTONS)  & - ((short) (long) msg->Code == (short) SELECTDOWN);
		IExec->ReplyMsg(& msg->ExecMessage );
	} else {
		button = FALSE;
	}
	return button ;
} 

void WaitLeftMouse(struct Window* win2) {
	long idcmp; struct IntuiMessage* msg=NULL; long class2, code2;
	
	
		IIntuition->GetWindowAttr(win2 ,(ULONG) WA_IDCMP ,(APTR) (long) & idcmp ,(ULONG) (long) sizeof( long) );
	
	-IIntuition->ModifyIDCMP(win2 ,(ULONG) (IDCMP_MOUSEBUTTONS | idcmp) );
	
	msg = (struct IntuiMessage*) IExec->GetMsg(win2->UserPort );
	do {
		if( (void*) msg == NULL) {
			IExec->WaitPort(win2->UserPort );
			msg = (struct IntuiMessage*) IExec->GetMsg(win2->UserPort );
		}
		
		class2 = (long) msg->Class;
		code2  = (long) msg->Code;
		IExec->ReplyMsg(& msg->ExecMessage );
		msg = (struct IntuiMessage*) NULL;
	} while(( - (class2 == IDCMP_MOUSEBUTTONS)  & - (code2 == SELECTDOWN))==0);
	return ;
}

long MouseX(struct Window* win2)  {
	return (long) win2->MouseX ;
}

long MouseY(struct Window* win2)  {
	return (long) win2->MouseY ;
}


long WaitIMessage(struct Window* win2) {
	long class2;
	struct MsgPort* port2=NULL; struct IntuiMessage* mes=NULL;
	
	port2 = win2->UserPort;
	while( (void*) (mes = (struct IntuiMessage*) IExec->GetMsg(port2 )) == NULL) {
		IExec->WaitPort(port2 );
	}
	
	class2 = (long) mes->Class;
	code  = (long) mes->Code;
	qual  = (long) mes->Qualifier;
	iaddr = mes->IAddress;
	IExec->ReplyMsg(& mes->ExecMessage );
	return class2;
} 

long MsgCode()  {
	return code;
}

long MsgQualifier()  {
	return qual;
}

APTR MsgIaddr()  {
	return iaddr;
}
void new_intuition2()  {
	tempGlobal_ARRAY_OF_textattr [0].ta_Name = (char*) "topaz.font";
	tempGlobal_ARRAY_OF_textattr [0].ta_YSize = (UWORD) (long) 8;
	tempGlobal_ARRAY_OF_textattr [0].ta_Style = (UBYTE) (long) FS_NORMAL;
	tempGlobal_ARRAY_OF_textattr [0].ta_Flags = (UBYTE) (long) (FPF_PROPORTIONAL | FPF_DESIGNED);
	return ;
}

/*** target/workbench/icon ***/
/* $Id: icon.h,v 1.18 2005/11/10 15:39:42 hjfrieden Exp $ */ 

#include <workbench/icon.h>
char* iconname=NULL;
void new_icon() ;
void new_icon()  {
	iconname = (char*) "icon.library";
	return ;
}

/*** target/icon ***/
/* $Id: icon_protos.h,v 1.11 2006/01/11 11:02:09 dwuerkner Exp $ */ 


#include <proto/icon.h>

struct Library* IconBase = NULL;
struct IconIFace* IIcon = NULL;

void new_icon2();

void new_icon2() {
	InitLibrary((char*) "icon.library", (struct Interface **) &IIcon);
	return ;
}

/*** target/wb ***/
/* $Id: wb_protos.h,v 1.11 2005/11/10 15:30:32 hjfrieden Exp $ */ 


#include <proto/wb.h>

struct Library* WorkbenchBase = NULL;
struct WorkbenchIFace* IWorkbench = NULL;

void new_wb();

void new_wb() {
	InitLibrary((char*) "workbench.library", (struct Interface **) &IWorkbench);
	return ;
}

/*** targetShared/CPP/AmigaOS/wb ***/





struct WBStartup* wbmessage=NULL;
void new_wb2();


void new_wb2() {
	if( main_argc == 0) {
		wbmessage = (struct WBStartup*) main_argv ;
	}
	return ;
}

/*** amigalib/cx ***/









const BOOLEAN ERR_NONE=0 ; const signed char ERR_ESC=1 ; const signed char ERR_ANGLE=2 ;
CxObj* userFilter(char** tt, char* action_name, long default_descr);
CxObj* hotKey(char* description, struct MsgPort* port2, long id);
void freeIEvents(struct InputEvent* events);
struct InputEvent* invertStringRev(char* str, struct KeyMap* km);
struct InputEvent* invertString(char* str, struct KeyMap* km);
char doesc(char c);
char* doangle(char* str, struct InputEvent* events);

CxObj* userFilter(char** tt, char* action_name, long default_descr) {
  long desc;
  CxObj* temp_PTR_TO_CXOBJ__VALUE_=NULL;
  desc = (long) NULL;
  if( - ((void*) IconBase==NULL)  | - ((void*) CxBase==NULL)  ) {
  	temp_PTR_TO_CXOBJ__VALUE_ = (CxObj*) NULL;
  	goto finally;
  }
  if( tt ) { desc=(long) IIcon->FindToolType(tt ,action_name );}
	temp_PTR_TO_CXOBJ__VALUE_ = ICommodities->CreateCxObj((ULONG) (long) CX_FILTER ,(( desc )? desc : default_descr) ,0 );
finally: 0; 
	return temp_PTR_TO_CXOBJ__VALUE_ ;
} 

CxObj* hotKey(char* description, struct MsgPort* port2, long id) {
  CxObj* filter3=NULL;
  if( (void*) CxBase==NULL ) {
  	filter3= (CxObj*) NULL;
  	goto finally;
  }
  if( (filter3=ICommodities->CreateCxObj((ULONG) (long) CX_FILTER ,(long) description ,0 ))) {
    ICommodities->AttachCxObj(filter3 ,ICommodities->CreateCxObj((ULONG) (long) CX_SEND ,(long) port2 ,(id) ) );
    ICommodities->AttachCxObj(filter3 ,ICommodities->CreateCxObj((ULONG) (long) CX_TRANSLATE ,(long) NULL ,0 ) );
    if( ICommodities->CxObjError(filter3 )) {
      ICommodities->DeleteCxObjAll(filter3 );
      filter3=(CxObj*) NULL;
    }
  }
finally: 0; 
	return filter3;
} 

void freeIEvents(struct InputEvent* events) {
  struct InputEvent* next=NULL;
  while( events) {
    next=events->ie_NextEvent;
    delete events; events=NULL; 
    events=next;
  }
	return ;
}

struct InputEvent* invertStringRev(char* str, struct KeyMap* km) {
  struct InputEvent* events=NULL; char c; struct InputEvent* curr=NULL; struct InputEvent* ie=NULL;
  long i;
  long temp_QUAD;
try {
	temp_QUAD = exception ;
	exception = 0 ;
  i = 0;
  if( (void*) CxBase==NULL ) {
  	events= (struct InputEvent*) NULL;
  	goto finally;
  }
  if( str) {
    if( str[i]) {
      curr=events=(ie = new struct InputEvent);
      c=str[i++];
      do {
        if( c=='<') {
          str=doangle(str, curr);
          i=0;
        } else {
          if( c=='\\' ) { c=doesc(str[i++]);}
          -ICommodities->InvertKeyMap((ULONG) (long) c ,curr ,km );
        }
        if( (c=str[i++])) {
          curr->ie_NextEvent=(ie = new struct InputEvent);
          curr=ie;
        }
      } while(( (long) c==(long) NULL)==0);
    }
  }
  goto finally;
finally: 0; 
} catch(...) {}
  if( exception) {
    freeIEvents(events);
    events = (struct InputEvent*) NULL;
  }
	if (exception!=0) {throw eException;} else {EMPTY;};
	exception = temp_QUAD ;
	return events;
} 

struct InputEvent* invertString(char* str, struct KeyMap* km) {
  struct InputEvent* events=NULL; char c; struct InputEvent* prev=NULL;
  long i;
  long temp_QUAD;
try {
	temp_QUAD = exception ;
	exception = 0 ;
  i = 0;
  if( (void*) CxBase==NULL ) {
  	events= (struct InputEvent*) NULL;
  	goto finally;
  }
  if( str) {
    if( str[i]) {
      while( c=str[i++]) {
        prev=events;
        events = new struct InputEvent;
        events->ie_NextEvent=prev;
        if( c=='<') {
          str=doangle(str, events);
          i = 0;
        } else {
          if( c=='\\' ) { c=doesc(str[i++]);}
          -ICommodities->InvertKeyMap((ULONG) (long) c ,events ,km );
        }
      }
    }
  }
  goto finally;
finally: 0; 
} catch(...) {}
  if( exception) {
    freeIEvents(events);
    events = (struct InputEvent*) NULL;
  }
	if (exception!=0) {throw eException;} else {EMPTY;};
	exception = temp_QUAD ;
	return events;
} 


char doesc(char c) {
  switch(  c) {
  case '\"':
  case '\'':
  case '<':
  case '\\':
    goto finally;
  	break;
  case '0':
    c= 0;
    goto finally;
  	break;
  case 'n':
  case 'r':
    c= '\r';
    goto finally;
  	break;
  case 't':
    c= '\t';
    goto finally;
  	break;
  default:
    Raise(ERR_ESC);
  	break;
  }
finally: 0; 
	return c;
} 

char* doangle(char* str, struct InputEvent* events) {
  char* s=NULL; char c; struct InputXpression ix;
  long i;
  i = 0;
  s=str;
  while( s[i] & - (s[i]!='>')  ) { i++;}
  if( (c=s[i] )) { s[i]=0;}
  if( ICommodities->ParseIX(str ,& ix )!=0 ) { Raise(ERR_ANGLE);}
  events->ie_Class=ix.ix_Class;
  events->ie_Code=ix.ix_Code;
  events->ie_Qualifier=ix.ix_Qualifier;
  s[i]=c;
  i++;
	return s;
} 

/*** PE/compatibility ***/
/* PE/compatibility.e 16-04-09
   The module is used by OPT AMIGAE.
*/ 
void WriteF(char* fmtString, long arg1=0, long arg2=0, long arg3=0, long arg4=0, long arg5=0, long arg6=0, long arg7=0, long arg8=0);

void WriteF(char* fmtString, long arg1, long arg2, long arg3, long arg4, long arg5, long arg6, long arg7, long arg8) {
	printf(fmtString ,arg1 ,arg2 ,arg3 ,arg4 ,arg5 ,arg6 ,arg7 ,arg8 );
	fflush(stdout);
	return ;
}

/*** Files:/old/projects_aug2011/facts/FACTS_portable ***/
/* FACTS: Finest Atomic Clock Time Synchroniser */ // PortablE version closes immediately after open (time signal?)





/* v4.10 in-progress version (marked as v4.9) */

/* changes:

03/06/02 - moved command execution so it only executes on successful sync
           (should be able to rx "address facts 'quit'" to quit after first sync)

03/06/02 - fixed install script; needs to be tested.

*/
/*
        EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS','Please start your TCP/IP stack!','OK'],0,NIL)

server:
    ioreq.io.command:=TR_GETSYSTIME
    DoIO(ioreq)
    ntps:=ioreq.time.secs+$92B6BD80
    ntpf:=ioreq.time.micro*4295 (4294.967295)
*/



/*
        'exec/nodes',
        'exec/lists',
        'amigalib/lists'
*/

class ntppkt;
class ntpctrl;

// DEF node[5]:ARRAY OF ln,list:PTR TO mlh

const BOOLEAN GADG_CHECK=0 ; const signed char GADG_SERVER=1 ; const signed char GADG_SAVE=2 ; const signed char GADG_AMIGA=3 ; const signed char GADG_DIFF=4 ; const signed char GADG_USE=5 ;
    const signed char GADG_TZNAME=6 ; const signed char GADG_TZ=7 ; const signed char GADG_DST=8 ; const signed char GADG_DSTNAME=9 ; const signed char GADG_SAVECONFIG=10 ; const signed char GADG_DEFPORT=11 ;
    const signed char GADG_TIMESERVER=12 ; const signed char GADG_CANCEL=13 ; const signed char GADG_QUIT=14 ; const signed char GADG_TIMEINT=15 ; const signed char GADG_PORT=16 ;
const BOOLEAN ERR_MEM=0 ; const signed char ERR_NOHOST=1 ; const signed char ERR_NOTCP=2 ; const signed char ERR_NOCLK=3 ; const signed char ERR_DEV=4 ; const signed char ERR_UTIL=5 ;
     const signed char ERR_GDTL=6 ; const signed char ERR_CXLIB=7 ; const signed char ERR_ICON=8 ; const signed char ERR_CMDY=9 ; const signed char ERR_NOBATT=10 ; const signed char ERR_WBLIB=11 ;
const BOOLEAN EVT_POPKEY=0 ; const signed char EVT_SAVEKEY=1 ;

class ntppkt: public object {
public:
    char header;
    char stratum;
    char poll;
    char precision;
    short rootdelay;
    short rootdelayf;
    short rootdisp;
    short rootdispf;
    char refid[4];
    long reftimes;
    long reftimef;
    long orgs;
    long orgf;
    long recs;
    long recf;
    long xmts;
    long xmtf;
};

class ntpctrl: public object {
public:
    char header;
    char req;
    short sequence;
    short status;
    short associd;
    short offset;
    short size;
//     data:PTR TO LONG
};

char* tmp2=NULL;
long* rargs=NULL; long templ; long args; long lock; long verstring; long buffer; long sock=-1;
char* server=NULL; char* temp=NULL; char* min=NULL; char* sec=NULL; char* wday=NULL; long wd; long mnth;
char* day=NULL; char* month=NULL; char* year=NULL; char* hour=NULL; long tzsecs=0;
long tz=0; long tzfile; long ntphost; long debug=0; long noquiet=1; long wbst=0;
struct TimeRequest* ioreq=NULL; struct MsgPort* msgport=NULL; struct Window* win=NULL; long pixsep=5; long wbstart=1;
struct TimeRequest* clkioreq=NULL; struct MsgPort* clkmsgport=NULL; long lssecs=0; struct TimeRequest* envioreq=NULL; struct MsgPort* envmsgport=NULL;
long gads[31]; struct Screen* mysc=NULL; long vi; long tbar; long utcoffset=3600;
long* glist=NULL; long cxpri=0; char* popkey=NULL; long winopen=0; struct TimeRequest* tioreq=NULL; struct MsgPort* nullmsgport=NULL;
long broker=NULL; struct MsgPort* broker_mp=NULL; long cxsigflag; long enabled=TRUE;
char* tmp=NULL; char* monthnames=NULL; char* daynames=NULL;
char* sysdate=NULL; char* errmsg=NULL; long servsecs;
long curtime; long curmics=0; char* tzname1=NULL; char* tzname2=NULL; long dst=0; long utc=1; long timeint=0;
struct InputEvent* inputevent=NULL; long filter=NULL; long menustrip=NULL; char* ntptime=NULL;
struct TimeVal* measure=NULL; struct TimeVal* timemem=NULL; long response; long arexxport; // timerbase:PTR TO lib,
long donotopenwin=0; long winsig=NULL; long filter2; char* savekey=NULL; long autodst=0;
struct NotifyRequest tznotify; long notifysig; struct MsgPort* notifyport=NULL; long ignoretz=0; long port=123; long service=123;
struct NotifyRequest ttnotify; long ttnotifysig; struct MsgPort* ttnotifyport=NULL; struct TimeVal* smeasure=NULL;
long timeport=37; long daytimeport=13; char* diffbox=NULL; long defuse=1; long defsave=1;
long ttsnot=0; long snot=0; struct MsgPort* appmenu=NULL; long appitem=NULL; long timeout=60; ntppkt pkt;
struct AppMessage* appmsg=NULL; long utcneg=0; long utcnum=1; long sntpport=123; long timeudpport=37;
char* ipaddr=NULL; long lastsntp=0; long mode2036=0; long batt2078=0; long base2015=0; long toolsmenu=1;
long extrahalf=0; long autodquiet=0; long stgemul=0; long chkstart=0; struct Locale* localeinfo=NULL;
long startmonth=3; long startwday=0; long starthour=1; long startweek=0; long endmonth=11; long endwday=0; long endhour=2; long endweek=4; long hemisphere=0;
long ostartmonth=3; long ostartwday=0; long ostarthour=1; long ostartweek=0; long oendmonth=11; long oendwday=0; long oendhour=2; long oendweek=4; long ohemisphere=0;
long dirlock=0; struct TimeRequest* dstioreq=NULL; struct MsgPort* dstmsgport=NULL; long defport=1;
long defdaytimeport=13; long deftimeport=37; long deftimeudpport=37; long defsntpport=123; long aninc=0;
long servmicros=0; ntpctrl ntpmsg; long winx=0; long winy=-1; struct Catalog* catalog=NULL;
long locstr[150]; char* lang=NULL; long mcast=0; char* yesno=NULL;
long longtz=0; long nextservice=0; long context=0; long trynext=0; char* defwintit=NULL; long txtxs=10;
long alwaysupdate=0; long zoomed=0; char* sd=NULL; long leapsec=0; long obsession=0; struct TextFont* txtf=NULL;
long socketlib; long socketver; char* cmd=NULL; long servnum=0; long clkdaemon=0; long dstoffset=3600; long nicedst=1;
char* st1db=NULL; char* st2db=NULL; long nocmd=1; long rcode=0; struct WBArg* wbarg=NULL;
char* fver=NULL; char* frev=NULL; char* frel=NULL; struct WBStartup* wbenchMsg=NULL;
long timeenv=0; long sdrift=0; long sdrifts=0; long mdrift=0; long mdrifts=0; long psecs=0; long pmics=0; long dadj=0; long pdadj=0; long guessed=0;
long difference=0; long lastsyncok=0; long forcesync=0;
long* tempGlobal_ILIST=NULL; long temp_ARRAY_OF_VALUE[5]; long* tempGlobal_ILIST2=NULL; long temp_ARRAY_OF_VALUE2[14]; long* tempGlobal_ILIST3=NULL; long temp_ARRAY_OF_VALUE3[15]; long* tempGlobal_ILIST4=NULL; long temp_ARRAY_OF_VALUE4[7]; long* tempGlobal_ILIST5=NULL; long temp_ARRAY_OF_VALUE5[7]; long* tempGlobal_ILIST6=NULL; long temp_ARRAY_OF_VALUE6[5]; long* tempGlobal_ILIST7=NULL; long temp_ARRAY_OF_VALUE7[5]; long* tempGlobal_ILIST8=NULL; long temp_ARRAY_OF_VALUE8[5]; long* tempGlobal_ILIST9=NULL; long temp_ARRAY_OF_VALUE9[5]; long* tempGlobal_ILIST10=NULL; long temp_ARRAY_OF_VALUE10[5]; struct TimeVal tempGlobal_ARRAY_OF_timeval[1 ]; long* tempGlobal_ILIST11=NULL; long temp_ARRAY_OF_VALUE11[5]; long* tempGlobal_ILIST12=NULL; long temp_ARRAY_OF_VALUE12[5]; long* tempGlobal_ILIST13=NULL; long temp_ARRAY_OF_VALUE13[5]; long* tempGlobal_ILIST14=NULL; long temp_ARRAY_OF_VALUE14[5]; long* tempGlobal_ILIST15=NULL; long temp_ARRAY_OF_VALUE15[5]; long* tempGlobal_ILIST16=NULL; long temp_ARRAY_OF_VALUE16[5]; long* tempGlobal_ILIST17=NULL; long temp_ARRAY_OF_VALUE17[5]; long* tempGlobal_ILIST18=NULL; long temp_ARRAY_OF_VALUE18[5]; long* tempGlobal_ILIST19=NULL; long temp_ARRAY_OF_VALUE19[5]; long* tempGlobal_ILIST20=NULL; long temp_ARRAY_OF_VALUE20[5]; long* tempGlobal_ILIST21=NULL; long temp_ARRAY_OF_VALUE21[7]; long* tempGlobal_ILIST22=NULL; long temp_ARRAY_OF_VALUE22[5]; long* tempGlobal_ILIST23=NULL; long temp_ARRAY_OF_VALUE23[5]; long* tempGlobal_ILIST24=NULL; long temp_ARRAY_OF_VALUE24[5]; long* tempGlobal_ILIST25=NULL; long temp_ARRAY_OF_VALUE25[5]; long* tempGlobal_ILIST26=NULL; long temp_ARRAY_OF_VALUE26[7]; long* tempGlobal_ILIST27=NULL; long temp_ARRAY_OF_VALUE27[7]; long* tempGlobal_ILIST28=NULL; long temp_ARRAY_OF_VALUE28[7]; long* tempGlobal_ILIST29=NULL; long temp_ARRAY_OF_VALUE29[33]; long* tempGlobal_ILIST30=NULL; long temp_ARRAY_OF_VALUE30[5]; long* tempGlobal_ILIST31=NULL; long temp_ARRAY_OF_VALUE31[3]; struct NewGadget tempGlobal_ARRAY_OF_newgadget[1 ]; long* tempGlobal_ILIST32=NULL; long temp_ARRAY_OF_VALUE32[7]; struct NewGadget tempGlobal_ARRAY_OF_newgadget2[1 ]; long* tempGlobal_ILIST33=NULL; long temp_ARRAY_OF_VALUE33[5]; struct NewGadget tempGlobal_ARRAY_OF_newgadget3[1 ]; long* tempGlobal_ILIST34=NULL; long temp_ARRAY_OF_VALUE34[7]; struct NewGadget tempGlobal_ARRAY_OF_newgadget4[1 ]; long* tempGlobal_ILIST35=NULL; long temp_ARRAY_OF_VALUE35[9]; struct NewGadget tempGlobal_ARRAY_OF_newgadget5[1 ]; long* tempGlobal_ILIST36=NULL; long temp_ARRAY_OF_VALUE36[5]; struct NewGadget tempGlobal_ARRAY_OF_newgadget6[1 ]; long* tempGlobal_ILIST37=NULL; long temp_ARRAY_OF_VALUE37[7]; struct NewGadget tempGlobal_ARRAY_OF_newgadget7[1 ]; long* tempGlobal_ILIST38=NULL; long temp_ARRAY_OF_VALUE38[7]; struct NewGadget tempGlobal_ARRAY_OF_newgadget8[1 ]; long* tempGlobal_ILIST39=NULL; long temp_ARRAY_OF_VALUE39[3]; struct NewGadget tempGlobal_ARRAY_OF_newgadget9[1 ]; long* tempGlobal_ILIST40=NULL; long temp_ARRAY_OF_VALUE40[3]; struct NewGadget tempGlobal_ARRAY_OF_newgadget10[1 ]; long* tempGlobal_ILIST41=NULL; long temp_ARRAY_OF_VALUE41[7]; struct NewGadget tempGlobal_ARRAY_OF_newgadget11[1 ]; long* tempGlobal_ILIST42=NULL; long temp_ARRAY_OF_VALUE42[7]; struct NewGadget tempGlobal_ARRAY_OF_newgadget12[1 ]; long* tempGlobal_ILIST43=NULL; long temp_ARRAY_OF_VALUE43[5]; struct NewGadget tempGlobal_ARRAY_OF_newgadget13[1 ]; long* tempGlobal_ILIST44=NULL; long temp_ARRAY_OF_VALUE44[7]; long* tempGlobal_ILIST45=NULL; long temp_ARRAY_OF_VALUE45[5]; long* tempGlobal_ILIST46=NULL; long temp_ARRAY_OF_VALUE46[5]; long* tempGlobal_ILIST47=NULL; long temp_ARRAY_OF_VALUE47[7]; long* tempGlobal_ILIST48=NULL; long temp_ARRAY_OF_VALUE48[7]; long* tempGlobal_ILIST49=NULL; long temp_ARRAY_OF_VALUE49[5]; long* tempGlobal_ILIST50=NULL; long temp_ARRAY_OF_VALUE50[5]; long* tempGlobal_ILIST51=NULL; long temp_ARRAY_OF_VALUE51[5]; long* tempGlobal_ILIST52=NULL; long temp_ARRAY_OF_VALUE52[5]; long* tempGlobal_ILIST53=NULL; long temp_ARRAY_OF_VALUE53[5]; long* tempGlobal_ILIST54=NULL; long temp_ARRAY_OF_VALUE54[7]; long* tempGlobal_ILIST55=NULL; long temp_ARRAY_OF_VALUE55[3]; long* tempGlobal_ILIST56=NULL; long temp_ARRAY_OF_VALUE56[5]; long* tempGlobal_ILIST57=NULL; long temp_ARRAY_OF_VALUE57[5]; long* tempGlobal_ILIST58=NULL; long temp_ARRAY_OF_VALUE58[7]; long* tempGlobal_ILIST59=NULL; long temp_ARRAY_OF_VALUE59[5]; long* tempGlobal_ILIST60=NULL; long temp_ARRAY_OF_VALUE60[7]; struct NewBroker tempGlobal_ARRAY_OF_newbroker[1 ]; long* tempGlobal_ILIST61=NULL; long temp_ARRAY_OF_VALUE61[12]; long* tempGlobal_ILIST62=NULL; long temp_ARRAY_OF_VALUE62[8]; long* tempGlobal_ILIST63=NULL; long temp_ARRAY_OF_VALUE63[8]; long* tempGlobal_ILIST64=NULL; long temp_ARRAY_OF_VALUE64[8]; long* tempGlobal_ILIST65=NULL; long temp_ARRAY_OF_VALUE65[8]; long* tempGlobal_ILIST66=NULL; long temp_ARRAY_OF_VALUE66[8]; long* tempGlobal_ILIST67=NULL; long temp_ARRAY_OF_VALUE67[8]; long* tempGlobal_ILIST68=NULL; long temp_ARRAY_OF_VALUE68[8]; long* tempGlobal_ILIST69=NULL; long temp_ARRAY_OF_VALUE69[8]; long* tempGlobal_ILIST70=NULL; long temp_ARRAY_OF_VALUE70[7]; struct NewMenu* tempGlobal_ARRAY_OF_newmenu=NULL; long* tempGlobal_ILIST71=NULL; long temp_ARRAY_OF_VALUE71[5]; long* tempGlobal_ILIST72=NULL; long temp_ARRAY_OF_VALUE72[7]; long* tempGlobal_ILIST73=NULL; long temp_ARRAY_OF_VALUE73[5]; long* tempGlobal_ILIST74=NULL; long temp_ARRAY_OF_VALUE74[7]; long* tempGlobal_ILIST75=NULL; long temp_ARRAY_OF_VALUE75[7]; long* tempGlobal_ILIST76=NULL; long temp_ARRAY_OF_VALUE76[5]; long* tempGlobal_ILIST77=NULL; long temp_ARRAY_OF_VALUE77[5]; long* tempGlobal_ILIST78=NULL; long temp_ARRAY_OF_VALUE78[5]; long* tempGlobal_ILIST79=NULL; long temp_ARRAY_OF_VALUE79[5]; long* tempGlobal_ILIST80=NULL; long temp_ARRAY_OF_VALUE80[7]; long* tempGlobal_ILIST81=NULL; long temp_ARRAY_OF_VALUE81[7]; long* tempGlobal_ILIST82=NULL; long temp_ARRAY_OF_VALUE82[5]; long* tempGlobal_ILIST83=NULL; long temp_ARRAY_OF_VALUE83[5]; long* tempGlobal_ILIST84=NULL; long temp_ARRAY_OF_VALUE84[5]; long* tempGlobal_ILIST85=NULL; long temp_ARRAY_OF_VALUE85[5]; long* tempGlobal_ILIST86=NULL; long temp_ARRAY_OF_VALUE86[5]; long* tempGlobal_ILIST87=NULL; long temp_ARRAY_OF_VALUE87[5]; long* tempGlobal_ILIST88=NULL; long temp_ARRAY_OF_VALUE88[7]; long* tempGlobal_ILIST89=NULL; long temp_ARRAY_OF_VALUE89[5];
int main(int argc, char** argv) ;
void dowbstartup();
long retrievetime();
void typetwo();
char* displaytimes(long returnsystime=0, long gmtadj=0);
char* sectostr(long secssince, long gmtadj=0);
void setclock(long seconds);
void getclock();
void resetclock();
void startgui();
long setupgadgets();
void actuallydosomething();
void addcommodity();
void readtooltypes();
void closegui();
void savetz(long useonly=0);
void saveconfig();
void menu();
void about();
void starttint();
void startclkint();
void stopclkint();
void startenvint();
void stopenvint();
long writetimeenv();
void dodefop(long seconds=0);
void dstswitch(long on, long quiet=2);
void setupnotify();
void startttnotify();
void adjustclock(long addon);
void closetcp();
void gettimeport();
void sntpinfo();
void startappmenu();
void removeappitem();
void dstpresets(long autodst2);
void startdst(long startval);
void verifyportnumbers();
void doaninc();
void connecterror();
long chkautodst_generic(long nextyear=0);
void readtz();
void tryalternative(long dodef=1);
void selectservice();
void updategui();
void syncclock(long dodef=1);
void showtime();
void startleapsec(long quick=0);
void leapsecchg();
void saveleapsecs();
void checkoldleapsecs();
void changelong(long state);
void stopnotify();
void stopttnotify();
void killautodsttimer();
void loadserver();
void autocompat();
signed char clockdrift();
void new_facts_portable() ;
void end_facts_portable() ;

int main(int argc, char** argv)  {
long msg, colon, ls;

main_argc = argc;

main_argv = argv;

try {
	new_base();
	new_base2();
	new_fastmem();
	new_battclock();
	new_utility();
	new_identify();
	new_dos();
	new_rxslib();
	new_serial();
	new_parallel();
	new_timer();
	new_monitor();
	new_rexxsyslib();
	new_identify2();
	new_locale();
	new_utility2();
	new_gfxbase();
	new_bsdsocket();
	new_graphics();
	new_arexx();
	new_commodities();
	new_workbench();
	new_classusr();
	new_gadtools();
	new_dos2();
	new_socket();
	new_datatypesclass();
	new_dos3();
	new_intuition();
	new_pictureclass();
	new_intuition2();
	new_icon();
	new_icon2();
	new_wb();
	new_wb2();
	new_facts_portable();

verstring=(long) "$VER: FACTS 5.8 (13.09.2009)";
StringF(fver,(char*) "5");
StringF(frev,(char*) "8");
StringF(frel,(char*) "5.06");

socketlib=(long) "bsdsocket.library";
socketver=4;

StrCopy(defwintit,(char*) "FACTS");

StrCopy(popkey,(char*) "ctrl alt f");

if( (void*) wbmessage==NULL) {
    wbstart=0;
    WriteF((char*) "%s %s by Chris Young <chris@unsatisfactorysoftware.co.uk>\n",(long) defwintit,(long) frel);
    WriteF((char*) "\251 1998-2009 Unsatisfactory Software\n\n");
}
if( (buffer=(long) IExec->AllocMem((ULONG) (long) 101 ,(ULONG) MEMF_CLEAR ))==0 ) { Raise(ERR_MEM);}

if( (void*) (UtilityBase=OpenLibrary_exec2((char*) "utility.library",(ULONG) (long) 0))==NULL ) { Raise(ERR_UTIL);}

verifyportnumbers();

locstr[MSG_TRANS_BY]=(long) "Chris Young";
locstr[MSG_DIFF_SLOW]=(long) "Clock was %s secs slow";
locstr[MSG_DIFF_FAST]=(long) "Clock was %s secs fast";
locstr[MSG_IN_DST]=(long) "You are currently in daylight saving time!";
locstr[MSG_TZ]=(long) "Your timezone is %s (%ld hrs from GMT)";
locstr[MSG_ERR_MEM]=(long) "Not enough memory!";
locstr[MSG_ERR_NOHOST]=(long) "Cannot connect to server";
locstr[MSG_ERR_NOTCP]=(long) "Please start an AmiTCP compatible TCP/IP stack!";
locstr[MSG_ERR_DEV]=(long) "Unable to open %s";
locstr[MSG_DST_SYNCNOW]=(long) "Synchronise Now";
locstr[MENU_SPECIAL_SHOWTIME]=(long) "Show Updating Time";
locstr[MENU_SETTINGS_ONERROR]=(long) "On Error";
locstr[MENU_SETTINGS_TRYNEXT]=(long) "Try Next Server";
locstr[MENU_SETTINGS_NEXTSERVICE]=(long) "Try Next Service";
locstr[MSG_ERR_AREXX]=(long) "FACTS requires ARexx to be active.\n\nMake sure RexxMast is running!";
locstr[MSG_ERR_NOBATT]=(long) "Error with battery backed-up clock";
locstr[MSG_GUI_NOTCP]=(long) "No TCP/IP stack!";
locstr[MSG_LOOKUP]=(long) "Looking up host";
locstr[MSG_DNS_HOST_NOT_FOUND]=(long) "Host unknown";
locstr[MSG_DNS_TRY_AGAIN]=(long) "Temporary error";
locstr[MSG_DNS_NO_RECOVERY]=(long) "Unexpected server failure";
locstr[MSG_DNS_NO_DATA]=(long) "No IP address for host";
locstr[MSG_DNS_DEFAULT]=(long) "Error with DNS lookup";
locstr[MSG_GUI_CONNECT]=(long) "Connect";
locstr[MSG_CONNECT]=(long) "Attempting to connect to";
locstr[MSG_RECEIVING_DATA]=(long) "Receiving data...";
locstr[MSG_SENDING_REQUEST]=(long) "Sending request...";
locstr[MSG_TIMED_OUT]=(long) "Timed out";
locstr[MSG_ALARM]=(long) "Alarm Condition";
locstr[MSG_ALARM_INFO]=(long) "clock not synchronised";
locstr[MSG_DIFF_MATCH]=(long) "Clocks Match!";
locstr[MSG_DIFF_SLOW]=(long) "Clock was %ld secs slow";
locstr[MSG_DIFF_FAST]=(long) "Clock was %ld secs fast";
locstr[MSG_GUI_BATTCLOCK]=(long) "An error occured while opening\nbattclock.resource";
locstr[LAB_GADG_SERVER]=(long) "Server:";
locstr[LAB_GADG_CHECK]=(long) "Check";
locstr[LAB_GADG_SAVE]=(long) "Adjust";
locstr[LAB_GADG_AMIGA]=(long) "Amiga:";
locstr[LAB_GADG_DIFF]=(long) "Diff:";
locstr[LAB_GADG_TZNAME]=(long) "Timezone:";
locstr[LAB_GADG_TZ]=(long) "\261hrs";
locstr[LAB_GADG_DST]=(long) "DST";
locstr[LAB_GADG_TIMESERVER]=(long) "Addr:";
locstr[LAB_GADG_TIMEINT]=(long) "Int";
locstr[LAB_GADG_DEFPORT]=(long) "Def";
locstr[MSG_QUIT]=(long) "Are you sure you want to quit?";
locstr[MSG_YES]=(long) "Yes";
locstr[MSG_NO]=(long) "No";
locstr[CX_DESCRIPTION]=(long) "Set your clock to NTP server time";
locstr[MSG_ERR_WRITING]=(long) "Error writing %s";
locstr[MENU_PROJECT]=(long) "Project";
locstr[MENU_PROJECT_SERVER_INFO]=(long) "Server Info...";
locstr[MENU_PROJECT_VERSION]=(long) "Version/MOTD...";
locstr[MENU_PROJECT_ABOUT]=(long) "About...";
locstr[MENU_PROJECT_HIDE]=(long) "Hide";
locstr[MENU_PROJECT_QUIT]=(long) "Quit";
locstr[MENU_SETTINGS]=(long) "Settings";
locstr[MENU_SETTINGS_WRITE_TIME_TO]=(long) "Write Time To";
locstr[MENU_SETTINGS_WRITE_SYSTEM]=(long) "System";
locstr[MENU_SETTINGS_WRITE_BATTCLOCK]=(long) "BattClock";
locstr[MENU_SETTINGS_AUTODST]=(long) "AutoDST";
locstr[MENU_SETTINGS_AUTODST_OFF]=(long) "Off";
locstr[MENU_SETTINGS_AUTODST_CUSTOM]=(long) "Custom";
locstr[MENU_SETTINGS_AUTODST_QUIET]=(long) "Quiet";
locstr[MENU_SETTINGS_AUTODST_EMULATE]=(long) "Emulate SummerTimeGuard";
locstr[MENU_SETTINGS_IGNORETZ]=(long) "Ignore TZ Changes";
locstr[MENU_SETTINGS_SERVICE]=(long) "Service";
locstr[MENU_SETTINGS_OFFSET]=(long) "Timeserver Offset";
locstr[MENU_SETTINGS_CHECK_ON_STARTUP]=(long) "Check On Startup";
locstr[MENU_SETTINGS_ANINC]=(long) "Use Aninc";
locstr[MENU_SETTINGS_COMPATIBILITY]=(long) "Compatibility";
locstr[MENU_SETTINGS_COMPAT_MODE]=(long) "2036 Mode";
locstr[MENU_SETTINGS_COMPAT_BASE]=(long) "2015 Is 2 Digit Base";
locstr[MENU_SETTINGS_COMPAT_BATTCLOCK]=(long) "BattClock 2078";
locstr[MENU_SETTINGS_APPMENU]=(long) "AppMenu Item";
locstr[MENU_SETTINGS_EXTRAHALF]=(long) "Extra Half Hour";
locstr[MENU_SETTINGS_SNAPSHOT]=(long) "Snapshot Window";
locstr[MENU_SETTINGS_SAVE]=(long) "Save Settings";
locstr[MENU_SPECIAL]=(long) "Special";
locstr[MENU_SPECIAL_RESET]=(long) "Reset BattClock";
locstr[MENU_SPECIAL_READ]=(long) "Read BattClock Time";
locstr[MENU_SPECIAL_DOWNLOAD]=(long) "Download Databases";
locstr[MENU_SPECIAL_BOREDOM]=(long) "Boredom Relief...";
locstr[MSG_EH]=(long) "Eh?";
locstr[MSG_DST_OUT]=(long) "No longer in DST";
locstr[MSG_DST_IN]=(long) "Now in DST";
locstr[MSG_INFO_IP]=(long) "IP Address:";
locstr[MSG_INFO_STRATUM]=(long) "Stratum:";
locstr[MSG_INFO_REFERENCE]=(long) "Reference:";
locstr[MSG_INFO_POLL]=(long) "Poll Interval:";
locstr[MSG_INFO_PRECISION]=(long) "Precision:";
locstr[MSG_INFO_ROOT_DELAY]=(long) "Root Delay:";
locstr[MSG_INFO_ROOT_DISP]=(long) "Root Dispersion:";
locstr[MSG_INFO_LEAP_ADDED]=(long) "Leap second will be added 00:00\n(last minute will have 61 seconds)";
locstr[MSG_INFO_LEAP_REMOVED]=(long) "Leap second will be removed 00:00\n(last minute will have 59 seconds)";
locstr[MSG_INFO_ALARM]=(long) "ALARM!";
locstr[MSG_INFO_USE_SNTP]=(long) "Use SNTP service for more information";
locstr[MSG_OK]=(long) "OK";
locstr[MSG_INFO_PUBLISHED]=(long) "Published Information...";
locstr[MSG_ERR_STRATUMONE]=(long) "Could not find Stratum 1 information file.";
locstr[MSG_ERR_STRATUMTWO]=(long) "Could not find Stratum 2 information file.";
locstr[MSG_ASK_DOWNLOAD]=(long) "Do you want to download this now?";
locstr[MSG_STRATUMONE_DATA]=(long) "(Stratum 1 Database)";
locstr[MSG_STRATUMTWO_DATA]=(long) "(Stratum 2 Database)";
locstr[MSG_INFO_NOINFO]=(long) "No information available";
locstr[MSG_RECEIVING]=(long) "Receiving";
locstr[MSG_ERR_STRATUM_LATEST]=(long) "You already have the latest version\nof the Stratum %s database.";
locstr[MSG_ERR_STRATUM_OTHER]=(long) "Stratum %s database\nError reported from server:";
locstr[MSG_RECEIVED]=(long) "Document Received";
locstr[MSG_CANCEL]=(long) "Cancel";
locstr[MSG_ASK_DOWNLOAD_NEWER]=(long) "Newer data available:\nDownload?";
locstr[MSG_MOTD_NOT_AVAILABLE]=(long) "MOTD: (not available)";
locstr[MSG_NEWER_NOT_AVAILABLE]=(long) "A new version of FACTS is not yet available";
locstr[MSG_VERSION_CHECK]=(long) "Checking Version...";
locstr[MSG_NEWER_AVAILABLE]=(long) "A new version of FACTS is\navailable for download.";
locstr[MSG_ERR_CONN_DEFAULT]=(long) "Error connecting";
locstr[MSG_ERR_DOWNLOADING_STRATUMONE]=(long) "Error while downloading Stratum 1 information";
locstr[MSG_ERR_DOWNLOADING_STRATUMTWO]=(long) "Error while downloading Stratum 2 information";
locstr[MSG_ERR_CONN_EBADF]=(long) "Socket descriptor not valid";
locstr[MSG_ERR_CONN_EADDRNOTAVAIL]=(long) "Address not available";
locstr[MSG_ERR_CONN_EAFNOSUPPORT]=(long) "Address cannot be used with this socket";
locstr[MSG_ERR_CONN_EISCONN]=(long) "Already connected";
locstr[MSG_ERR_CONN_ETIMEDOUT]=(long) "Connection timed out";
locstr[MSG_ERR_CONN_ECONNREFUSED]=(long) "Connection refused";
locstr[MSG_ERR_CONN_ENETUNREACH]=(long) "Network unreachable";
locstr[MSG_ERR_CONN_EADDRINUSE]=(long) "Address already in use";
locstr[MSG_ERR_CONN_EINPROGRESS]=(long) "Socket non-blocking; connection cannot be immediately completed";
locstr[MSG_ERR_CONN_EALREADY]=(long) "Socket non-blocking; previous connection not yet completed";
locstr[APP_MENU]=(long) "Sync Clock";
locstr[MENU_SETTINGS_AUTODST_LONGTZ]=(long) "Long TZ Format";
locstr[MSG_LEAPSEC]=(long) "Leap second!";
locstr[MENU_SETTINGS_LEAPSEC]=(long) "Obey Leap Seconds";
locstr[MENU_SETTINGS_COMPAT_CLKDAEMON]=(long) "ClockDaemon";
locstr[MSG_ICON_NOTIFY]=(long) "The Facts icon file has been modified.\nDo you want to update Facts with the new configuration?\nThis will lose any unsaved settings in Facts.";
locstr[MSG_ICON_NOTIFY_OPTS]=(long) "Update|Ignore";
locstr[MENU_SETTINGS_TIMEENV]=(long) "Save Time To Env-var";
locstr[MSG_GUESSED]=(long) " (guessed)";
StrCopy(lang,(char*) "en");

if( (LocaleBase=OpenLibrary_exec2((char*) "locale.library",(ULONG) (long) 0))) {

    localeinfo=ILocale->OpenLocale((char*) NULL );

    catalog=ILocale->OpenCatalogA((struct Locale*) NULL ,"facts.catalog" ,(struct TagItem*) tempGlobal_ILIST  );

StrCopy(lang,localeinfo->loc_LanguageName,2);

// WriteF('/s/n',localeinfo.calendartype)

for( ls=0 ; ls<=(long) 134; ls++) {
    locstr[ls]=(long) ILocale->GetCatalogStr(catalog ,ls ,(char*) locstr[ls] );
}

    StringF(daynames,(char*) "%3.3s%3.3s%3.3s%3.3s%3.3s%3.3s%3.3s",(long) ILocale->GetLocaleStr(localeinfo ,(ULONG) (long) ABDAY_1 ), (long) ILocale->GetLocaleStr(localeinfo ,(ULONG) (long) ABDAY_2 ), (long) ILocale->GetLocaleStr(localeinfo ,(ULONG) (long) ABDAY_3 ), (long) ILocale->GetLocaleStr(localeinfo ,(ULONG) (long) ABDAY_4 ), (long) ILocale->GetLocaleStr(localeinfo ,(ULONG) (long) ABDAY_5 ), (long) ILocale->GetLocaleStr(localeinfo ,(ULONG) (long) ABDAY_6 ), (long) ILocale->GetLocaleStr(localeinfo ,(ULONG) (long) ABDAY_7 ));
    tempGlobal_ILIST2 [0]= (long) ILocale->GetLocaleStr(localeinfo ,(ULONG) (long) ABMON_1 );
    tempGlobal_ILIST2 [1]= (long) ILocale->GetLocaleStr(localeinfo ,(ULONG) (long) ABMON_2 );
    tempGlobal_ILIST2 [2]= (long) ILocale->GetLocaleStr(localeinfo ,(ULONG) (long) ABMON_3 );
    tempGlobal_ILIST2 [3]= (long) ILocale->GetLocaleStr(localeinfo ,(ULONG) (long) ABMON_4 );
    tempGlobal_ILIST2 [4]= (long) ILocale->GetLocaleStr(localeinfo ,(ULONG) (long) ABMON_5 );
    tempGlobal_ILIST2 [5]= (long) ILocale->GetLocaleStr(localeinfo ,(ULONG) (long) ABMON_6 );
    tempGlobal_ILIST2 [6]= (long) ILocale->GetLocaleStr(localeinfo ,(ULONG) (long) ABMON_7 );
    tempGlobal_ILIST2 [7]= (long) ILocale->GetLocaleStr(localeinfo ,(ULONG) (long) ABMON_8 );
    tempGlobal_ILIST2 [8]= (long) ILocale->GetLocaleStr(localeinfo ,(ULONG) (long) ABMON_9 );
    tempGlobal_ILIST2 [9]= (long) ILocale->GetLocaleStr(localeinfo ,(ULONG) (long) ABMON_10 );
    tempGlobal_ILIST2 [10]= (long) ILocale->GetLocaleStr(localeinfo ,(ULONG) (long) ABMON_11 );
    tempGlobal_ILIST2 [11]= (long) ILocale->GetLocaleStr(localeinfo ,(ULONG) (long) ABMON_12 );
    StringFL(monthnames,(char*) "   %3.3s%3.3s%3.3s%3.3s%3.3s%3.3s%3.3s%3.3s%3.3s%3.3s%3.3s%3.3s",tempGlobal_ILIST2 );
} else {
    StrCopy(monthnames,(char*) "   JanFebMarAprMayJunJulAugSepOctNovDec");
    StrCopy(daynames,(char*) "SunMonTueWedThuFriSat");
}

StringF(yesno,(char*) "%s|%s",locstr[MSG_YES],locstr[MSG_NO]);

if( (long) (timemem=(struct TimeVal*) IExec->AllocMem((ULONG) sizeof( struct TimeVal) ,(ULONG) MEMF_CLEAR ))==(long) 0 ) { Raise(ERR_MEM);}

// readtz()
nullmsgport=IExec->CreateMsgPort();
ioreq=(struct TimeRequest*) IExec->CreateIORequest(nullmsgport ,(ULONG) sizeof( struct MsgPort) );

if( OpenDevice_timer((char*) "timer.device",(ULONG) (long) 0,(struct IORequest*) ioreq,(ULONG) (long) NULL)!=0 ) { Raise(ERR_DEV);}
TimerBase=ioreq->Request.io_Device;

loadserver();

/*

IF lock:=Lock('ENV:TIMESERVER',ACCESS_READ)
    UnLock(lock)
    ntphost:=Open('ENV:TIMESERVER',MODE_OLDFILE)
    Fgets(ntphost,buffer,100)
    Close(ntphost)
    StrCopy(temp,buffer)
    colon:=InStr(temp,':')
    IF colon<>-1
        StrCopy(server,temp,colon)
        MidStr(temp,temp,colon+1)
        port:=Val(temp)
//        service:=Val(temp)
        colon:=InStr(temp,':',colon+1)
        IF colon<>-1
            MidStr(temp,temp,colon+1)
            service:=Val(temp)
            colon:=InStr(temp,':',colon+1)
            IF colon<>-1
                MidStr(temp,temp,colon+1)
                timeout:=Val(temp)
            ENDIF
        ELSE
            service:=port
        ENDIF
    ELSE
        StrCopy(server,temp)
    ENDIF
//    WriteF('\s \d',server,port)
ELSE
    StrCopy(server,'pool.ntp.org')   // tick.usno.navy.mil
ENDIF
*/

if( wbstart==0) {
readtz();

    rargs=tempGlobal_ILIST3 ;
    templ=(long) "SERVER,PORT/K,SNTP/S,TIMETCP/S,TIMEUDP/S,DAYTIME/S,EXTRAHALF/S,AUTODST/K,GUI=STAYRESIDENT/S,INT=TIMEINTERVAL/K,OPENWIN=OPENGUI/S,CX_POPKEY/K,CLOCKDAEMON/S";
    if( (args=(long) IDOS->ReadArgs((char*) templ ,rargs ,(struct RDArgs*) NULL ))) {
        if( rargs[0] ) { StrCopy(server,(char*) rargs[0]);}
        if( rargs[6] ) { extrahalf=1;}
        if( rargs[8] ) { wbst=1;}
        if( rargs[7]) {
            autodst=Val((char*) rargs[7]);
            if( wbst==0 ) { dstpresets(autodst);}
        }
        if( rargs[9] ) { timeint=Val((char*) rargs[9]);}
        if( rargs[10]) {
            donotopenwin=0;
        } else {
            donotopenwin=1;
        }
        if( rargs[11] ) { StrCopy(popkey,(char*) rargs[10]);}
        if( rargs[12] ) { clkdaemon=1;}
        if( rargs[1]) {
            port=Val((char*) rargs[1]);
            service=Val((char*) rargs[1]);
            if( rargs[2] ) { service=123;}
            if( rargs[3] ) { service=37;}
            if( rargs[4] ) { service=372;}
            if( rargs[5] ) { service=13;}
        } else {
            if( rargs[2]) {
                service=123;
                port=123;
            }
            if( rargs[3]) {
                service=37;
                port=37;
            }
            if( rargs[4]) {
                service=372;
                port=37;
            }
            if( rargs[5]) {
                service=13;
                port=13;
            }
        }
        
        IDOS->FreeArgs((struct RDArgs*) args );
    }


if( wbst) {
    wbstart=2;
}

rcode=retrievetime();
if( rcode==0) {
    dodefop();
    displaytimes();
} else {
    if( rcode==1) {
        rcode=10;
    } else {
        rcode=5;
    }
}

}

if( wbstart ) { dowbstartup();}

} catch(...) {}
try {
    if( exception) {
        switch( exception) {
            case ERR_MEM:
                StrCopy(errmsg,(char*) locstr[MSG_ERR_MEM]);
                rcode=20;
            	break;
            case ERR_NOHOST:
                StrCopy(errmsg,(char*) locstr[MSG_ERR_NOHOST]);
                rcode=5;
            	break;
            case ERR_NOTCP:
                StrCopy(errmsg,(char*) locstr[MSG_ERR_NOTCP]);
                rcode=10;
            	break;
            case ERR_DEV:
                StringF(errmsg,(char*) locstr[MSG_ERR_DEV],(long) "timer.device");
                rcode=20;
            	break;
            case ERR_UTIL:
                StringF(errmsg,(char*) locstr[MSG_ERR_DEV],(long) "utility.library");
                rcode=20;
            	break;
            case ERR_WBLIB:
                StringF(errmsg,(char*) locstr[MSG_ERR_DEV],(long) "workbench.library");
                rcode=20;
            	break;
            case ERR_GDTL:
                StringF(errmsg,(char*) locstr[MSG_ERR_DEV],(long) "gadtools.library");
                rcode=20;
            	break;
            case ERR_CXLIB:
                StringF(errmsg,(char*) locstr[MSG_ERR_DEV],(long) "commodities.library");
                rcode=20;
            	break;
            case ERR_ICON:
                StringF(errmsg,(char*) locstr[MSG_ERR_DEV],(long) "icon.library");
                rcode=20;
            	break;
            case ERR_CMDY: StrCopy(errmsg,(char*) "") ;// Already running!')
            	break;
            case QuadChara('D','O','U','B'): StrCopy(errmsg,(char*) locstr[MSG_ERR_AREXX]);
            	break;
            case QuadChara(0,'S','I','G'): StrCopy(errmsg,(char*) locstr[MSG_ERR_AREXX]);
            	break;
            case QuadChara(0,'M','E','M')    : StrCopy(errmsg,(char*) locstr[MSG_ERR_MEM]);
            	break;
            case ERR_NOBATT:
                StrCopy(errmsg,(char*) locstr[MSG_ERR_NOBATT]);
                rcode=5;
            	break;
        }
        
        if( wbstart) {
            if( EstrLen(errmsg)>0 ) {
            	tempGlobal_ILIST4 [0]= sizeof( struct EasyStruct);
            	tempGlobal_ILIST4 [3]= (long) errmsg;
            	IIntuition->EasyRequestArgs((struct Window*) (long) 0 ,(struct EasyStruct*) tempGlobal_ILIST4  ,(ULONG*) (long) 0 ,(APTR) (long) NULL );
            }
        } else {
            WriteF((char*) "%s\n",(long) errmsg);
        }
        
    }

if( wbstart) {

  if( appitem ) { -IWorkbench->RemoveAppMenuItem((struct AppMenuItem*) appitem );}
  if( appmenu) {
    // Clear away any messages that arrived at the last moment
    // and let Workbench know we're done with the messages
    while( appmsg=(struct AppMessage*) IExec->GetMsg(appmenu )) { IExec->ReplyMsg((struct Message*) appmsg );}
    IExec->DeleteMsgPort(appmenu );
  }

stopnotify();
stopttnotify();

if( arexxport ) { rx_ClosePort((struct MsgPort*) arexxport);}

    if( tioreq) {
        if( (long) IExec->CheckIO((struct IORequest*) tioreq )==(long) 0) {
            IExec->AbortIO((struct IORequest*) tioreq );
            IExec->WaitIO((struct IORequest*) tioreq );
        }
        CloseDevice_timer((struct IORequest*) tioreq);
        IExec->DeleteIORequest( /*Required:*/ (IORequest*)(APTR) (long) tioreq );
        if( msgport ) { IExec->DeleteMsgPort(msgport );}
    }
    
    if( clkioreq) {
        if( (long) IExec->CheckIO((struct IORequest*) clkioreq )==(long) 0) {
            IExec->AbortIO((struct IORequest*) clkioreq );
            IExec->WaitIO((struct IORequest*) clkioreq );
        }
        CloseDevice_timer((struct IORequest*) clkioreq);
        IExec->DeleteIORequest( /*Required:*/ (IORequest*)(APTR) (long) clkioreq );
        if( clkmsgport ) { IExec->DeleteMsgPort(clkmsgport );}
    }

    if( envioreq) {
        if( (long) IExec->CheckIO((struct IORequest*) envioreq )==(long) 0) {
            IExec->AbortIO((struct IORequest*) envioreq );
            IExec->WaitIO((struct IORequest*) envioreq );
        }
        CloseDevice_timer((struct IORequest*) envioreq);
        IExec->DeleteIORequest( /*Required:*/ (IORequest*)(APTR) (long) envioreq );
        if( envmsgport ) { IExec->DeleteMsgPort(envmsgport );}
    }

killautodsttimer();

// IF inputevent THEN freeIEvents(inputevent)
if( broker ) { ICommodities->DeleteCxObjAll((CxObj*) broker );}
if( broker_mp) {
    while( msg=(long) IExec->GetMsg(broker_mp )) { IExec->ReplyMsg((struct Message*) msg );}
    IExec->DeleteMsgPort(broker_mp );
}

if( mysc ) { IIntuition->UnlockPubScreen((char*) NULL ,mysc );}
if( win ) { IIntuition->CloseWindow(win );}

} // end wbmessage stuff

if( ioreq ) { CloseDevice_timer((struct IORequest*) ioreq);}

if( wbstart) {
// **** bodges up CLI interface for some reason
    if( ioreq ) { IExec->DeleteIORequest( /*Required:*/ (IORequest*)(APTR) (long) ioreq );}
}

if( nullmsgport) {
    while( msg=(long) IExec->GetMsg(nullmsgport )) { IExec->ReplyMsg((struct Message*) msg );}
    IExec->DeleteMsgPort(nullmsgport );
}

    if( buffer ) { IExec->FreeMem((APTR) buffer ,(ULONG) (long) 101 );}

    if( timemem ) { IExec->FreeMem((APTR) (long) timemem ,(ULONG) sizeof( struct TimeVal) );}

// IF sock THEN CloseSocket(sock)
// IF tcp THEN Close(tcp)

if( dirlock ) { IDOS->UnLock((BPTR) dirlock );}

if( localeinfo ) { ILocale->CloseLocale(localeinfo );}

/*
IF amigaguidebase
    CloseAmigaGuide(context)
    CloseLibrary(amigaguidebase)
ENDIF
*/

    if( IdentifyBase ) { CloseLibrary_exec2(IdentifyBase);}
    if( LocaleBase ) { CloseLibrary_exec2(LocaleBase);}
    if( WorkbenchBase ) { CloseLibrary_exec2(WorkbenchBase);}
    if( SocketBase ) { CloseLibrary_exec2(SocketBase);}
    if( UtilityBase ) { CloseLibrary_exec2(UtilityBase);}
    if( GadToolsBase ) { CloseLibrary_exec2(GadToolsBase);}
    if( CxBase ) { CloseLibrary_exec2(CxBase);}
    if( IconBase ) { CloseLibrary_exec2(IconBase);}
	end_facts_portable();
	end_intuition();
	end_dos2();
	end_dos();
	end_arexx();
	end_graphics();
	end_exec();
	end_fastmem();
} catch(...) {}
	if (exception==-1) return (int) exceptionInfo; /*finish the CleanUp() call*/
	return (int) rcode;
 //   IF alsbase THEN CloseLibrary(alsbase)
} 

void dowbstartup() {
// tint
msgport=IExec->CreateMsgPort();
tioreq=(struct TimeRequest*) IExec->CreateIORequest(msgport ,(ULONG) sizeof( struct MsgPort) );
if( OpenDevice_timer((char*) "timer.device",(ULONG) (long) UNIT_VBLANK,(struct IORequest*) tioreq,(ULONG) (long) NULL)!=0 ) { Raise(ERR_DEV);}

// updating clock
clkmsgport=IExec->CreateMsgPort();
clkioreq=(struct TimeRequest*) IExec->CreateIORequest(clkmsgport ,(ULONG) sizeof( struct MsgPort) );
if( OpenDevice_timer((char*) "timer.device",(ULONG) (long) UNIT_VBLANK,(struct IORequest*) clkioreq,(ULONG) (long) NULL)!=0 ) { Raise(ERR_DEV);}
startclkint();

// autodst
dstmsgport=IExec->CreateMsgPort();
dstioreq=(struct TimeRequest*) IExec->CreateIORequest(dstmsgport ,(ULONG) sizeof( struct MsgPort) );
if( OpenDevice_timer((char*) "timer.device",(ULONG) (long) UNIT_WAITUNTIL,(struct IORequest*) dstioreq,(ULONG) (long) NULL)!=0 ) { Raise(ERR_DEV);}

if( (void*) (GadToolsBase=OpenLibrary_exec2((char*) "gadtools.library",(ULONG) (long) 0))==NULL ) { Raise(ERR_GDTL);}
if( (void*) (CxBase=OpenLibrary_exec2((char*) "commodities.library",(ULONG) (long) 0))==NULL ) { Raise(ERR_CXLIB);}
if( (void*) (IconBase=OpenLibrary_exec2((char*) "icon.library",(ULONG) (long) 0))==NULL ) { Raise(ERR_ICON);}
if( (void*) (WorkbenchBase=OpenLibrary_exec2((char*) "workbench.library",(ULONG) (long) 0))==NULL ) { Raise(ERR_WBLIB);}
// alsbase:=OpenLibrary('als.library',6)

if( wbstart==1) {
    wbenchMsg=wbmessage;
    wbarg=(struct WBArg*) wbenchMsg->sm_ArgList;

readtooltypes();
readtz();
loadserver();

// env-var
envmsgport=IExec->CreateMsgPort();
envioreq=(struct TimeRequest*) IExec->CreateIORequest(envmsgport ,(ULONG) sizeof( struct MsgPort) );
if( OpenDevice_timer((char*) "timer.device",(ULONG) (long) UNIT_VBLANK,(struct IORequest*) envioreq,(ULONG) (long) NULL)!=0 ) { Raise(ERR_DEV);}
if( timeenv ) { startenvint();}

}

setupgadgets();
if( mysc) {
    IIntuition->UnlockPubScreen((char*) NULL ,mysc );
    mysc=(struct Screen*) NULL;
}
addcommodity();
if( chkstart ) { syncclock();}
if( batt2078 ) { getclock();}
if( autodst ) { dstpresets(autodst);}
checkoldleapsecs();

//chkautodst_generic()

if( ignoretz==0 ) { setupnotify();}

actuallydosomething();
	return ;

}

long retrievetime() {
struct ClockData servertime; long sock2, conn, size;
struct hostent* remote=NULL; struct sockaddr_in addr; struct sockaddr* addrs=NULL;
struct in_addr* url=NULL;
long fhand;
long ret;

// WriteF('/s/n',server)

if( service==13 ) { utc=2 ;} else { utc=1;}

lastsntp=0;
lastsyncok=0;

if( winopen ) { IIntuition->SetWindowPointerA(win ,(struct TagItem*) tempGlobal_ILIST5  );}

if( (long) (SocketBase=OpenLibrary_exec2((char*) socketlib,(ULONG) socketver))==(long) 0) {
    if( wbstart) {
        // *** no TCP/IP stack! message ***
//        EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS','Please start your TCP/IP stack!','OK'],0,NIL)
        StrCopy(ntptime,(char*) locstr[MSG_GUI_NOTCP]);
		if( winopen) {
		    tempGlobal_ILIST6 [1]= (long) ntptime;
		    IGadTools->GT_SetGadgetAttrsA((struct Gadget*) gads[GADG_SERVER] ,win ,(struct Requester*) NULL ,(struct TagItem*) tempGlobal_ILIST6  );
	    	IIntuition->SetWindowPointerA(win ,(struct TagItem*) tempGlobal_ILIST7  );
		}
        
    } else {
        WriteF((char*) locstr[MSG_GUI_NOTCP]);
        WriteF((char*) "\n\n");
WriteF((char*) "********");
         Raise(ERR_NOTCP);
    }

	ret= 1;

	goto finally;

}
/*
	IF timeenv
		size:=clockdrift()
		guessed:=TRUE
	->    Gt_SetGadgetAttrsA(gads[GADG_SERVER],win,NIL,[GTTX_TEXT,'drift',NIL])

		RETURN size
	ELSE
		RETURN 1
	ENDIF
*/

	if( wbstart) {
    // *** show attempting to connect in server time box ***
//    WriteF('attempting to connect...')
		StrCopy(ntptime,(char*) locstr[MSG_LOOKUP]);
		if( winopen ) {
			tempGlobal_ILIST8 [1]= (long) ntptime;
			IGadTools->GT_SetGadgetAttrsA((struct Gadget*) gads[GADG_SERVER] ,win ,(struct Requester*) NULL ,(struct TagItem*) tempGlobal_ILIST8  );
		}

	} else {
    	WriteF((char*) locstr[MSG_LOOKUP]);
    	WriteF((char*) " %s...\n\n",(long) server);
	}

	remote = ISocket->gethostbyname(server );

	if( (long) remote==(long) 0) {
    	conn=ISocket->Errno();
	    switch( conn) {
    	    case HOST_NOT_FOUND:
        	StrCopy(ntptime,(char*) locstr[MSG_DNS_HOST_NOT_FOUND]);
    	    	break;
	        case TRY_AGAIN:
    	    StrCopy(ntptime,(char*) locstr[MSG_DNS_TRY_AGAIN]);
	        	break;
	        case NO_RECOVERY:
    	    StrCopy(ntptime,(char*) locstr[MSG_DNS_NO_RECOVERY]);
	        	break;
        	case NO_DATA:
	        StrCopy(ntptime,(char*) locstr[MSG_DNS_NO_DATA]);
        		break;
    	    default:
        	StrCopy(ntptime,(char*) locstr[MSG_DNS_DEFAULT]);
    		break;
    	}

		if( winopen) {
		    tempGlobal_ILIST9 [1]= (long) ntptime;
		    IGadTools->GT_SetGadgetAttrsA((struct Gadget*) gads[GADG_SERVER] ,win ,(struct Requester*) NULL ,(struct TagItem*) tempGlobal_ILIST9  );
    		IIntuition->SetWindowPointerA(win ,(struct TagItem*) tempGlobal_ILIST10  );
		} else {
    		if( wbstart==0 ) { WriteF((char*) "%s\n",(long) ntptime);}
		}

		closetcp();
		ret= 3;
		goto finally;

// addr.addr.addr:=Inet_addr(server)
//    Raise(ERR_MEM)

	} else {

url=& addr.sin_addr;

url=(struct in_addr*) remote->h_addr_list[0] ;//[0] in_addr   Inet_addr(remote. etc)

IExec->CopyMem((APTR) (long) url ,(APTR) (long) & addr.sin_addr ,(ULONG) sizeof( struct in_addr) );

// CopyMem(remote.addr_list,url,10)

}

  addr.sin_port = (in_port_t) port ;// Htons(port)
  addr.sin_family = (sa_family_t) (long) AF_INET;
  addr.sin_len=sizeof( struct sockaddr_in);

if( service<100) {
    sock2=ISocket->socket(AF_INET ,SOCK_STREAM ,IPPROTO_TCP );
} else {
    if( mcast) {
        sock2=ISocket->socket(AF_INET ,SOCK_DGRAM ,3 );
    } else {
        sock2=ISocket->socket(AF_INET ,SOCK_DGRAM ,IPPROTO_UDP );
    }
}

tempGlobal_ARRAY_OF_timeval [0].Seconds = (ULONG) timeout;

ISocket->setsockopt(sock2 ,SOL_SOCKET ,SO_RCVTIMEO ,(APTR) (long) tempGlobal_ARRAY_OF_timeval  ,(socklen_t) sizeof( struct TimeVal) );


addrs=(struct sockaddr*) & addr;

// IF service<100

StrCopy(ipaddr,ISocket->Inet_NtoA(addr.sin_addr.s_addr ));

if( wbstart) {
    // *** show attempting to connect in server time box ***
//    WriteF('attempting to connect...')
StrCopy(ntptime,(char*) locstr[MSG_GUI_CONNECT]);
StrAdd(ntptime,(char*) " ");
StrAdd(ntptime,ipaddr);
if( winopen ) {
	tempGlobal_ILIST11 [1]= (long) ntptime;
	IGadTools->GT_SetGadgetAttrsA((struct Gadget*) gads[GADG_SERVER] ,win ,(struct Requester*) NULL ,(struct TagItem*) tempGlobal_ILIST11  );
}

} else {
    WriteF((char*) locstr[MSG_CONNECT]);
    WriteF((char*) " %s...\n\n",(long) ipaddr);
}

conn=ISocket->connect(sock2 ,addrs ,(socklen_t) sizeof( struct sockaddr_in) );
if( conn==-1) {
    connecterror();
    closetcp();
    ret= 2;
    goto finally;
}

// service<100 ENDIF

if( service<100) {
    StrCopy(ntptime,(char*) locstr[MSG_RECEIVING_DATA]);
} else {
    StrCopy(ntptime,(char*) locstr[MSG_SENDING_REQUEST]);
}

    if( wbstart) {
        if( winopen ) {
        	tempGlobal_ILIST12 [1]= (long) ntptime;
        	IGadTools->GT_SetGadgetAttrsA((struct Gadget*) gads[GADG_SERVER] ,win ,(struct Requester*) NULL ,(struct TagItem*) tempGlobal_ILIST12  );
        }
    } else {
        WriteF((char*) "%s\n",(long) ntptime);
    }

// check current time
// GetSysTime(measure)

ioreq->Request.io_Command=(UWORD) (long) TR_GETSYSTIME;

IExec->DoIO((struct IORequest*) ioreq );

IExec->CopyMem((APTR) (long) & ioreq->Time ,(APTR) (long) timemem ,(ULONG) sizeof( struct TimeVal) );

measure=timemem;

curtime=(long) measure->Seconds;
curmics=(long) measure->Microseconds;

// end checktime

if( service<100) {
    size=ISocket->recv(sock2 ,(APTR) buffer ,50 ,(long) NULL );
}
if( service==372) {
    ISocket->send(sock2 ,(APTR) (long) " " ,1 ,(long) NULL );

    StrCopy(ntptime,(char*) locstr[MSG_RECEIVING_DATA]);
    if( wbstart) {
        if( winopen ) {
        	tempGlobal_ILIST13 [1]= (long) ntptime;
        	IGadTools->GT_SetGadgetAttrsA((struct Gadget*) gads[GADG_SERVER] ,win ,(struct Requester*) NULL ,(struct TagItem*) tempGlobal_ILIST13  );
        }
    } else {
        WriteF((char*) "%s\n",(long) ntptime);
    }

    size=ISocket->recv(sock2 ,(APTR) buffer ,4 ,(long) NULL );
}


/******* debug - possibly micro compensation code in future
    ioreq.io.command:=TR_GETSYSTIME
    DoIO(ioreq)
    WriteF('\n\d \d',ioreq.time.secs,ioreq.time.micro)
****/

if( service==123) {

/****** temp query test ********

ntpmsg.header:=%11001110
ntpmsg.req:=%00000001 -> _____
ntpmsg.sequence:=1
ntpmsg.status:=0
ntpmsg.associd:=0
ntpmsg.offset:=0
ntpmsg.size:=7

CopyMem(ntpmsg,buffer,12)
StrCopy(temp,'stratum')
CopyMem(temp,buffer+12,EstrLen(temp))

-> ntpmsg.data:=temp

    Send(sock,buffer,12+ntpmsg.size,NIL)

    IF fhand:=Open('dh1:temp-send',MODE_NEWFILE)
    FputC(fhand,buffer[0])
    FputC(fhand,buffer[1])
    FputC(fhand,buffer[2])
    FputC(fhand,buffer[3])
    FputC(fhand,buffer[4])
    FputC(fhand,buffer[5])
    FputC(fhand,buffer[6])
    FputC(fhand,buffer[7])
    FputC(fhand,buffer[8])
    FputC(fhand,buffer[9])
    FputC(fhand,buffer[10])
    FputC(fhand,buffer[11])
    FputC(fhand,buffer[12])
    FputC(fhand,buffer[13])
    FputC(fhand,buffer[14])
    FputC(fhand,buffer[15])
    FputC(fhand,buffer[16])
    FputC(fhand,buffer[17])
    Close(fhand)
    ENDIF

    size:=Recv(sock,ntpmsg,12,NIL)
    size:=Recv(sock,buffer,ntpmsg.size,NIL)
->    ntpmsg.data:=buffer
    WriteF('\s\n',ntpmsg)
    WriteF('\s\n',buffer)
    WriteF('\d\n',ntpmsg.size)
    
    IF fhand:=Open('dh1:temp-recv',MODE_NEWFILE)
    Fputs(fhand,ntpmsg)
    FputC(fhand,buffer[0])
    FputC(fhand,buffer[1])
    FputC(fhand,buffer[2])
    FputC(fhand,buffer[3])
    FputC(fhand,buffer[4])
    FputC(fhand,buffer[5])
    FputC(fhand,buffer[6])
    FputC(fhand,buffer[7])
    FputC(fhand,buffer[8])
    FputC(fhand,buffer[9])
    FputC(fhand,buffer[10])
    FputC(fhand,buffer[11])
    FputC(fhand,buffer[12])
    FputC(fhand,buffer[13])
    FputC(fhand,buffer[14])
    FputC(fhand,buffer[15])
    FputC(fhand,buffer[16])
    FputC(fhand,buffer[17])
    FputC(fhand,buffer[18])
    FputC(fhand,buffer[19])
    FputC(fhand,buffer[20])
    FputC(fhand,buffer[21])
    FputC(fhand,buffer[22])
    FputC(fhand,buffer[23])
    FputC(fhand,buffer[24])
    FputC(fhand,buffer[25])
    FputC(fhand,buffer[26])
    FputC(fhand,buffer[27])
    FputC(fhand,buffer[28])
    FputC(fhand,buffer[29])
    FputC(fhand,buffer[30])
    FputC(fhand,buffer[31])
    FputC(fhand,buffer[32])
    FputC(fhand,buffer[33])
    FputC(fhand,buffer[34])
    FputC(fhand,buffer[35])
    FputC(fhand,buffer[36])
    FputC(fhand,buffer[37])
    FputC(fhand,buffer[38])
    FputC(fhand,buffer[39])
    FputC(fhand,buffer[40])
    FputC(fhand,buffer[41])
    FputC(fhand,buffer[42])
    FputC(fhand,buffer[43])
    FputC(fhand,buffer[44])
    FputC(fhand,buffer[45])
    FputC(fhand,buffer[46])
    FputC(fhand,buffer[47])
    FputC(fhand,buffer[48])
    FputC(fhand,buffer[49])
    Close(fhand)
    ENDIF

******* temp query test ends ****/

    pkt.header=0xCB/*11001011*/;
    pkt.stratum=0;
    pkt.poll=0;
    pkt.precision=0;
    pkt.rootdelay=0;
    pkt.rootdelayf=0;
    pkt.rootdisp=0;
    pkt.rootdispf=0;
    pkt.refid[0]=0;
    pkt.refid[1]=0;
    pkt.refid[2]=0;
    pkt.refid[3]=0;
    pkt.reftimes=0;
    pkt.reftimef=0;
    pkt.orgs=0;
    pkt.orgf=0;
    pkt.recs=0;
    pkt.recf=0;
    pkt.xmts=(long) measure->Seconds + (long) 0x92B6BD80 - tzsecs;
    pkt.xmtf=(( 12 >= 0 )? (long) measure->Microseconds << 12 : (long) measure->Microseconds >> labs(12 ) )/ 1000000;
  //  pkt.xmtf:=measure.micro*4295

//WriteF('/d/n',pkt.xmtf)

    ISocket->send(sock2 ,(APTR) (long) & pkt ,48 ,(long) NULL );

    StrCopy(ntptime,(char*) locstr[MSG_RECEIVING_DATA]);
    if( wbstart) {
        if( winopen ) {
        	tempGlobal_ILIST14 [1]= (long) ntptime;
        	IGadTools->GT_SetGadgetAttrsA((struct Gadget*) gads[GADG_SERVER] ,win ,(struct Requester*) NULL ,(struct TagItem*) tempGlobal_ILIST14  );
        }
    } else {
        WriteF((char*) "%s\n",(long) ntptime);
    }

    size=ISocket->recv(sock2 ,(APTR) (long) & pkt ,48 ,(long) NULL );

} // end SNTP

if( size<1) {
    StrCopy(ntptime,(char*) locstr[MSG_TIMED_OUT]);
    if( wbstart) {
        if( winopen) {
            tempGlobal_ILIST15 [1]= (long) ntptime;
            IGadTools->GT_SetGadgetAttrsA((struct Gadget*) gads[GADG_SERVER] ,win ,(struct Requester*) NULL ,(struct TagItem*) tempGlobal_ILIST15  );
            IIntuition->SetWindowPointerA(win ,(struct TagItem*) tempGlobal_ILIST16  );
        }
    } else {
        WriteF((char*) "%s\n",(long) ntptime);
    }
    closetcp();
    ret= 4;
    goto finally;
}

// IF sock THEN CloseSocket(sock)
// IF sock2 THEN CloseSocket(sock2)

// IF debug=0 THEN Raise(ERR_MEM)

if( service==37) {
    gettimeport();
    goto finally;
}

if( service==372) {
    gettimeport();
    goto finally;
}

if( service==123) {
    lastsntp=1;
    leapsec=( 6 >= 0 )? pkt.header >> 6 : pkt.header << labs(6 );
// WriteF('/d/n',leapsec)
    if( leapsec==3 ) {// Alarm Condition
        StrCopy(ntptime,(char*) locstr[MSG_ALARM]);
        if( wbstart) {
            if( winopen) {
                tempGlobal_ILIST17 [1]= (long) ntptime;
                IGadTools->GT_SetGadgetAttrsA((struct Gadget*) gads[GADG_SERVER] ,win ,(struct Requester*) NULL ,(struct TagItem*) tempGlobal_ILIST17  );
                IIntuition->SetWindowPointerA(win ,(struct TagItem*) tempGlobal_ILIST18  );
            }
        } else {
            WriteF((char*) "%s: %s\n",(long) ntptime,locstr[MSG_ALARM_INFO]);
        }
        closetcp();
        ret= 5;
        goto finally;
    } else {
if( debug ) { leapsec=1 /******* DEBUG *******/;}
           if( leapsec ) { startleapsec();}
    }
    gettimeport();
    goto finally;
}

StrCopy(ntptime,(char*) buffer);

if( size<3) {
// EstrLen(ntptime)<3
    typetwo();
    goto finally;
}

closetcp();

/********* UTC=0 section! ******
IF utc=0
    IF wbstart
    -> *** if "show local time" box unchecked, display time
    IF winopen THEN Gt_SetGadgetAttrsA(gads[GADG_SERVER],win,NIL,[GTTX_TEXT,ntptime,NIL])
->    WriteF('\s\n',ntptime)
    ENDIF
ENDIF

******* UTC=0 section! ******/

/*
ELSE
    WriteF('Current Time: \s\n',buffer)

ENDIF
*/
// *** get current system time ***
/*
ioreq.io.command:=TR_GETSYSTIME

DoIO(ioreq)

curtime:=ioreq.time.secs

-> SubTime(ioreq.time,measure)

response:=(ioreq.time.secs-measure.secs)/2
*/
response=0;
// microtime:=ioreq.time.micro/2

// WriteF('response /d/n',response)

// curmicros:=ioreq.time.micro
// *** got it! ***


MidStr(hour,ntptime,11,2);
MidStr(min,ntptime,14,2);
MidStr(sec,ntptime,17,2);
MidStr(day,ntptime,8,2);
MidStr(month,ntptime,4,3);
MidStr(year,ntptime,20,4);
MidStr(wday,ntptime,0,3);

// WriteF('/s /s /s/n',sec,min,wday)


mnth=InStr(monthnames,month)/3;

wd=InStr(daynames,wday)/3;

servertime.sec=(UWORD) Val(sec);
servertime.min=(UWORD) Val(min);
servertime.hour=(UWORD) Val(hour);
servertime.mday=(UWORD) Val(day);
servertime.month=(UWORD) mnth;
servertime.year=(UWORD) Val(year);
servertime.wday=(UWORD) wd;

// WriteF('/d /d /d /d /d /d /d/n',servertime.sec,servertime.min,servertime.hour,servertime.mday,servertime.month,servertime.year,servertime.wday)

// curtime.micro:=???

if( utc==2 ) { response=response-utcoffset;}

servsecs=(long) IUtility->CheckDate(& servertime )-tzsecs+response;

if( extrahalf) {
    if( tzsecs<0 ) { servsecs=servsecs+1800 ;} else { servsecs=servsecs-1800;}
}

/*
servsecs:=Date2Amiga(servertime)-tzsecs

WriteF('\d\n',servsecs)

WriteF('\s\n',sectostr(servsecs))

-> servsecs:=CheckDate(servertime)

IF servsecs=0-tzsecs
        StrCopy(temp,'INVALID CLOCK DATA!\nPlease report the following to\nfacts@unsatisfactorysoftware.co.uk\n\nServer: ')
        StrAdd(temp,server)
        StrAdd(temp,'\nTime: ')
        StrAdd(temp,ntptime)
        StrAdd(temp,'\nTZ: ')
        StrAdd(temp,tz)
    IF wbstart
        EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS',temp,'OK'],0,NIL)
        -> *** show INVALID DATA in miggy box, actual server time in server box
        Gt_SetGadgetAttrsA(gads[GADG_SERVER],win,NIL,[GTTX_TEXT,ntptime,NIL])
        StrCopy(sysdate,'INVALID CLOCK DATA!')
        Gt_SetGadgetAttrsA(gads[GADG_AMIGA],win,NIL,[GTTX_TEXT,sysdate,NIL])
        RETURN 1
    ELSE
        WriteF('\d\n\d\n',tz,tzsecs)
        Raise(ERR_NOCLK)
    ENDIF
ENDIF
*/
// setdate()

if( winopen ) { IIntuition->SetWindowPointerA(win ,(struct TagItem*) tempGlobal_ILIST19  );}
finally: 0; 
	return ret;

} 

void typetwo() {
struct ClockData servertime;

// size:=
ISocket->recv(sock ,(APTR) buffer ,50 ,(long) NULL );

// Fgets(tcp,buffer,50)

StrCopy(ntptime,(char*) buffer);

// WriteF('/s/n',ntptime)

closetcp();

/*
IF tcp
    Close(tcp)
    tcp:=NIL
ENDIF

IF socketbase
    CloseLibrary(socketbase)
    socketbase:=NIL
ENDIF
*/

//deal with UTC0 later

// *** get current system time ***
/*

ioreq.io.command:=TR_GETSYSTIME

DoIO(ioreq)

curtime:=ioreq.time.secs

response:=(ioreq.time.secs-measure.secs)/2
*/

response=0;

// *** got it! ***

MidStr(hour,ntptime,15,2);
MidStr(min,ntptime,18,2);
MidStr(sec,ntptime,21,2);
MidStr(day,ntptime,12,2);
MidStr(month,ntptime,9,3);
MidStr(year,ntptime,6,2) ;// TWO DIGITS FOR YEAR!
// MidStr(wday,ntptime,0,3) -> none!

// WriteF('/s /s /s/n',sec,min,wday)


// mnth:=(InStr(monthnames,month))/3

// wd:=(InStr(daynames,wday))/3

if( base2015) {
    if( Val(year)<15 ) { wd=Val(year)+2100 ;} else { wd=Val(year)+2000;}
} else {
    if( Val(year)<78 ) { wd=Val(year)+2000 ;} else { wd=Val(year)+1900;}
}

servertime.sec=(UWORD) Val(sec);
servertime.min=(UWORD) Val(min);
servertime.hour=(UWORD) Val(hour);
servertime.mday=(UWORD) Val(day);
servertime.month=(UWORD) Val(month);
servertime.year=(UWORD) wd;
servertime.wday=(UWORD) (long) 0;

// WriteF('/d /d /d /d /d /d /d/n',servertime.sec,servertime.min,servertime.hour,servertime.mday,servertime.month,servertime.year,servertime.wday)

// Show server time

/********* UTC=0 section! ******
IF utc=0
    servsecs:=CheckDate(servertime)+response
    StrCopy(ntptime,sectostr(servsecs))
    IF winopen THEN Gt_SetGadgetAttrsA(gads[GADG_SERVER],win,NIL,[GTTX_TEXT,ntptime,NIL])
ENDIF
****** end utc=0 *******/

if( utc==2 ) { response=response-utcoffset;}

servsecs=(long) IUtility->CheckDate(& servertime )-tzsecs+response;
if( extrahalf) {
    if( tzsecs<0 ) { servsecs=servsecs+1800 ;} else { servsecs=servsecs-1800;}
}

if( winopen ) { IIntuition->SetWindowPointerA(win ,(struct TagItem*) tempGlobal_ILIST20  );}
	return ;



}




char* displaytimes(long returnsystime, long gmtadj) {
struct TimeVal servtv;

if( winopen ) { IIntuition->SetWindowPointerA(win ,(struct TagItem*) tempGlobal_ILIST21  );}

// IF utc ********* UTC IS ALWAYS SET ************

/*
IF clkdaemon
    IF dst THEN servsecs:=servsecs+dstoffset
ENDIF
*/

    StrCopy(ntptime,sectostr(servsecs,gmtadj)) ;//localdate

if( guessed) {
	StrAdd(ntptime,(char*) locstr[MSG_GUESSED]);
	guessed=0;
}

//    WriteF('/s (/d)/n',localdate,servsecs)

// IF utc
    if( winopen ) {
    	tempGlobal_ILIST22 [1]= (long) ntptime;
    	IGadTools->GT_SetGadgetAttrsA((struct Gadget*) gads[GADG_SERVER] ,win ,(struct Requester*) NULL ,(struct TagItem*) tempGlobal_ILIST22  );
    }
// ENDIF **** END UTC ***************

StrCopy(sysdate,sectostr(curtime));

// WriteF('/s (/d)/n',sysdate,curtime)

if( winopen ) {

	tempGlobal_ILIST23 [1]= (long) sysdate;

	IGadTools->GT_SetGadgetAttrsA((struct Gadget*) gads[GADG_AMIGA] ,win ,(struct Requester*) NULL ,(struct TagItem*) tempGlobal_ILIST23  );

}


// IF service <> 123

	servtv.Seconds=(ULONG) servsecs;
	servtv.Microseconds=(ULONG) servmicros;

	smeasure=& servtv;
	// CopyMem(servtv,smeasure,SIZEOF timeval)


	ITimer->SubTime(measure ,smeasure );
	difference=(long) measure->Seconds;

// difference:=curtime-servsecs
// ENDIF

if( difference==0) {
    StrCopy(diffbox,(char*) locstr[MSG_DIFF_MATCH]);
} else {
        if( difference<0) {
            StringF(diffbox,(char*) locstr[MSG_DIFF_SLOW],-difference);
        } else {
            StringF(diffbox,(char*) locstr[MSG_DIFF_FAST],difference);
        }
}

    if( wbstart==0) {
        WriteF((char*) "%s\n",(long) diffbox);
    }

if( winopen) {
    tempGlobal_ILIST24 [1]= (long) diffbox;
    IGadTools->GT_SetGadgetAttrsA((struct Gadget*) gads[GADG_DIFF] ,win ,(struct Requester*) NULL ,(struct TagItem*) tempGlobal_ILIST24  );

// format = Mon Oct 26 22:53:57 1998
    IIntuition->SetWindowPointerA(win ,(struct TagItem*) tempGlobal_ILIST25  );

}

if( returnsystime ) {

	ntptime= sysdate;

	goto finally;

}
finally: 0; 
	return ntptime;
} 

char* sectostr(long secssince, long gmtadj) {
struct ClockData datestring;

if( dadj==0 ) { gmtadj=0;}

if( gmtadj) {
    secssince=secssince+tzsecs;
    if( extrahalf) {
        if( tzsecs<0 ) { secssince=secssince-1800 ;} else { secssince=secssince+1800;}
    }
}

IUtility->Amiga2Date((ULONG) secssince ,& datestring );

MidStr(tmp2,daynames,(long) ((long) datestring.wday*(long) 3),3);


StrAdd(tmp2,(char*) " ");
StrAdd(tmp2,RightStr(temp,StringF(temp,(char*) "%ld",(long) ((long) datestring.mday+(long) 100)),2));

StrAdd(tmp2,(char*) " ");
StrAdd(tmp2,MidStr(temp,monthnames,(long) ((long) datestring.month*(long) 3),3));

StrAdd(tmp2,(char*) " ");
StrAdd(tmp2,StringF(temp,(char*) "%ld",(long) datestring.year));

StrAdd(tmp2,(char*) " ");
    StrAdd(tmp2,RightStr(temp,StringF(temp,(char*) "%ld",(long) ((long) datestring.hour+(long) 100)),2));
StrAdd(tmp2,(char*) ":");


    StrAdd(tmp2,RightStr(temp,StringF(temp,(char*) "%ld",(long) ((long) datestring.min+(long) 100)),2));


StrAdd(tmp2,(char*) ":");
StrAdd(tmp2,RightStr(temp,StringF(temp,(char*) "%ld",(long) ((long) datestring.sec+(long) 100)),2));
	return tmp2;


// WriteF('/s/n',tmp2)

} 

void setclock(long seconds) {
long micros=0, psdrift=0, pmdrift=0, psdrifts=0, pmdrifts=0;

/*
IF clkdaemon
    IF dst THEN seconds:=seconds+dstoffset
ENDIF
*/

// IF seconds=0 THEN seconds:=servsecs  ALREADY DONE!!!!
micros=servmicros;

// WriteF('secs: /d/nmicros: /d/n/n',seconds,micros)
/************ 4.1 TEST *********/

if( - (psecs!=0)  & - (dadj==0)) {
/* Get the time again to ensure accuracy of difference counter and drift
GetSysTime(measure)
curtime:=measure.secs
curmics:=measure.micro*/

	pmdrift=mdrift;
	pmdrifts=mdrifts;

	mdrift=((curtime-seconds) )* 1000000+(curmics-micros);
	mdrifts=seconds-psecs;

/* incremental adjustment, not sure how to do this without using fractions*/
	if( pdadj) {
		mdrift=mdrift+pmdrift;
	} else {
		mdrift=(mdrift+pmdrift)/2;
	}

/*
	IF mdrift>99999.0
		mdrift:=mdrift-100000
		sdrift:=sdrift+1
	ENDIF
	IF mdrift<-99999
		mdrift:=mdrift+100000
		sdrift:=sdrift-1
	ENDIF
*/

}

	pdadj=dadj;
	dadj=0;
	psecs=seconds;
	pmics=micros;

if( defuse) {
    ioreq->Request.io_Command=(UWORD) (long) TR_SETSYSTIME;
    ioreq->Time.Seconds=(ULONG) seconds;
    if( service==123 ) { ioreq->Time.Microseconds=(ULONG) micros;}
    IExec->DoIO((struct IORequest*) ioreq );
}

if( defsave) {
    if( (BattClockBase=(struct Library*) OpenResource_battclock(battclockname))) {
        IBattClock->WriteBattClock((ULONG) seconds );
    } else {
        if( wbstart) {
            tempGlobal_ILIST26 [0]= sizeof( struct EasyStruct);
            tempGlobal_ILIST26 [3]= locstr[MSG_GUI_BATTCLOCK];
            IIntuition->EasyRequestArgs((struct Window*) (long) 0 ,(struct EasyStruct*) tempGlobal_ILIST26  ,(ULONG*) (long) 0 ,(APTR) (long) NULL );
        } else {
            Raise(ERR_NOBATT);
        }
    }
}
	return ;

}

void getclock() {
if( (BattClockBase=(struct Library*) OpenResource_battclock(battclockname))) {
    ioreq->Time.Seconds=IBattClock->ReadBattClock();
    if( batt2078) {
        ioreq->Time.Seconds=(ULONG) ((long) ioreq->Time.Seconds+ (long) 0xBC191380);
    }
    ioreq->Request.io_Command=(UWORD) (long) TR_SETSYSTIME;
    IExec->DoIO((struct IORequest*) ioreq );
} else {
    if( wbstart==0) {
        Raise(ERR_NOBATT);
    } else {
        tempGlobal_ILIST27 [0]= sizeof( struct EasyStruct);
        tempGlobal_ILIST27 [3]= locstr[MSG_GUI_BATTCLOCK];
        IIntuition->EasyRequestArgs((struct Window*) (long) 0 ,(struct EasyStruct*) tempGlobal_ILIST27  ,(ULONG*) (long) 0 ,(APTR) (long) NULL );
    }
}
	return ;
}

void resetclock() {
if( (BattClockBase=(struct Library*) OpenResource_battclock(battclockname))) {
    IBattClock->ResetBattClock();
} else {
    if( wbstart==0) {
        Raise(ERR_NOBATT);
    } else {
        tempGlobal_ILIST28 [0]= sizeof( struct EasyStruct);
        tempGlobal_ILIST28 [3]= locstr[MSG_GUI_BATTCLOCK];
        IIntuition->EasyRequestArgs((struct Window*) (long) 0 ,(struct EasyStruct*) tempGlobal_ILIST28  ,(ULONG*) (long) 0 ,(APTR) (long) NULL );
    }
}
	return ;
}

void startgui() {
long wdth; short zoompos[4];

noquiet=0;

if( (void*) mysc==NULL ) { mysc=IIntuition->LockPubScreen((char*) NULL );}

wdth=setupgadgets()  ;// was 344
menu();

zoompos[0]=-1;
zoompos[1]=-1;
zoompos[2]=txtxs*30    ;// 290 - 355
zoompos[3]=tbar+3;
// zoompos[4]:=NIL

if( winopen==0) {

if( winy==-1 ) { winy=tbar+3;}

tempGlobal_ILIST29 [1]= winx;

tempGlobal_ILIST29 [3]= winy;

tempGlobal_ILIST29 [5]= wdth;

tempGlobal_ILIST29 [7]= pixsep*5+(long) ((long) mysc->Font->ta_YSize*(long) 5)+2;

tempGlobal_ILIST29 [11]= (long) glist;

tempGlobal_ILIST29 [13]= (long) defwintit;

tempGlobal_ILIST29 [29]= (long) zoompos;

if( (win=IIntuition->OpenWindowTagList((struct NewWindow*) NULL ,(struct TagItem*) tempGlobal_ILIST29  ))) {

  IGadTools->GT_RefreshWindow(win ,(struct Requester*) NULL );

if( zoomed ) { IIntuition->ZipWindow(win );}

if( -IGadTools->LayoutMenusA((struct Menu*) menustrip ,(APTR) vi ,(struct TagItem*) tempGlobal_ILIST30  )) {
  -IIntuition->SetMenuStrip(win ,(struct Menu*) menustrip );
}

if( mysc) {
    IIntuition->UnlockPubScreen((char*) NULL ,mysc );
    mysc=(struct Screen*) NULL;
}

winsig=( (long) win->UserPort->mp_SigBit >= 0 )? 1 << (long) win->UserPort->mp_SigBit : 1 >> labs((long) win->UserPort->mp_SigBit );
winopen=1;
}
}
	return ;

}

long setupgadgets() {
long gad, maxi, butsize, bottomlinesize, intsize, widthts;
long temp_VALUE, temp_VALUE2, temp_VALUE3, temp_VALUE4, temp_VALUE5, temp_VALUE6, temp_VALUE7, temp_VALUE8;

if( (void*) mysc==NULL ) { mysc=IIntuition->LockPubScreen((char*) NULL );}
//  tbar:=mysc.font.ysize+mysc.wbortop+1 /* "real" tbar */
  tbar=(long) mysc->Font->ta_YSize+mysc->WBorTop-2;
txtf = IGraphics->OpenFont(mysc->Font );
txtxs=(long) ((long) txtf->tf_XSize+(long) 1);

  vi=(long) IGadTools->GetVisualInfoA(mysc ,(struct TagItem*) tempGlobal_ILIST31  );
  // GadTools gadgets require this step to be taken
  gad=(long) IGadTools->CreateContext((struct Gadget**) & glist );

temp_VALUE = strlen((char*) locstr[LAB_GADG_SERVER] );

temp_VALUE2 = strlen((char*) locstr[LAB_GADG_AMIGA] );

maxi=( temp_VALUE >temp_VALUE2 )? temp_VALUE : temp_VALUE2 ;
temp_VALUE3 = strlen((char*) locstr[LAB_GADG_DIFF] );
maxi=( maxi>temp_VALUE3 )? maxi : temp_VALUE3 ;
temp_VALUE4 = strlen((char*) locstr[LAB_GADG_TIMESERVER] );
maxi=( maxi>temp_VALUE4 )? maxi : temp_VALUE4 ;
maxi=(maxi+1)*txtxs;

temp_VALUE5 = strlen((char*) locstr[LAB_GADG_CHECK] );

temp_VALUE6 = strlen((char*) locstr[LAB_GADG_SAVE] );

butsize=(( temp_VALUE5 >temp_VALUE6 )? temp_VALUE5 : temp_VALUE6 )*txtxs+10;
// was 70

temp_VALUE7 = strlen((char*) locstr[LAB_GADG_TIMEINT] );

temp_VALUE8 = strlen((char*) locstr[LAB_GADG_DEFPORT] );

intsize=(( temp_VALUE7 >temp_VALUE8 )? temp_VALUE7 : temp_VALUE8 )*txtxs+maxi;

bottomlinesize=strlen((char*) locstr[LAB_GADG_TZNAME] )+strlen((char*) locstr[LAB_GADG_TZ] )+strlen((char*) locstr[LAB_GADG_DST] )+206;

// x90

  tempGlobal_ARRAY_OF_newgadget [0].ng_LeftEdge = (strlen((char*) locstr[LAB_GADG_TZNAME] )+1)*txtxs;

  tempGlobal_ARRAY_OF_newgadget [0].ng_TopEdge = pixsep*5+tbar+(long) ((long) mysc->Font->ta_YSize*(long) 4);

  tempGlobal_ARRAY_OF_newgadget [0].ng_Width = txtxs*5+5;

  tempGlobal_ARRAY_OF_newgadget [0].ng_Height = (short) (long) ((long) mysc->Font->ta_YSize+(long) 4);

  tempGlobal_ARRAY_OF_newgadget [0].ng_GadgetText = (char*) locstr[LAB_GADG_TZNAME];

  tempGlobal_ARRAY_OF_newgadget [0].ng_TextAttr = mysc->Font;

  tempGlobal_ARRAY_OF_newgadget [0].ng_VisualInfo = (APTR) vi;

  tempGlobal_ILIST32 [1]= (long) tzname1;

  gads[GADG_TZNAME]=(long) IGadTools->CreateGadgetA((ULONG) (long) STRING_KIND ,(struct Gadget*) gad ,(struct NewGadget*) tempGlobal_ARRAY_OF_newgadget  ,(struct TagItem*) tempGlobal_ILIST32  );
                   
// above was 45 for width
 
//x185
  tempGlobal_ARRAY_OF_newgadget2 [0].ng_LeftEdge = (strlen((char*) locstr[LAB_GADG_TZNAME] )+strlen((char*) locstr[LAB_GADG_TZ] )+6)*txtxs+15;
  tempGlobal_ARRAY_OF_newgadget2 [0].ng_TopEdge = pixsep*5+tbar+(long) ((long) mysc->Font->ta_YSize*(long) 4);
  tempGlobal_ARRAY_OF_newgadget2 [0].ng_Width = txtxs*4+5;
  tempGlobal_ARRAY_OF_newgadget2 [0].ng_Height = (short) (long) ((long) mysc->Font->ta_YSize+(long) 4);
  tempGlobal_ARRAY_OF_newgadget2 [0].ng_GadgetText = (char*) locstr[LAB_GADG_TZ];
  tempGlobal_ARRAY_OF_newgadget2 [0].ng_TextAttr = mysc->Font;
  tempGlobal_ARRAY_OF_newgadget2 [0].ng_VisualInfo = (APTR) vi;
  tempGlobal_ILIST33 [1]= -tz;
  gads[GADG_TZ]=(long) IGadTools->CreateGadgetA((ULONG) (long) INTEGER_KIND ,(struct Gadget*) gads[GADG_TZNAME] ,(struct NewGadget*) tempGlobal_ARRAY_OF_newgadget2  ,(struct TagItem*) tempGlobal_ILIST33  );

// 270

  tempGlobal_ARRAY_OF_newgadget3 [0].ng_LeftEdge = (strlen((char*) locstr[LAB_GADG_TZNAME] )+strlen((char*) locstr[LAB_GADG_TZ] )+strlen((char*) locstr[LAB_GADG_DST] )+10)*txtxs+30;

  tempGlobal_ARRAY_OF_newgadget3 [0].ng_TopEdge = pixsep*5+tbar+(long) ((long) mysc->Font->ta_YSize*(long) 4);

  tempGlobal_ARRAY_OF_newgadget3 [0].ng_Width = (short) (long) ((long) mysc->Font->ta_YSize+(long) 7);

  tempGlobal_ARRAY_OF_newgadget3 [0].ng_Height = (short) (long) ((long) mysc->Font->ta_YSize+(long) 4);

  tempGlobal_ARRAY_OF_newgadget3 [0].ng_GadgetText = (char*) locstr[LAB_GADG_DST];

  tempGlobal_ARRAY_OF_newgadget3 [0].ng_TextAttr = mysc->Font;

  tempGlobal_ARRAY_OF_newgadget3 [0].ng_VisualInfo = (APTR) vi;

  tempGlobal_ILIST34 [1]= dst;

  gads[GADG_DST]=(long) IGadTools->CreateGadgetA((ULONG) (long) CHECKBOX_KIND ,(struct Gadget*) gads[GADG_TZ] ,(struct NewGadget*) tempGlobal_ARRAY_OF_newgadget3  ,(struct TagItem*) tempGlobal_ILIST34  );

// 300

  tempGlobal_ARRAY_OF_newgadget4 [0].ng_LeftEdge = (strlen((char*) locstr[LAB_GADG_TZNAME] )+strlen((char*) locstr[LAB_GADG_TZ] )+strlen((char*) locstr[LAB_GADG_DST] )+10)*txtxs+42+(long) mysc->Font->ta_YSize;

  tempGlobal_ARRAY_OF_newgadget4 [0].ng_TopEdge = pixsep*5+tbar+(long) ((long) mysc->Font->ta_YSize*(long) 4);

  tempGlobal_ARRAY_OF_newgadget4 [0].ng_Width = txtxs*6+5;

  tempGlobal_ARRAY_OF_newgadget4 [0].ng_Height = (short) (long) ((long) mysc->Font->ta_YSize+(long) 4);

  tempGlobal_ARRAY_OF_newgadget4 [0].ng_TextAttr = mysc->Font;

  tempGlobal_ARRAY_OF_newgadget4 [0].ng_VisualInfo = (APTR) vi;

  tempGlobal_ILIST35 [1]= (long) tzname2;

  tempGlobal_ILIST35 [5]= dst-1;

  gads[GADG_DSTNAME]=(long) IGadTools->CreateGadgetA((ULONG) (long) STRING_KIND ,(struct Gadget*) gads[GADG_DST] ,(struct NewGadget*) tempGlobal_ARRAY_OF_newgadget4  ,(struct TagItem*) tempGlobal_ILIST35  );

/*
bottomlinesize:=((StrLen(locstr[LAB_GADG_TZNAME])+StrLen(locstr[LAB_GADG_TZ])+StrLen(locstr[LAB_GADG_DST])+5)*txtxs)+192+mysc.font.ysize
*/

bottomlinesize=(strlen((char*) locstr[LAB_GADG_TZNAME] )+strlen((char*) locstr[LAB_GADG_TZ] )+strlen((char*) locstr[LAB_GADG_DST] )+16)*txtxs+47+(long) mysc->Font->ta_YSize;

bottomlinesize=( bottomlinesize>intsize+210+txtxs*4 )? bottomlinesize : intsize+210+txtxs*4;
bottomlinesize=( bottomlinesize>maxi+210+butsize )? bottomlinesize : maxi+210+butsize;

widthts=bottomlinesize-(( intsize+210+txtxs*4>maxi+205+butsize )? intsize+210+txtxs*4 : maxi+205+butsize);
  tempGlobal_ARRAY_OF_newgadget5 [0].ng_LeftEdge = maxi;
  tempGlobal_ARRAY_OF_newgadget5 [0].ng_TopEdge = pixsep*4+tbar+(long) ((long) mysc->Font->ta_YSize*(long) 3);
  tempGlobal_ARRAY_OF_newgadget5 [0].ng_Width = 195+widthts-txtxs*5;
  tempGlobal_ARRAY_OF_newgadget5 [0].ng_Height = (short) (long) ((long) mysc->Font->ta_YSize+(long) 4);
  tempGlobal_ARRAY_OF_newgadget5 [0].ng_GadgetText = (char*) locstr[LAB_GADG_TIMESERVER];
  tempGlobal_ARRAY_OF_newgadget5 [0].ng_TextAttr = mysc->Font;
  tempGlobal_ARRAY_OF_newgadget5 [0].ng_VisualInfo = (APTR) vi;
  tempGlobal_ILIST36 [1]= (long) server;
  gads[GADG_TIMESERVER]=(long) IGadTools->CreateGadgetA((ULONG) (long) STRING_KIND ,(struct Gadget*) gads[GADG_DSTNAME] ,(struct NewGadget*) tempGlobal_ARRAY_OF_newgadget5  ,(struct TagItem*) tempGlobal_ILIST36  );

// below xposn was 233

  tempGlobal_ARRAY_OF_newgadget6 [0].ng_LeftEdge = 195+maxi-txtxs*4+widthts;

  tempGlobal_ARRAY_OF_newgadget6 [0].ng_TopEdge = pixsep*4+tbar+(long) ((long) mysc->Font->ta_YSize*(long) 3);

  tempGlobal_ARRAY_OF_newgadget6 [0].ng_Width = txtxs*4+5;

  tempGlobal_ARRAY_OF_newgadget6 [0].ng_Height = (short) (long) ((long) mysc->Font->ta_YSize+(long) 4);

  tempGlobal_ARRAY_OF_newgadget6 [0].ng_TextAttr = mysc->Font;

  tempGlobal_ARRAY_OF_newgadget6 [0].ng_VisualInfo = (APTR) vi;

  tempGlobal_ILIST37 [1]= port;

  tempGlobal_ILIST37 [3]= defport;

  gads[GADG_PORT]=(long) IGadTools->CreateGadgetA((ULONG) (long) INTEGER_KIND ,(struct Gadget*) gads[GADG_TIMESERVER] ,(struct NewGadget*) tempGlobal_ARRAY_OF_newgadget6  ,(struct TagItem*) tempGlobal_ILIST37  );

//****
  tempGlobal_ARRAY_OF_newgadget7 [0].ng_LeftEdge = maxi;
  tempGlobal_ARRAY_OF_newgadget7 [0].ng_TopEdge = pixsep+tbar;
  tempGlobal_ARRAY_OF_newgadget7 [0].ng_Width = 200+widthts;
  tempGlobal_ARRAY_OF_newgadget7 [0].ng_Height = (short) (long) ((long) mysc->Font->ta_YSize+(long) 4);
  tempGlobal_ARRAY_OF_newgadget7 [0].ng_GadgetText = (char*) locstr[LAB_GADG_SERVER];
  tempGlobal_ARRAY_OF_newgadget7 [0].ng_TextAttr = mysc->Font;
  tempGlobal_ARRAY_OF_newgadget7 [0].ng_VisualInfo = (APTR) vi;
  tempGlobal_ILIST38 [3]= (long) ntptime;
  gads[GADG_SERVER]=(long) IGadTools->CreateGadgetA((ULONG) (long) TEXT_KIND ,(struct Gadget*) gads[GADG_PORT] ,(struct NewGadget*) tempGlobal_ARRAY_OF_newgadget7  ,(struct TagItem*) tempGlobal_ILIST38  );
// 130

// x posn was 275

  tempGlobal_ARRAY_OF_newgadget8 [0].ng_LeftEdge = 200+maxi+5+widthts;

  tempGlobal_ARRAY_OF_newgadget8 [0].ng_TopEdge = pixsep+tbar;

  tempGlobal_ARRAY_OF_newgadget8 [0].ng_Width = butsize;

  tempGlobal_ARRAY_OF_newgadget8 [0].ng_Height = (short) (long) ((long) mysc->Font->ta_YSize+(long) 4);

  tempGlobal_ARRAY_OF_newgadget8 [0].ng_GadgetText = (char*) locstr[LAB_GADG_CHECK];

  tempGlobal_ARRAY_OF_newgadget8 [0].ng_TextAttr = mysc->Font;

  tempGlobal_ARRAY_OF_newgadget8 [0].ng_VisualInfo = (APTR) vi;

  gads[GADG_CHECK]=(long) IGadTools->CreateGadgetA((ULONG) (long) BUTTON_KIND ,(struct Gadget*) gads[GADG_SERVER] ,(struct NewGadget*) tempGlobal_ARRAY_OF_newgadget8  ,(struct TagItem*) tempGlobal_ILIST39  );
//150
  tempGlobal_ARRAY_OF_newgadget9 [0].ng_LeftEdge = 200+maxi+5+widthts;
  tempGlobal_ARRAY_OF_newgadget9 [0].ng_TopEdge = pixsep*2+tbar+(long) ((long) mysc->Font->ta_YSize*(long) 1);
  tempGlobal_ARRAY_OF_newgadget9 [0].ng_Width = butsize;
  tempGlobal_ARRAY_OF_newgadget9 [0].ng_Height = (short) (long) ((long) mysc->Font->ta_YSize+(long) 4);
  tempGlobal_ARRAY_OF_newgadget9 [0].ng_GadgetText = (char*) locstr[LAB_GADG_SAVE];
  tempGlobal_ARRAY_OF_newgadget9 [0].ng_TextAttr = mysc->Font;
  tempGlobal_ARRAY_OF_newgadget9 [0].ng_VisualInfo = (APTR) vi;
  gads[GADG_SAVE]=(long) IGadTools->CreateGadgetA((ULONG) (long) BUTTON_KIND ,(struct Gadget*) gads[GADG_CHECK] ,(struct NewGadget*) tempGlobal_ARRAY_OF_newgadget9  ,(struct TagItem*) tempGlobal_ILIST40  );
                    
  tempGlobal_ARRAY_OF_newgadget10 [0].ng_LeftEdge = maxi;
  tempGlobal_ARRAY_OF_newgadget10 [0].ng_TopEdge = pixsep*2+tbar+(long) mysc->Font->ta_YSize;
  tempGlobal_ARRAY_OF_newgadget10 [0].ng_Width = 200+widthts;
  tempGlobal_ARRAY_OF_newgadget10 [0].ng_Height = (short) (long) ((long) mysc->Font->ta_YSize+(long) 4);
  tempGlobal_ARRAY_OF_newgadget10 [0].ng_GadgetText = (char*) locstr[LAB_GADG_AMIGA];
  tempGlobal_ARRAY_OF_newgadget10 [0].ng_TextAttr = mysc->Font;
  tempGlobal_ARRAY_OF_newgadget10 [0].ng_VisualInfo = (APTR) vi;
  tempGlobal_ILIST41 [3]= (long) sysdate;
  gads[GADG_AMIGA]=(long) IGadTools->CreateGadgetA((ULONG) (long) TEXT_KIND ,(struct Gadget*) gads[GADG_SAVE] ,(struct NewGadget*) tempGlobal_ARRAY_OF_newgadget10  ,(struct TagItem*) tempGlobal_ILIST41  );
                    
  tempGlobal_ARRAY_OF_newgadget11 [0].ng_LeftEdge = maxi;
  tempGlobal_ARRAY_OF_newgadget11 [0].ng_TopEdge = pixsep*3+tbar+(long) ((long) mysc->Font->ta_YSize*(long) 2);
  tempGlobal_ARRAY_OF_newgadget11 [0].ng_Width = 200+widthts;
  tempGlobal_ARRAY_OF_newgadget11 [0].ng_Height = (short) (long) ((long) mysc->Font->ta_YSize+(long) 4);
  tempGlobal_ARRAY_OF_newgadget11 [0].ng_GadgetText = (char*) locstr[LAB_GADG_DIFF];
  tempGlobal_ARRAY_OF_newgadget11 [0].ng_TextAttr = mysc->Font;
  tempGlobal_ARRAY_OF_newgadget11 [0].ng_VisualInfo = (APTR) vi;
  tempGlobal_ILIST42 [3]= (long) diffbox;
  gads[GADG_DIFF]=(long) IGadTools->CreateGadgetA((ULONG) (long) TEXT_KIND ,(struct Gadget*) gads[GADG_AMIGA] ,(struct NewGadget*) tempGlobal_ARRAY_OF_newgadget11  ,(struct TagItem*) tempGlobal_ILIST42  );
                
// was 308

  tempGlobal_ARRAY_OF_newgadget12 [0].ng_LeftEdge = intsize+205+widthts;

  tempGlobal_ARRAY_OF_newgadget12 [0].ng_TopEdge = pixsep*3+tbar+(long) ((long) mysc->Font->ta_YSize*(long) 2);

  tempGlobal_ARRAY_OF_newgadget12 [0].ng_Width = txtxs*4+5;

  tempGlobal_ARRAY_OF_newgadget12 [0].ng_Height = (short) (long) ((long) mysc->Font->ta_YSize+(long) 4);

  tempGlobal_ARRAY_OF_newgadget12 [0].ng_GadgetText = (char*) locstr[LAB_GADG_TIMEINT];

  tempGlobal_ARRAY_OF_newgadget12 [0].ng_TextAttr = mysc->Font;

  tempGlobal_ARRAY_OF_newgadget12 [0].ng_VisualInfo = (APTR) vi;

  tempGlobal_ILIST43 [1]= timeint;

  gads[GADG_TIMEINT]=(long) IGadTools->CreateGadgetA((ULONG) (long) INTEGER_KIND ,(struct Gadget*) gads[GADG_DIFF] ,(struct NewGadget*) tempGlobal_ARRAY_OF_newgadget12  ,(struct TagItem*) tempGlobal_ILIST43  );

// was 320   width was 37
  tempGlobal_ARRAY_OF_newgadget13 [0].ng_LeftEdge = intsize+205+12+widthts;
  tempGlobal_ARRAY_OF_newgadget13 [0].ng_TopEdge = pixsep*4+tbar+(long) ((long) mysc->Font->ta_YSize*(long) 3);
  tempGlobal_ARRAY_OF_newgadget13 [0].ng_Width = (short) (long) ((long) mysc->Font->ta_YSize+(long) 7);
  tempGlobal_ARRAY_OF_newgadget13 [0].ng_Height = (short) (long) ((long) mysc->Font->ta_YSize+(long) 4);
  tempGlobal_ARRAY_OF_newgadget13 [0].ng_GadgetText = (char*) locstr[LAB_GADG_DEFPORT];
  tempGlobal_ARRAY_OF_newgadget13 [0].ng_TextAttr = mysc->Font;
  tempGlobal_ARRAY_OF_newgadget13 [0].ng_VisualInfo = (APTR) vi;
  tempGlobal_ILIST44 [1]= defport;
  gads[GADG_DEFPORT]=(long) IGadTools->CreateGadgetA((ULONG) (long) CHECKBOX_KIND ,(struct Gadget*) gads[GADG_TIMEINT] ,(struct NewGadget*) tempGlobal_ARRAY_OF_newgadget13  ,(struct TagItem*) tempGlobal_ILIST44  );

// width was 25
/*
node[0].name := 'First node'
node[1].name := 'Second node'
node[2].name := 'Third node'
node[3].name := 'Fourth node'
node[4].name := 'Fifth node'
newList(list)
AddTail(list,node[0])
AddTail(list,node[1])
AddTail(list,node[2])
AddTail(list,node[3])
AddTail(list,node[4])

  gads[GADG_TESTGADG]:=CreateGadgetA(LISTVIEW_KIND, gads[GADG_DEFPORT],
                    [maxi, (pixsep*4)+tbar+(mysc.font.ysize*3),
                     (140+widthts), (mysc.font.ysize+40),  -> was 4
                     locstr[LAB_GADG_TIMESERVER], mysc.font,
                     GADG_TESTGADG, 0,
                     vi,NIL]:newgadget,
                    [GTLV_LABELS,list,NIL])
*/
IGraphics->CloseFont(txtf );
	return bottomlinesize;

} 

void actuallydosomething() {
  struct IntuiMessage* imsg=NULL; struct Gadget* gad=NULL; long terminated=FALSE, class2;
long signal, msgid, msgtype, msg, clgui=0, tisig, oldtz, clksig, tmpint, envsig, colon;
  long menunumber, menunum, itemnum, subnum; struct MenuItem* item=NULL;
  long dstsig, arexxsig, rexxcmd, arexxmsg, rc; char* result=NULL; long appmenusig;
  char* temp_ARRAY_OF_CHAR=NULL; long temp_QUAD;

try {
	result = NewString(100);
	temp_QUAD = exception ;
	exception = 0 ;

    arexxport=(long) rx_OpenPort((char*) "FACTS");

    arexxsig= ret2 ;

// ******* IMPORTANT! IF timeinterval is set SendIO etc etc msgport

starttint();

tisig=( (long) msgport->mp_SigBit >= 0 )? 1 << (long) msgport->mp_SigBit : 1 >> labs((long) msgport->mp_SigBit );
clksig=( (long) clkmsgport->mp_SigBit >= 0 )? 1 << (long) clkmsgport->mp_SigBit : 1 >> labs((long) clkmsgport->mp_SigBit );
dstsig=( (long) dstmsgport->mp_SigBit >= 0 )? 1 << (long) dstmsgport->mp_SigBit : 1 >> labs((long) dstmsgport->mp_SigBit );
notifysig=( (long) notifyport->mp_SigBit >= 0 )? 1 << (long) notifyport->mp_SigBit : 1 >> labs((long) notifyport->mp_SigBit );
ttnotifysig=( (long) ttnotifyport->mp_SigBit >= 0 )? 1 << (long) ttnotifyport->mp_SigBit : 1 >> labs((long) ttnotifyport->mp_SigBit );
appmenusig=( (long) appmenu->mp_SigBit >= 0 )? 1 << (long) appmenu->mp_SigBit : 1 >> labs((long) appmenu->mp_SigBit );
envsig=( (long) envmsgport->mp_SigBit >= 0 )? 1 << (long) envmsgport->mp_SigBit : 1 >> labs((long) envmsgport->mp_SigBit );

if( forcesync ) { syncclock();}

  do {

signal=(long) IExec->Wait((ULONG) (winsig | cxsigflag | tisig | arexxsig | notifysig | ttnotifysig | appmenusig | dstsig | clksig | envsig) );

if( signal & winsig) {

    // Use Gt_GetIMsg() and Gt_ReplyIMsg() for handling IntuiMessages
    // with GadTools gadgets.
    while( (struct IntuiMessage*) ((long) - (terminated==FALSE)  & (long) (imsg=IGadTools->GT_GetIMsg(win->UserPort )))) {
      // Gt_ReplyIMsg() at end of loop
      class2=(long) imsg->Class;
      switch( class2) {
    case IDCMP_NEWSIZE:
        IIntuition->SetWindowTitles(win ,defwintit ,(char*) (long) -1 );
//        IF win.height < (tbar+10) THEN 
// startclkint() -> ELSE stopclkint()
    	break;

      case IDCMP_GADGETUP  :// Buttons only report GADGETUP
        gad=(struct Gadget*) imsg->IAddress;
/*
            IF gad.gadgetid=GADG_USE
                IF retrievetime()=0
                    setdate()
                displaytimes()
            ENDIF
        ENDIF
*/
            if( (short) (long) gad->GadgetID==(short) GADG_SAVE ) { syncclock();}

        if( (short) (long) gad->GadgetID==(short) GADG_CHECK ) { syncclock(0) ;}// do not "dodefop"

        if( (short) (long) gad->GadgetID==(short) GADG_DST) {
              dst=(long) imsg->Code;
              oldtz=tz;
              if( dst ) { tz=tz-nicedst ;} else { tz=tz+nicedst;}
              tzsecs=tz*3600;
            adjustclock((oldtz-tz)*3600);
                 tempGlobal_ILIST45 [1]= dst-1;
                 IGadTools->GT_SetGadgetAttrsA((struct Gadget*) gads[GADG_DSTNAME] ,win ,(struct Requester*) NULL ,(struct TagItem*) tempGlobal_ILIST45  );
                 tempGlobal_ILIST46 [1]= -tz;
                 IGadTools->GT_SetGadgetAttrsA((struct Gadget*) gads[GADG_TZ] ,win ,(struct Requester*) NULL ,(struct TagItem*) tempGlobal_ILIST46  );
                 savetz(1);
        }

        if( (short) (long) gad->GadgetID==(short) GADG_TZNAME) {
            StrCopy(tzname1,((struct StringInfo*) gad->SpecialInfo)->Buffer);
            savetz(1);
        }

        if( (short) (long) gad->GadgetID==(short) GADG_DSTNAME) {
            StrCopy(tzname2,((struct StringInfo*) gad->SpecialInfo)->Buffer);
            savetz(1);
        }

        if( (short) (long) gad->GadgetID==(short) GADG_TZ) {
            oldtz=tz;
            tz=-Val(((struct StringInfo*) gad->SpecialInfo)->Buffer);
            tzsecs=tz*3600;
            adjustclock((oldtz-tz)*3600);
            savetz(1) ;// save ENV:TZ
        }

        if( (short) (long) gad->GadgetID==(short) GADG_TIMESERVER) {
            StrCopy(server,((struct StringInfo*) gad->SpecialInfo)->Buffer);
        }
        
        if( (short) (long) gad->GadgetID==(short) GADG_PORT) {
            port=Val(((struct StringInfo*) gad->SpecialInfo)->Buffer);
            if( service==13 ) { daytimeport=port;}
            if( service==37 ) { timeport=port;}
            if( service==123 ) { sntpport=port;}
            if( service==372 ) { timeudpport=port;}
        }

        if( (short) (long) gad->GadgetID==(short) GADG_DEFPORT) {
            defport=(long) imsg->Code;
            if( defport) {
                if( service==13 ) { port=defdaytimeport;}
                if( service==37 ) { port=deftimeport;}
                if( service==123 ) { port=defsntpport;}
                if( service==372 ) { port=deftimeudpport;}
            } else {
                if( service==13 ) { port=daytimeport;}
                if( service==37 ) { port=timeport;}
                if( service==123 ) { port=sntpport;}
                if( service==372 ) { port=timeudpport;}
            }
                 tempGlobal_ILIST47 [1]= defport;
                 tempGlobal_ILIST47 [3]= port;
                 IGadTools->GT_SetGadgetAttrsA((struct Gadget*) gads[GADG_PORT] ,win ,(struct Requester*) NULL ,(struct TagItem*) tempGlobal_ILIST47  );
        }
/*
        IF gad.gadgetid=GADG_QUIT
            IF EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS',locstr[MSG_QUIT],yesno],0,NIL) THEN terminated:=TRUE
        ENDIF
        
        IF gad.gadgetid=GADG_CANCEL
            clgui:=1 -> closegui()
        ENDIF
*/        
        if( (short) (long) gad->GadgetID==(short) GADG_TIMEINT) {
            timeint=Val(((struct StringInfo*) gad->SpecialInfo)->Buffer);
            starttint();
        }

      	break;

      case IDCMP_CLOSEWINDOW:
        clgui=1 ;// closegui()

      	break;
      case IDCMP_REFRESHWINDOW:
        // This handling is REQUIRED with GadTools.

        IGadTools->GT_BeginRefresh(win );
        IGadTools->GT_EndRefresh(win ,TRUE );
      	break;

      case IDCMP_MENUPICK:
        IIntuition->SetWindowPointerA(win ,(struct TagItem*) tempGlobal_ILIST48  );
        menunumber=(short) (long) imsg->Code  & 0xFFFF;
        while(menunumber!=MENUNULL) {
            item=IIntuition->ItemAddress((struct Menu*) menustrip ,(ULONG) menunumber );
            menunum=menunumber  & 0x1F;
            itemnum=(( 5 >= 0 )? (menunumber) >> 5 : (menunumber) << labs(5 ))& 0x3F;
            subnum=(( 11 >= 0 )? (menunumber) >> 11 : (menunumber) << labs(11 ))& 0x1F;
            
            if( menunum==0 ) {// Project
              if( itemnum==0 ) { sntpinfo();}
           //   IF itemnum=2 THEN checkfactsver()
              if( itemnum==2 ) { about();}
              if( itemnum==4 ) { clgui=1;}
              if( itemnum==5 ) { terminated=TRUE;}
if( itemnum==5 ) { printf("#1\n" ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 );}
/*
                IF EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS',locstr[MSG_QUIT],yesno],0,NIL) THEN terminated:=TRUE
              ENDIF
*/
            }

            if( menunum==1) {
                if( itemnum==0) {
                    if( subnum==0) {
                        if( (short) (long) item->Flags & CHECKED) {
                            defuse=1;
                        } else {
                            defuse=0;
                        }
                    }
                    if( subnum==1) {
                        if( (short) (long) item->Flags & CHECKED) {
                            defsave=1;
                        } else {
                            defsave=0;
                        }
                    }
                }
                
                if( itemnum==1) {
                    if( subnum>=22) {
                        if( subnum==22) {
                            if( (short) (long) item->Flags & CHECKED) {
                                autodquiet=1;
                            } else {
                                autodquiet=0;
                            }
                        }
                        
                        if( subnum==23) {
                            if( (short) (long) item->Flags & CHECKED) {
                                stgemul=1;
                                savetz();
                            } else {
                                stgemul=0;
                                (BOOL)(0!=IDOS->DeleteFile("ENVARC:SUMMERTIME" ));
                            }
                        }
                        if( subnum==24) {
                            if( (short) (long) item->Flags & CHECKED) {
                                changelong(1);
                            } else {
                                changelong(0);
                            }
                        }
                    } else {
                        if( (short) (long) item->Flags & CHECKED) {
                            if( subnum==20 ) {// Custom mode
                                autodst=-1;
                                dstpresets(autodst);
                            } else {
                                autodst=subnum;
                                if( autodst==0 ) { (BOOL)(0!=IDOS->DeleteFile("ENVARC:SUMMERTIME" ));} else { dstpresets(autodst);}
                            }
                        }
                    }
                }
                
                if( itemnum==2) {
                    if( (short) (long) item->Flags & CHECKED) {
                        obsession=1;
                    } else {
                        obsession=0;
                    }
                }

                if( itemnum==3) {
                    if( (short) (long) item->Flags & CHECKED) {
                        ignoretz=1;
                        stopnotify();
                    } else {
                        ignoretz=0;
                        setupnotify();
                    }
                }
                
                if( itemnum==4) {
                    if( subnum==0) {
                        if( (short) (long) item->Flags & CHECKED) {
                            service=123;
                            if( defport ) { port=defsntpport ;} else { port=sntpport;}
                                 tempGlobal_ILIST49 [1]= port;
                                 IGadTools->GT_SetGadgetAttrsA((struct Gadget*) gads[GADG_PORT] ,win ,(struct Requester*) NULL ,(struct TagItem*) tempGlobal_ILIST49  );
                        }
                    }

                    if( subnum==1) {
                        if( (short) (long) item->Flags & CHECKED) {
                            service=372;
                            if( defport ) { port=deftimeudpport ;} else { port=timeudpport;}
                                 tempGlobal_ILIST50 [1]= port;
                                 IGadTools->GT_SetGadgetAttrsA((struct Gadget*) gads[GADG_PORT] ,win ,(struct Requester*) NULL ,(struct TagItem*) tempGlobal_ILIST50  );
                        }
                    }


                    if( subnum==2) {
                        if( (short) (long) item->Flags & CHECKED) {
                            service=37;
                            if( defport ) { port=deftimeport ;} else { port=timeport;}
                                 tempGlobal_ILIST51 [1]= port;
                                 IGadTools->GT_SetGadgetAttrsA((struct Gadget*) gads[GADG_PORT] ,win ,(struct Requester*) NULL ,(struct TagItem*) tempGlobal_ILIST51  );
                        }
                    }
                    
                    if( subnum==3) {
                        if( (short) (long) item->Flags & CHECKED) {
                            service=13;
                            if( defport ) { port=defdaytimeport ;} else { port=daytimeport;}
                                 tempGlobal_ILIST52 [1]= port;
                                 IGadTools->GT_SetGadgetAttrsA((struct Gadget*) gads[GADG_PORT] ,win ,(struct Requester*) NULL ,(struct TagItem*) tempGlobal_ILIST52  );
                        }
                    }

                }

                if( itemnum==5) {
                    if( subnum==0) {
                        if( (short) (long) item->Flags & CHECKED) {
                            utcneg=-1;
                            utcoffset=utcnum*3600*utcneg;
                        }
                    }
                    
                    if( subnum==1) {
                        if( (short) (long) item->Flags & CHECKED) {
                            utcneg=0;
                            utcoffset=utcnum*3600*utcneg;
                        }
                    }
                    
                    if( subnum>2) {
                        utcnum=subnum-3;
                        utcoffset=utcnum*3600*utcneg;
                    }
                    
                }

                if( itemnum==6) {
                    if( (short) (long) item->Flags & CHECKED) { chkstart=1 ;} else { chkstart=0;}
                }

                if( itemnum==7) {
                    if( (short) (long) item->Flags & CHECKED) {
			timeenv=1;
			startenvint();
		 } else {
			 timeenv=0;
			stopenvint();
		}
                }

                if( itemnum==8) {
                    if( subnum==0) {
                        if( (short) (long) item->Flags & CHECKED) {
                            trynext=1;
                            aninc=0;
                            nextservice=0;
                        } else {
                            trynext=0;
                        }
                    }

                    if( subnum==2) {
                        if( (short) (long) item->Flags & CHECKED) {
                            aninc=1;
                            trynext=0;
                        } else {
                            aninc=0;
                        }
                    }

                    if( subnum==3) {
                        if( (short) (long) item->Flags & CHECKED) {
                            nextservice=1;
                            trynext=0;
                        } else {
                            nextservice=0;
                        }
                    }

                }

                if( itemnum==9) {
                    if( subnum==0) {
                        if( (short) (long) item->Flags & CHECKED) { mode2036=1 ;} else { mode2036=0;}
                    }

                    if( subnum==1) {
                        if( (short) (long) item->Flags & CHECKED) { base2015=1 ;} else { base2015=0;}
                    }
                    
                    if( subnum==2) {
                        if( (short) (long) item->Flags & CHECKED) { batt2078=1 ;} else { batt2078=0;}
                    }

                    if( subnum==4) {
                        if( (short) (long) item->Flags & CHECKED) {
                            clkdaemon=1;
                        } else {
                            clkdaemon=0;
                        }
                    }
                }

                if( itemnum==10) {
                    if( (short) (long) item->Flags & CHECKED) {
                        toolsmenu=1;
                        startappmenu();
                    } else {
                        toolsmenu=0;
                        removeappitem();
                    }
                }
                
                if( itemnum==11) {
                    if( (short) (long) item->Flags & CHECKED) {
                        extrahalf=1;
                            if( tzsecs<0 ) { adjustclock(1800) ;} else { adjustclock(-1800);}
                    } else {
                        extrahalf=0;
                            if( tzsecs<0 ) { adjustclock(-1800) ;} else { adjustclock(1800);}
                    }
                }

                if( itemnum==12) {
                    if( (short) (long) item->Flags & CHECKED) {
                        alwaysupdate=1;
                         // startclkint()
                    } else {
                        alwaysupdate=0;
			// stopclkint()
                    }
                }

                if( itemnum==14) {
                    // snapshot
                    winx=win->LeftEdge;
                    winy=win->TopEdge;
                    if( win->Height < tbar+10  ) { zoomed=1 ;} else { zoomed=0;}
                }

                if( itemnum==15 ) { saveconfig();}
            }


            if( menunum==2 ) {// Special
                if( itemnum==0 ) { resetclock();}
                if( itemnum==1 ) { getclock();}

//                IF itemnum=2 THEN showtime()

            //    IF itemnum=3 THEN downloadfiles()

           //     IF itemnum=3 THEN Als(0)
            }

            menunumber=(short) (long) item->NextSelect  & 0xFFFF;
        }
        IIntuition->SetWindowPointerA(win ,(struct TagItem*) tempGlobal_ILIST53  );

      	break;
    }
IGadTools->GT_ReplyIMsg(imsg );
}
}

if( signal & cxsigflag) {
    while( msg=(long) IExec->GetMsg(broker_mp )) {
        msgid=ICommodities->CxMsgID((CxMsg*) msg );
        msgtype=(long) ICommodities->CxMsgType((CxMsg*) msg );
        IExec->ReplyMsg((struct Message*) msg );
        
        switch( msgtype) {
        case CXM_IEVENT:
            switch( msgid) {
            case EVT_POPKEY:
            if( win) {
                IIntuition->WindowToFront(win );
            } else {
                startgui();
            }
            ICommodities->AddIEvents(inputevent );
            	break;
            case EVT_SAVEKEY:
            syncclock();
            ICommodities->AddIEvents(inputevent );
            	break;
            }
        	break;
        case CXM_COMMAND:
            switch( msgid) {
            case CXCMD_DISABLE:
            ICommodities->ActivateCxObj((CxObj*) broker ,FALSE );
            enabled=FALSE;
            	break;
            case CXCMD_ENABLE:
            ICommodities->ActivateCxObj((CxObj*) broker ,TRUE );
            enabled=TRUE;
            	break;
            case CXCMD_APPEAR:
            if( win) {
                IIntuition->WindowToFront(win );
            } else {
                startgui();
            }
            	break;
            case CXCMD_DISAPPEAR:
            clgui=1 ;//closegui()
            	break;
            case CXCMD_KILL:
            terminated=TRUE;
printf("#2\n" ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 );
            	break;
            case CXCMD_UNIQUE:
            if( win) {
                IIntuition->WindowToFront(win );
            } else {
                startgui();
            }
            	break;
            }
        	break;
        }
    }
}

if( appmenusig) {
  while( appmsg=(struct AppMessage*) IExec->GetMsg(appmenu )) {
/*
    SELECT appmsg.id
        CASE 0
*/
syncclock();
//    ENDSELECT
    IExec->ReplyMsg((struct Message*) appmsg );
  }
}

if( arexxsig!=(long) NULL) {
    if( signal & arexxsig) {
        do {
            if( win ) { IIntuition->SetWindowPointerA(win ,(struct TagItem*) tempGlobal_ILIST54  );}
            arexxmsg=(long) rx_GetMsg((struct MsgPort*) arexxport);
            rexxcmd= ret2 ;
            rc=0;
            StrCopy(result,(char*) "");
            if( arexxmsg!=(long) NULL) {
                
                if( (char*) rexxcmd) {
                temp_ARRAY_OF_CHAR = UpperStr((char*) rexxcmd) ;
                } else { 
                temp_ARRAY_OF_CHAR = (char*) rexxcmd;
                }
                temp_ARRAY_OF_CHAR ;
                if( StrCmp((char*) rexxcmd,(char*) "COPYRIGHT")) {
                    StrCopy(result,(char*) "\251 1998-2009 Unsatisfactory Software");
                }
                
                if( StrCmp((char*) rexxcmd,(char*) "VERSION")) {
                    StrCopy(result,fver);
                }
                
                if( StrCmp((char*) rexxcmd,(char*) "REVISION")) {
                    StrCopy(result,frev);
                }
                
                if( StrCmp((char*) rexxcmd,(char*) "HIDE")) {
                    clgui=1;
                }
                
                if( StrCmp((char*) rexxcmd,(char*) "SHOW")) {
                    if( win) {
                        IIntuition->WindowToFront(win );
                    } else {
                        startgui();
                    }
                }
                
                if( StrCmp((char*) rexxcmd,(char*) "DISABLE")) {
                    ICommodities->ActivateCxObj((CxObj*) broker ,FALSE );
                    enabled=FALSE;
                }
                
                if( StrCmp((char*) rexxcmd,(char*) "ENABLE")) {
                    ICommodities->ActivateCxObj((CxObj*) broker ,TRUE );
                    enabled=TRUE;
                }
                
                if( StrCmp((char*) rexxcmd,(char*) "QUIT")) {
                    terminated=TRUE;
printf("#3\n" ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 );
                }
                
                if( StrCmp((char*) rexxcmd,(char*) "GETTIME")) {
                    if( retrievetime()==0) {
                        StrCopy(result,displaytimes());
                    } else {
                        ioreq->Request.io_Command=(UWORD) (long) TR_GETSYSTIME;
                        IExec->DoIO((struct IORequest*) ioreq );
                        curtime=(long) ioreq->Time.Seconds;
                        StrCopy(result,sectostr(curtime));
                    }
                }

                if( StrCmp((char*) rexxcmd,(char*) "GETGMT")) {
                    if( retrievetime()==0) {
                        StrCopy(result,displaytimes());
                    } else {
                        ioreq->Request.io_Command=(UWORD) (long) TR_GETSYSTIME;
                        IExec->DoIO((struct IORequest*) ioreq );
                        curtime=(long) ioreq->Time.Seconds;
                        StrCopy(result,sectostr(curtime,1));
                    }
                }
                
                if( StrCmp((char*) rexxcmd,(char*) "SETTIME")) {
                    if( (rc=retrievetime())==0) {
                        dodefop();
                        displaytimes();
                    } else {
                        tryalternative();
                    }
                }

                if( StrCmp((char*) rexxcmd,(char*) "DSTMODE",7)) {
                    if( strlen((char*) rexxcmd )>7) {
                        dstswitch(Val(MidStr(temp,(char*) rexxcmd,8)),1);
                    } else {
                        StringF(result,(char*) "%ld",dst);
                    }
                }

                if( StrCmp((char*) rexxcmd,(char*) "SERVER",6)) {
                    if( strlen((char*) rexxcmd )>6) {
					    MidStr(temp,(char*) rexxcmd,7);
					    colon=InStr(temp,(char*) ":");
    					if( colon!=-1) {
        					StrCopy(server,temp,colon);
        					MidStr(temp,temp,colon+1);
        					port=Val(temp);
					    } else {
    	    				StrCopy(server,temp);
    					}

				        if( winopen ) { updategui();}
                    } else {
                        StringF(result,(char*) "%s",(long) server);
                    }
                }

                if( StrCmp((char*) rexxcmd,(char*) "SERVICE",7)) {
                    if( strlen((char*) rexxcmd )>7) {
					    MidStr(temp,(char*) rexxcmd,8);
		                if( StrCmp(temp,(char*) "SNTP")) {
							service=123;
						}
	            	    if( StrCmp(temp,(char*) "TIMEUDP")) {
							service=372;
						}
		                if( StrCmp(temp,(char*) "TIMETCP")) {
							service=37;
						}
	            	    if( StrCmp(temp,(char*) "DAYTIME")) {
							service=13;
						}

			            if( defport) {
    	    		        if( service==13 ) { port=defdaytimeport;}
        	        		if( service==37 ) { port=deftimeport;}
		    	            if( service==123 ) { port=defsntpport;}
        			        if( service==372 ) { port=deftimeudpport;}
            			}

				        if( winopen ) { updategui();}
					} else {
   	    		        if( service==13 ) { StrCopy(temp,(char*) "DAYTIME");}
   	    		        if( service==37 ) { StrCopy(temp,(char*) "TIMETCP");}
   	    		        if( service==372 ) { StrCopy(temp,(char*) "TIMEUDP");}
   	    		        if( service==123 ) { StrCopy(temp,(char*) "SNTP");}

                        StringF(result,(char*) "%s",(long) temp);
					}
                }

                if( StrCmp((char*) rexxcmd,(char*) "TIMEOUT",7)) {
                    if( strlen((char*) rexxcmd )>7) {
					    MidStr(temp,(char*) rexxcmd,8);
						timeout=Val(temp);
					} else {
                        StringF(result,(char*) "%ld",timeout);
					}
				}

                if( StrCmp((char*) rexxcmd,(char*) "TIMEZONE")) {
                    tmpint = tz*-100+10000;
                    if( extrahalf) {
                        if( tzsecs<0 ) { tmpint = tmpint+30 ;} else { tmpint = tmpint - 30;}
                    }
                    if( tmpint<0 ) { StrCopy(result,(char*) "-") ;} else { StrCopy(result,(char*) "+");}
                    StrAdd(result,RightStr(temp,StringF(temp,(char*) "%ld",tmpint),4));
                }

                if( StrCmp((char*) rexxcmd,(char*) "SAVECFG") ) { saveconfig();}

            rx_ReplyMsg((struct RexxMsg*) arexxmsg,rc,result);
        }
        if( win ) { IIntuition->SetWindowPointerA(win ,(struct TagItem*) tempGlobal_ILIST55  );}
        } while(( arexxmsg==(long) NULL)==0);
    }
}

if( signal & notifysig) {
    while( msg=(long) IExec->GetMsg(notifyport )) {
//        WriteF('notified/n')
    if( ignoretz==0) {
        oldtz=tz;
        readtz();
        adjustclock((oldtz-tz)*3600);
        if( winopen) {
                 tempGlobal_ILIST56 [1]= dst;
                 IGadTools->GT_SetGadgetAttrsA((struct Gadget*) gads[GADG_DST] ,win ,(struct Requester*) NULL ,(struct TagItem*) tempGlobal_ILIST56  );
                 tempGlobal_ILIST57 [1]= (long) tzname1;
                 IGadTools->GT_SetGadgetAttrsA((struct Gadget*) gads[GADG_TZNAME] ,win ,(struct Requester*) NULL ,(struct TagItem*) tempGlobal_ILIST57  );
                 tempGlobal_ILIST58 [1]= dst-1;
                 tempGlobal_ILIST58 [3]= (long) tzname2;
                 IGadTools->GT_SetGadgetAttrsA((struct Gadget*) gads[GADG_DSTNAME] ,win ,(struct Requester*) NULL ,(struct TagItem*) tempGlobal_ILIST58  );
                 tempGlobal_ILIST59 [1]= -tz;
                 IGadTools->GT_SetGadgetAttrsA((struct Gadget*) gads[GADG_TZ] ,win ,(struct Requester*) NULL ,(struct TagItem*) tempGlobal_ILIST59  );
        }

    }
        IExec->ReplyMsg((struct Message*) msg );
    }
}

if( signal & ttnotifysig) {
    while( msg=(long) IExec->GetMsg(ttnotifyport )) {
        tempGlobal_ILIST60 [0]= sizeof( struct EasyStruct);
        tempGlobal_ILIST60 [3]= locstr[MSG_ICON_NOTIFY];
        tempGlobal_ILIST60 [4]= locstr[MSG_ICON_NOTIFY_OPTS];
        tmpint=IIntuition->EasyRequestArgs((struct Window*) (long) 0 ,(struct EasyStruct*) tempGlobal_ILIST60  ,(ULONG*) (long) 0 ,(APTR) (long) NULL );
        if( tmpint==1 /* update */) {
            readtooltypes() /* load tooltype settings */;
            if( winopen ) { updategui();}
        }
        IExec->ReplyMsg((struct Message*) msg );
    }
}

if( signal & tisig) {
    while( msg=(long) IExec->GetMsg(msgport )) {
        if( enabled) {
            syncclock();
        }

//        ReplyMsg(msg) ->  *** eh? need this, don't we?
        starttint();
    }
}

if( signal & envsig) {
    while( msg=(long) IExec->GetMsg(envmsgport )) {
   //     IF timeenv THEN writetimeenv()

//        ReplyMsg(msg) ->  *** eh? need this, don't we?
        startenvint();
    }
}


if( signal & clksig) {
    while( msg=(long) IExec->GetMsg(clkmsgport )) {
        if( enabled) {
            if( win ) { showtime();}
        }

//        ReplyMsg(msg) ->  *** eh? need this, don't we?
        startclkint();
    }
}


if( signal & dstsig) {
    while( msg=(long) IExec->GetMsg(dstmsgport )) {
//        ReplyMsg(msg) ->  *** eh? need this, don't we?
//        WriteF('test/n')
        if( leapsec ) { leapsecchg();}
        if( autodst ) { startdst(chkautodst_generic());}
    }
}

if( clgui) {
    closegui();
    clgui=0;
}

} while(( terminated)==0);

saveleapsecs();

} catch(...) {}
PrintException();//###
	DisposeString(result );
	if (exception!=0) {throw eException;} else {EMPTY;};
	exception = temp_QUAD ;
	return ;
}

void addcommodity() {
broker_mp=IExec->CreateMsgPort();

tempGlobal_ARRAY_OF_newbroker [0].nb_Descr = (char*) locstr[CX_DESCRIPTION];

tempGlobal_ARRAY_OF_newbroker [0].nb_Pri = cxpri;

tempGlobal_ARRAY_OF_newbroker [0].nb_Port = broker_mp;

if( (broker=(long) ICommodities->CxBroker((struct NewBroker*) tempGlobal_ARRAY_OF_newbroker  ,(long*) NULL ))==(long) NULL ) { Raise(ERR_CMDY);}

cxsigflag=( (long) broker_mp->mp_SigBit >= 0 )? 1 << (long) broker_mp->mp_SigBit : 1 >> labs((long) broker_mp->mp_SigBit );

filter=(long) hotKey(popkey,broker_mp,EVT_POPKEY);
ICommodities->AttachCxObj((CxObj*) broker ,(CxObj*) filter );
inputevent=invertStringRev((char*) NULL,(struct KeyMap*) NULL);

filter2=(long) hotKey(savekey,broker_mp,EVT_SAVEKEY);
ICommodities->AttachCxObj((CxObj*) broker ,(CxObj*) filter2 );
inputevent=invertStringRev((char*) NULL,(struct KeyMap*) NULL);

ICommodities->ActivateCxObj((CxObj*) broker ,TRUE );

appmenu=IExec->CreateMsgPort();

if( toolsmenu ) { startappmenu();}

if( donotopenwin==0 ) { startgui();}
	return ;
}

void readtooltypes() {
struct DiskObject* dobj=NULL; long tt, toolarray, olddir=-1;

    if( (long) wbarg->wa_Lock  & - ((long) wbarg->wa_Name!=(long) 0)  ) { olddir=(long) IDOS->CurrentDir(wbarg->wa_Lock );}
    dobj=IIcon->GetDiskObject(wbarg->wa_Name );

if( dobj) {

toolarray=(long) dobj->do_ToolTypes;

if( (tt=(long) IIcon->FindToolType((char**) toolarray ,"CXPRIORITY" ))) { cxpri=Val((char*) tt);}
if( (tt=(long) IIcon->FindToolType((char**) toolarray ,"CXPOPUP" ))) {
    if( (BOOL)(0!=IIcon->MatchToolValue((char*) tt ,"YES" ))) { winopen=0;}
    if( (BOOL)(0!=IIcon->MatchToolValue((char*) tt ,"NO" ))) { donotopenwin=1;}
}

if( (tt=(long) IIcon->FindToolType((char**) toolarray ,"CXPOPKEY" ))) { StrCopy(popkey,(char*) tt);}
/*
ELSE
    StrCopy(popkey,'ctrl alt f')
ENDIF
*/

if( (tt=(long) IIcon->FindToolType((char**) toolarray ,"CHECKKEY" ))) {
    StrCopy(savekey,(char*) tt);
} else {
    StrCopy(savekey,(char*) "ctrl shift alt f");
}


if( EstrLen(tzname2)<3) {
    if( (tt=(long) IIcon->FindToolType((char**) toolarray ,"DSTNAME" ))) {
        StrCopy(tzname2,(char*) tt,4);
        if( EstrLen(tzname2)<4 ) { StrAdd(tzname2,(char*) "    ");}
    } else {
        StrCopy(tzname2,(char*) "DST ");
    }
}

    if( (tt=(long) IIcon->FindToolType((char**) toolarray ,"COMMAND" ))) {
        StrCopy(cmd,(char*) tt,512);
    } else {
        nocmd=1;
    }
/*
StrCopy(st1db,'PROGDIR:clock1.htm')
StrCopy(st2db,'PROGDIR:clock2.htm')

    IF tt:=FindToolType(toolarray,'STRATUM1DB')
        StrCopy(st1db,tt,512)
    ENDIF
    IF tt:=FindToolType(toolarray,'STRATUM2DB')
        StrCopy(st2db,tt,512)
    ENDIF
*/

if( (tt=(long) IIcon->FindToolType((char**) toolarray ,"TINT" ))) { timeint=Val((char*) tt);}

// IF tt:=FindToolType(toolarray,'TIMEOUT') THEN timeout:=Val(tt)

if( (tt=(long) IIcon->FindToolType((char**) toolarray ,"UTCOFFSET" ))) {
    utcoffset=Val((char*) tt);
    utcnum=utcoffset/3600;

    if( utcnum<0) {
        utcneg=-1;
    } else {
        utcneg=0;
    }

    utcnum=labs(utcnum );
}

if( (tt=(long) IIcon->FindToolType((char**) toolarray ,"OPMODE" ))) {
    defsave=1;
    defuse=1;
}

if( (tt=(long) IIcon->FindToolType((char**) toolarray ,"WRITETO" ))) {
    defsave=0;
    defuse=0;
    if( (BOOL)(0!=IIcon->MatchToolValue((char*) tt ,"BATTCLOCK" ))) { defsave=1;}
    if( (BOOL)(0!=IIcon->MatchToolValue((char*) tt ,"SYSTEM" ))) { defuse=1;}
}

if( IIcon->FindToolType((char**) toolarray ,"DEBUG" )) { debug=1;}

if( (tt=(long) IIcon->FindToolType((char**) toolarray ,"COMPATIBILITY" ))) {

    if( (BOOL)(0!=IIcon->MatchToolValue((char*) tt ,"MODE2036" ))) {
        mode2036=1;
    }

    if( (BOOL)(0!=IIcon->MatchToolValue((char*) tt ,"BASE2015" ))) {
        base2015=1;
    }

    if( (BOOL)(0!=IIcon->MatchToolValue((char*) tt ,"BATT2078" ))) {
        batt2078=1;
    }

}

if( (tt=(long) IIcon->FindToolType((char**) toolarray ,"AUTODST" ))) { autodst=Val((char*) tt);}

if( longtz==0) {
    if( (tt=(long) IIcon->FindToolType((char**) toolarray ,"DSTSTARTMONTH" ))) { ostartmonth=Val((char*) tt);}
    if( (tt=(long) IIcon->FindToolType((char**) toolarray ,"DSTSTARTWDAY" ))) { ostartwday=Val((char*) tt);}
    if( (tt=(long) IIcon->FindToolType((char**) toolarray ,"DSTSTARTHOUR" ))) { ostarthour=Val((char*) tt);}
    if( (tt=(long) IIcon->FindToolType((char**) toolarray ,"DSTSTARTWEEK" ))) { ostartweek=Val((char*) tt);}

    if( (tt=(long) IIcon->FindToolType((char**) toolarray ,"DSTENDMONTH" ))) { oendmonth=Val((char*) tt);}
    if( (tt=(long) IIcon->FindToolType((char**) toolarray ,"DSTENDWDAY" ))) { oendwday=Val((char*) tt);}
    if( (tt=(long) IIcon->FindToolType((char**) toolarray ,"DSTENDHOUR" ))) { oendhour=Val((char*) tt);}
    if( (tt=(long) IIcon->FindToolType((char**) toolarray ,"DSTENDWEEK" ))) { oendweek=Val((char*) tt);}

if( ostartweek==5 ) { ostartweek=0;}
if( oendweek==5 ) { oendweek=0;}

}

    if( (tt=(long) IIcon->FindToolType((char**) toolarray ,"DSTHEMISPHERE" ))) {

        if( (BOOL)(0!=IIcon->MatchToolValue((char*) tt ,"NORTH" ))) {
            ohemisphere=0;
        }

        if( (BOOL)(0!=IIcon->MatchToolValue((char*) tt ,"SOUTH" ))) {
            ohemisphere=1;
        }

    }

if( IIcon->FindToolType((char**) toolarray ,"IGNORETZ" )) { ignoretz=1 ;} else { ignoretz=0;}
if( IIcon->FindToolType((char**) toolarray ,"OBSESSION" )) { obsession=1 ;} else { obsession=0;}
if( IIcon->FindToolType((char**) toolarray ,"CLOCKDAEMON" )) { clkdaemon=1 ;} else { clkdaemon=0;}

if( IIcon->FindToolType((char**) toolarray ,"MULTICAST" )) { mcast=1 ;} else { mcast=0;}
// or anycast
if( (tt=(long) IIcon->FindToolType((char**) toolarray ,"DSTADJUST" ))) {
    dstoffset=Val((char*) tt);
    nicedst=dstoffset/3600;
}

// IF tt:=FindToolType(toolarray,'SERVICE') THEN service:=Val(tt)
if( (tt=(long) IIcon->FindToolType((char**) toolarray ,"DEFSERVER" ))) { servnum=Val((char*) tt);}
if( (tt=(long) IIcon->FindToolType((char**) toolarray ,"TIMEPORT" ))) { timeport=Val((char*) tt);}
if( (tt=(long) IIcon->FindToolType((char**) toolarray ,"DAYTIMEPORT" ))) { daytimeport=Val((char*) tt);}
if( (tt=(long) IIcon->FindToolType((char**) toolarray ,"SNTPPORT" ))) { sntpport=Val((char*) tt);}
if( (tt=(long) IIcon->FindToolType((char**) toolarray ,"UDPTIMEPORT" ))) { timeudpport=Val((char*) tt);}
if( IIcon->FindToolType((char**) toolarray ,"APPMENU" )) { toolsmenu=1 ;} else { toolsmenu=0;}
if( IIcon->FindToolType((char**) toolarray ,"EXTRAHALF" )) { extrahalf=1 ;} else { extrahalf=0;}
if( IIcon->FindToolType((char**) toolarray ,"TIMEENV" )) { timeenv=1 ;} else { timeenv=0;}
if( IIcon->FindToolType((char**) toolarray ,"QUIETDST" )) { autodquiet=1 ;} else { autodquiet=0;}
if( longtz ) { autodquiet=1;}
if( IIcon->FindToolType((char**) toolarray ,"STGEMUL" )) { stgemul=1 ;} else { stgemul=0;}
if( IIcon->FindToolType((char**) toolarray ,"CHECKONSTARTUP" )) { chkstart=1 ;} else { chkstart=0;}
if( (tt=(long) IIcon->FindToolType((char**) toolarray ,"ONERROR" ))) {
        if( (BOOL)(0!=IIcon->MatchToolValue((char*) tt ,"ANINC" ))) { aninc=1 ;} else { aninc=0;}
        if( (BOOL)(0!=IIcon->MatchToolValue((char*) tt ,"NEXTSERVICE" ))) { nextservice=1 ;} else { nextservice=0;}
        if( (BOOL)(0!=IIcon->MatchToolValue((char*) tt ,"USELIST" ))) { trynext=1 ;} else { trynext=0;}
}
if( IIcon->FindToolType((char**) toolarray ,"ALWAYSUPDATE" )) { alwaysupdate=1 ;} else { alwaysupdate=0;}

if( (tt=(long) IIcon->FindToolType((char**) toolarray ,"WINX" ))) { winx=Val((char*) tt);}
if( (tt=(long) IIcon->FindToolType((char**) toolarray ,"WINY" ))) { winy=Val((char*) tt);}
if( IIcon->FindToolType((char**) toolarray ,"ZOOMED" )) { zoomed=1 ;} else { zoomed=0;}

// IF FindToolType(toolarray,'DEFPORT')
    defport=0 ;// was 1
if( (- (service==13 )& port)==defdaytimeport ) { defport=1;}
if( (- (service==37 )& port)==deftimeport ) { defport=1;}
if( (- (service==123 )& port)==defsntpport ) { defport=1;}
if( (- (service==372 )& port)==deftimeudpport ) { defport=1;}
/*
    IF service=13 THEN port:=defdaytimeport
    IF service=37 THEN port:=deftimeport
    IF service=123 THEN port:=defsntpport
    IF service=372 THEN port:=deftimeudpport
*/
/*
ELSE
    defport:=0
*/
/*
    IF service=13 THEN port:=daytimeport
    IF service=37 THEN port:=timeport
    IF service=123 THEN port:=sntpport
    IF service=372 THEN port:=timeudpport
*/
// ENDIF

IIcon->FreeDiskObject(dobj );
dobj=(struct DiskObject*) NULL;

startttnotify();

}

if( olddir!=-1 ) { IDOS->CurrentDir((BPTR) olddir );}
	return ;

}

void closegui() {
if( win ) { IIntuition->CloseWindow(win );}

if( mysc ) { IIntuition->UnlockPubScreen((char*) NULL ,mysc );}
mysc=(struct Screen*) NULL;
winopen=0;
win=(struct Window*) NULL;
winsig=(long) NULL;
	return ;
}

void savetz(long useonly) {
char* tzstring=NULL; char* stgstr=NULL; long stgtz, wdltzs, wdltze, fhand=0;
long temp_QUAD;
try {
	tzstring = NewString(50);
	stgstr = NewString(50);
	temp_QUAD = exception ;
	exception = 0 ;

if( EstrLen(tzname1)<3 ) { StrAdd(tzname1,(char*) "xxx");}

if( stgemul) {
    stgtz=-tz;
    if( dst ) { stgtz=stgtz-1;}
    stgtz=stgtz*60;
   
 tempGlobal_ILIST61 [0]= stgtz;
 tempGlobal_ILIST61 [1]= startweek;
 tempGlobal_ILIST61 [2]= startwday;
 tempGlobal_ILIST61 [3]= startmonth;
 tempGlobal_ILIST61 [4]= starthour*60;
 tempGlobal_ILIST61 [5]= endweek;
 tempGlobal_ILIST61 [6]= endwday;
 tempGlobal_ILIST61 [7]= endmonth;
 tempGlobal_ILIST61 [8]= endhour*60;
 tempGlobal_ILIST61 [9]= stgtz+60;
 StringFL(stgstr,(char*) "NT:%ld:%ld:%ld:%ld:%ld:%ld:%ld:%ld:%ld:%ld:",tempGlobal_ILIST61 );
}

StrCopy(tzstring,tzname1,3);

if( longtz) {
/*
    IF dst
        StrAdd(tzstring,RealF(temp,(tz-1)!,0))
    ELSE
*/
        StrAdd(tzstring,RealF(temp,(float) tz,0));
//    ENDIF
} else {
    StrAdd(tzstring,RealF(temp,(float) tz,0));
}

if( dst) {
    if( EstrLen(tzname2)<3 ) { StrAdd(tzname2,(char*) " xxx");}
    StrAdd(tzstring,tzname2);
    StrAdd(stgstr,(char*) "YES");
} else {
    StrAdd(stgstr,(char*) "NO");
}

// WriteF('/s-/d-/s/n',tzname1,tz,tzstring)

if( stgemul) {
    if( useonly==0) {
        if( (tzfile=(long) IDOS->Open("ENVARC:SUMMERTIME" ,MODE_NEWFILE ))) {
            IDOS->FPuts((BPTR) tzfile ,stgstr );
            (BOOL)(0!=IDOS->Close((BPTR) tzfile ));
        } else {
            tempGlobal_ILIST62 [0]= sizeof( struct EasyStruct);
            tempGlobal_ILIST62 [3]= locstr[MSG_ERR_WRITING];
            IIntuition->EasyRequestArgs((struct Window*) (long) 0 ,(struct EasyStruct*) tempGlobal_ILIST62  ,(ULONG*) (long) 0 ,(APTR) (long) NULL );
        }
    }

    if( (tzfile=(long) IDOS->Open("ENV:SUMMERTIME" ,MODE_NEWFILE ))) {
        IDOS->FPuts((BPTR) tzfile ,stgstr );
        (BOOL)(0!=IDOS->Close((BPTR) tzfile ));
    } else {
        tempGlobal_ILIST63 [0]= sizeof( struct EasyStruct);
        tempGlobal_ILIST63 [3]= locstr[MSG_ERR_WRITING];
        IIntuition->EasyRequestArgs((struct Window*) (long) 0 ,(struct EasyStruct*) tempGlobal_ILIST63  ,(ULONG*) (long) 0 ,(APTR) (long) NULL );
    }
}

if( longtz) {
    if( dst==0) {
        if( EstrLen(tzname2)<3 ) { StrAdd(tzname2,(char*) " xxx");}
        StrAdd(tzstring,tzname2);
   }

if( ostartweek==0 ) { wdltzs=5 ;} else { wdltzs=ostartweek;}
if( oendweek==0 ) { wdltze=5 ;} else { wdltze=oendweek;}

 StringF(temp,(char*) ",M%ld.%ld.%ld/%ld,M%ld.%ld.%ld/%ld",ostartmonth,wdltzs,ostartwday,ostarthour,oendmonth,wdltze,oendwday,oendhour);
    StrAdd(tzstring,temp);
}

    if( (fhand=(long) IDOS->Lock("ENV:FACTS" ,ACCESS_WRITE ))==0) {
        fhand=(long) IDOS->CreateDir("ENV:FACTS" );
    }
    IDOS->UnLock((BPTR) fhand );

if( (tzfile=(long) IDOS->Open("ENV:TZ" ,MODE_NEWFILE ))) {
IDOS->FPuts((BPTR) tzfile ,tzstring );
(BOOL)(0!=IDOS->Close((BPTR) tzfile ));
} else {
    tempGlobal_ILIST64 [0]= sizeof( struct EasyStruct);
    tempGlobal_ILIST64 [3]= locstr[MSG_ERR_WRITING];
    IIntuition->EasyRequestArgs((struct Window*) (long) 0 ,(struct EasyStruct*) tempGlobal_ILIST64  ,(ULONG*) (long) 0 ,(APTR) (long) NULL );
}
    if( (tzfile=(long) IDOS->Open("ENV:FACTS/DST" ,MODE_NEWFILE ))) {
        IDOS->FPutC((BPTR) tzfile ,dst );
        (BOOL)(0!=IDOS->Close((BPTR) tzfile ));
    } else {
        tempGlobal_ILIST65 [0]= sizeof( struct EasyStruct);
        tempGlobal_ILIST65 [3]= locstr[MSG_ERR_WRITING];
        IIntuition->EasyRequestArgs((struct Window*) (long) 0 ,(struct EasyStruct*) tempGlobal_ILIST65  ,(ULONG*) (long) 0 ,(APTR) (long) NULL );
    }
    if( (fhand=(long) IDOS->Lock("ENVARC:FACTS" ,ACCESS_WRITE ))==0) {
        fhand=(long) IDOS->CreateDir("ENVARC:FACTS" );
    }
    IDOS->UnLock((BPTR) fhand );
    if( (tzfile=(long) IDOS->Open("ENVARC:FACTS/DST" ,MODE_NEWFILE ))) {
        IDOS->FPutC((BPTR) tzfile ,dst );
        (BOOL)(0!=IDOS->Close((BPTR) tzfile ));
    } else {
        tempGlobal_ILIST66 [0]= sizeof( struct EasyStruct);
        tempGlobal_ILIST66 [3]= locstr[MSG_ERR_WRITING];
        IIntuition->EasyRequestArgs((struct Window*) (long) 0 ,(struct EasyStruct*) tempGlobal_ILIST66  ,(ULONG*) (long) 0 ,(APTR) (long) NULL );
    }

if( useonly==0) {
    if( (tzfile=(long) IDOS->Open("ENVARC:TZ" ,MODE_NEWFILE ))) {
        IDOS->FPuts((BPTR) tzfile ,tzstring );
        (BOOL)(0!=IDOS->Close((BPTR) tzfile ));
    } else {
tempGlobal_ILIST67 [0]= sizeof( struct EasyStruct);
tempGlobal_ILIST67 [3]= locstr[MSG_ERR_WRITING];
IIntuition->EasyRequestArgs((struct Window*) (long) 0 ,(struct EasyStruct*) tempGlobal_ILIST67  ,(ULONG*) (long) 0 ,(APTR) (long) NULL );
    }
}
} catch(...) {}
	DisposeString(stgstr );
	DisposeString(tzstring );
	if (exception!=0) {throw eException;} else {EMPTY;};
	exception = temp_QUAD ;
	return ;

}

void saveconfig() {
long olddir=-1, oldtooltypes; long newtooltypes[50]; char* tmptt1=NULL;
char* tmptt2=NULL; char* tmptt3=NULL; char* tmptt4=NULL; char* tmptt5=NULL; char* tmptt6=NULL;
char* tmptt7=NULL; char* tmptt8=NULL; char* tmptt10=NULL;
char* tmptt11=NULL; char* tmptt12=NULL; char* tmptt13=NULL; char* tmptt15=NULL;
char* tmptt22=NULL; char* tmptt23=NULL; char* tmptt24=NULL; char* tmptt25=NULL;
char* tmptt26=NULL; char* tmptt27=NULL; char* tmptt28=NULL; char* tmptt29=NULL;
char* tmptt30=NULL; char* tmptt31=NULL; char* tmptt32=NULL; char* tmptt33=NULL;
char* tmptt34=NULL; char* tmptt35=NULL; char* tmptt36=NULL;

// no tmptt9 or tmptt14!

struct DiskObject* dobj=NULL; /* ,wbarg:PTR TO wbarg,wbenchMsg:PTR TO wbstartup */
char* tmp3=NULL;
char* servfile=NULL;
long temp_QUAD;
try {
	tmptt1 = NewString(50);
	tmptt2 = NewString(20);
	tmptt3 = NewString(50);
	tmptt4 = NewString(100);
	tmptt5 = NewString(20);
	tmptt6 = NewString(20);
	tmptt7 = NewString(20);
	tmptt8 = NewString(20);
	tmptt10 = NewString(20);
	tmptt11 = NewString(20);
	tmptt12 = NewString(20);
	tmptt13 = NewString(20);
	tmptt15 = NewString(50);
	tmptt22 = NewString(20);
	tmptt23 = NewString(20);
	tmptt24 = NewString(20);
	tmptt25 = NewString(20);
	tmptt26 = NewString(20);
	tmptt27 = NewString(20);
	tmptt28 = NewString(20);
	tmptt29 = NewString(20);
	tmptt30 = NewString(20);
	tmptt31 = NewString(20);
	tmptt32 = NewString(40);
	tmptt33 = NewString(20);
	tmptt34 = NewString(20);
	tmptt35 = NewString(512);
	tmptt36 = NewString(512);
	tmp3 = NewString(10);
	servfile = NewString(100);
	temp_QUAD = exception ;
	exception = 0 ;

savetz();

if( wbstart==2 ) { goto finally;}
/*
    wbenchMsg:=wbmessage
    wbarg:=wbenchMsg.arglist
*/
    StringF(temp,(char*) "%s:%ld:%ld:%ld",(long) server,port,service,timeout);

StringF(servfile,(char*) "ENV:FACTS/TIMESERVER.%ld",servnum);

if( (ntphost=(long) IDOS->Open(servfile ,MODE_NEWFILE ))) {
    IDOS->FPuts((BPTR) ntphost ,temp );
    (BOOL)(0!=IDOS->Close((BPTR) ntphost ));
} else {
    tempGlobal_ILIST68 [0]= sizeof( struct EasyStruct);
    tempGlobal_ILIST68 [3]= locstr[MSG_ERR_WRITING];
    tempGlobal_ILIST68 [5]= (long) servfile;
    IIntuition->EasyRequestArgs((struct Window*) (long) 0 ,(struct EasyStruct*) tempGlobal_ILIST68  ,(ULONG*) (long) 0 ,(APTR) (long) NULL );
}

StringF(servfile,(char*) "ENVARC:FACTS/TIMESERVER.%ld",servnum);


if( (ntphost=(long) IDOS->Open(servfile ,MODE_NEWFILE ))) {
    IDOS->FPuts((BPTR) ntphost ,temp );
    (BOOL)(0!=IDOS->Close((BPTR) ntphost ));
} else {
    tempGlobal_ILIST69 [0]= sizeof( struct EasyStruct);
    tempGlobal_ILIST69 [3]= locstr[MSG_ERR_WRITING];
    tempGlobal_ILIST69 [5]= (long) servfile;
    IIntuition->EasyRequestArgs((struct Window*) (long) 0 ,(struct EasyStruct*) tempGlobal_ILIST69  ,(ULONG*) (long) 0 ,(APTR) (long) NULL );
}

stopttnotify();

if( (long) wbarg->wa_Lock  & - ((long) wbarg->wa_Name!=(long) 0)  ) { olddir=(long) IDOS->CurrentDir(wbarg->wa_Lock );}

if( (dobj=IIcon->GetDiskObject(wbarg->wa_Name ))) {

oldtooltypes=(long) dobj->do_ToolTypes;

StrCopy(tmptt1,(char*) "CXPOPKEY=");
StrAdd(tmptt1,popkey);
newtooltypes[1]=(long) tmptt1;

StringF(tmptt2,(char*) "CXPRIORITY=%ld",cxpri);
newtooltypes[2]=(long) tmptt2;

if( donotopenwin) {
    newtooltypes[3]=(long) "CXPOPUP=NO";
} else {
    newtooltypes[3]=(long) "CXPOPUP=YES";
}

newtooltypes[0]=(long) "DONOTWAIT";

StrCopy(tmptt3,(char*) "CHECKKEY=");
StrAdd(tmptt3,savekey);
newtooltypes[4]=(long) tmptt3;

StrCopy(tmptt4,(char*) "WRITETO=");
if( defsave ) { StrAdd(tmptt4,(char*) "BATTCLOCK");}
if( defuse ) { StrAdd(tmptt4,(char*) "|SYSTEM");}
newtooltypes[5]=(long) tmptt4;

StrCopy(tmptt5,(char*) "TINT=");
StrAdd(tmptt5,RealF(temp,(float) timeint,0));
newtooltypes[6]=(long) tmptt5;

StrCopy(tmptt6,(char*) "DSTNAME=");
StrAdd(tmptt6,tzname2);
newtooltypes[7]=(long) tmptt6;

if( zoomed ) { newtooltypes[8]=(long) "ZOOMED";} else { newtooltypes[8]=(long) "(ZOOMED)";}

// IF defport THEN newtooltypes[8]:='DEFPORT' ELSE newtooltypes[8]:='(DEFPORT)'

StringF(tmptt11,(char*) "AUTODST=%ld",autodst);
newtooltypes[9]=(long) tmptt11;

if( ignoretz ) { newtooltypes[10]=(long) "IGNORETZ";} else { newtooltypes[10]=(long) "(IGNORETZ)";}

StringF(tmptt7,(char*) "DAYTIMEPORT=%ld",daytimeport);
newtooltypes[11]=(long) tmptt7;

StringF(tmptt8,(char*) "TIMEPORT=%ld",timeport);
newtooltypes[12]=(long) tmptt8;

/*
StringF(tmptt9,'SERVICE=\d',service)
newtooltypes[13]:=tmptt9
*/

if( alwaysupdate ) { newtooltypes[13]=(long) "ALWAYSUPDATE";} else { newtooltypes[13]=(long) "(ALWAYSUPDATE)";}

StringF(tmptt10,(char*) "UTCOFFSET=%ld",utcoffset);
newtooltypes[14]=(long) tmptt10;

StringF(tmptt12,(char*) "SNTPPORT=%ld",sntpport);
newtooltypes[15]=(long) tmptt12;

StringF(tmptt13,(char*) "UDPTIMEPORT=%ld",timeudpport);
newtooltypes[16]=(long) tmptt13;

/*
StringF(tmptt14,'TIMEOUT=\d',timeout)
newtooltypes[17]:=tmptt14
*/

if( trynext) {
    newtooltypes[17]=(long) "ONERROR=USELIST";
} else {
    StrCopy(tmptt32,(char*) "ONERROR=");
    if( aninc ) { StrAdd(tmptt32,(char*) "ANINC|");}
    if( nextservice ) { StrAdd(tmptt32,(char*) "NEXTSERVICE");}
    newtooltypes[17]=(long) tmptt32;
}


StrCopy(tmptt15,(char*) "COMPATIBILITY=");
    if( mode2036) {
        StrAdd(tmptt15,(char*) "MODE2036|");
    }
    if( base2015) {
        StrAdd(tmptt15,(char*) "BASE2015|");
    }
    if( batt2078) {
        StrAdd(tmptt15,(char*) "BATT2078");
    }

newtooltypes[18]=(long) tmptt15;

if( toolsmenu ) { newtooltypes[19]=(long) "APPMENU";} else { newtooltypes[19]=(long) "(APPMENU)";}
if( extrahalf ) { newtooltypes[20]=(long) "EXTRAHALF";} else { newtooltypes[20]=(long) "(EXTRAHALF)";}

if( autodquiet ) { newtooltypes[21]=(long) "QUIETDST";} else { newtooltypes[21]=(long) "(QUIETDST)";}

StringF(tmptt22,(char*) "DSTSTARTMONTH=%ld",ostartmonth);
newtooltypes[22]=(long) tmptt22;
StringF(tmptt23,(char*) "DSTSTARTWDAY=%ld",ostartwday);
newtooltypes[23]=(long) tmptt23;
StringF(tmptt24,(char*) "DSTSTARTHOUR=%ld",ostarthour);
newtooltypes[24]=(long) tmptt24;
StringF(tmptt25,(char*) "DSTSTARTWEEK=%ld",ostartweek);
newtooltypes[25]=(long) tmptt25;

StringF(tmptt26,(char*) "DSTENDMONTH=%ld",oendmonth);
newtooltypes[26]=(long) tmptt26;
StringF(tmptt27,(char*) "DSTENDWDAY=%ld",oendwday);
newtooltypes[27]=(long) tmptt27;
StringF(tmptt28,(char*) "DSTENDHOUR=%ld",oendhour);
newtooltypes[28]=(long) tmptt28;
StringF(tmptt29,(char*) "DSTENDWEEK=%ld",oendweek);
newtooltypes[29]=(long) tmptt29;

if( ohemisphere==0 ) { newtooltypes[30]=(long) "DSTHEMISPHERE=NORTH";} else { newtooltypes[31]=(long) "DSTHEMISPHERE=SOUTH";}

if( stgemul ) { newtooltypes[31]=(long) "STGEMUL";} else { newtooltypes[31]=(long) "(STGEMUL)";}

if( chkstart ) { newtooltypes[32]=(long) "CHECKONSTARTUP";} else { newtooltypes[32]=(long) "(CHECKONSTARTUP)";}

StringF(tmptt30,(char*) "WINX=%ld",winx);
newtooltypes[33]=(long) tmptt30;

StringF(tmptt31,(char*) "WINY=%ld",winy);
newtooltypes[34]=(long) tmptt31;

if( obsession ) { newtooltypes[35]=(long) "OBSESSION";} else { newtooltypes[35]=(long) "(OBSESSION)";}

StringF(temp,(char*) "COMMAND=%s",(long) cmd);
newtooltypes[36]=(long) temp;

if( clkdaemon ) { newtooltypes[37]=(long) "CLOCKDAEMON";} else { newtooltypes[37]=(long) "(CLOCKDAEMON)";}

StringF(tmptt33,(char*) "DEFSERVER=%ld",servnum);
newtooltypes[38]=(long) tmptt33;

StringF(tmptt34,(char*) "DSTADJUST=%ld",dstoffset);
newtooltypes[39]=(long) tmptt34;
/*
StrCopy(tmptt35,'STRATUM1DB=')
StrAdd(tmptt35,st1db)
newtooltypes[40]:=tmptt35
StrCopy(tmptt36,'STRATUM2DB=')
StrAdd(tmptt36,st2db)
newtooltypes[41]:=tmptt36
*/

if( timeenv ) { newtooltypes[40]=(long) "TIMEENV";} else { newtooltypes[40]=(long) "(TIMEENV)";}

newtooltypes[41]=(long) NULL;

dobj->do_ToolTypes=(char**) newtooltypes;

StringF(tmp3,(char*) locstr[MSG_ERR_WRITING],(long) "facts.info");
if( (BOOL)(0!=IIcon->PutDiskObject(wbarg->wa_Name ,dobj ))==0) {
        tempGlobal_ILIST70 [0]= sizeof( struct EasyStruct);
        tempGlobal_ILIST70 [3]= (long) tmp3;
        tempGlobal_ILIST70 [4]= locstr[MSG_OK];
        IIntuition->EasyRequestArgs((struct Window*) (long) 0 ,(struct EasyStruct*) tempGlobal_ILIST70  ,(ULONG*) (long) 0 ,(APTR) (long) NULL );
}

dobj->do_ToolTypes=(char**) oldtooltypes;
IIcon->FreeDiskObject(dobj );

startttnotify();

}
if( olddir!=-1 ) { IDOS->CurrentDir((BPTR) olddir );}
finally: 0; 
} catch(...) {}
	DisposeString(servfile );
	DisposeString(tmp3 );
	DisposeString(tmptt36 );
	DisposeString(tmptt35 );
	DisposeString(tmptt34 );
	DisposeString(tmptt33 );
	DisposeString(tmptt32 );
	DisposeString(tmptt31 );
	DisposeString(tmptt30 );
	DisposeString(tmptt29 );
	DisposeString(tmptt28 );
	DisposeString(tmptt27 );
	DisposeString(tmptt26 );
	DisposeString(tmptt25 );
	DisposeString(tmptt24 );
	DisposeString(tmptt23 );
	DisposeString(tmptt22 );
	DisposeString(tmptt15 );
	DisposeString(tmptt13 );
	DisposeString(tmptt12 );
	DisposeString(tmptt11 );
	DisposeString(tmptt10 );
	DisposeString(tmptt8 );
	DisposeString(tmptt7 );
	DisposeString(tmptt6 );
	DisposeString(tmptt5 );
	DisposeString(tmptt4 );
	DisposeString(tmptt3 );
	DisposeString(tmptt2 );
	DisposeString(tmptt1 );
	if (exception!=0) {throw eException;} else {EMPTY;};
	exception = temp_QUAD ;
	return ;

}


void menu() {
long defopmen[3]; long i, alsmen=0; long autodstmen[20]; long ignoretzmen=0;
long servmen[5]; long utcomen[15]; long utcplus, utcminus, mode2036men;
long batt2078men, base2015men, appmenumen, halfmen, autodquietmen, custommen, stgemulmen;
long chkstartmen, savesetmen=NM_ITEMDISABLED, anincmen, longtzmen;
long trynextmen, nextservmen, showtimemen, obsessmen, clkdaemonmen, ddbmen=0;
long timeenvmen;

if( wbstart==1 ) { savesetmen=0;}

for( i=0 ; i<=(long) 19; i++) {
autodstmen[i]=CHECKIT;
}
custommen=CHECKIT;

if( autodst!=-1) {
    autodstmen[autodst]=CHECKIT | CHECKED;
} else {
    custommen=CHECKIT | CHECKED;
}

if( aninc==0 ) { anincmen=MENUTOGGLE | CHECKIT  ;} else { anincmen=CHECKIT | MENUTOGGLE | CHECKED;}

if( obsession==0 ) { obsessmen=MENUTOGGLE | CHECKIT  ;} else { obsessmen=CHECKIT | MENUTOGGLE | CHECKED;}
if( timeenv==0 ) { timeenvmen=MENUTOGGLE | CHECKIT  ;} else { timeenvmen=CHECKIT | MENUTOGGLE | CHECKED;}

if( nextservice==0 ) { nextservmen=MENUTOGGLE | CHECKIT  ;} else { nextservmen=CHECKIT | MENUTOGGLE | CHECKED;}

if( trynext==0 ) { trynextmen=MENUTOGGLE | CHECKIT  ;} else { trynextmen=CHECKIT | MENUTOGGLE | CHECKED;}
// trynextmen:=(trynextmen OR NM_ITEMDISABLED)

if( ignoretz ) { ignoretzmen=CHECKIT | MENUTOGGLE | CHECKED  ;} else { ignoretzmen=CHECKIT | MENUTOGGLE;}

// IF alsbase=NIL THEN
alsmen=NM_ITEMDISABLED;

if( defuse ) { defopmen[1]=CHECKIT | MENUTOGGLE | CHECKED ;} else { defopmen[1]=CHECKIT | MENUTOGGLE;}
if( defsave ) { defopmen[2]=CHECKIT | MENUTOGGLE | CHECKED ;} else { defopmen[2]=CHECKIT | MENUTOGGLE;}

servmen[1]=CHECKIT;
servmen[2]=CHECKIT;
servmen[3]=CHECKIT;
servmen[4]=CHECKIT;

if( service==13 ) { servmen[2]=CHECKIT | CHECKED;}
if( service==37 ) { servmen[1]=CHECKIT | CHECKED;}
if( service==123 ) { servmen[3]=CHECKIT | CHECKED;}
if( service==372 ) { servmen[4]=CHECKIT | CHECKED;}

if( mode2036 ) { mode2036men=CHECKIT | MENUTOGGLE | CHECKED  ;} else { mode2036men=CHECKIT | MENUTOGGLE;}
if( base2015 ) { base2015men=CHECKIT | MENUTOGGLE | CHECKED  ;} else { base2015men=CHECKIT | MENUTOGGLE;}
if( batt2078 ) { batt2078men=CHECKIT | MENUTOGGLE | CHECKED  ;} else { batt2078men=CHECKIT | MENUTOGGLE;}
if( toolsmenu ) { appmenumen=CHECKIT | MENUTOGGLE | CHECKED  ;} else { appmenumen=CHECKIT | MENUTOGGLE;}
if( extrahalf ) { halfmen=CHECKIT | MENUTOGGLE | CHECKED  ;} else { halfmen=CHECKIT | MENUTOGGLE;}

for( i=0 ; i<=(long) 12; i++) {
utcomen[i]=CHECKIT;
}
utcplus=CHECKIT;
utcminus=CHECKIT;

if( autodquiet ) { autodquietmen=CHECKIT | MENUTOGGLE | CHECKED  ;} else { autodquietmen=CHECKIT | MENUTOGGLE;}
if( clkdaemon ) { clkdaemonmen=CHECKIT | MENUTOGGLE | CHECKED  ;} else { clkdaemonmen=CHECKIT | MENUTOGGLE;}
if( alwaysupdate ) { showtimemen=CHECKIT | MENUTOGGLE | CHECKED  ;} else { showtimemen=CHECKIT | MENUTOGGLE;}
if( stgemul ) { stgemulmen=CHECKIT | MENUTOGGLE | CHECKED  ;} else { stgemulmen=CHECKIT | MENUTOGGLE;}
if( chkstart ) { chkstartmen=CHECKIT | MENUTOGGLE | CHECKED  ;} else { chkstartmen=CHECKIT | MENUTOGGLE;}

utcomen[utcnum]=CHECKIT | CHECKED;

if( utcneg) {
    utcminus=CHECKIT | CHECKED;
} else {
    utcplus=CHECKIT | CHECKED;
}

if( longtz) {
    longtzmen=CHECKIT | MENUTOGGLE | CHECKED;
    autodquietmen=CHECKIT | MENUTOGGLE | CHECKED | NM_ITEMDISABLED;
    ignoretzmen=CHECKIT | MENUTOGGLE | CHECKED | NM_ITEMDISABLED;
} else {
    longtzmen=CHECKIT | MENUTOGGLE;
}

/*
IF StrCmp(st1db,'http://',7) AND StrCmp(st2db,'http://',7)
    ddbmen:=NM_ITEMDISABLED
ENDIF
*/

tempGlobal_ARRAY_OF_newmenu [0].nm_Label = (char*) locstr[MENU_PROJECT];

tempGlobal_ARRAY_OF_newmenu [1].nm_Label = (char*) locstr[MENU_PROJECT_SERVER_INFO];

tempGlobal_ARRAY_OF_newmenu [3].nm_Label = (char*) locstr[MENU_PROJECT_ABOUT];

tempGlobal_ARRAY_OF_newmenu [5].nm_Label = (char*) locstr[MENU_PROJECT_HIDE];

tempGlobal_ARRAY_OF_newmenu [6].nm_Label = (char*) locstr[MENU_PROJECT_QUIT];

tempGlobal_ARRAY_OF_newmenu [7].nm_Label = (char*) locstr[MENU_SETTINGS];

tempGlobal_ARRAY_OF_newmenu [8].nm_Label = (char*) locstr[MENU_SETTINGS_WRITE_TIME_TO];

tempGlobal_ARRAY_OF_newmenu [9].nm_Label = (char*) locstr[MENU_SETTINGS_WRITE_SYSTEM];

tempGlobal_ARRAY_OF_newmenu [9].nm_Flags = (UWORD) defopmen[1];

tempGlobal_ARRAY_OF_newmenu [10].nm_Label = (char*) locstr[MENU_SETTINGS_WRITE_BATTCLOCK];

tempGlobal_ARRAY_OF_newmenu [10].nm_Flags = (UWORD) defopmen[2];

tempGlobal_ARRAY_OF_newmenu [11].nm_Label = (char*) locstr[MENU_SETTINGS_AUTODST];

tempGlobal_ARRAY_OF_newmenu [12].nm_Label = (char*) locstr[MENU_SETTINGS_AUTODST_OFF];

tempGlobal_ARRAY_OF_newmenu [12].nm_Flags = (UWORD) autodstmen[0];

tempGlobal_ARRAY_OF_newmenu [13].nm_Flags = (UWORD) autodstmen[1];

tempGlobal_ARRAY_OF_newmenu [14].nm_Flags = (UWORD) autodstmen[2];

tempGlobal_ARRAY_OF_newmenu [15].nm_Flags = (UWORD) autodstmen[3];

tempGlobal_ARRAY_OF_newmenu [16].nm_Flags = (UWORD) autodstmen[4];

tempGlobal_ARRAY_OF_newmenu [17].nm_Flags = (UWORD) autodstmen[5];

tempGlobal_ARRAY_OF_newmenu [18].nm_Flags = (UWORD) autodstmen[6];

tempGlobal_ARRAY_OF_newmenu [19].nm_Flags = (UWORD) autodstmen[7];

tempGlobal_ARRAY_OF_newmenu [20].nm_Flags = (UWORD) autodstmen[8];

tempGlobal_ARRAY_OF_newmenu [21].nm_Flags = (UWORD) autodstmen[9];

tempGlobal_ARRAY_OF_newmenu [22].nm_Flags = (UWORD) autodstmen[10];

tempGlobal_ARRAY_OF_newmenu [23].nm_Flags = (UWORD) autodstmen[11];

tempGlobal_ARRAY_OF_newmenu [24].nm_Flags = (UWORD) autodstmen[12];

tempGlobal_ARRAY_OF_newmenu [25].nm_Flags = (UWORD) autodstmen[13];

tempGlobal_ARRAY_OF_newmenu [26].nm_Flags = (UWORD) autodstmen[14];

tempGlobal_ARRAY_OF_newmenu [27].nm_Flags = (UWORD) autodstmen[15];

tempGlobal_ARRAY_OF_newmenu [28].nm_Flags = (UWORD) autodstmen[16];

tempGlobal_ARRAY_OF_newmenu [29].nm_Flags = (UWORD) autodstmen[17];

tempGlobal_ARRAY_OF_newmenu [30].nm_Flags = (UWORD) autodstmen[18];

tempGlobal_ARRAY_OF_newmenu [31].nm_Flags = (UWORD) autodstmen[19];

tempGlobal_ARRAY_OF_newmenu [32].nm_Label = (char*) locstr[MENU_SETTINGS_AUTODST_CUSTOM];

tempGlobal_ARRAY_OF_newmenu [32].nm_Flags = (UWORD) custommen;

tempGlobal_ARRAY_OF_newmenu [34].nm_Label = (char*) locstr[MENU_SETTINGS_AUTODST_QUIET];

tempGlobal_ARRAY_OF_newmenu [34].nm_Flags = (UWORD) autodquietmen;

tempGlobal_ARRAY_OF_newmenu [35].nm_Label = (char*) locstr[MENU_SETTINGS_AUTODST_EMULATE];

tempGlobal_ARRAY_OF_newmenu [35].nm_Flags = (UWORD) stgemulmen;

tempGlobal_ARRAY_OF_newmenu [36].nm_Label = (char*) locstr[MENU_SETTINGS_AUTODST_LONGTZ];

tempGlobal_ARRAY_OF_newmenu [36].nm_Flags = (UWORD) longtzmen;

tempGlobal_ARRAY_OF_newmenu [37].nm_Label = (char*) locstr[MENU_SETTINGS_LEAPSEC];

tempGlobal_ARRAY_OF_newmenu [37].nm_Flags = (UWORD) obsessmen;

tempGlobal_ARRAY_OF_newmenu [38].nm_Label = (char*) locstr[MENU_SETTINGS_IGNORETZ];

tempGlobal_ARRAY_OF_newmenu [38].nm_Flags = (UWORD) ignoretzmen;

tempGlobal_ARRAY_OF_newmenu [39].nm_Label = (char*) locstr[MENU_SETTINGS_SERVICE];

tempGlobal_ARRAY_OF_newmenu [40].nm_Flags = (UWORD) servmen[3];

tempGlobal_ARRAY_OF_newmenu [41].nm_Flags = (UWORD) servmen[4];

tempGlobal_ARRAY_OF_newmenu [42].nm_Flags = (UWORD) servmen[1];

tempGlobal_ARRAY_OF_newmenu [43].nm_Flags = (UWORD) servmen[2];

tempGlobal_ARRAY_OF_newmenu [44].nm_Label = (char*) locstr[MENU_SETTINGS_OFFSET];

tempGlobal_ARRAY_OF_newmenu [45].nm_Flags = (UWORD) utcplus;

tempGlobal_ARRAY_OF_newmenu [46].nm_Flags = (UWORD) utcminus;

tempGlobal_ARRAY_OF_newmenu [48].nm_Flags = (UWORD) utcomen[0];

tempGlobal_ARRAY_OF_newmenu [49].nm_Flags = (UWORD) utcomen[1];

tempGlobal_ARRAY_OF_newmenu [50].nm_Flags = (UWORD) utcomen[2];

tempGlobal_ARRAY_OF_newmenu [51].nm_Flags = (UWORD) utcomen[3];

tempGlobal_ARRAY_OF_newmenu [52].nm_Flags = (UWORD) utcomen[4];

tempGlobal_ARRAY_OF_newmenu [53].nm_Flags = (UWORD) utcomen[5];

tempGlobal_ARRAY_OF_newmenu [54].nm_Flags = (UWORD) utcomen[6];

tempGlobal_ARRAY_OF_newmenu [55].nm_Flags = (UWORD) utcomen[7];

tempGlobal_ARRAY_OF_newmenu [56].nm_Flags = (UWORD) utcomen[8];

tempGlobal_ARRAY_OF_newmenu [57].nm_Flags = (UWORD) utcomen[9];

tempGlobal_ARRAY_OF_newmenu [58].nm_Flags = (UWORD) utcomen[10];

tempGlobal_ARRAY_OF_newmenu [59].nm_Flags = (UWORD) utcomen[11];

tempGlobal_ARRAY_OF_newmenu [60].nm_Flags = (UWORD) utcomen[12];

tempGlobal_ARRAY_OF_newmenu [61].nm_Label = (char*) locstr[MENU_SETTINGS_CHECK_ON_STARTUP];

tempGlobal_ARRAY_OF_newmenu [61].nm_Flags = (UWORD) chkstartmen;

tempGlobal_ARRAY_OF_newmenu [62].nm_Label = (char*) locstr[MENU_SETTINGS_TIMEENV];

tempGlobal_ARRAY_OF_newmenu [62].nm_Flags = (UWORD) timeenvmen;

tempGlobal_ARRAY_OF_newmenu [63].nm_Label = (char*) locstr[MENU_SETTINGS_ONERROR];

tempGlobal_ARRAY_OF_newmenu [64].nm_Label = (char*) locstr[MENU_SETTINGS_TRYNEXT];

tempGlobal_ARRAY_OF_newmenu [64].nm_Flags = (UWORD) trynextmen;

tempGlobal_ARRAY_OF_newmenu [66].nm_Label = (char*) locstr[MENU_SETTINGS_ANINC];

tempGlobal_ARRAY_OF_newmenu [66].nm_Flags = (UWORD) anincmen;

tempGlobal_ARRAY_OF_newmenu [67].nm_Label = (char*) locstr[MENU_SETTINGS_NEXTSERVICE];

tempGlobal_ARRAY_OF_newmenu [67].nm_Flags = (UWORD) nextservmen;

tempGlobal_ARRAY_OF_newmenu [68].nm_Label = (char*) locstr[MENU_SETTINGS_COMPATIBILITY];

tempGlobal_ARRAY_OF_newmenu [69].nm_Label = (char*) locstr[MENU_SETTINGS_COMPAT_MODE];

tempGlobal_ARRAY_OF_newmenu [69].nm_Flags = (UWORD) mode2036men;

tempGlobal_ARRAY_OF_newmenu [70].nm_Label = (char*) locstr[MENU_SETTINGS_COMPAT_BASE];

tempGlobal_ARRAY_OF_newmenu [70].nm_Flags = (UWORD) base2015men;

tempGlobal_ARRAY_OF_newmenu [71].nm_Label = (char*) locstr[MENU_SETTINGS_COMPAT_BATTCLOCK];

tempGlobal_ARRAY_OF_newmenu [71].nm_Flags = (UWORD) batt2078men;

tempGlobal_ARRAY_OF_newmenu [73].nm_Label = (char*) locstr[MENU_SETTINGS_COMPAT_CLKDAEMON];

tempGlobal_ARRAY_OF_newmenu [73].nm_Flags = (UWORD) clkdaemonmen;

tempGlobal_ARRAY_OF_newmenu [74].nm_Label = (char*) locstr[MENU_SETTINGS_APPMENU];

tempGlobal_ARRAY_OF_newmenu [74].nm_Flags = (UWORD) appmenumen;

tempGlobal_ARRAY_OF_newmenu [75].nm_Label = (char*) locstr[MENU_SETTINGS_EXTRAHALF];

tempGlobal_ARRAY_OF_newmenu [75].nm_Flags = (UWORD) halfmen;

tempGlobal_ARRAY_OF_newmenu [76].nm_Label = (char*) locstr[MENU_SPECIAL_SHOWTIME];

tempGlobal_ARRAY_OF_newmenu [76].nm_Flags = (UWORD) showtimemen;

tempGlobal_ARRAY_OF_newmenu [78].nm_Label = (char*) locstr[MENU_SETTINGS_SNAPSHOT];

tempGlobal_ARRAY_OF_newmenu [79].nm_Label = (char*) locstr[MENU_SETTINGS_SAVE];

tempGlobal_ARRAY_OF_newmenu [79].nm_Flags = (UWORD) savesetmen;

tempGlobal_ARRAY_OF_newmenu [80].nm_Label = (char*) locstr[MENU_SPECIAL];

tempGlobal_ARRAY_OF_newmenu [81].nm_Label = (char*) locstr[MENU_SPECIAL_RESET];

tempGlobal_ARRAY_OF_newmenu [82].nm_Label = (char*) locstr[MENU_SPECIAL_READ];

tempGlobal_ARRAY_OF_newmenu [84].nm_Label = (char*) locstr[MENU_SPECIAL_BOREDOM];

tempGlobal_ARRAY_OF_newmenu [84].nm_Flags = (UWORD) alsmen;

menustrip=(long) IGadTools->CreateMenusA((struct NewMenu*) tempGlobal_ARRAY_OF_newmenu  ,(struct TagItem*) tempGlobal_ILIST71  );
	return ;

// old tag ... ,GTMN_FRONTPEN,1

}

void about() {
char* abstr=NULL; char* bsdver=NULL;
long temp_QUAD;
try {
	abstr = NewString(2048);
	bsdver = NewString(50);
	temp_QUAD = exception ;
	exception = 0 ;

StringF(abstr,(char*) "FACTS by Chris Young\n\251 2009 Unsatisfactory Software\n\nThe Finest Amiga Clock Time Synchroniser\n\nhttp://www.unsatisfactorysoftware.co.uk\nfacts@unsatisfactorysoftware.co.uk\n\n");
StrAdd(abstr,(char*) "Release: ");
StrAdd(abstr,frel);
StrAdd(abstr,(char*) " (v");
StrAdd(abstr,fver);
StrAdd(abstr,(char*) ".");
StrAdd(abstr,frev);
StrAdd(abstr,(char*) ")");
StrAdd(abstr,(char*) "\nTranslation by: ");
StrAdd(abstr,(char*) locstr[MSG_TRANS_BY]);

StrAdd(abstr,(char*) "\nARexx Port: FACTS");
StrAdd(abstr,(char*) "\nTCP/IP: ");

if( (IdentifyBase=OpenLibrary_exec2((char*) "identify.library",(ULONG) (long) 9))) {
    StrAdd(abstr,IIdentify->IdHardware((ULONG) (long) IDHW_TCPIP ,(struct TagItem*) NULL ));
    StrAdd(abstr,(char*) "\nBattClock: ");
    StrAdd(abstr,IIdentify->IdHardware((ULONG) (long) IDHW_BATTCLOCK ,(struct TagItem*) NULL ));
    CloseLibrary_exec2(IdentifyBase);
    IdentifyBase=(struct Library*) NULL;
} else {
    if( (SocketBase=OpenLibrary_exec2((char*) socketlib,(ULONG) socketver))) {
      StrCopy(bsdver,(char*) SocketBase->lib_IdString,strlen((char*) SocketBase->lib_IdString )-2);
      CloseLibrary_exec2(SocketBase);
      SocketBase=(struct Library*) NULL;
      StrAdd(abstr,bsdver);
    }
}

	// StrAdd(abstr,'/nm: ')
//        StrAdd(abstr,StringF(temp,'/nClock drift: /dµs/s',mdrift))

// StrAdd(abstr,'/n/n')

// (A poor quality attempt at "AtomTime",/nbut on the Amiga)/n/n

    tempGlobal_ILIST72 [0]= sizeof( struct EasyStruct);

    tempGlobal_ILIST72 [3]= (long) abstr;

    tempGlobal_ILIST72 [4]= locstr[MSG_EH];

    IIntuition->EasyRequestArgs((struct Window*) (long) 0 ,(struct EasyStruct*) tempGlobal_ILIST72  ,(ULONG*) (long) 0 ,(APTR) (long) NULL );
} catch(...) {}
	DisposeString(bsdver );
	DisposeString(abstr );
	if (exception!=0) {throw eException;} else {EMPTY;};
	exception = temp_QUAD ;
	return ;
}

void starttint() {
if( (long) IExec->CheckIO((struct IORequest*) tioreq )==(long) 0) {
    IExec->AbortIO((struct IORequest*) tioreq );
    IExec->WaitIO((struct IORequest*) tioreq );
}

if( timeint>0) {
            tioreq->Request.io_Command=(UWORD) (long) TR_ADDREQUEST;
            tioreq->Time.Seconds=(ULONG) timeint;
            tioreq->Time.Microseconds=(ULONG) (long) 0;
            IExec->SendIO((struct IORequest*) tioreq );
}
	return ;
}

void startclkint() {
            clkioreq->Request.io_Command=(UWORD) (long) TR_ADDREQUEST;
            clkioreq->Time.Seconds=(ULONG) (long) 1;
            clkioreq->Time.Microseconds=(ULONG) (long) 0;
            IExec->SendIO((struct IORequest*) clkioreq );
	return ;
}

void stopclkint() {
if( (long) IExec->CheckIO((struct IORequest*) clkioreq )==(long) 0) {
    IExec->AbortIO((struct IORequest*) clkioreq );
    IExec->WaitIO((struct IORequest*) clkioreq );
}
	return ;


}

void startenvint() {
long envint;

envint = writetimeenv();
            envioreq->Request.io_Command=(UWORD) (long) TR_ADDREQUEST;
            envioreq->Time.Seconds=(ULONG) envint;
            envioreq->Time.Microseconds=(ULONG) (long) 0;
            IExec->SendIO((struct IORequest*) envioreq );
	return ;
}

void stopenvint() {
if( (long) IExec->CheckIO((struct IORequest*) envioreq )==(long) 0) {
    IExec->AbortIO((struct IORequest*) envioreq );
    IExec->WaitIO((struct IORequest*) envioreq );
}
	return ;
}

long writetimeenv() {
long ret;
struct ClockData datestring; long fhand;
                    ioreq->Request.io_Command=(UWORD) (long) TR_GETSYSTIME;
                    IExec->DoIO((struct IORequest*) ioreq );
                    curtime=(long) ioreq->Time.Seconds;

IUtility->Amiga2Date((ULONG) curtime ,& datestring );

    if( (fhand=(long) IDOS->Open("ENV:Facts/time" ,MODE_NEWFILE ))) {
    IDOS->FPuts((BPTR) fhand ,StringF(temp,(char*) "%02.2ld:%02.2ld",(long) datestring.hour,(long) datestring.min) );
    (BOOL)(0!=IDOS->Close((BPTR) fhand ));
    }

ret= (long) ((long) (UWORD) (long) 60-(long) datestring.sec);

goto finally;
finally: 0; 
	return ret;
} 

void dodefop(long seconds) {
if( clkdaemon) {
    if( dst ) { servsecs=servsecs+dstoffset;}
}

if( seconds==0 ) { seconds=servsecs;}

setclock(seconds);

if( nocmd==0 ) { IDOS->SystemTagList(cmd ,(struct TagItem*) tempGlobal_ILIST73  );}
	return ;

/*
IF defsave THEN setclock(seconds)
IF defuse THEN setdate(seconds)
*/

}

void dstswitch(long on, long quiet) {
long res=0; char* rest=NULL; long ttimeint=0;
long temp_QUAD;
try {
	rest = NewString(100);
	temp_QUAD = exception ;
	exception = 0 ;

if( quiet==2 ) { quiet=autodquiet;}

if( enabled==0 ) { goto finally;}

StringF(rest,(char*) "%s|%s",locstr[MSG_DST_SYNCNOW],locstr[MSG_OK]);

if( dst) {
    if( on==0) {
        dst=0;
        if( clkdaemon==0) {
          tz=tz+nicedst;
/*
           ioreq.io.command:=TR_GETSYSTIME
            DoIO(ioreq)
            dodefop(ioreq.time.secs-3600)
*/
			adjustclock(-3600);
        if( quiet==0) {
            tempGlobal_ILIST74 [0]= sizeof( struct EasyStruct);
            tempGlobal_ILIST74 [3]= locstr[MSG_DST_OUT];
            tempGlobal_ILIST74 [4]= (long) rest;
            res=IIntuition->EasyRequestArgs((struct Window*) (long) 0 ,(struct EasyStruct*) tempGlobal_ILIST74  ,(ULONG*) (long) 0 ,(APTR) (long) NULL );
            if( res==1) {
				forcesync=1;
			}
        }

        }
            savetz();

    }
} else {
    if( on) {
        dst=1;
        if( clkdaemon==0) {
          tz=tz-nicedst;
        
/*
            ioreq.io.command:=TR_GETSYSTIME
            DoIO(ioreq)
        
            dodefop(ioreq.time.secs+3600)
*/
			adjustclock(3600);
        if( quiet==0) {
            tempGlobal_ILIST75 [0]= sizeof( struct EasyStruct);
            tempGlobal_ILIST75 [3]= locstr[MSG_DST_IN];
            tempGlobal_ILIST75 [4]= (long) rest;
            res=IIntuition->EasyRequestArgs((struct Window*) (long) 0 ,(struct EasyStruct*) tempGlobal_ILIST75  ,(ULONG*) (long) 0 ,(APTR) (long) NULL );
            if( res==1) {
				forcesync=1;
			}

        }

        }
            savetz();

    }
}

             tzsecs=tz*3600;
             if( winopen) {
                 tempGlobal_ILIST76 [1]= dst;
                 IGadTools->GT_SetGadgetAttrsA((struct Gadget*) gads[GADG_DST] ,win ,(struct Requester*) NULL ,(struct TagItem*) tempGlobal_ILIST76  );
                 tempGlobal_ILIST77 [1]= dst-1;
                 IGadTools->GT_SetGadgetAttrsA((struct Gadget*) gads[GADG_DSTNAME] ,win ,(struct Requester*) NULL ,(struct TagItem*) tempGlobal_ILIST77  );
                    tempGlobal_ILIST78 [1]= -tz;
                    IGadTools->GT_SetGadgetAttrsA((struct Gadget*) gads[GADG_TZ] ,win ,(struct Requester*) NULL ,(struct TagItem*) tempGlobal_ILIST78  );
        }
finally: 0; 
} catch(...) {}
	DisposeString(rest );
	if (exception!=0) {throw eException;} else {EMPTY;};
	exception = temp_QUAD ;
	return ;

}

void setupnotify() {
notifyport=IExec->CreateMsgPort();
tznotify.nr_Name=(char*) "ENV:TZ";
tznotify.nr_Flags=(ULONG) (long) NRF_SEND_MESSAGE;
tznotify.nr_stuff.nr_Msg.nr_Port=notifyport;
snot=(BOOL)(0!=IDOS->StartNotify(& tznotify ));
	return ;
}

void startttnotify() {
StrCopy(tmp,wbarg->wa_Name);
StrAdd(tmp,(char*) ".info");
stopttnotify();
ttnotifyport=IExec->CreateMsgPort();
ttnotify.nr_Name=tmp;
ttnotify.nr_Flags=(ULONG) (long) NRF_SEND_MESSAGE;
ttnotify.nr_stuff.nr_Msg.nr_Port=ttnotifyport;
ttsnot=(BOOL)(0!=IDOS->StartNotify(& ttnotify ));
	return ;
// WriteF('/d /s /s/n',ttsnot,wbarg.name,tmp)
}


void adjustclock(long addon) {
if( ignoretz==0) {
    ioreq->Request.io_Command=(UWORD) (long) TR_GETSYSTIME;
    IExec->DoIO((struct IORequest*) ioreq );
    dodefop((long) ioreq->Time.Seconds+addon);
}
	return ;
}

void closetcp() {

if( sock!=-1) {
    ISocket->CloseSocket(sock );
    sock=-1;
}

/*
IF tcp
    Close(tcp)
    tcp:=NIL
ENDIF
*/

if( SocketBase) {
    CloseLibrary_exec2(SocketBase);
    SocketBase=(struct Library*) NULL;
}
	return ;

}

void gettimeport() {
long dess, desf, ntps, ntpf, rtrips, rtripf, ntpfrac;
char* tmp3=NULL;
long temp_QUAD;
try {
	tmp3 = NewString(50);
	temp_QUAD = exception ;
	exception = 0 ;
closetcp();
// WriteF('/d: ',Long(buffer))

if( service==123) {
	ITimer->GetSysTime(measure );
	curtime=(long) measure->Seconds;
	curmics=(long) measure->Microseconds;
    /* ioreq.io.command:=TR_GETSYSTIME
    DoIO(ioreq) */
    dess=(long) measure->Seconds+(long) 0x92B6BD80-tzsecs;
    desf=(( 12 >= 0 )? (long) measure->Microseconds << 12 : (long) measure->Microseconds >> labs(12 ) )/ 1000000;

    // local clock offset 
    difference = (pkt.recs-pkt.orgs+(pkt.xmts-dess))/2;
    // roundtrip delay
    rtrips = dess-pkt.orgs  - (pkt.recs-pkt.xmts);
    rtripf = desf-pkt.orgf  - (pkt.recf-pkt.xmtf);

    servsecs=pkt.xmts-(long) 0x92B6BD80+rtrips;
servmicros=(( 12 >= 0 )? (pkt.xmtf * 1000000) >> 12 : (pkt.xmtf * 1000000) << labs(12 ))+rtripf+500000;

// WriteF('/d/nrtripf/d/n',servmicros,rtripf)
if( servmicros>999999) {
    servmicros=servmicros-1000000;
    servsecs=servsecs+1;
}

if( servmicros<0) {
    servmicros=0;
}

// servmicros:=Mul(Shr(pkt.xmtf,16),1000000)

// servmicros:=!Mul(Div(1.0,Fpow(2.0,32.0)),ntpfrac)!

// servmicros:=!Mul(
/*
servmicros:=!Fpow(31.0,2.0)
servmicros:=!Div(1.0,servmicros)
*/

// ,ntpfrac)

/*
RealF(tmp,pkt.xmtf!,8)
StrAdd(temp,tmp)
ntpfrac:=RealVal(temp)
servmicros:=Mul(ntpfrac,1000000.0)
servmicros:=Shr(servmicros,32)
*/
// servmicros:=!Div(1.0,Mul(ntpfrac,1000000.0))!
// servmicros:=Mul(Div(1,2^32),pkt.xmtf)

//    servmicros:=((pkt.xmtf+rtripf)/4295)+500000 /****** WAS: (pkt.xmtf+rtripf)/4295 *******/


/*
RealF(temp,servmicros,4)

WriteF('\s\n\d\n',temp,servmicros)
*/

} else {
servsecs=(*(long*) buffer )-(long) 0x92B6BD80 /* num secs from  1 Jan 1900 00:00:00
                                  **            to 31 Dec 1977 23:59:59
                                  */;
}

if( mode2036 ) { servsecs=servsecs+1833517695;}

/*
ioreq.io.command:=TR_GETSYSTIME
DoIO(ioreq)
curtime:=ioreq.time.secs
response:=(ioreq.time.secs-measure.secs)/2
*/

response=0;

/********* UTC=0 section! ******
IF utc=0
    servsecs:=servsecs
    StrCopy(ntptime,sectostr(servsecs))
    IF winopen THEN Gt_SetGadgetAttrsA(gads[GADG_SERVER],win,NIL,[GTTX_TEXT,ntptime,NIL])
ENDIF
********** END UTC=0 ********/

if( utc==2 ) { response=response-utcoffset;}

servsecs=servsecs-tzsecs+response;
if( extrahalf) {
    if( tzsecs<0 ) { servsecs=servsecs+1800 ;} else { servsecs=servsecs-1800;}
}

if( winopen ) { IIntuition->SetWindowPointerA(win ,(struct TagItem*) tempGlobal_ILIST79  );}

lastsyncok=1;
} catch(...) {}
	DisposeString(tmp3 );
	if (exception!=0) {throw eException;} else {EMPTY;};
	exception = temp_QUAD ;
	return ;

}

void sntpinfo() {
char* info=NULL; long x=0, fhand=NULL; char* reqtmp1=NULL;
long temp_QUAD;
try {
	info = NewString(2048);
	reqtmp1 = NewString(200);
	temp_QUAD = exception ;
	exception = 0 ;

if( lastsyncok ) {//EstrLen(ipaddr)>2
    StringF(info,(char*) "%s %s",locstr[MSG_INFO_IP],(long) ipaddr);

	if( lastsntp==1) {

		StringF(temp,(char*) "\n%s %ld",locstr[MSG_INFO_STRATUM],pkt.stratum);
		StrAdd(info,temp);

		if( pkt.stratum>1) {
    		StringF(temp,(char*) "\n%s %ld.%ld.%ld.%ld",locstr[MSG_INFO_REFERENCE],pkt.refid[0],pkt.refid[1],pkt.refid[2],pkt.refid[3]);
    		StrAdd(info,temp);
		} else {
    		StringF(temp,(char*) "\n%s %s",locstr[MSG_INFO_REFERENCE],(long) StrCopy(temp,pkt.refid,4));
	    	StrAdd(info,temp);
		}

		StringF(temp,(char*) "\n%s %ld",locstr[MSG_INFO_POLL],pkt.poll);
		StrAdd(info,temp);

		StringF(temp,(char*) "\n%s %ld",locstr[MSG_INFO_PRECISION],pkt.precision-256);
		StrAdd(info,temp);

		StringF(temp,(char*) "\n%s %ld.%ld",locstr[MSG_INFO_ROOT_DELAY],pkt.rootdelay,pkt.rootdelayf);
		StrAdd(info,temp);

		StringF(temp,(char*) "\n%s %ld.%ld",locstr[MSG_INFO_ROOT_DISP],pkt.rootdisp,pkt.rootdispf);
		StrAdd(info,temp);

		if( leapsec==1 ) { StrAdd(info,(char*) "\n\n%s",locstr[MSG_INFO_LEAP_ADDED]) ;}// Leap second will be added 00:00')
		if( leapsec==2 ) { StrAdd(info,(char*) "\n\n%s",locstr[MSG_INFO_LEAP_REMOVED]) ;}// Leap second will be removed 00:00')
		if( leapsec==3 ) { StrAdd(info,(char*) "\n\n%s",locstr[MSG_INFO_ALARM]) ;}// ALARM!')

	} else {
    	StrAdd(info,(char*) "\n\n%s",locstr[MSG_INFO_USE_SNTP]) ;//Use SNTP service for more information')
	}

    StringF(temp,(char*) "%s",locstr[MSG_OK]);
   	tempGlobal_ILIST80 [0]= sizeof( struct EasyStruct);
   	tempGlobal_ILIST80 [3]= (long) info;
   	tempGlobal_ILIST80 [4]= (long) temp;
   	x=IIntuition->EasyRequestArgs((struct Window*) (long) 0 ,(struct EasyStruct*) tempGlobal_ILIST80  ,(ULONG*) (long) 0 ,(APTR) (long) NULL );

} else {
	StrCopy(temp,(char*) locstr[MSG_INFO_NOINFO]);
	tempGlobal_ILIST81 [0]= sizeof( struct EasyStruct);
	tempGlobal_ILIST81 [3]= (long) temp;
	IIntuition->EasyRequestArgs((struct Window*) (long) 0 ,(struct EasyStruct*) tempGlobal_ILIST81  ,(ULONG*) (long) 0 ,(APTR) (long) NULL );
}
} catch(...) {}
	DisposeString(reqtmp1 );
	DisposeString(info );
	if (exception!=0) {throw eException;} else {EMPTY;};
	exception = temp_QUAD ;
	return ;


/****** info request two ******/

// IF x<>0 THEN RETURN

//**** downloadfiles()

/*

IF checkdataver(2)=1
    downloadfiles(2)
ENDIF

IF fhand:=Open(st2db,MODE_OLDFILE)
    x:=findinfo(fhand)
    IF x THEN StrAdd(temp,locstr[MSG_STRATUMTWO_DATA])
    Close(fhand)
ELSE
StringF(reqtmp1,'\s\n\n\s',locstr[MSG_ERR_STRATUMONE],locstr[MSG_ASK_DOWNLOAD])


    IF EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS',reqtmp1,yesno],0,NIL)
        downloadfiles(2)
        IF fhand:=Open(st2db,MODE_OLDFILE)
            x:=findinfo(fhand)
            IF x THEN StrAdd(temp,locstr[MSG_STRATUMTWO_DATA])
            Close(fhand)
        ENDIF
    ENDIF
ENDIF


IF x=0
    IF checkdataver(1)=1
        downloadfiles(1)
    ENDIF
    IF fhand:=Open(st1db,MODE_OLDFILE)
        x:=findinfo(fhand)
        IF x THEN StrAdd(temp,locstr[MSG_STRATUMONE_DATA])
        Close(fhand)
    ELSE

        StringF(reqtmp1,'\s\n\n\s',locstr[MSG_ERR_STRATUMTWO],locstr[MSG_ASK_DOWNLOAD])

        IF EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS',reqtmp1,yesno],0,NIL)
            downloadfiles(1)
            IF fhand:=Open(st1db,MODE_OLDFILE)
                x:=findinfo(fhand)
                IF x THEN StrAdd(temp,locstr[MSG_STRATUMONE_DATA])
                Close(fhand)
            ENDIF
        ENDIF
    ENDIF
ENDIF

IF x=0 THEN StrCopy(temp,locstr[MSG_INFO_NOINFO])

EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS',temp,'OK'],0,NIL)
*/
}


/*
PROC findinfo(fhand)
DEF x,info[2048]:STRING,done=0,lastupd[50]:STRING,br,obr

    WHILE (x:=Fgets(fhand,buffer,100))<>0
->    EXIT x=-1
->    StrCopy(temp,buffer,x)
    IF StrCmp(buffer,'Last-Modified:',14)
        MidStr(lastupd,buffer,0,StrLen(buffer)-2)
    ENDIF
->    IF StrCmp(MidStr(tmp,buffer,11,EstrLen(server)),server)
    IF (StrCmp(buffer,'<p>',3)) OR (StrCmp(buffer,'<P>',3))
         IF InStr(buffer,server)<>-1
            br:=InStr(buffer,'<br>')
            IF br<>-1
                MidStr(info,buffer,3,br-3)
                StrAdd(info,'\n')
              -> StrAdd(info,MidStr(temp,buffer,br+4,StrLen(buffer)-(br+4)))
            ELSE
                MidStr(info,buffer,0,StrLen(buffer)-2)
            ENDIF
            done:=1
            WHILE (x:=Fgets(fhand,buffer,100))<>0
                EXIT StrLen(buffer)<=2
                EXIT StrCmp(buffer,'<p>',3)

            br:=InStr(buffer,'<br>')
            IF br<>-1
                StrAdd(info,MidStr(temp,buffer,0,br))
                StrAdd(info,'\n')
              -> StrAdd(info,MidStr(temp,buffer,br+4,StrLen(buffer)-(br+4)))
            ELSE
                IF (br:=InStr(buffer,'</p>'))
              StrAdd(info,MidStr(temp,buffer,0,br))
                -> done:=2
                ELSE
              StrAdd(info,MidStr(temp,buffer,0,StrLen(buffer)-2))
                ENDIF
            ENDIF
            -> EXIT done=2
            ENDWHILE
        ENDIF
    ENDIF
    EXIT done>=1
ENDWHILE
IF done>=1
            StrCopy(temp,info)
            StrAdd(temp,'\n\n')
            StrAdd(temp,lastupd)
            StrAdd(temp,'\n\n')
ENDIF
ENDPROC done
*/

/*
PROC getdataver(stratum)
DEF fhand=NIL,ver[50]:STRING,done=0

IF stratum=1 THEN StrCopy(tmp,st1db) ELSE StrCopy(tmp,st2db)

IF StrCmp(tmp,'http://',7)
    StrCopy(ver,'On-line')
    RETURN ver
ENDIF

IF fhand:=Open(tmp,MODE_OLDFILE)
        WHILE (Fgets(fhand,buffer,100))<>0
            IF StrCmp(buffer,'Last-Modified:',14)
                MidStr(ver,buffer,15,StrLen(buffer)-17)
                done:=1
            ENDIF
            EXIT done=1
        ENDWHILE
        Close(fhand)
ENDIF

ENDPROC ver

PROC getpage(sock,stratum)
DEF ret:LONG
DEF fhand=NIL,x,tmp2[50]:STRING

**
RightStr(tmp2,tmp,10)

StrCopy(tmp,'PROGDIR:')
StrAdd(tmp,tmp2)
**

IF stratum<>2
    StrCopy(tmp,st1db)
ELSE
    StrCopy(tmp,st2db)
ENDIF

IF fhand:=Open(tmp,MODE_OLDFILE)
        WHILE (Fgets(fhand,buffer,100))<>0
            IF StrCmp(buffer,'Date:',5) -> was Last-Modified:
                StrAdd(temp,'If-Modified-Since: ')
                StrAdd(temp,MidStr(tmp2,buffer,6))
            ENDIF
        ENDWHILE
        Close(fhand)
ENDIF

StrAdd(temp,'\b\n')

Send(sock,temp,EstrLen(temp),0)

-> WriteF('\s\n',temp)

-> Send(sd, "GET ", 4, 0) != 4) || (send(sd, u, i, 0) != i)) {
-> 'www.eecis.udel.edu','www.eecis.udel.edu',

RightStr(tmp2,tmp,10)

StringF(ntptime,'\s ',locstr[MSG_RECEIVING])
StrAdd(ntptime,tmp2)
StrAdd(ntptime,'...')

IF winopen THEN Gt_SetGadgetAttrsA(gads[GADG_SERVER],win,NIL,[GTTX_TEXT,ntptime,NIL])

-> WriteF('\s\n',tmp)

x:=Recv(sock,buffer,100,0)

-> WriteF('[getp]buff: \s\n',buffer)

IF StrCmp(MidStr(temp,buffer,9,3),'200',3)=0
    IF StrCmp(MidStr(temp,buffer,9,3),'304',3)
        StringF(temp,locstr[MSG_ERR_STRATUM_LATEST],MidStr(tmp,tmp2,5,1))
    ELSE
        x:=InStr(buffer,'\b\n')
        StringF(temp,locstr[MSG_ERR_STRATUM_OTHER],MidStr(tmp,tmp2,5,1))
        StrAdd(temp,'\n\n\s',MidStr(tmp,buffer,9,x))
    ENDIF

    IF EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS',temp,locstr[MSG_CANCEL]],0,NIL)=0
        -> 304 = 'Server document is same version as local copy'
        StrCopy(ntptime,' ')
          IF winopen THEN Gt_SetGadgetAttrsA(gads[GADG_SERVER],win,NIL,[GTTX_TEXT,ntptime,NIL])
        closetcp()
        RETURN -1
    ENDIF
ENDIF

IF fhand:=Open(tmp,MODE_NEWFILE) -> temp

Write(fhand,buffer,x)

WHILE (x:=Recv(sock,buffer,100,0))>0
    Write(fhand,buffer,x)
ENDWHILE

Close(fhand)
ENDIF

closetcp()

-> FreeMem(bigbuffer,2048)

StrCopy(ntptime,locstr[MSG_RECEIVED])
IF winopen THEN Gt_SetGadgetAttrsA(gads[GADG_SERVER],win,NIL,[GTTX_TEXT,ntptime,NIL])
ENDPROC ret

PROC checkdataver(strat)
DEF ret:LONG
DEF sock,fhand,mod=1,tmp2[50]:STRING,file[512]:STRING

IF strat=1 THEN StrCopy(file,st1db) ELSE StrCopy(file,st2db)

IF StrCmp(file,'http://',7) THEN RETURN 0

StrCopy(tmp,'http://www.eecis.udel.edu/~mills/ntp/')
IF strat=1 THEN StrAdd(tmp,'clock1.htm') ELSE StrAdd(tmp,'clock2.htm')
-> StrAdd(tmp,file)

IF (sock:=httpconnect('www.eecis.udel.edu',1,1))>-1
-> ****** ,1,1  ^^^

**
    StrCopy(ntptime,'Checking Header...')
    IF winopen THEN Gt_SetGadgetAttrsA(gads[GADG_SERVER],win,NIL,[GTTX_TEXT,ntptime,NIL])
**

-> StrCopy(tmp,'PROGDIR:')
StrCopy(tmp,file)

IF fhand:=Open(tmp,MODE_OLDFILE)
        WHILE (Fgets(fhand,buffer,100))<>0
            IF StrCmp(buffer,'Date:',5) -> was: Last-Modified:
                StrAdd(temp,'If-Modified-Since: ')
                StrAdd(temp,MidStr(tmp2,buffer,6))
            ENDIF
        ENDWHILE
        Close(fhand)
ENDIF

StrAdd(temp,'\b\n')

    Send(sock,temp,EstrLen(temp),0)

Recv(sock,buffer,100,0)

closetcp()

**
    StrCopy(ntptime,' ')
    IF winopen THEN Gt_SetGadgetAttrsA(gads[GADG_SERVER],win,NIL,[GTTX_TEXT,ntptime,NIL])
**

IF StrCmp(MidStr(temp,buffer,9,3),'200',3)
    IF EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS',locstr[MSG_ASK_DOWNLOAD_NEWER],yesno],0,NIL)
        RETURN 1
    ENDIF
ENDIF
ENDIF

-> WriteF('[data]buff: \s\nchkstr: \s\n',buffer,temp)

ENDPROC ret
*/

/*
PROC getmotd()
DEF sock,fhand,motd[500]:STRING,x

StrCopy(motd,locstr[MSG_MOTD_NOT_AVAILABLE])

StrCopy(tmp,'http://www.unsatisfactory.freeserve.co.uk/MOTD')
IF (sock:=httpconnect('www.unsatisfactory.freeserve.co.uk',0,0))>-1
    StrAdd(temp,'\b\n')
    Send(sock,temp,EstrLen(temp),0)

    IF fhand:=Open('T:unsat.motd',MODE_NEWFILE)

        WHILE (x:=Recv(sock,buffer,100,0))>0
            Write(fhand,buffer,x)
        ENDWHILE

        Close(fhand)
    ENDIF

closetcp()

    IF fhand:=Open('T:unsat.motd',MODE_OLDFILE)
        WHILE (Fgets(fhand,buffer,100))<>0
            IF StrCmp(buffer,'Last-Modified:',14)
                StrCopy(motd,'MOTD [')
                MidStr(tmp,buffer,15,StrLen(buffer)-17)
                StrAdd(motd,tmp)
                StrAdd(motd,']\n\n')
            ENDIF
            IF StrCmp(buffer,'MOTD: ',6)
                StrAdd(motd,MidStr(tmp,buffer,6,StrLen(buffer)-2))
**
            ELSE
                StrAdd(motd,buffer,StrLen(buffer)-2)
**
            ENDIF
        ENDWHILE
        Close(fhand)
    ENDIF

DeleteFile('T:unsat.motd')

ENDIF

ENDPROC motd

PROC checkfactsver()
DEF sock,fhand,x,cverstr[1024]:STRING,chkd=0,olddir=-1

StrCopy(cverstr,locstr[MSG_NEWER_NOT_AVAILABLE])

StrCopy(tmp,'http://www.aminet.org/aminet/comm/tcp/facts.readme')
IF (sock:=httpconnect('www.aminet.org'))>-1
    StrAdd(temp,'\b\n')
    Send(sock,temp,EstrLen(temp),0)

    StrCopy(ntptime,locstr[MSG_VERSION_CHECK])
    IF winopen THEN Gt_SetGadgetAttrsA(gads[GADG_SERVER],win,NIL,[GTTX_TEXT,ntptime,NIL])

    IF fhand:=Open('T:FACTS.VERCHECK',MODE_NEWFILE)

        WHILE (x:=Recv(sock,buffer,100,0))>0
            Write(fhand,buffer,x)
        ENDWHILE

        Close(fhand)
    ENDIF

closetcp()

    IF fhand:=Open('T:FACTS.VERCHECK',MODE_OLDFILE)
        WHILE (Fgets(fhand,buffer,100))<>0
            IF StrCmp(buffer,'Version: ',9)
                IF StrCmp(MidStr(temp,buffer,10),frel)=0 -> ,3
                    StringF(cverstr,'\s\n\nVersion: \s\n\nDownload from Aminet /comm/tcp/facts.lha or\nhttp://www.unsatisfactorysoftware.co.uk/',locstr[MSG_NEWER_AVAILABLE],MidStr(tmp,buffer,10,StrLen(buffer)-11))
                ENDIF
            chkd:=1
            
-> no requester!
-> IF StrCmp(MidStr(temp,buffer,10),'2.2c\n')=0 THEN WriteF('2.2c\n')
-> *** req above

-> WriteF('[ver]buff: \s\ntmp: \s\n',buffer,temp)
            ENDIF
            EXIT chkd=1
        ENDWHILE
        Close(fhand)
    ENDIF

DeleteFile('T:FACTS.VERCHECK')

StrAdd(cverstr,'\n\n')
StrAdd(cverstr,getmotd())

    (EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS',cverstr,'OK'],0,NIL))
**
        IF (sock:=httpconnect('de.aminet.net',0,0,1))>-1

            StrCopy(ntptime,'Downloading via FTP...')
            IF winopen THEN Gt_SetGadgetAttrsA(gads[GADG_SERVER],win,NIL,[GTTX_TEXT,ntptime,NIL])

            WHILE (x:=Recv(sock,buffer,100,0))>0
                -> do nothing :-)
            ENDWHILE

            StrCopy(temp,'USER anonymous\b\n')
            Send(sock,temp,EstrLen(temp),0)
            WHILE (x:=Recv(sock,buffer,100,0))>0
                -> do nothing :-)
            ENDWHILE

            StrCopy(temp,'PASS facts-user@domain.com\b\n')
            Send(sock,temp,EstrLen(temp),0)
            WHILE (x:=Recv(sock,buffer,100,0))>0
                -> do nothing
            ENDWHILE

            StrCopy(temp,'TYPE I\b\n')
            Send(sock,temp,EstrLen(temp),0)
            WHILE (x:=Recv(sock,buffer,100,0))>0
                -> do nothing
            ENDWHILE

            StrCopy(temp,'CWD pub/aminet/comm/tcp\b\n')
            Send(sock,temp,EstrLen(temp),0)
            WHILE (x:=Recv(sock,buffer,100,0))>0
                -> do nothing
            ENDWHILE

            StrCopy(temp,'PASV\b\n')
            Send(sock,temp,EstrLen(temp),0)
            WHILE (x:=Recv(sock,buffer,100,0))>0
                -> do nothing
            ENDWHILE

            StrCopy(temp,'RETR facts.lha\b\n')
            Send(sock,temp,EstrLen(temp),0)
            fhand:=Open('T:facts.lha',MODE_NEWFILE)
            WHILE (x:=Recv(sock,buffer,100,0))>0
                Write(fhand,buffer,x)
            ENDWHILE
            Close(fhand)
            
            StrCopy(temp,'QUIT\b\n')
            Send(sock,temp,EstrLen(temp),0)
            WHILE (x:=Recv(sock,buffer,100,0))>0
                -> do nothing
            ENDWHILE
            
            closetcp()

            StrCopy(ntptime,'Dearchiving FACTS...')
            IF winopen THEN Gt_SetGadgetAttrsA(gads[GADG_SERVER],win,NIL,[GTTX_TEXT,ntptime,NIL])
            
            SystemTagList('C:LhA >NIL: x T:facts.lha T:',[NIL])
            
            StrCopy(ntptime,'Starting Install...')
            IF winopen THEN Gt_SetGadgetAttrsA(gads[GADG_SERVER],win,NIL,[GTTX_TEXT,ntptime,NIL])
            
            IF dirlock:=Lock('T:Facts',ACCESS_READ)
                olddir:=CurrentDir(dirlock)
                SystemTagList('C:Installer >NIL: T:FACTS/Install APPNAME FACTS DEFUSER EXPERT',[SYS_ASYNCH,TRUE,NIL]) -> NP_CURRENTDIR,dirlock,NIL])
                IF olddir<>-1 THEN CurrentDir(olddir)
                UnLock(dirlock)
                dirlock:=0
            ELSE
                EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS','An error occured during\narchive extraction','OK'],0,NIL)
            ENDIF
        ENDIF
    ENDIF
**
    StrCopy(ntptime,' ')
    IF winopen THEN Gt_SetGadgetAttrsA(gads[GADG_SERVER],win,NIL,[GTTX_TEXT,ntptime,NIL])

ELSE
    EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS',locstr[MSG_ERR_CONN_DEFAULT],locstr[MSG_OK]],0,NIL)
ENDIF

ENDPROC


PROC downloadfiles(stratum=0)
DEF s

-> http://www.eecis.udel.edu/~mills/ntp/clock1.htm
-> http://www.eecis.udel.edu/~mills/ntp/clock2.htm

IF stratum<>2
        StrCopy(tmp,'http://www.eecis.udel.edu/~mills/ntp/clock1.htm')
        IF (s:=httpconnect('www.eecis.udel.edu'))=-1
            EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS',locstr[MSG_ERR_DOWNLOADING_STRATUMONE],locstr[MSG_OK]],0,NIL)
        ELSE
            getpage(s,1) -> stratum
        ENDIF
ENDIF

IF stratum<>1
        StrCopy(tmp,'http://www.eecis.udel.edu/~mills/ntp/clock2.htm')
        IF (s:=httpconnect('www.eecis.udel.edu'))=-1
            EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS',locstr[MSG_ERR_DOWNLOADING_STRATUMTWO],locstr[MSG_OK]],0,NIL)
        ELSE
            getpage(s,2) -> stratum
        ENDIF
ENDIF

ENDPROC

PROC httpconnect(hostname,headonly=0,quiet=0,ftpmode=0)
DEF sock,conn,size,portnum=80,useragent[100]:STRING
DEF remote=NIL:PTR TO hostent,addr:sockaddr_in,addrs=NIL:PTR TO sockaddr
DEF url=NIL:PTR TO in_addr,fhand=NIL

IF ftpmode THEN portnum:=21

IF (socketbase:=OpenLibrary(socketlib,socketver))=NIL
IF quiet=0
    IF wbstart
        StrCopy(ntptime,locstr[MSG_GUI_NOTCP])
        IF winopen THEN Gt_SetGadgetAttrsA(gads[GADG_SERVER],win,NIL,[GTTX_TEXT,ntptime,NIL])
    ENDIF
ENDIF
RETURN -1
ENDIF

-> IF winopen THEN SetWindowPointerA(win,[WA_BUSYPOINTER,TRUE,WA_POINTERDELAY,TRUE,NIL])

IF quiet=0
IF wbstart
    StringF(ntptime,'\s...',locstr[MSG_LOOKUP])
    IF winopen THEN Gt_SetGadgetAttrsA(gads[GADG_SERVER],win,NIL,[GTTX_TEXT,ntptime,NIL])
ENDIF
ENDIF

remote := Gethostbyname(hostname)

-> http://www.eecis.udel.edu/~mills/ntp/clock1.htm
-> http://www.eecis.udel.edu/~mills/ntp/clock2.htm

IF remote=0
IF quiet=0
    conn:=Errno()
    SELECT conn
        CASE HOST_NOT_FOUND
        StrCopy(ntptime,locstr[MSG_DNS_HOST_NOT_FOUND])
        CASE TRY_AGAIN
        StrCopy(ntptime,locstr[MSG_DNS_TRY_AGAIN])
        CASE NO_RECOVERY
        StrCopy(ntptime,locstr[MSG_DNS_NO_RECOVERY])
        CASE NO_DATA
        StrCopy(ntptime,locstr[MSG_DNS_NO_DATA])
        DEFAULT
        StrCopy(ntptime,locstr[MSG_DNS_DEFAULT])
    ENDSELECT

IF winopen
    Gt_SetGadgetAttrsA(gads[GADG_SERVER],win,NIL,[GTTX_TEXT,ntptime,NIL])
ENDIF
ENDIF
closetcp()
RETURN -1

ELSE

url:=addr.addr

url:=remote.addr_list[0] ->[0] in_addr   Inet_addr(remote. etc)

CopyMem(url,addr.addr,SIZEOF in_addr)

ENDIF

  addr.port := portnum -> 80 -> port -> Htons(port)
  addr.family := AF_INET
  addr.len:=SIZEOF sockaddr_in

    sock:=Socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)

-> Setsockopt(sock,SOL_SOCKET,SO_KEEPALIVE,1,1)
->  setsockopt(sd, SOL_SOCKET, SO_KEEPALIVE, &ONE, sizeof ONE);

addrs:=addr

IF wbstart
IF quiet=0
StrCopy(ntptime,locstr[MSG_GUI_CONNECT])
StrAdd(ntptime,Inet_NtoA(addr.addr.addr))
IF winopen THEN Gt_SetGadgetAttrsA(gads[GADG_SERVER],win,NIL,[GTTX_TEXT,ntptime,NIL])
ENDIF
ENDIF

conn:=Connect(sock,addrs,SIZEOF sockaddr_in)
IF conn=-1
    IF quiet=0
        connecterror()
    ENDIF
    closetcp()
    RETURN -1
ENDIF

-> IF fhand:=Open('T:FACTS.TEST',MODE_OLDFILE)

IF quiet=0
StrCopy(ntptime,locstr[MSG_SENDING_REQUEST])
IF winopen THEN Gt_SetGadgetAttrsA(gads[GADG_SERVER],win,NIL,[GTTX_TEXT,ntptime,NIL])
ENDIF

IF ftpmode THEN RETURN sock

IF headonly
    StrCopy(temp,'HEAD ')
ELSE
    StrCopy(temp,'GET ')
ENDIF
StrAdd(temp,tmp)
StrAdd(temp,' HTTP/1.1\b\nHost: ')
StrAdd(temp,hostname)

IF identifybase:=OpenLibrary('identify.library',9)
StringF(useragent,'\b\nUser-Agent: FACTS/\s.\s [\s] bsdsocket/\d.\d (AmigaOS \s; \s)\b\n',fver,frev,lang,socketbase.version,socketbase.revision,IdHardware(IDHW_OSNR,NIL),IdHardware(IDHW_TCPIP,NIL))
StrAdd(temp,useragent)
    CloseLibrary(identifybase)
    identifybase:=NIL
ELSE
StringF(useragent,'\b\nUser-Agent: FACTS/\s.\s [\s] bsdsocket/\d.\d (AmigaOS)\b\n',fver,frev,lang,socketbase.version,socketbase.revision)
StrAdd(temp,useragent)
ENDIF
ENDPROC sock


*/

void startappmenu() {
  appitem=(long) IWorkbench->AddAppMenuItemA((ULONG) (long) 0 ,(ULONG) (long) 0 ,(char*) locstr[APP_MENU] ,appmenu ,(struct TagItem*) NULL );
	return ;

}

void removeappitem() {
  if( appitem ) { -IWorkbench->RemoveAppMenuItem((struct AppMenuItem*) appitem );}
/*
  IF appmenu
    -> Clear away any messages that arrived at the last moment
    -> and let Workbench know we're done with the messages
    WHILE appmsg:=GetMsg(appmenu) DO ReplyMsg(appmsg)
    DeleteMsgPort(appmenu)
  ENDIF
appmenu:=NIL
*/

appitem=(long) NULL;
	return ;
}

void dstpresets(long autodst2) {
if( autodst2==0 ) { goto finally;}

switch( autodst2) {
    case -1 :// Custom
    startmonth=ostartmonth;
    startwday=ostartwday;
    starthour=ostarthour;
    startweek=ostartweek;
    endmonth=oendmonth;
    endwday=oendwday;
    endhour=oendhour;
    endweek=oendweek;
    hemisphere=ohemisphere;
    	break;

    case 1 :// Australia (Sydney/Melbourne)
    startmonth=10;
    startwday=0;
    starthour=2;
    startweek=0;
    endmonth=3;
    endwday=0;
    endhour=2;
    endweek=0;
    hemisphere=1;

    	break;

    case 2 :// Australia (Tasmania/NSW)
    startmonth=10;
    startwday=0;
    starthour=2;
    startweek=1;
    endmonth=3;
    endwday=0;
    endhour=2;
    endweek=0;
    hemisphere=1;

    	break;

    case 3 :// Azerbaijan
    startmonth=3;
    startwday=0;
    starthour=1;
    startweek=0;
    endmonth=10;
    endwday=0;
    endhour=1;
    endweek=0;
    hemisphere=0;

    	break;

    case 4 :// Brazil
    startmonth=10;
    startwday=0;
    starthour=0;
    startweek=2;
    endmonth=2;
    endwday=0;
    endhour=0;
    endweek=3;
    hemisphere=1;

    	break;

    case 5 :// Thule/Newfoundland/Mexico/Bahamas
    startmonth=4;
    startwday=0;
    starthour=2;
    startweek=1;
    endmonth=10;
    endwday=0;
    endhour=2;
    endweek=0;
    hemisphere=0;

    	break;

    case 6 :// Chile
    startmonth=10;
    startwday=0;
    starthour=0;
    startweek=2;
    endmonth=3;
    endwday=0;
    endhour=0;
    endweek=2;
    hemisphere=1;

    	break;

    case 7 :// Cuba/Turks/Caicos
    startmonth=4;
    startwday=0;
    starthour=0;
    startweek=1;
    endmonth=10;
    endwday=0;
    endhour=0;
    endweek=0;
    hemisphere=0;

    	break;

    case 8 :// Egypt
    startmonth=4;
    startwday=6;
    starthour=0;
    startweek=4;
    endmonth=9;
    endwday=5;
    endhour=23;
    endweek=0;
    hemisphere=0;

    	break;

    case 9 :// Europe (Central)
    startmonth=3;
    startwday=0;
    starthour=2;
    startweek=0;
    endmonth=10;
    endwday=0;
    endhour=3;
    endweek=0;
    hemisphere=0;

    	break;

    case 10 :// Falklands
    startmonth=9;
    startwday=0;
    starthour=0;
    startweek=2;
    endmonth=4;
    endwday=0;
    endhour=0;
    endweek=3;
    hemisphere=1;

    	break;

    case 11 :// Fiji
    startmonth=11;
    startwday=0;
    starthour=2;
    startweek=1;
    endmonth=2;
    endwday=0;
    endhour=3;
    endweek=0;
    hemisphere=1;

    	break;

    case 12 :// Jordan
    startmonth=4;
    startwday=6;
    starthour=0;
    startweek=1;
    endmonth=9;
    endwday=6;
    endhour=0;
    endweek=3;
    hemisphere=0;

    	break;

    case 13 :// Lebanon/Mongol
    startmonth=3;
    startwday=0;
    starthour=0;
    startweek=0;
    endmonth=9;
    endwday=0;
    endhour=0;
    endweek=0;
    hemisphere=0;

    	break;

    case 14 :// NZ
    startmonth=10;
    startwday=0;
    starthour=2;
    startweek=1;
    endmonth=3;
    endwday=0;
    endhour=2;
    endweek=3;
    hemisphere=1;

    	break;

    case 15 :// Paraguay
    startmonth=10;
    startwday=0;
    starthour=0;
    startweek=1;
    endmonth=2;
    endwday=0;
    endhour=0;
    endweek=0;
    hemisphere=1;

    	break;

    case 16 :// RussiaAsia  Kirgizstan
    startmonth=3;
    startwday=0;
    starthour=2;
    startweek=0;
    endmonth=10;
    endwday=0;
    endhour=2;
    endweek=0;
    hemisphere=0;

    	break;

    case 17 :// UK   Europe (West)
    startmonth=3;
    startwday=0;
    starthour=1;
    startweek=0;
    endmonth=10;
    endwday=0;
    endhour=2;
    endweek=0;
    hemisphere=0;

    	break;

    case 18 :// United States & Canada
    startmonth=3;
    startwday=0;
    starthour=2;
    startweek=2;
    endmonth=11;
    endwday=0;
    endhour=2;
    endweek=1;
    hemisphere=0;

    	break;

    case 19 :// Zion
    startmonth=4;
    startwday=6;
    starthour=2;
    startweek=1;
    endmonth=9;
    endwday=6;
    endhour=2;
    endweek=1;
    hemisphere=0;

    	break;

}

startdst(chkautodst_generic());
finally: 0; 
	return ;
}

void startdst(long startval) {

if( (long) IExec->CheckIO((struct IORequest*) dstioreq )==(long) 0) {
    IExec->AbortIO((struct IORequest*) dstioreq );
    IExec->WaitIO((struct IORequest*) dstioreq );
}

// WriteF('/d/n',autodst)

// IF longtz=0

if( autodst!=0) {

// WriteF('/d/n',startval)

            dstioreq->Request.io_Command=(UWORD) (long) TR_ADDREQUEST;
            dstioreq->Time.Seconds=(ULONG) startval;
            dstioreq->Time.Microseconds=(ULONG) (long) 0;
            IExec->SendIO((struct IORequest*) dstioreq );
}
// ENDIF

if( (- (leapsec==1 )| leapsec)==2) { startleapsec();}
	return ;

}

void verifyportnumbers() {
struct servent* servie=NULL; // ,protoe:PTR TO protoent

if( (void*) (SocketBase=OpenLibrary_exec2((char*) socketlib,(ULONG) socketver))==NULL ) { goto finally;}

servie=ISocket->getservbyname("daytime" ,"tcp" );
if( (void*) servie!=NULL ) { defdaytimeport=servie->s_port;}

servie=ISocket->getservbyname("time" ,"tcp" );
if( (void*) servie!=NULL ) { deftimeport=servie->s_port;}

servie=ISocket->getservbyname("time" ,"udp" );
if( (void*) servie!=NULL ) { deftimeudpport=servie->s_port;}

servie=ISocket->getservbyname("ntp" ,"udp" );
if( (void*) servie!=NULL ) { defsntpport=servie->s_port;}

/*
protoe:=Getprotobyname('tcp')
IF protoe<>NIL THEN tcpproto:=protoe.proto

protoe:=Getprotobyname('udp')
IF protoe<>NIL THEN udpproto:=protoe.proto
*/

CloseLibrary_exec2(SocketBase);
SocketBase=(struct Library*) NULL;
finally: 0; 
	return ;
}

void doaninc() {
long dot;

dot=InStr(server,(char*) ".")-1;
server[dot]=server[dot]+aninc;

if( aninc==1 ) { aninc=-1 ;} else { aninc=1;}
	return ;

}

void connecterror() {
long conn;

    conn=ISocket->Errno();
    switch( conn) {
        case EBADF:
        StrCopy(ntptime,(char*) locstr[MSG_ERR_CONN_EBADF]);
        	break;
        case EADDRNOTAVAIL:
        StrCopy(ntptime,(char*) locstr[MSG_ERR_CONN_EADDRNOTAVAIL]);
        	break;
        case EAFNOSUPPORT:
        StrCopy(ntptime,(char*) locstr[MSG_ERR_CONN_EAFNOSUPPORT]);
        	break;
        case EISCONN:
        StrCopy(ntptime,(char*) locstr[MSG_ERR_CONN_EISCONN]);
        	break;
        case ETIMEDOUT:
        StrCopy(ntptime,(char*) locstr[MSG_ERR_CONN_ETIMEDOUT]);
        	break;
        case ECONNREFUSED:
        StrCopy(ntptime,(char*) locstr[MSG_ERR_CONN_ECONNREFUSED]);
        	break;
        case ENETUNREACH:
        StrCopy(ntptime,(char*) locstr[MSG_ERR_CONN_ENETUNREACH]);
        	break;
        case EADDRINUSE:
        StrCopy(ntptime,(char*) locstr[MSG_ERR_CONN_EADDRINUSE]);
        	break;
        case EINPROGRESS:
        StrCopy(ntptime,(char*) locstr[MSG_ERR_CONN_EINPROGRESS]);
        	break;
        case EALREADY:
        StrCopy(ntptime,(char*) locstr[MSG_ERR_CONN_EALREADY]);
        	break;
        default:
        StrCopy(ntptime,(char*) locstr[MSG_ERR_CONN_DEFAULT]);
    	break;
    }
//    Raise(ERR_MEM)
if( winopen) {
    tempGlobal_ILIST82 [1]= (long) ntptime;
    IGadTools->GT_SetGadgetAttrsA((struct Gadget*) gads[GADG_SERVER] ,win ,(struct Requester*) NULL ,(struct TagItem*) tempGlobal_ILIST82  );
}
	return ;
}

long chkautodst_generic(long nextyear) {
long ret;
struct ClockData datestr; struct ClockData dststr; struct ClockData utcstr;
long dstsecs, utcsecs, dstoff=0, dston=1, stday;
long sm, sw, swd, sh, em, ew, ewd, eh;

if( hemisphere==1) {
// Switch everything around!
    dstoff=1;
    dston=0;
    em=startmonth;
    sm=endmonth;
    ewd=startwday;
    swd=endwday;
    eh=starthour;
    sh=endhour;
    ew=startweek;
    sw=endweek;
} else {
    sm=startmonth;
    em=endmonth;
    swd=startwday;
    ewd=endwday;
    sh=starthour;
    eh=endhour;
    sw=startweek;
    ew=endweek;
}

ioreq->Request.io_Command=(UWORD) (long) TR_GETSYSTIME;

IExec->DoIO((struct IORequest*) ioreq );

IUtility->Amiga2Date(ioreq->Time.Seconds ,& datestr );

IExec->CopyMem((APTR) (long) & ioreq->Time ,(APTR) (long) timemem ,(ULONG) sizeof( struct TimeVal) );

if( nextyear ) { datestr.year=(UWORD) (long) ((long) datestr.year+(long) 1);}

dststr.mday=(UWORD) ((long) datestr.mday-(long) datestr.wday+swd);

if( sw==0) {
    if( sm==2 ) { stday=21 ;} else { stday=25;}
    while( (long) dststr.mday<stday ) { dststr.mday=(UWORD) (long) ((long) dststr.mday+(long) 7);}
} else {
    stday=(sw-1)*7;
    while( (long) dststr.mday<=stday ) { dststr.mday=(UWORD) (long) ((long) dststr.mday+(long) 7);}
    if( (long) dststr.mday>stday+7  ) { dststr.mday=(UWORD) (long) ((long) dststr.mday-(long) 7);}
}

    dststr.month=(UWORD) sm;
    dststr.wday=(UWORD) swd;
    dststr.hour=(UWORD) sh;
    dststr.year=datestr.year;
    dststr.min=(UWORD) (long) 0;
    dststr.sec=(UWORD) (long) 0;

    dstsecs=(long) IUtility->Date2Amiga(& dststr );

    utcstr.mday=(UWORD) ((long) datestr.mday-(long) datestr.wday+ewd);
    
if( ew==0) {
    if( em==2 ) { stday=21 ;} else { stday=25;}
    while( (long) utcstr.mday<stday ) { utcstr.mday=(UWORD) (long) ((long) utcstr.mday+(long) 7);}
} else {
    stday=(ew-1)*7;
    while( (long) utcstr.mday<stday ) { utcstr.mday=(UWORD) (long) ((long) utcstr.mday+(long) 7);}
    if( (long) utcstr.mday>stday+7  ) { utcstr.mday=(UWORD) (long) ((long) utcstr.mday-(long) 7);}
}

//    WriteF('sunday /d/n',utcstr.mday)
    
    utcstr.month=(UWORD) em;
    utcstr.wday=(UWORD) ewd;
    utcstr.hour=(UWORD) eh;
    utcstr.year=datestr.year;
    utcstr.min=(UWORD) (long) 0;
    utcstr.sec=(UWORD) (long) 0;

    utcsecs=(long) IUtility->Date2Amiga(& utcstr );

if( (long) timemem->Seconds<dstsecs ) { dstswitch(dstoff);}

if( (long) timemem->Seconds>=dstsecs) {
    if( - ((long) timemem->Seconds>utcsecs)-3600) {
        if( (long) timemem->Seconds>=utcsecs) {
            dstswitch(dstoff);
        } else if( (long) timemem->Seconds<utcsecs-3600) {
            dstswitch(dston);
        }
    }
}

// WriteF('utc: /d/ndst: /d/ncurrent: /d/n',utcsecs,dstsecs,timemem.secs)
if( (long) timemem->Seconds>=utcsecs) {
    if( (long) timemem->Seconds>=dstsecs) {
        utcsecs=chkautodst_generic(1);
        ret= utcsecs;
        goto finally;
    }
}

if( dstsecs<utcsecs) {
  if( dstsecs>(long) timemem->Seconds ) {
  	ret= dstsecs ;
  	goto finally;
  } else { 
  	ret= utcsecs;
  	goto finally;
  }
} else {
  if( utcsecs>(long) timemem->Seconds ) {
  	ret= utcsecs ;
  	goto finally;
  } else { 
  	ret= dstsecs;
  	goto finally;
  }
}
finally: 0; 
	return ret;

} 

void readtz() {
long comma, comma2, dot, dot2; char* tmp22=NULL;
long temp_QUAD;
try {
	tmp22 = NewString(30);
	temp_QUAD = exception ;
	exception = 0 ;

/*
        IF tzfile:=Open('ENV:TZ',MODE_OLDFILE)
            Fgets(tzfile,buffer,50)
            Close(tzfile)
            MidStr(temp,buffer,3)
            tz:=Val(temp)
            tzsecs:=tz*3600
            MidStr(tzname1,buffer,0,3)
            IF StrLen(buffer)>6
                dst:=1
                MidStr(tzname2,buffer,StrLen(buffer)-4,4)
                IF Val(MidStr(temp,tzname2,0,1))>0
                    MidStr(tzname2,buffer,StrLen(buffer)-3,3)
                    StrAdd(tzname2,' ')
                ENDIF

            ELSE
                dst:=0
            ENDIF
        ENDIF
*/

    if( (tzfile=(long) IDOS->Open("ENV:FACTS/DST" ,MODE_OLDFILE ))) {
        dst=IDOS->FGetC((BPTR) tzfile );
        (BOOL)(0!=IDOS->Close((BPTR) tzfile ));
}

if( (tzfile=(long) IDOS->Open("ENV:TZ" ,MODE_OLDFILE ))) {
    IDOS->FGets((BPTR) tzfile ,(char*) buffer ,(ULONG) (long) 50 );
    (BOOL)(0!=IDOS->Close((BPTR) tzfile ));
    /**** old routine below ****/
    MidStr(temp,(char*) buffer,3);
    tz=Val(temp);
    tzsecs=tz*3600;
    MidStr(tzname1,(char*) buffer,0,3);
    if( strlen((char*) buffer )>6) {
        if( (comma=InStr((char*) buffer,(char*) ","))!=-1) {
            MidStr(tzname2,(char*) buffer,comma-4,4);
            if( Val(MidStr(temp,tzname2,0,1))>0) {
                MidStr(tzname2,(char*) buffer,comma-3,3);
                StrAdd(tzname2,(char*) " ");
            }
            comma2=InStr((char*) buffer,(char*) ",",comma+1);
            MidStr(temp,(char*) buffer,comma+2,comma2-comma-2);
            dot=InStr(temp,(char*) ".");
            MidStr(tmp22,temp,0,dot);
            ostartmonth=Val(tmp22);
            dot2=InStr(temp,(char*) ".",dot+1);
            MidStr(tmp22,temp,dot+1,dot2-dot-1);
            ostartweek=Val(tmp22);
            if( ostartweek==5 ) { ostartweek=0;}
            MidStr(tmp22,temp,dot2+1,1);
            ostartwday=Val(tmp22);
if( (dot=InStr(temp,(char*) "/",dot2+1))!=-1) {
    MidStr(tmp22,temp,dot2+3,dot-dot2-2);
    ostarthour=Val(tmp22);
} else {
    ostarthour=2;
}

            comma=InStr((char*) buffer,(char*) ",",comma2+1);
            MidStr(temp,(char*) buffer,comma2+2,comma-comma2-2);
            dot=InStr(temp,(char*) ".");
            MidStr(tmp22,temp,0,dot);
            oendmonth=Val(tmp22);
            dot2=InStr(temp,(char*) ".",dot+1);
            MidStr(tmp22,temp,dot+1,dot2-dot-1);
            oendweek=Val(tmp22);
            if( oendweek==5 ) { ostartweek=0;}
            MidStr(tmp22,temp,dot2+1,1);
            oendwday=Val(tmp22);
if( (dot=InStr(temp,(char*) "/",dot2+1))!=-1) {
    MidStr(tmp22,temp,dot2+3,dot-dot2-2);
    oendhour=Val(tmp22);
} else {
    oendhour=2;
}

if( ostartweek==5 ) { ostartweek=0;}
if( oendweek==5 ) { oendweek=0;}

// IF autodst<>0 THEN autodst:=-1
changelong(1);
dstpresets(autodst);

        } else {
/**** old routine ****/
            dst=1;
            MidStr(tzname2,(char*) buffer,strlen((char*) buffer )-4,4);
            if( Val(MidStr(temp,tzname2,0,1))>0) {
                MidStr(tzname2,(char*) buffer,strlen((char*) buffer )-3,3);
                StrAdd(tzname2,(char*) " ");
            }
            if( wbstart==0 ) { WriteF((char*) "%s (%s)\n",locstr[MSG_IN_DST],(long) tzname2);}
        }
    }
    if( wbstart==0) {
        WriteF((char*) locstr[MSG_TZ],(long) tzname1,-tz);
        WriteF((char*) "\n\n");
    }
} else {
    if( LocaleBase) {
        StrCopy(tzname1,(char*) "SYS");
        tz=localeinfo->loc_GMTOffset/60;
    } else {
        StrCopy(tzname1,(char*) "UTC");
    }
}
} catch(...) {}
	DisposeString(tmp22 );
	if (exception!=0) {throw eException;} else {EMPTY;};
	exception = temp_QUAD ;
	return ;

}

void tryalternative(long dodef) {
long s;

s=servnum;

if( trynext) {
do {
    servnum=servnum+1;
    loadserver();
    if( servnum==s) {
        if( winopen ) { updategui();}
        goto finally;
    }
    if( retrievetime()==0) {
        if( dodef ) { dodefop();}
        displaytimes();
        if( winopen ) { updategui();}
        goto finally;
    }
//    servnum:=servnum+1
} while(( servnum==s)==0);
// servnum:=s
if( winopen ) { updategui();}
goto finally;
}

s=service;

if( nextservice) {
    selectservice();
    while( s!=service) {
    if( retrievetime()==0) {
        if( dodef ) { dodefop();}
        displaytimes();
        if( winopen ) { updategui();}
        goto finally;
//    ELSE
// >>>>>>>>
    }
    selectservice();
    }
}

if( aninc) {
    doaninc();
if( nextservice) {
    selectservice();
    while( s!=service) {
    if( retrievetime()==0) {
        if( dodef ) { dodefop();}
        displaytimes();
        if( winopen ) { updategui();}
        goto finally;
//    ELSE
// >>>>>>>>
    }
    selectservice();
    }
}
}

if( winopen ) { updategui();}
finally: 0; 
	return ;
}

void selectservice() {
    switch( service) {
        case 123:
            service=372;
            port=timeudpport;
        	break;
        case 372:
            service=37;
            port=timeport;
        	break;
        case 37:
            service=13;
            port=daytimeport;
        	break;
        case 13:
            service=123;
            port=sntpport;
        	break;
    }
	return ;
}

void updategui() {
        menu();
        if( -IGadTools->LayoutMenusA((struct Menu*) menustrip ,(APTR) vi ,(struct TagItem*) tempGlobal_ILIST83  )) {
            -IIntuition->SetMenuStrip(win ,(struct Menu*) menustrip );
        }

tempGlobal_ILIST84 [1]= (long) server;

IGadTools->GT_SetGadgetAttrsA((struct Gadget*) gads[GADG_TIMESERVER] ,win ,(struct Requester*) NULL ,(struct TagItem*) tempGlobal_ILIST84  );
tempGlobal_ILIST85 [1]= port;
IGadTools->GT_SetGadgetAttrsA((struct Gadget*) gads[GADG_PORT] ,win ,(struct Requester*) NULL ,(struct TagItem*) tempGlobal_ILIST85  );
tempGlobal_ILIST86 [1]= timeint;
IGadTools->GT_SetGadgetAttrsA((struct Gadget*) gads[GADG_TIMEINT] ,win ,(struct Requester*) NULL ,(struct TagItem*) tempGlobal_ILIST86  );
	return ;
}

void syncclock(long dodef) {
                if( retrievetime()==0) {
                    if( dodef ) { dodefop();}
                    displaytimes();
                } else {
                    tryalternative(dodef);
                }
	return ;

// IF nocmd=0 THEN SystemTagList(cmd,[SYS_ASYNCH,TRUE,NIL])
// moved above to dodefop routine so it only executes on success 03/06/02
}

/*
PROC help()
DEF nag:newamigaguide,guide[100]:STRING
IF amigaguidebase:=OpenLibrary('amigaguide.library',0)

StrCopy(guide,'DH1:Programming/Projects/FACTS/facts3.0-distrib/facts.guide')
    nag.name:=guide

    nag.screen:=NIL
    nag.pubscreen:='Workbench'
    nag.hostport:='FACTS'
    nag.clientport:='FACTSGUIDE'
    nag.client:=NIL
    nag.node:='Main'
    nag.line:=0
    
    context:=OpenAmigaGuideAsyncA(nag,NIL)
ENDIF
ENDPROC
*/

void showtime() {
                    ioreq->Request.io_Command=(UWORD) (long) TR_GETSYSTIME;
                    IExec->DoIO((struct IORequest*) ioreq );
                    curtime=(long) ioreq->Time.Seconds;
                    StrCopy(sd,sectostr(curtime));
if( win) {
    if( win->Height < tbar+10) {
        IIntuition->SetWindowTitles(win ,sd ,(char*) (long) -1 );
    } else {
        StrCopy(sysdate,sd);
        if( alwaysupdate ) {
        	tempGlobal_ILIST87 [1]= (long) sysdate;
        	IGadTools->GT_SetGadgetAttrsA((struct Gadget*) gads[GADG_AMIGA] ,win ,(struct Requester*) NULL ,(struct TagItem*) tempGlobal_ILIST87  );
        }
    }
}
	return ;
}

void startleapsec(long quick) {
long secs; struct ClockData datestr;

if( obsession==0 ) { goto finally;}

if( (long) IExec->CheckIO((struct IORequest*) dstioreq )==(long) 0) {
    IExec->AbortIO((struct IORequest*) dstioreq );
    IExec->WaitIO((struct IORequest*) dstioreq );
}

/* FACTS should already know the current time - or as close as necessary.

IF gettime
->	GetSysTime(measure)

ioreq.io.command:=TR_GETSYSTIME
DoIO(ioreq)

CopyMem(ioreq.time,timemem,SIZEOF timeval)

measure:=timemem.secs

curtime:=measure.secs

ENDIF

ioreq.time.secs:=ioreq.time.secs+86400
Amiga2Date(ioreq.time.secs,datestr)
*/

if( quick==0) {
curtime=curtime+86400;

IUtility->Amiga2Date((ULONG) curtime ,& datestr );

    datestr.hour=(UWORD) (long) 0;
    datestr.min=(UWORD) (long) 0;
    datestr.sec=(UWORD) (long) 0;

    lssecs=(long) IUtility->Date2Amiga(& datestr );
}

// WriteF('/d/n/d/n',curtime,secs)

            dstioreq->Request.io_Command=(UWORD) (long) TR_ADDREQUEST;
            dstioreq->Time.Seconds=(ULONG) lssecs;
            dstioreq->Time.Microseconds=(ULONG) (long) 0;
            IExec->SendIO((struct IORequest*) dstioreq );

saveleapsecs();
finally: 0; 
	return ;

}

void leapsecchg() {
ITimer->GetSysTime(measure );
/*
        ioreq.io.command:=TR_GETSYSTIME
        DoIO(ioreq)

CopyMem(ioreq.time,timemem,SIZEOF timeval)

measure:=timemem
*/
curtime=(long) measure->Seconds;
        
if( leapsec==1 ) { dodefop(curtime+1);}
if( leapsec==2 ) { dodefop(curtime-1);}

leapsec=0;

if( autodquiet==0 ) {

	tempGlobal_ILIST88 [0]= sizeof( struct EasyStruct);

	tempGlobal_ILIST88 [3]= locstr[MSG_LEAPSEC];

	IIntuition->EasyRequestArgs((struct Window*) (long) 0 ,(struct EasyStruct*) tempGlobal_ILIST88  ,(ULONG*) (long) 0 ,(APTR) (long) NULL );

}

// WriteF('leap second/n')

saveleapsecs();
	return ;

}

void saveleapsecs() {
long fhand=0;

if( obsession==0 ) { goto finally;}

    if( (fhand=(long) IDOS->Lock("ENVARC:FACTS" ,ACCESS_WRITE ))==0) {
        fhand=(long) IDOS->CreateDir("ENVARC:FACTS" );
    }
    IDOS->UnLock((BPTR) fhand );
    if( (fhand=(long) IDOS->Open("ENVARC:FACTS/LEAPSEC" ,MODE_NEWFILE ))) {
        StringF(temp,(char*) "%ld,%ld",leapsec,lssecs);
        IDOS->FPuts((BPTR) fhand ,temp );
        (BOOL)(0!=IDOS->Close((BPTR) fhand ));
    }
finally: 0; 
	return ;
}

void checkoldleapsecs() {
long fhand=0, comma=0;

if( obsession==0 ) { goto finally;}

    if( (fhand=(long) IDOS->Open("ENVARC:FACTS/LEAPSEC" ,MODE_OLDFILE ))) {
        IDOS->FGets((BPTR) fhand ,(char*) buffer ,(ULONG) (long) 100 );
        (BOOL)(0!=IDOS->Close((BPTR) fhand ));
        StrCopy(temp,(char*) buffer);
        comma=InStr(temp,(char*) ",");
        leapsec=Val(StrCopy(tmp,temp,comma));
        lssecs=Val(MidStr(temp,temp,comma+1));
        if( (- (leapsec>0 )& leapsec)<3 ) { startleapsec(1);}
    }
finally: 0; 
	return ;
}

void changelong(long state) {
if( state) {
    longtz=1;
    autodquiet=1;
    ignoretz=1;
    if( autodst!=0 ) { autodst=-1;}
    stopnotify();
} else {
    longtz=0;
}

if( win) {
    menu();
    if( -IGadTools->LayoutMenusA((struct Menu*) menustrip ,(APTR) vi ,(struct TagItem*) tempGlobal_ILIST89  )) {
        -IIntuition->SetMenuStrip(win ,(struct Menu*) menustrip );
    }
}
	return ;
}

void stopnotify() {
long msg=0;
    if( snot) {
        IDOS->EndNotify(& tznotify );
        if( notifyport) {
            while( msg=(long) IExec->GetMsg(notifyport )) { IExec->ReplyMsg((struct Message*) msg );}
            IExec->DeleteMsgPort(notifyport );
        }
    snot=0;
    }
	return ;
}

void stopttnotify() {
long msg=0;
    if( ttsnot) {
        IDOS->EndNotify(& ttnotify );
        if( ttnotifyport) {
            while( msg=(long) IExec->GetMsg(ttnotifyport )) { IExec->ReplyMsg((struct Message*) msg );}
            IExec->DeleteMsgPort(ttnotifyport );
        }
    ttsnot=0;
    }
	return ;
}

void killautodsttimer() {
    if( dstioreq) {
        if( (long) IExec->CheckIO((struct IORequest*) dstioreq )==(long) 0) {
            IExec->AbortIO((struct IORequest*) dstioreq );
            IExec->WaitIO((struct IORequest*) dstioreq );
        }
        CloseDevice_timer((struct IORequest*) dstioreq);
        IExec->DeleteIORequest( /*Required:*/ (IORequest*)(APTR) (long) dstioreq );
        if( dstmsgport ) { IExec->DeleteMsgPort(dstmsgport );}
    }
	return ;
}

void loadserver() {
char* servfile=NULL; long colon;
long temp_QUAD;
try {
	servfile = NewString(100);
	temp_QUAD = exception ;
	exception = 0 ;

StringF(servfile,(char*) "ENV:FACTS/TIMESERVER.%ld",servnum);

if( (lock=(long) IDOS->Lock(servfile ,ACCESS_READ ))) {
    IDOS->UnLock((BPTR) lock );
} else {
    if( servnum>0) {
        servnum=0;
        StringF(servfile,(char*) "ENV:FACTS/TIMESERVER.%ld",servnum);
        if( (lock=(long) IDOS->Lock(servfile ,ACCESS_READ ))) {
            IDOS->UnLock((BPTR) lock );
        } else {
            StrCopy(server,(char*) "pool.ntp.org");
            goto finally;
        }
    } else {
        StrCopy(server,(char*) "pool.ntp.org");
        goto finally;
    }
}

    ntphost=(long) IDOS->Open(servfile ,MODE_OLDFILE );
    IDOS->FGets((BPTR) ntphost ,(char*) buffer ,(ULONG) (long) 100 );
    (BOOL)(0!=IDOS->Close((BPTR) ntphost ));
    StrCopy(temp,(char*) buffer);
    colon=InStr(temp,(char*) ":");
    if( colon!=-1) {
        StrCopy(server,temp,colon);
        MidStr(temp,temp,colon+1);
        port=Val(temp);
//        service:=Val(temp)
        colon=InStr(temp,(char*) ":",colon+1);
        if( colon!=-1) {
            MidStr(temp,temp,colon+1);
            service=Val(temp);
            colon=InStr(temp,(char*) ":",colon+1);
            if( colon!=-1) {
                MidStr(temp,temp,colon+1);
                timeout=Val(temp);
            }
        } else {
            service=port;
        }
    } else {
        StrCopy(server,temp);
    }
finally: 0; 
} catch(...) {}
	DisposeString(servfile );
	if (exception!=0) {throw eException;} else {EMPTY;};
	exception = temp_QUAD ;
	return ;


}

void autocompat() {
                        ioreq->Request.io_Command=(UWORD) (long) TR_GETSYSTIME;
                        IExec->DoIO((struct IORequest*) ioreq );
                        curtime=(long) ioreq->Time.Seconds;
                        curmics=(long) ioreq->Time.Microseconds;
	return ;


}

signed char clockdrift() {
long sdrifted=0, mdrifted=0;
signed char temp_BYTE;

// check current time
ITimer->GetSysTime(measure );
/*
ioreq.io.command:=TR_GETSYSTIME

DoIO(ioreq)

CopyMem(ioreq.time,timemem,SIZEOF timeval)

measure:=timemem
*/
curtime=(long) measure->Seconds;
curmics=(long) measure->Microseconds;

// end checktime

if( mdrift) {
	mdrifted=(curtime-psecs)/mdrifts*mdrift;

	sdrifted=mdrifted / 1000000;

	servsecs=curtime+sdrifted;
	servmicros=curmics+(mdrifted-sdrifted * 1000000);

/*
	IF servmicros>99999
		servmicros:=servmicros-1000000
		servsecs:=servsecs+1
	ENDIF
	*/

	dadj=1;

	temp_BYTE = 0 ;// ok

	goto finally;

}

temp_BYTE = 1 ;// failed

goto finally;
	temp_BYTE = 1;
finally: 0; 
	return temp_BYTE ;
} 
void new_facts_portable()  {
	tmp2 = NewString(100);
	server = NewString(1024);
	temp = NewString(2048);
	min = NewString(5);
	sec = NewString(5);
	wday = NewString(5);
	day = NewString(5);
	month = NewString(5);
	year = NewString(5);
	hour = NewString(10);
	popkey = NewString(256);
	tmp = NewString(100);
	monthnames = NewString(100);
	daynames = NewString(100);
	sysdate = NewString(100);
	errmsg = NewString(512);
	tzname1 = NewString(3);
	tzname2 = NewString(4);
	ntptime = NewString(50);
	savekey = NewString(256);
	diffbox = NewString(100);
	ipaddr = NewString(20);
	lang = NewString(2);
	yesno = NewString(20);
	defwintit = NewString(15);
	sd = NewString(30);
	cmd = NewString(512);
	st1db = NewString(512);
	st2db = NewString(512);
	fver = NewString(3);
	frev = NewString(3);
	frel = NewString(6);
	tempGlobal_ILIST = InitList(temp_ARRAY_OF_VALUE ,3);
	tempGlobal_ILIST [0]= OC_BuiltInLanguage;
	tempGlobal_ILIST [1]= (long) "english";
	tempGlobal_ILIST [2]= (long) NULL;
	SetList(tempGlobal_ILIST ,3);
	tempGlobal_ILIST2 = InitList(temp_ARRAY_OF_VALUE2 ,12);
	SetList(tempGlobal_ILIST2 ,12);
	tempGlobal_ILIST3 = InitList(temp_ARRAY_OF_VALUE3 ,13);
	tempGlobal_ILIST3 [0]= 0;
	tempGlobal_ILIST3 [1]= 0;
	tempGlobal_ILIST3 [2]= 0;
	tempGlobal_ILIST3 [3]= 0;
	tempGlobal_ILIST3 [4]= 0;
	tempGlobal_ILIST3 [5]= 0;
	tempGlobal_ILIST3 [6]= 0;
	tempGlobal_ILIST3 [7]= 0;
	tempGlobal_ILIST3 [8]= 0;
	tempGlobal_ILIST3 [9]= 0;
	tempGlobal_ILIST3 [10]= 0;
	tempGlobal_ILIST3 [11]= 0;
	tempGlobal_ILIST3 [12]= 0;
	SetList(tempGlobal_ILIST3 ,13);
	tempGlobal_ILIST4 = InitList(temp_ARRAY_OF_VALUE4 ,5);
	tempGlobal_ILIST4 [1]= 0;
	tempGlobal_ILIST4 [2]= (long) "FACTS";
	tempGlobal_ILIST4 [4]= (long) "OK";
	SetList(tempGlobal_ILIST4 ,5);
	tempGlobal_ILIST5 = InitList(temp_ARRAY_OF_VALUE5 ,5);
	tempGlobal_ILIST5 [0]= WA_BusyPointer;
	tempGlobal_ILIST5 [1]= TRUE;
	tempGlobal_ILIST5 [2]= WA_PointerDelay;
	tempGlobal_ILIST5 [3]= TRUE;
	tempGlobal_ILIST5 [4]= (long) NULL;
	SetList(tempGlobal_ILIST5 ,5);
	tempGlobal_ILIST6 = InitList(temp_ARRAY_OF_VALUE6 ,3);
	tempGlobal_ILIST6 [0]= GTTX_Text;
	tempGlobal_ILIST6 [2]= (long) NULL;
	SetList(tempGlobal_ILIST6 ,3);
	tempGlobal_ILIST7 = InitList(temp_ARRAY_OF_VALUE7 ,3);
	tempGlobal_ILIST7 [0]= WA_PointerDelay;
	tempGlobal_ILIST7 [1]= TRUE;
	tempGlobal_ILIST7 [2]= (long) NULL;
	SetList(tempGlobal_ILIST7 ,3);
	tempGlobal_ILIST8 = InitList(temp_ARRAY_OF_VALUE8 ,3);
	tempGlobal_ILIST8 [0]= GTTX_Text;
	tempGlobal_ILIST8 [2]= (long) NULL;
	SetList(tempGlobal_ILIST8 ,3);
	tempGlobal_ILIST9 = InitList(temp_ARRAY_OF_VALUE9 ,3);
	tempGlobal_ILIST9 [0]= GTTX_Text;
	tempGlobal_ILIST9 [2]= (long) NULL;
	SetList(tempGlobal_ILIST9 ,3);
	tempGlobal_ILIST10 = InitList(temp_ARRAY_OF_VALUE10 ,3);
	tempGlobal_ILIST10 [0]= WA_PointerDelay;
	tempGlobal_ILIST10 [1]= TRUE;
	tempGlobal_ILIST10 [2]= (long) NULL;
	SetList(tempGlobal_ILIST10 ,3);
	tempGlobal_ARRAY_OF_timeval [0].Microseconds = (ULONG) (long) 0;
	tempGlobal_ILIST11 = InitList(temp_ARRAY_OF_VALUE11 ,3);
	tempGlobal_ILIST11 [0]= GTTX_Text;
	tempGlobal_ILIST11 [2]= (long) NULL;
	SetList(tempGlobal_ILIST11 ,3);
	tempGlobal_ILIST12 = InitList(temp_ARRAY_OF_VALUE12 ,3);
	tempGlobal_ILIST12 [0]= GTTX_Text;
	tempGlobal_ILIST12 [2]= (long) NULL;
	SetList(tempGlobal_ILIST12 ,3);
	tempGlobal_ILIST13 = InitList(temp_ARRAY_OF_VALUE13 ,3);
	tempGlobal_ILIST13 [0]= GTTX_Text;
	tempGlobal_ILIST13 [2]= (long) NULL;
	SetList(tempGlobal_ILIST13 ,3);
	tempGlobal_ILIST14 = InitList(temp_ARRAY_OF_VALUE14 ,3);
	tempGlobal_ILIST14 [0]= GTTX_Text;
	tempGlobal_ILIST14 [2]= (long) NULL;
	SetList(tempGlobal_ILIST14 ,3);
	tempGlobal_ILIST15 = InitList(temp_ARRAY_OF_VALUE15 ,3);
	tempGlobal_ILIST15 [0]= GTTX_Text;
	tempGlobal_ILIST15 [2]= (long) NULL;
	SetList(tempGlobal_ILIST15 ,3);
	tempGlobal_ILIST16 = InitList(temp_ARRAY_OF_VALUE16 ,3);
	tempGlobal_ILIST16 [0]= WA_PointerDelay;
	tempGlobal_ILIST16 [1]= TRUE;
	tempGlobal_ILIST16 [2]= (long) NULL;
	SetList(tempGlobal_ILIST16 ,3);
	tempGlobal_ILIST17 = InitList(temp_ARRAY_OF_VALUE17 ,3);
	tempGlobal_ILIST17 [0]= GTTX_Text;
	tempGlobal_ILIST17 [2]= (long) NULL;
	SetList(tempGlobal_ILIST17 ,3);
	tempGlobal_ILIST18 = InitList(temp_ARRAY_OF_VALUE18 ,3);
	tempGlobal_ILIST18 [0]= WA_PointerDelay;
	tempGlobal_ILIST18 [1]= TRUE;
	tempGlobal_ILIST18 [2]= (long) NULL;
	SetList(tempGlobal_ILIST18 ,3);
	tempGlobal_ILIST19 = InitList(temp_ARRAY_OF_VALUE19 ,3);
	tempGlobal_ILIST19 [0]= WA_PointerDelay;
	tempGlobal_ILIST19 [1]= TRUE;
	tempGlobal_ILIST19 [2]= (long) NULL;
	SetList(tempGlobal_ILIST19 ,3);
	tempGlobal_ILIST20 = InitList(temp_ARRAY_OF_VALUE20 ,3);
	tempGlobal_ILIST20 [0]= WA_PointerDelay;
	tempGlobal_ILIST20 [1]= TRUE;
	tempGlobal_ILIST20 [2]= (long) NULL;
	SetList(tempGlobal_ILIST20 ,3);
	tempGlobal_ILIST21 = InitList(temp_ARRAY_OF_VALUE21 ,5);
	tempGlobal_ILIST21 [0]= WA_BusyPointer;
	tempGlobal_ILIST21 [1]= TRUE;
	tempGlobal_ILIST21 [2]= WA_PointerDelay;
	tempGlobal_ILIST21 [3]= TRUE;
	tempGlobal_ILIST21 [4]= (long) NULL;
	SetList(tempGlobal_ILIST21 ,5);
	tempGlobal_ILIST22 = InitList(temp_ARRAY_OF_VALUE22 ,3);
	tempGlobal_ILIST22 [0]= GTTX_Text;
	tempGlobal_ILIST22 [2]= (long) NULL;
	SetList(tempGlobal_ILIST22 ,3);
	tempGlobal_ILIST23 = InitList(temp_ARRAY_OF_VALUE23 ,3);
	tempGlobal_ILIST23 [0]= GTTX_Text;
	tempGlobal_ILIST23 [2]= (long) NULL;
	SetList(tempGlobal_ILIST23 ,3);
	tempGlobal_ILIST24 = InitList(temp_ARRAY_OF_VALUE24 ,3);
	tempGlobal_ILIST24 [0]= GTTX_Text;
	tempGlobal_ILIST24 [2]= (long) NULL;
	SetList(tempGlobal_ILIST24 ,3);
	tempGlobal_ILIST25 = InitList(temp_ARRAY_OF_VALUE25 ,3);
	tempGlobal_ILIST25 [0]= WA_PointerDelay;
	tempGlobal_ILIST25 [1]= TRUE;
	tempGlobal_ILIST25 [2]= (long) NULL;
	SetList(tempGlobal_ILIST25 ,3);
	tempGlobal_ILIST26 = InitList(temp_ARRAY_OF_VALUE26 ,5);
	tempGlobal_ILIST26 [1]= 0;
	tempGlobal_ILIST26 [2]= (long) "FACTS";
	tempGlobal_ILIST26 [4]= (long) "OK";
	SetList(tempGlobal_ILIST26 ,5);
	tempGlobal_ILIST27 = InitList(temp_ARRAY_OF_VALUE27 ,5);
	tempGlobal_ILIST27 [1]= 0;
	tempGlobal_ILIST27 [2]= (long) "FACTS";
	tempGlobal_ILIST27 [4]= (long) "OK";
	SetList(tempGlobal_ILIST27 ,5);
	tempGlobal_ILIST28 = InitList(temp_ARRAY_OF_VALUE28 ,5);
	tempGlobal_ILIST28 [1]= 0;
	tempGlobal_ILIST28 [2]= (long) "FACTS";
	tempGlobal_ILIST28 [4]= (long) "OK";
	SetList(tempGlobal_ILIST28 ,5);
	tempGlobal_ILIST29 = InitList(temp_ARRAY_OF_VALUE29 ,31);
	tempGlobal_ILIST29 [0]= WA_Left;
	tempGlobal_ILIST29 [2]= WA_Top;
	tempGlobal_ILIST29 [4]= WA_InnerWidth;
	tempGlobal_ILIST29 [6]= WA_InnerHeight;
	tempGlobal_ILIST29 [8]= WA_IDCMP;
	tempGlobal_ILIST29 [9]= IDCMP_CLOSEWINDOW | IDCMP_REFRESHWINDOW | BUTTONIDCMP | IDCMP_MENUPICK | IDCMP_NEWSIZE;
	tempGlobal_ILIST29 [10]= WA_Gadgets;
	tempGlobal_ILIST29 [12]= WA_Title;
	tempGlobal_ILIST29 [14]= WA_DragBar;
	tempGlobal_ILIST29 [15]= TRUE;
	tempGlobal_ILIST29 [16]= WA_DepthGadget;
	tempGlobal_ILIST29 [17]= TRUE;
	tempGlobal_ILIST29 [18]= WA_CloseGadget;
	tempGlobal_ILIST29 [19]= TRUE;
	tempGlobal_ILIST29 [20]= WA_Activate;
	tempGlobal_ILIST29 [21]= TRUE;
	tempGlobal_ILIST29 [22]= WA_NewLookMenus;
	tempGlobal_ILIST29 [23]= TRUE;
	tempGlobal_ILIST29 [24]= WA_ScreenTitle;
	tempGlobal_ILIST29 [25]= (long) "FACTS  \251 1998-2009 Unsatisfactory Software";
	tempGlobal_ILIST29 [26]= WA_AutoAdjust;
	tempGlobal_ILIST29 [27]= TRUE;
	tempGlobal_ILIST29 [28]= WA_Zoom;
	tempGlobal_ILIST29 [30]= (long) NULL;
	SetList(tempGlobal_ILIST29 ,31);
	tempGlobal_ILIST30 = InitList(temp_ARRAY_OF_VALUE30 ,3);
	tempGlobal_ILIST30 [0]= GTMN_NewLookMenus;
	tempGlobal_ILIST30 [1]= TRUE;
	tempGlobal_ILIST30 [2]= (long) NULL;
	SetList(tempGlobal_ILIST30 ,3);
	tempGlobal_ILIST31 = InitList(temp_ARRAY_OF_VALUE31 ,1);
	tempGlobal_ILIST31 [0]= (long) NULL;
	SetList(tempGlobal_ILIST31 ,1);
	tempGlobal_ARRAY_OF_newgadget [0].ng_GadgetID = (UWORD) (long) GADG_TZNAME;
	tempGlobal_ARRAY_OF_newgadget [0].ng_Flags = (ULONG) (long) 0;
	tempGlobal_ARRAY_OF_newgadget [0].ng_UserData = (APTR) (long) NULL;
	tempGlobal_ILIST32 = InitList(temp_ARRAY_OF_VALUE32 ,5);
	tempGlobal_ILIST32 [0]= GTST_String;
	tempGlobal_ILIST32 [2]= GTST_MaxChars;
	tempGlobal_ILIST32 [3]= 3;
	tempGlobal_ILIST32 [4]= (long) NULL;
	SetList(tempGlobal_ILIST32 ,5);
	tempGlobal_ARRAY_OF_newgadget2 [0].ng_GadgetID = (UWORD) (long) GADG_TZ;
	tempGlobal_ARRAY_OF_newgadget2 [0].ng_Flags = (ULONG) (long) 0;
	tempGlobal_ARRAY_OF_newgadget2 [0].ng_UserData = (APTR) (long) NULL;
	tempGlobal_ILIST33 = InitList(temp_ARRAY_OF_VALUE33 ,3);
	tempGlobal_ILIST33 [0]= GTIN_Number;
	tempGlobal_ILIST33 [2]= (long) NULL;
	SetList(tempGlobal_ILIST33 ,3);
	tempGlobal_ARRAY_OF_newgadget3 [0].ng_GadgetID = (UWORD) (long) GADG_DST;
	tempGlobal_ARRAY_OF_newgadget3 [0].ng_Flags = (ULONG) (long) 0;
	tempGlobal_ARRAY_OF_newgadget3 [0].ng_UserData = (APTR) (long) NULL;
	tempGlobal_ILIST34 = InitList(temp_ARRAY_OF_VALUE34 ,5);
	tempGlobal_ILIST34 [0]= GTCB_Checked;
	tempGlobal_ILIST34 [2]= GTCB_Scaled;
	tempGlobal_ILIST34 [3]= TRUE;
	tempGlobal_ILIST34 [4]= (long) NULL;
	SetList(tempGlobal_ILIST34 ,5);
	tempGlobal_ARRAY_OF_newgadget4 [0].ng_GadgetText = (char*) "";
	tempGlobal_ARRAY_OF_newgadget4 [0].ng_GadgetID = (UWORD) (long) GADG_DSTNAME;
	tempGlobal_ARRAY_OF_newgadget4 [0].ng_Flags = (ULONG) (long) 0;
	tempGlobal_ARRAY_OF_newgadget4 [0].ng_UserData = (APTR) (long) NULL;
	tempGlobal_ILIST35 = InitList(temp_ARRAY_OF_VALUE35 ,7);
	tempGlobal_ILIST35 [0]= GTST_String;
	tempGlobal_ILIST35 [2]= GTST_MaxChars;
	tempGlobal_ILIST35 [3]= 4;
	tempGlobal_ILIST35 [4]= GA_Disabled;
	tempGlobal_ILIST35 [6]= (long) NULL;
	SetList(tempGlobal_ILIST35 ,7);
	tempGlobal_ARRAY_OF_newgadget5 [0].ng_GadgetID = (UWORD) (long) GADG_TIMESERVER;
	tempGlobal_ARRAY_OF_newgadget5 [0].ng_Flags = (ULONG) (long) 0;
	tempGlobal_ARRAY_OF_newgadget5 [0].ng_UserData = (APTR) (long) NULL;
	tempGlobal_ILIST36 = InitList(temp_ARRAY_OF_VALUE36 ,3);
	tempGlobal_ILIST36 [0]= GTST_String;
	tempGlobal_ILIST36 [2]= (long) NULL;
	SetList(tempGlobal_ILIST36 ,3);
	tempGlobal_ARRAY_OF_newgadget6 [0].ng_GadgetText = (char*) "";
	tempGlobal_ARRAY_OF_newgadget6 [0].ng_GadgetID = (UWORD) (long) GADG_PORT;
	tempGlobal_ARRAY_OF_newgadget6 [0].ng_Flags = (ULONG) (long) 0;
	tempGlobal_ARRAY_OF_newgadget6 [0].ng_UserData = (APTR) (long) NULL;
	tempGlobal_ILIST37 = InitList(temp_ARRAY_OF_VALUE37 ,5);
	tempGlobal_ILIST37 [0]= GTIN_Number;
	tempGlobal_ILIST37 [2]= GA_Disabled;
	tempGlobal_ILIST37 [4]= (long) NULL;
	SetList(tempGlobal_ILIST37 ,5);
	tempGlobal_ARRAY_OF_newgadget7 [0].ng_GadgetID = (UWORD) (long) GADG_SERVER;
	tempGlobal_ARRAY_OF_newgadget7 [0].ng_Flags = (ULONG) (long) 0;
	tempGlobal_ARRAY_OF_newgadget7 [0].ng_UserData = (APTR) (long) NULL;
	tempGlobal_ILIST38 = InitList(temp_ARRAY_OF_VALUE38 ,5);
	tempGlobal_ILIST38 [0]= GTTX_Border;
	tempGlobal_ILIST38 [1]= TRUE;
	tempGlobal_ILIST38 [2]= GTTX_Text;
	tempGlobal_ILIST38 [4]= (long) NULL;
	SetList(tempGlobal_ILIST38 ,5);
	tempGlobal_ARRAY_OF_newgadget8 [0].ng_GadgetID = (UWORD) (long) GADG_CHECK;
	tempGlobal_ARRAY_OF_newgadget8 [0].ng_Flags = (ULONG) (long) 0;
	tempGlobal_ARRAY_OF_newgadget8 [0].ng_UserData = (APTR) (long) NULL;
	tempGlobal_ILIST39 = InitList(temp_ARRAY_OF_VALUE39 ,1);
	tempGlobal_ILIST39 [0]= (long) NULL;
	SetList(tempGlobal_ILIST39 ,1);
	tempGlobal_ARRAY_OF_newgadget9 [0].ng_GadgetID = (UWORD) (long) GADG_SAVE;
	tempGlobal_ARRAY_OF_newgadget9 [0].ng_Flags = (ULONG) (long) 0;
	tempGlobal_ARRAY_OF_newgadget9 [0].ng_UserData = (APTR) (long) NULL;
	tempGlobal_ILIST40 = InitList(temp_ARRAY_OF_VALUE40 ,1);
	tempGlobal_ILIST40 [0]= (long) NULL;
	SetList(tempGlobal_ILIST40 ,1);
	tempGlobal_ARRAY_OF_newgadget10 [0].ng_GadgetID = (UWORD) (long) GADG_AMIGA;
	tempGlobal_ARRAY_OF_newgadget10 [0].ng_Flags = (ULONG) (long) 0;
	tempGlobal_ARRAY_OF_newgadget10 [0].ng_UserData = (APTR) (long) NULL;
	tempGlobal_ILIST41 = InitList(temp_ARRAY_OF_VALUE41 ,5);
	tempGlobal_ILIST41 [0]= GTTX_Border;
	tempGlobal_ILIST41 [1]= TRUE;
	tempGlobal_ILIST41 [2]= GTTX_Text;
	tempGlobal_ILIST41 [4]= (long) NULL;
	SetList(tempGlobal_ILIST41 ,5);
	tempGlobal_ARRAY_OF_newgadget11 [0].ng_GadgetID = (UWORD) (long) GADG_DIFF;
	tempGlobal_ARRAY_OF_newgadget11 [0].ng_Flags = (ULONG) (long) 0;
	tempGlobal_ARRAY_OF_newgadget11 [0].ng_UserData = (APTR) (long) NULL;
	tempGlobal_ILIST42 = InitList(temp_ARRAY_OF_VALUE42 ,5);
	tempGlobal_ILIST42 [0]= GTTX_Border;
	tempGlobal_ILIST42 [1]= TRUE;
	tempGlobal_ILIST42 [2]= GTTX_Text;
	tempGlobal_ILIST42 [4]= (long) NULL;
	SetList(tempGlobal_ILIST42 ,5);
	tempGlobal_ARRAY_OF_newgadget12 [0].ng_GadgetID = (UWORD) (long) GADG_TIMEINT;
	tempGlobal_ARRAY_OF_newgadget12 [0].ng_Flags = (ULONG) (long) 0;
	tempGlobal_ARRAY_OF_newgadget12 [0].ng_UserData = (APTR) (long) NULL;
	tempGlobal_ILIST43 = InitList(temp_ARRAY_OF_VALUE43 ,3);
	tempGlobal_ILIST43 [0]= GTIN_Number;
	tempGlobal_ILIST43 [2]= (long) NULL;
	SetList(tempGlobal_ILIST43 ,3);
	tempGlobal_ARRAY_OF_newgadget13 [0].ng_GadgetID = (UWORD) (long) GADG_DEFPORT;
	tempGlobal_ARRAY_OF_newgadget13 [0].ng_Flags = (ULONG) (long) 0;
	tempGlobal_ARRAY_OF_newgadget13 [0].ng_UserData = (APTR) (long) NULL;
	tempGlobal_ILIST44 = InitList(temp_ARRAY_OF_VALUE44 ,5);
	tempGlobal_ILIST44 [0]= GTCB_Checked;
	tempGlobal_ILIST44 [2]= GTCB_Scaled;
	tempGlobal_ILIST44 [3]= TRUE;
	tempGlobal_ILIST44 [4]= (long) NULL;
	SetList(tempGlobal_ILIST44 ,5);
	tempGlobal_ILIST45 = InitList(temp_ARRAY_OF_VALUE45 ,3);
	tempGlobal_ILIST45 [0]= GA_Disabled;
	tempGlobal_ILIST45 [2]= (long) NULL;
	SetList(tempGlobal_ILIST45 ,3);
	tempGlobal_ILIST46 = InitList(temp_ARRAY_OF_VALUE46 ,3);
	tempGlobal_ILIST46 [0]= GTIN_Number;
	tempGlobal_ILIST46 [2]= (long) NULL;
	SetList(tempGlobal_ILIST46 ,3);
	tempGlobal_ILIST47 = InitList(temp_ARRAY_OF_VALUE47 ,5);
	tempGlobal_ILIST47 [0]= GA_Disabled;
	tempGlobal_ILIST47 [2]= GTIN_Number;
	tempGlobal_ILIST47 [4]= (long) NULL;
	SetList(tempGlobal_ILIST47 ,5);
	tempGlobal_ILIST48 = InitList(temp_ARRAY_OF_VALUE48 ,5);
	tempGlobal_ILIST48 [0]= WA_BusyPointer;
	tempGlobal_ILIST48 [1]= TRUE;
	tempGlobal_ILIST48 [2]= WA_PointerDelay;
	tempGlobal_ILIST48 [3]= TRUE;
	tempGlobal_ILIST48 [4]= (long) NULL;
	SetList(tempGlobal_ILIST48 ,5);
	tempGlobal_ILIST49 = InitList(temp_ARRAY_OF_VALUE49 ,3);
	tempGlobal_ILIST49 [0]= GTIN_Number;
	tempGlobal_ILIST49 [2]= (long) NULL;
	SetList(tempGlobal_ILIST49 ,3);
	tempGlobal_ILIST50 = InitList(temp_ARRAY_OF_VALUE50 ,3);
	tempGlobal_ILIST50 [0]= GTIN_Number;
	tempGlobal_ILIST50 [2]= (long) NULL;
	SetList(tempGlobal_ILIST50 ,3);
	tempGlobal_ILIST51 = InitList(temp_ARRAY_OF_VALUE51 ,3);
	tempGlobal_ILIST51 [0]= GTIN_Number;
	tempGlobal_ILIST51 [2]= (long) NULL;
	SetList(tempGlobal_ILIST51 ,3);
	tempGlobal_ILIST52 = InitList(temp_ARRAY_OF_VALUE52 ,3);
	tempGlobal_ILIST52 [0]= GTIN_Number;
	tempGlobal_ILIST52 [2]= (long) NULL;
	SetList(tempGlobal_ILIST52 ,3);
	tempGlobal_ILIST53 = InitList(temp_ARRAY_OF_VALUE53 ,3);
	tempGlobal_ILIST53 [0]= WA_BusyPointer;
	tempGlobal_ILIST53 [1]= FALSE;
	tempGlobal_ILIST53 [2]= (long) NULL;
	SetList(tempGlobal_ILIST53 ,3);
	tempGlobal_ILIST54 = InitList(temp_ARRAY_OF_VALUE54 ,5);
	tempGlobal_ILIST54 [0]= WA_BusyPointer;
	tempGlobal_ILIST54 [1]= TRUE;
	tempGlobal_ILIST54 [2]= WA_PointerDelay;
	tempGlobal_ILIST54 [3]= TRUE;
	tempGlobal_ILIST54 [4]= (long) NULL;
	SetList(tempGlobal_ILIST54 ,5);
	tempGlobal_ILIST55 = InitList(temp_ARRAY_OF_VALUE55 ,1);
	tempGlobal_ILIST55 [0]= (long) NULL;
	SetList(tempGlobal_ILIST55 ,1);
	tempGlobal_ILIST56 = InitList(temp_ARRAY_OF_VALUE56 ,3);
	tempGlobal_ILIST56 [0]= GTCB_Checked;
	tempGlobal_ILIST56 [2]= (long) NULL;
	SetList(tempGlobal_ILIST56 ,3);
	tempGlobal_ILIST57 = InitList(temp_ARRAY_OF_VALUE57 ,3);
	tempGlobal_ILIST57 [0]= GTST_String;
	tempGlobal_ILIST57 [2]= (long) NULL;
	SetList(tempGlobal_ILIST57 ,3);
	tempGlobal_ILIST58 = InitList(temp_ARRAY_OF_VALUE58 ,5);
	tempGlobal_ILIST58 [0]= GA_Disabled;
	tempGlobal_ILIST58 [2]= GTST_String;
	tempGlobal_ILIST58 [4]= (long) NULL;
	SetList(tempGlobal_ILIST58 ,5);
	tempGlobal_ILIST59 = InitList(temp_ARRAY_OF_VALUE59 ,3);
	tempGlobal_ILIST59 [0]= GTIN_Number;
	tempGlobal_ILIST59 [2]= (long) NULL;
	SetList(tempGlobal_ILIST59 ,3);
	tempGlobal_ILIST60 = InitList(temp_ARRAY_OF_VALUE60 ,5);
	tempGlobal_ILIST60 [1]= 0;
	tempGlobal_ILIST60 [2]= (long) "FACTS";
	SetList(tempGlobal_ILIST60 ,5);
	tempGlobal_ARRAY_OF_newbroker [0].nb_Version = NB_VERSION;
	tempGlobal_ARRAY_OF_newbroker [0].nb_Pri  = 0;
	tempGlobal_ARRAY_OF_newbroker [0].nb_Name = (char*) "FACTS";
	tempGlobal_ARRAY_OF_newbroker [0].nb_Title = (char*) "Finest Amiga Clock Time Synchroniser";
	tempGlobal_ARRAY_OF_newbroker [0].nb_Unique = NBU_UNIQUE | NBU_NOTIFY;
	tempGlobal_ARRAY_OF_newbroker [0].nb_Flags = COF_SHOW_HIDE;
	tempGlobal_ARRAY_OF_newbroker [0].nb_ReservedChannel  = (WORD) (long) 0;
	tempGlobal_ARRAY_OF_newbroker [0].nb_ReservedChannel = 0;
	tempGlobal_ILIST61 = InitList(temp_ARRAY_OF_VALUE61 ,10);
	SetList(tempGlobal_ILIST61 ,10);
	tempGlobal_ILIST62 = InitList(temp_ARRAY_OF_VALUE62 ,6);
	tempGlobal_ILIST62 [1]= 0;
	tempGlobal_ILIST62 [2]= (long) "FACTS";
	tempGlobal_ILIST62 [4]= (long) "OK";
	tempGlobal_ILIST62 [5]= (long) "ENVARC:SUMMERTIME";
	SetList(tempGlobal_ILIST62 ,6);
	tempGlobal_ILIST63 = InitList(temp_ARRAY_OF_VALUE63 ,6);
	tempGlobal_ILIST63 [1]= 0;
	tempGlobal_ILIST63 [2]= (long) "FACTS";
	tempGlobal_ILIST63 [4]= (long) "OK";
	tempGlobal_ILIST63 [5]= (long) "ENV:SUMMERTIME";
	SetList(tempGlobal_ILIST63 ,6);
	tempGlobal_ILIST64 = InitList(temp_ARRAY_OF_VALUE64 ,6);
	tempGlobal_ILIST64 [1]= 0;
	tempGlobal_ILIST64 [2]= (long) "FACTS";
	tempGlobal_ILIST64 [4]= (long) "OK";
	tempGlobal_ILIST64 [5]= (long) "ENV:TZ";
	SetList(tempGlobal_ILIST64 ,6);
	tempGlobal_ILIST65 = InitList(temp_ARRAY_OF_VALUE65 ,6);
	tempGlobal_ILIST65 [1]= 0;
	tempGlobal_ILIST65 [2]= (long) "FACTS";
	tempGlobal_ILIST65 [4]= (long) "OK";
	tempGlobal_ILIST65 [5]= (long) "ENV:FACTS/DST";
	SetList(tempGlobal_ILIST65 ,6);
	tempGlobal_ILIST66 = InitList(temp_ARRAY_OF_VALUE66 ,6);
	tempGlobal_ILIST66 [1]= 0;
	tempGlobal_ILIST66 [2]= (long) "FACTS";
	tempGlobal_ILIST66 [4]= (long) "OK";
	tempGlobal_ILIST66 [5]= (long) "ENVARC:FACTS/DST";
	SetList(tempGlobal_ILIST66 ,6);
	tempGlobal_ILIST67 = InitList(temp_ARRAY_OF_VALUE67 ,6);
	tempGlobal_ILIST67 [1]= 0;
	tempGlobal_ILIST67 [2]= (long) "FACTS";
	tempGlobal_ILIST67 [4]= (long) "OK";
	tempGlobal_ILIST67 [5]= (long) "ENVARC:TZ";
	SetList(tempGlobal_ILIST67 ,6);
	tempGlobal_ILIST68 = InitList(temp_ARRAY_OF_VALUE68 ,6);
	tempGlobal_ILIST68 [1]= 0;
	tempGlobal_ILIST68 [2]= (long) "FACTS";
	tempGlobal_ILIST68 [4]= (long) "OK";
	SetList(tempGlobal_ILIST68 ,6);
	tempGlobal_ILIST69 = InitList(temp_ARRAY_OF_VALUE69 ,6);
	tempGlobal_ILIST69 [1]= 0;
	tempGlobal_ILIST69 [2]= (long) "FACTS";
	tempGlobal_ILIST69 [4]= (long) "OK";
	SetList(tempGlobal_ILIST69 ,6);
	tempGlobal_ILIST70 = InitList(temp_ARRAY_OF_VALUE70 ,5);
	tempGlobal_ILIST70 [1]= 0;
	tempGlobal_ILIST70 [2]= (long) "FACTS";
	SetList(tempGlobal_ILIST70 ,5);
	tempGlobal_ARRAY_OF_newmenu = (struct NewMenu*) FastNew(86 * sizeof( struct NewMenu)) ;
	tempGlobal_ARRAY_OF_newmenu [0].nm_Type = (UBYTE) (long) NM_TITLE;
	tempGlobal_ARRAY_OF_newmenu [0].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [0].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [0].nm_Flags = (UWORD) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [0].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [0].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [1].nm_Type = (UBYTE) (long) NM_ITEM;
	tempGlobal_ARRAY_OF_newmenu [1].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [1].nm_CommKey = (char*) "I";
	tempGlobal_ARRAY_OF_newmenu [1].nm_Flags = (UWORD) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [1].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [1].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [2].nm_Type = (UBYTE) (long) NM_ITEM;
	tempGlobal_ARRAY_OF_newmenu [2].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [2].nm_Label = NM_BARLABEL;
	tempGlobal_ARRAY_OF_newmenu [2].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [2].nm_Flags = (UWORD) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [2].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [2].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [3].nm_Type = (UBYTE) (long) NM_ITEM;
	tempGlobal_ARRAY_OF_newmenu [3].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [3].nm_CommKey = (char*) "A";
	tempGlobal_ARRAY_OF_newmenu [3].nm_Flags = (UWORD) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [3].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [3].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [4].nm_Type = (UBYTE) (long) NM_ITEM;
	tempGlobal_ARRAY_OF_newmenu [4].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [4].nm_Label = NM_BARLABEL;
	tempGlobal_ARRAY_OF_newmenu [4].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [4].nm_Flags = (UWORD) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [4].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [4].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [5].nm_Type = (UBYTE) (long) NM_ITEM;
	tempGlobal_ARRAY_OF_newmenu [5].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [5].nm_CommKey = (char*) "H";
	tempGlobal_ARRAY_OF_newmenu [5].nm_Flags = (UWORD) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [5].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [5].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [6].nm_Type = (UBYTE) (long) NM_ITEM;
	tempGlobal_ARRAY_OF_newmenu [6].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [6].nm_CommKey = (char*) "Q";
	tempGlobal_ARRAY_OF_newmenu [6].nm_Flags = (UWORD) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [6].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [6].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [7].nm_Type = (UBYTE) (long) NM_TITLE;
	tempGlobal_ARRAY_OF_newmenu [7].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [7].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [7].nm_Flags = (UWORD) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [7].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [7].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [8].nm_Type = (UBYTE) (long) NM_ITEM;
	tempGlobal_ARRAY_OF_newmenu [8].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [8].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [8].nm_Flags = (UWORD) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [8].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [8].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [9].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [9].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [9].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [9].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [9].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [10].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [10].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [10].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [10].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [10].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [11].nm_Type = (UBYTE) (long) NM_ITEM;
	tempGlobal_ARRAY_OF_newmenu [11].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [11].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [11].nm_Flags = (UWORD) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [11].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [11].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [12].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [12].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [12].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [12].nm_MutualExclude = 2097150;
	tempGlobal_ARRAY_OF_newmenu [12].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [13].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [13].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [13].nm_Label = (char*) "Australia (Sydney/Melbourne)";
	tempGlobal_ARRAY_OF_newmenu [13].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [13].nm_MutualExclude = 2097149;
	tempGlobal_ARRAY_OF_newmenu [13].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [14].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [14].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [14].nm_Label = (char*) "Australia (Tasmania/NSW)";
	tempGlobal_ARRAY_OF_newmenu [14].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [14].nm_MutualExclude = 2097147;
	tempGlobal_ARRAY_OF_newmenu [14].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [15].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [15].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [15].nm_Label = (char*) "Azerbaijan";
	tempGlobal_ARRAY_OF_newmenu [15].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [15].nm_MutualExclude = 2097143;
	tempGlobal_ARRAY_OF_newmenu [15].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [16].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [16].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [16].nm_Label = (char*) "Brazil";
	tempGlobal_ARRAY_OF_newmenu [16].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [16].nm_MutualExclude = 2097135;
	tempGlobal_ARRAY_OF_newmenu [16].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [17].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [17].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [17].nm_Label = (char*) "Newfoundland/Mexico";
	tempGlobal_ARRAY_OF_newmenu [17].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [17].nm_MutualExclude = 2097119;
	tempGlobal_ARRAY_OF_newmenu [17].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [18].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [18].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [18].nm_Label = (char*) "Chile";
	tempGlobal_ARRAY_OF_newmenu [18].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [18].nm_MutualExclude = 2097087;
	tempGlobal_ARRAY_OF_newmenu [18].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [19].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [19].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [19].nm_Label = (char*) "Cuba/Turks/Caicos";
	tempGlobal_ARRAY_OF_newmenu [19].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [19].nm_MutualExclude = 2097023;
	tempGlobal_ARRAY_OF_newmenu [19].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [20].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [20].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [20].nm_Label = (char*) "Egypt";
	tempGlobal_ARRAY_OF_newmenu [20].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [20].nm_MutualExclude = 2096895;
	tempGlobal_ARRAY_OF_newmenu [20].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [21].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [21].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [21].nm_Label = (char*) "Europe";
	tempGlobal_ARRAY_OF_newmenu [21].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [21].nm_MutualExclude = 2096639;
	tempGlobal_ARRAY_OF_newmenu [21].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [22].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [22].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [22].nm_Label = (char*) "Falklands";
	tempGlobal_ARRAY_OF_newmenu [22].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [22].nm_MutualExclude = 2096127;
	tempGlobal_ARRAY_OF_newmenu [22].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [23].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [23].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [23].nm_Label = (char*) "Fiji";
	tempGlobal_ARRAY_OF_newmenu [23].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [23].nm_MutualExclude = 2095103;
	tempGlobal_ARRAY_OF_newmenu [23].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [24].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [24].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [24].nm_Label = (char*) "Jordan";
	tempGlobal_ARRAY_OF_newmenu [24].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [24].nm_MutualExclude = 2093055;
	tempGlobal_ARRAY_OF_newmenu [24].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [25].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [25].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [25].nm_Label = (char*) "Lebanon/Mongol";
	tempGlobal_ARRAY_OF_newmenu [25].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [25].nm_MutualExclude = 2088959;
	tempGlobal_ARRAY_OF_newmenu [25].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [26].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [26].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [26].nm_Label = (char*) "New Zealand";
	tempGlobal_ARRAY_OF_newmenu [26].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [26].nm_MutualExclude = 2080767;
	tempGlobal_ARRAY_OF_newmenu [26].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [27].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [27].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [27].nm_Label = (char*) "Paraguay";
	tempGlobal_ARRAY_OF_newmenu [27].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [27].nm_MutualExclude = 2064383;
	tempGlobal_ARRAY_OF_newmenu [27].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [28].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [28].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [28].nm_Label = (char*) "Russia/Asia/Kirgizstan";
	tempGlobal_ARRAY_OF_newmenu [28].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [28].nm_MutualExclude = 2031615;
	tempGlobal_ARRAY_OF_newmenu [28].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [29].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [29].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [29].nm_Label = (char*) "United Kingdom";
	tempGlobal_ARRAY_OF_newmenu [29].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [29].nm_MutualExclude = 1966079;
	tempGlobal_ARRAY_OF_newmenu [29].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [30].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [30].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [30].nm_Label = (char*) "USA and Canada";
	tempGlobal_ARRAY_OF_newmenu [30].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [30].nm_MutualExclude = 1835007;
	tempGlobal_ARRAY_OF_newmenu [30].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [31].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [31].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [31].nm_Label = (char*) "Zion";
	tempGlobal_ARRAY_OF_newmenu [31].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [31].nm_MutualExclude = 1572863;
	tempGlobal_ARRAY_OF_newmenu [31].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [32].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [32].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [32].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [32].nm_MutualExclude = 1048575;
	tempGlobal_ARRAY_OF_newmenu [32].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [33].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [33].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [33].nm_Label = NM_BARLABEL;
	tempGlobal_ARRAY_OF_newmenu [33].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [33].nm_Flags = (UWORD) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [33].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [33].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [34].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [34].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [34].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [34].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [34].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [35].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [35].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [35].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [35].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [35].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [36].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [36].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [36].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [36].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [36].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [37].nm_Type = (UBYTE) (long) NM_ITEM;
	tempGlobal_ARRAY_OF_newmenu [37].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [37].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [37].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [37].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [38].nm_Type = (UBYTE) (long) NM_ITEM;
	tempGlobal_ARRAY_OF_newmenu [38].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [38].nm_CommKey = (char*) "C";
	tempGlobal_ARRAY_OF_newmenu [38].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [38].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [39].nm_Type = (UBYTE) (long) NM_ITEM;
	tempGlobal_ARRAY_OF_newmenu [39].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [39].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [39].nm_Flags = (UWORD) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [39].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [39].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [40].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [40].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [40].nm_Label = (char*) "sntp";
	tempGlobal_ARRAY_OF_newmenu [40].nm_CommKey = (char*) "N";
	tempGlobal_ARRAY_OF_newmenu [40].nm_MutualExclude = 14;
	tempGlobal_ARRAY_OF_newmenu [40].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [41].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [41].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [41].nm_Label = (char*) "time/udp";
	tempGlobal_ARRAY_OF_newmenu [41].nm_CommKey = (char*) "U";
	tempGlobal_ARRAY_OF_newmenu [41].nm_MutualExclude = 13;
	tempGlobal_ARRAY_OF_newmenu [41].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [42].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [42].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [42].nm_Label = (char*) "time/tcp";
	tempGlobal_ARRAY_OF_newmenu [42].nm_CommKey = (char*) "T";
	tempGlobal_ARRAY_OF_newmenu [42].nm_MutualExclude = 11;
	tempGlobal_ARRAY_OF_newmenu [42].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [43].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [43].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [43].nm_Label = (char*) "daytime";
	tempGlobal_ARRAY_OF_newmenu [43].nm_CommKey = (char*) "D";
	tempGlobal_ARRAY_OF_newmenu [43].nm_MutualExclude = 7;
	tempGlobal_ARRAY_OF_newmenu [43].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [44].nm_Type = (UBYTE) (long) NM_ITEM;
	tempGlobal_ARRAY_OF_newmenu [44].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [44].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [44].nm_Flags = (UWORD) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [44].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [44].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [45].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [45].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [45].nm_Label = (char*) "+";
	tempGlobal_ARRAY_OF_newmenu [45].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [45].nm_MutualExclude = 2;
	tempGlobal_ARRAY_OF_newmenu [45].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [46].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [46].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [46].nm_Label = (char*) "-";
	tempGlobal_ARRAY_OF_newmenu [46].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [46].nm_MutualExclude = 1;
	tempGlobal_ARRAY_OF_newmenu [46].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [47].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [47].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [47].nm_Label = NM_BARLABEL;
	tempGlobal_ARRAY_OF_newmenu [47].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [47].nm_Flags = (UWORD) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [47].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [47].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [48].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [48].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [48].nm_Label = (char*) "0";
	tempGlobal_ARRAY_OF_newmenu [48].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [48].nm_MutualExclude = 65528-8;
	tempGlobal_ARRAY_OF_newmenu [48].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [49].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [49].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [49].nm_Label = (char*) "1";
	tempGlobal_ARRAY_OF_newmenu [49].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [49].nm_MutualExclude = 65528-16;
	tempGlobal_ARRAY_OF_newmenu [49].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [50].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [50].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [50].nm_Label = (char*) "2";
	tempGlobal_ARRAY_OF_newmenu [50].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [50].nm_MutualExclude = 65528-32;
	tempGlobal_ARRAY_OF_newmenu [50].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [51].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [51].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [51].nm_Label = (char*) "3";
	tempGlobal_ARRAY_OF_newmenu [51].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [51].nm_MutualExclude = 65528-64;
	tempGlobal_ARRAY_OF_newmenu [51].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [52].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [52].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [52].nm_Label = (char*) "4";
	tempGlobal_ARRAY_OF_newmenu [52].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [52].nm_MutualExclude = 65528-128;
	tempGlobal_ARRAY_OF_newmenu [52].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [53].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [53].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [53].nm_Label = (char*) "5";
	tempGlobal_ARRAY_OF_newmenu [53].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [53].nm_MutualExclude = 65528-256;
	tempGlobal_ARRAY_OF_newmenu [53].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [54].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [54].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [54].nm_Label = (char*) "6";
	tempGlobal_ARRAY_OF_newmenu [54].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [54].nm_MutualExclude = 65528-512;
	tempGlobal_ARRAY_OF_newmenu [54].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [55].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [55].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [55].nm_Label = (char*) "7";
	tempGlobal_ARRAY_OF_newmenu [55].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [55].nm_MutualExclude = 65528-1024;
	tempGlobal_ARRAY_OF_newmenu [55].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [56].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [56].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [56].nm_Label = (char*) "8";
	tempGlobal_ARRAY_OF_newmenu [56].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [56].nm_MutualExclude = 65528-2048;
	tempGlobal_ARRAY_OF_newmenu [56].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [57].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [57].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [57].nm_Label = (char*) "9";
	tempGlobal_ARRAY_OF_newmenu [57].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [57].nm_MutualExclude = 65528-4096;
	tempGlobal_ARRAY_OF_newmenu [57].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [58].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [58].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [58].nm_Label = (char*) "10";
	tempGlobal_ARRAY_OF_newmenu [58].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [58].nm_MutualExclude = 65528-8192;
	tempGlobal_ARRAY_OF_newmenu [58].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [59].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [59].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [59].nm_Label = (char*) "11";
	tempGlobal_ARRAY_OF_newmenu [59].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [59].nm_MutualExclude = 65528-16384;
	tempGlobal_ARRAY_OF_newmenu [59].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [60].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [60].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [60].nm_Label = (char*) "12";
	tempGlobal_ARRAY_OF_newmenu [60].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [60].nm_MutualExclude = 65528-32768;
	tempGlobal_ARRAY_OF_newmenu [60].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [61].nm_Type = (UBYTE) (long) NM_ITEM;
	tempGlobal_ARRAY_OF_newmenu [61].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [61].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [61].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [61].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [62].nm_Type = (UBYTE) (long) NM_ITEM;
	tempGlobal_ARRAY_OF_newmenu [62].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [62].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [62].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [62].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [63].nm_Type = (UBYTE) (long) NM_ITEM;
	tempGlobal_ARRAY_OF_newmenu [63].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [63].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [63].nm_Flags = (UWORD) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [63].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [63].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [64].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [64].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [64].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [64].nm_MutualExclude = 12;
	tempGlobal_ARRAY_OF_newmenu [64].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [65].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [65].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [65].nm_Label = NM_BARLABEL;
	tempGlobal_ARRAY_OF_newmenu [65].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [65].nm_Flags = (UWORD) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [65].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [65].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [66].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [66].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [66].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [66].nm_MutualExclude = 1;
	tempGlobal_ARRAY_OF_newmenu [66].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [67].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [67].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [67].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [67].nm_MutualExclude = 1;
	tempGlobal_ARRAY_OF_newmenu [67].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [68].nm_Type = (UBYTE) (long) NM_ITEM;
	tempGlobal_ARRAY_OF_newmenu [68].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [68].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [68].nm_Flags = (UWORD) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [68].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [68].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [69].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [69].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [69].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [69].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [69].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [70].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [70].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [70].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [70].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [70].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [71].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [71].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [71].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [71].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [71].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [72].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [72].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [72].nm_Label = NM_BARLABEL;
	tempGlobal_ARRAY_OF_newmenu [72].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [72].nm_Flags = (UWORD) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [72].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [72].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [73].nm_Type = (UBYTE) (long) NM_SUB;
	tempGlobal_ARRAY_OF_newmenu [73].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [73].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [73].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [73].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [74].nm_Type = (UBYTE) (long) NM_ITEM;
	tempGlobal_ARRAY_OF_newmenu [74].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [74].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [74].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [74].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [75].nm_Type = (UBYTE) (long) NM_ITEM;
	tempGlobal_ARRAY_OF_newmenu [75].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [75].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [75].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [75].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [76].nm_Type = (UBYTE) (long) NM_ITEM;
	tempGlobal_ARRAY_OF_newmenu [76].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [76].nm_CommKey = (char*) ".";
	tempGlobal_ARRAY_OF_newmenu [76].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [76].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [77].nm_Type = (UBYTE) (long) NM_ITEM;
	tempGlobal_ARRAY_OF_newmenu [77].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [77].nm_Label = NM_BARLABEL;
	tempGlobal_ARRAY_OF_newmenu [77].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [77].nm_Flags = (UWORD) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [77].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [77].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [78].nm_Type = (UBYTE) (long) NM_ITEM;
	tempGlobal_ARRAY_OF_newmenu [78].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [78].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [78].nm_Flags = (UWORD) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [78].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [78].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [79].nm_Type = (UBYTE) (long) NM_ITEM;
	tempGlobal_ARRAY_OF_newmenu [79].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [79].nm_CommKey = (char*) "S";
	tempGlobal_ARRAY_OF_newmenu [79].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [79].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [80].nm_Type = (UBYTE) (long) NM_TITLE;
	tempGlobal_ARRAY_OF_newmenu [80].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [80].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [80].nm_Flags = (UWORD) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [80].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [80].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [81].nm_Type = (UBYTE) (long) NM_ITEM;
	tempGlobal_ARRAY_OF_newmenu [81].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [81].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [81].nm_Flags = (UWORD) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [81].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [81].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [82].nm_Type = (UBYTE) (long) NM_ITEM;
	tempGlobal_ARRAY_OF_newmenu [82].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [82].nm_CommKey = (char*) "L";
	tempGlobal_ARRAY_OF_newmenu [82].nm_Flags = (UWORD) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [82].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [82].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [83].nm_Type = (UBYTE) (long) NM_ITEM;
	tempGlobal_ARRAY_OF_newmenu [83].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [83].nm_Label = NM_BARLABEL;
	tempGlobal_ARRAY_OF_newmenu [83].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [83].nm_Flags = (UWORD) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [83].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [83].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [84].nm_Type = (UBYTE) (long) NM_ITEM;
	tempGlobal_ARRAY_OF_newmenu [84].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [84].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [84].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [84].nm_UserData = (APTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [85].nm_Type = (UBYTE) (long) NM_END;
	tempGlobal_ARRAY_OF_newmenu [85].nm_Label  = (STRPTR) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [85].nm_Label = (char*) NULL;
	tempGlobal_ARRAY_OF_newmenu [85].nm_CommKey = (char*) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [85].nm_Flags = (UWORD) (long) 0;
	tempGlobal_ARRAY_OF_newmenu [85].nm_MutualExclude = 0;
	tempGlobal_ARRAY_OF_newmenu [85].nm_UserData = (APTR) (long) 0;
	tempGlobal_ILIST71 = InitList(temp_ARRAY_OF_VALUE71 ,3);
	tempGlobal_ILIST71 [0]= GTMN_FullMenu;
	tempGlobal_ILIST71 [1]= TRUE;
	tempGlobal_ILIST71 [2]= (long) NULL;
	SetList(tempGlobal_ILIST71 ,3);
	tempGlobal_ILIST72 = InitList(temp_ARRAY_OF_VALUE72 ,5);
	tempGlobal_ILIST72 [1]= 0;
	tempGlobal_ILIST72 [2]= (long) "About FACTS...";
	SetList(tempGlobal_ILIST72 ,5);
	tempGlobal_ILIST73 = InitList(temp_ARRAY_OF_VALUE73 ,3);
	tempGlobal_ILIST73 [0]= SYS_Asynch;
	tempGlobal_ILIST73 [1]= TRUE;
	tempGlobal_ILIST73 [2]= (long) NULL;
	SetList(tempGlobal_ILIST73 ,3);
	tempGlobal_ILIST74 = InitList(temp_ARRAY_OF_VALUE74 ,5);
	tempGlobal_ILIST74 [1]= 0;
	tempGlobal_ILIST74 [2]= (long) "FACTS";
	SetList(tempGlobal_ILIST74 ,5);
	tempGlobal_ILIST75 = InitList(temp_ARRAY_OF_VALUE75 ,5);
	tempGlobal_ILIST75 [1]= 0;
	tempGlobal_ILIST75 [2]= (long) "FACTS";
	SetList(tempGlobal_ILIST75 ,5);
	tempGlobal_ILIST76 = InitList(temp_ARRAY_OF_VALUE76 ,3);
	tempGlobal_ILIST76 [0]= GTCB_Checked;
	tempGlobal_ILIST76 [2]= (long) NULL;
	SetList(tempGlobal_ILIST76 ,3);
	tempGlobal_ILIST77 = InitList(temp_ARRAY_OF_VALUE77 ,3);
	tempGlobal_ILIST77 [0]= GA_Disabled;
	tempGlobal_ILIST77 [2]= (long) NULL;
	SetList(tempGlobal_ILIST77 ,3);
	tempGlobal_ILIST78 = InitList(temp_ARRAY_OF_VALUE78 ,3);
	tempGlobal_ILIST78 [0]= GTIN_Number;
	tempGlobal_ILIST78 [2]= (long) NULL;
	SetList(tempGlobal_ILIST78 ,3);
	tempGlobal_ILIST79 = InitList(temp_ARRAY_OF_VALUE79 ,3);
	tempGlobal_ILIST79 [0]= WA_PointerDelay;
	tempGlobal_ILIST79 [1]= TRUE;
	tempGlobal_ILIST79 [2]= (long) NULL;
	SetList(tempGlobal_ILIST79 ,3);
	tempGlobal_ILIST80 = InitList(temp_ARRAY_OF_VALUE80 ,5);
	tempGlobal_ILIST80 [1]= 0;
	tempGlobal_ILIST80 [2]= (long) "FACTS";
	SetList(tempGlobal_ILIST80 ,5);
	tempGlobal_ILIST81 = InitList(temp_ARRAY_OF_VALUE81 ,5);
	tempGlobal_ILIST81 [1]= 0;
	tempGlobal_ILIST81 [2]= (long) "FACTS";
	tempGlobal_ILIST81 [4]= (long) "OK";
	SetList(tempGlobal_ILIST81 ,5);
	tempGlobal_ILIST82 = InitList(temp_ARRAY_OF_VALUE82 ,3);
	tempGlobal_ILIST82 [0]= GTTX_Text;
	tempGlobal_ILIST82 [2]= (long) NULL;
	SetList(tempGlobal_ILIST82 ,3);
	tempGlobal_ILIST83 = InitList(temp_ARRAY_OF_VALUE83 ,3);
	tempGlobal_ILIST83 [0]= GTMN_NewLookMenus;
	tempGlobal_ILIST83 [1]= TRUE;
	tempGlobal_ILIST83 [2]= (long) NULL;
	SetList(tempGlobal_ILIST83 ,3);
	tempGlobal_ILIST84 = InitList(temp_ARRAY_OF_VALUE84 ,3);
	tempGlobal_ILIST84 [0]= GTST_String;
	tempGlobal_ILIST84 [2]= (long) NULL;
	SetList(tempGlobal_ILIST84 ,3);
	tempGlobal_ILIST85 = InitList(temp_ARRAY_OF_VALUE85 ,3);
	tempGlobal_ILIST85 [0]= GTIN_Number;
	tempGlobal_ILIST85 [2]= (long) NULL;
	SetList(tempGlobal_ILIST85 ,3);
	tempGlobal_ILIST86 = InitList(temp_ARRAY_OF_VALUE86 ,3);
	tempGlobal_ILIST86 [0]= GTIN_Number;
	tempGlobal_ILIST86 [2]= (long) NULL;
	SetList(tempGlobal_ILIST86 ,3);
	tempGlobal_ILIST87 = InitList(temp_ARRAY_OF_VALUE87 ,3);
	tempGlobal_ILIST87 [0]= GTTX_Text;
	tempGlobal_ILIST87 [2]= (long) NULL;
	SetList(tempGlobal_ILIST87 ,3);
	tempGlobal_ILIST88 = InitList(temp_ARRAY_OF_VALUE88 ,5);
	tempGlobal_ILIST88 [1]= 0;
	tempGlobal_ILIST88 [2]= (long) "FACTS";
	tempGlobal_ILIST88 [4]= (long) "OK";
	SetList(tempGlobal_ILIST88 ,5);
	tempGlobal_ILIST89 = InitList(temp_ARRAY_OF_VALUE89 ,3);
	tempGlobal_ILIST89 [0]= GTMN_NewLookMenus;
	tempGlobal_ILIST89 [1]= TRUE;
	tempGlobal_ILIST89 [2]= (long) NULL;
	SetList(tempGlobal_ILIST89 ,3);
	return ;
}
void end_facts_portable()  {
	FastDispose(tempGlobal_ARRAY_OF_newmenu , -999) ;
	NULLA;
	DisposeString(frel );
	DisposeString(frev );
	DisposeString(fver );
	DisposeString(st2db );
	DisposeString(st1db );
	DisposeString(cmd );
	DisposeString(sd );
	DisposeString(defwintit );
	DisposeString(yesno );
	DisposeString(lang );
	DisposeString(ipaddr );
	DisposeString(diffbox );
	DisposeString(savekey );
	DisposeString(ntptime );
	DisposeString(tzname2 );
	DisposeString(tzname1 );
	DisposeString(errmsg );
	DisposeString(sysdate );
	DisposeString(daynames );
	DisposeString(monthnames );
	DisposeString(tmp );
	DisposeString(popkey );
	DisposeString(hour );
	DisposeString(year );
	DisposeString(month );
	DisposeString(day );
	DisposeString(wday );
	DisposeString(sec );
	DisposeString(min );
	DisposeString(temp );
	DisposeString(server );
	DisposeString(tmp2 );
	return ;
}
