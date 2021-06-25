/* FACTS: Finest Atomic Clock Time Synchroniser */

OPT PREPROCESS,LARGE
#define UNSIGNED(x) ((x) AND $FFFF)

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
    ntps:=ioreq.time.secs+2461449600
    ntpf:=ioreq.time.micro*4295 (4294.967295)
*/


MODULE     'dos/dos',
        'exec/memory',
        'dos/dosextens',
        'devices/timer',
        'exec/ports',
        'utility/date',
        'exec/io',
        'utility',
        'intuition/intuition',
        'intuition/gadgetclass',
        'libraries/gadtools',
        'intuition/screens',
        'gadtools',
        'graphics/text',
        'commodities',
        'exec/libraries',
        'libraries/commodities',
        'icon',
        'workbench/workbench',
        'workbench/startup',
        'devices/inputevent',
        'amigalib/cx',
        'resources/battclock',
	'other/battclock',
        'timer',
        'tools/arexx',
        'dos/notify',
        'als',
        'socket',
        'sys/socket',
        'netinet/in',
        'netdb',
        'wb',
        'dos/dostags',
        'sys/errno',
        'locale',
        'libraries/locale',
/*
        'libraries/amigaguide',
        'amigaguide',
*/
        'identify',
        'libraries/identify',
        'facts_const'
/*
        'exec/nodes',
        'exec/lists',
        'amigalib/lists'
*/

OBJECT ntppkt
    header:CHAR
    stratum:CHAR
    poll:CHAR
    precision:CHAR
    rootdelay:INT
    rootdelayf:INT
    rootdisp:INT
    rootdispf:INT
    refid[4]:ARRAY OF CHAR
    reftimes:LONG
    reftimef:LONG
    orgs:LONG
    orgf:LONG
    recs:LONG
    recf:LONG
    xmts:LONG
    xmtf:LONG
ENDOBJECT

OBJECT ntpctrl
    header:CHAR
    req:CHAR
    sequence:INT
    status:INT
    associd:INT
    offset:INT
    size:INT
->     data:PTR TO LONG
ENDOBJECT

DEF tmp2[100]:STRING
DEF rargs=NIL:PTR TO LONG,templ,args,lock,verstring,buffer,sock=-1
DEF server[1024]:STRING,temp[2048]:STRING,min[5]:STRING,sec[5]:STRING,wday[5]:STRING,wd,mnth
DEF day[5]:STRING,month[5]:STRING,year[5]:STRING,hour[10]:STRING,tzsecs=0
DEF tz=0,tzfile,ntphost,debug=0,noquiet=1,wbst=0
DEF ioreq:timerequest,msgport:mp,win:PTR TO window,pixsep=5,wbstart=1
DEF clkioreq:timerequest,clkmsgport:mp,lssecs=0,envioreq:timerequest,envmsgport:mp
DEF gads[31]:ARRAY OF LONG,mysc:PTR TO screen,vi,tbar,utcoffset=3600
DEF glist:PTR TO LONG,cxpri=0,popkey[256]:STRING,winopen=0,tioreq:timerequest,nullmsgport:mp
DEF broker=NIL,broker_mp=NIL:PTR TO mp,cxsigflag,enabled=TRUE
DEF tmp[100]:STRING,monthnames[100]:STRING,daynames[100]:STRING
DEF sysdate[100]:STRING,errmsg[512]:STRING,servsecs
DEF curtime,curmics=0,tzname1[3]:STRING,tzname2[4]:STRING,dst=0,utc=1,timeint=0
DEF inputevent:PTR TO inputevent,filter=NIL,menustrip=NIL,ntptime[50]:STRING
DEF measure:PTR TO timeval,timerbase:PTR TO lib,timemem:timeval,response,arexxport
DEF donotopenwin=0,winsig=NIL,filter2,savekey[256]:STRING,autodst=0
DEF tznotify:notifyrequest,notifysig,notifyport:mp,ignoretz=0,port=123,service=123
DEF ttnotify:notifyrequest,ttnotifysig,ttnotifyport:mp,smeasure:PTR TO timeval
DEF timeport=37,daytimeport=13,diffbox[100]:STRING,defuse=1,defsave=1
DEF ttsnot=0,snot=0,appmenu=NIL:PTR TO mp, appitem=NIL,timeout=60,pkt:ntppkt
DEF appmsg=NIL:PTR TO appmessage,utcneg=0,utcnum=1,sntpport=123,timeudpport=37
DEF ipaddr[20]:STRING,lastsntp=0,mode2036=0,batt2078=0,base2015=0,toolsmenu=1
DEF extrahalf=0,autodquiet=0,stgemul=0,chkstart=0,localeinfo:PTR TO locale
DEF startmonth=3,startwday=0,starthour=1,startweek=0,endmonth=11,endwday=0,endhour=2,endweek=4,hemisphere=0
DEF ostartmonth=3,ostartwday=0,ostarthour=1,ostartweek=0,oendmonth=11,oendwday=0,oendhour=2,oendweek=4,ohemisphere=0
DEF dirlock=0,dstioreq:timerequest,dstmsgport:mp,socketbase:PTR TO lib,defport=1
DEF defdaytimeport=13,deftimeport=37,deftimeudpport=37,defsntpport=123,aninc=0
DEF servmicros=0,ntpmsg:ntpctrl,winx=0,winy=-1,catalog:PTR TO catalog
DEF locstr[150]:ARRAY OF LONG,lang[2]:STRING,mcast=0,yesno[20]:STRING
DEF longtz=0,nextservice=0,context=0,trynext=0,defwintit[15]:STRING,txtxs=10
DEF alwaysupdate=0,zoomed=0,sd[30]:STRING,leapsec=0,obsession=0,txtf:PTR TO textfont
DEF socketlib,socketver,cmd[512]:STRING,servnum=0,clkdaemon=0,dstoffset=3600,nicedst=1
DEF st1db[512]:STRING,st2db[512]:STRING,nocmd=1,rcode=0,wbarg:PTR TO wbarg
DEF fver[3]:STRING,frev[3]:STRING,frel[6]:STRING,workbenchbase:PTR TO lib,wbenchMsg:PTR TO wbstartup
DEF timeenv=0,sdrift=0,sdrifts=0,mdrift=0,mdrifts=0,psecs=0,pmics=0,dadj=0,pdadj=0,guessed=0
DEF difference=0,lastsyncok=0,forcesync=0
DEF addrs=NIL:PTR TO sockaddr,addr:sockaddr_in

-> DEF node[5]:ARRAY OF ln,list:PTR TO mlh

ENUM GADG_CHECK,GADG_SERVER,GADG_SAVE,GADG_AMIGA,GADG_DIFF,GADG_USE,
    GADG_TZNAME,GADG_TZ,GADG_DST,GADG_DSTNAME,GADG_SAVECONFIG,GADG_DEFPORT,
    GADG_TIMESERVER,GADG_CANCEL,GADG_QUIT,GADG_TIMEINT,GADG_PORT
ENUM    ERR_NONE,ERR_MEM,ERR_NOHOST,ERR_NOTCP,ERR_NOCLK,ERR_DEV,ERR_UTIL,
     ERR_GDTL,ERR_CXLIB,ERR_ICON,ERR_CMDY,ERR_NOBATT,ERR_WBLIB
ENUM EVT_POPKEY,EVT_SAVEKEY

PROC main() HANDLE
DEF msg,colon,ls

verstring:='$VER: FACTS 5.9 (05.10.2012)' /* internal 5.1, release 5.0 */
fver:='5'
frev:='9'
frel:='5.07'

socketlib:='bsdsocket.library'
socketver:=4

StrCopy(defwintit,'FACTS')

StrCopy(popkey,'ctrl alt f')

IF wbmessage=NIL
    wbstart:=0
    WriteF('\s \s by Chris Young <chris@unsatisfactorysoftware.co.uk>\n',defwintit,frel)
    WriteF('© 1998-2007 Unsatisfactory Software\n\n')
ENDIF
IF (buffer:=AllocMem(101,MEMF_CLEAR))=0 THEN Raise(ERR_MEM)

IF (utilitybase:=OpenLibrary('utility.library',0))=NIL THEN Raise(ERR_UTIL)

verifyportnumbers()

locstr[MSG_TRANS_BY]:='Chris Young'
locstr[MSG_DIFF_SLOW]:='Clock was \s secs slow'
locstr[MSG_DIFF_FAST]:='Clock was \s secs fast'
locstr[MSG_IN_DST]:='You are currently in daylight saving time!'
locstr[MSG_TZ]:='Your timezone is \s (\d hrs from GMT)'
locstr[MSG_ERR_MEM]:='Not enough memory!'
locstr[MSG_ERR_NOHOST]:='Cannot connect to server'
locstr[MSG_ERR_NOTCP]:='Please start an AmiTCP compatible TCP/IP stack!'
locstr[MSG_ERR_DEV]:='Unable to open \s'
locstr[MSG_DST_SYNCNOW]:='Synchronise Now'
locstr[MENU_SPECIAL_SHOWTIME]:='Show Updating Time'
locstr[MENU_SETTINGS_ONERROR]:='On Error'
locstr[MENU_SETTINGS_TRYNEXT]:='Try Next Server'
locstr[MENU_SETTINGS_NEXTSERVICE]:='Try Next Service'
locstr[MSG_ERR_AREXX]:='FACTS requires ARexx to be active.\n\nMake sure RexxMast is running!'
locstr[MSG_ERR_NOBATT]:='Error with battery backed-up clock'
locstr[MSG_GUI_NOTCP]:='No TCP/IP stack!'
locstr[MSG_LOOKUP]:='Looking up host'
locstr[MSG_DNS_HOST_NOT_FOUND]:='Host unknown'
locstr[MSG_DNS_TRY_AGAIN]:='Temporary error'
locstr[MSG_DNS_NO_RECOVERY]:='Unexpected server failure'
locstr[MSG_DNS_NO_DATA]:='No IP address for host'
locstr[MSG_DNS_DEFAULT]:='Error with DNS lookup'
locstr[MSG_GUI_CONNECT]:='Connect'
locstr[MSG_CONNECT]:='Attempting to connect to'
locstr[MSG_RECEIVING_DATA]:='Receiving data...'
locstr[MSG_SENDING_REQUEST]:='Sending request...'
locstr[MSG_TIMED_OUT]:='Timed out'
locstr[MSG_ALARM]:='Alarm Condition'
locstr[MSG_ALARM_INFO]:='clock not synchronised'
locstr[MSG_DIFF_MATCH]:='Clocks Match!'
locstr[MSG_DIFF_SLOW]:='Clock was \d secs slow'
locstr[MSG_DIFF_FAST]:='Clock was \d secs fast'
locstr[MSG_GUI_BATTCLOCK]:='An error occured while opening\nbattclock.resource'
locstr[LAB_GADG_SERVER]:='Server:'
locstr[LAB_GADG_CHECK]:='Check'
locstr[LAB_GADG_SAVE]:='Adjust'
locstr[LAB_GADG_AMIGA]:='Amiga:'
locstr[LAB_GADG_DIFF]:='Diff:'
locstr[LAB_GADG_TZNAME]:='Timezone:'
locstr[LAB_GADG_TZ]:='±hrs'
locstr[LAB_GADG_DST]:='DST'
locstr[LAB_GADG_TIMESERVER]:='Addr:'
locstr[LAB_GADG_TIMEINT]:='Int'
locstr[LAB_GADG_DEFPORT]:='Def'
locstr[MSG_QUIT]:='Are you sure you want to quit?'
locstr[MSG_YES]:='Yes'
locstr[MSG_NO]:='No'
locstr[CX_DESCRIPTION]:='Set your clock to NTP server time'
locstr[MSG_ERR_WRITING]:='Error writing \s'
locstr[MENU_PROJECT]:='Project'
locstr[MENU_PROJECT_SERVER_INFO]:='Server Info...'
locstr[MENU_PROJECT_VERSION]:='Version/MOTD...'
locstr[MENU_PROJECT_ABOUT]:='About...'
locstr[MENU_PROJECT_HIDE]:='Hide'
locstr[MENU_PROJECT_QUIT]:='Quit'
locstr[MENU_SETTINGS]:='Settings'
locstr[MENU_SETTINGS_WRITE_TIME_TO]:='Write Time To'
locstr[MENU_SETTINGS_WRITE_SYSTEM]:='System'
locstr[MENU_SETTINGS_WRITE_BATTCLOCK]:='BattClock'
locstr[MENU_SETTINGS_AUTODST]:='AutoDST'
locstr[MENU_SETTINGS_AUTODST_OFF]:='Off'
locstr[MENU_SETTINGS_AUTODST_CUSTOM]:='Custom'
locstr[MENU_SETTINGS_AUTODST_QUIET]:='Quiet'
locstr[MENU_SETTINGS_AUTODST_EMULATE]:='Emulate SummerTimeGuard'
locstr[MENU_SETTINGS_IGNORETZ]:='Ignore TZ Changes'
locstr[MENU_SETTINGS_SERVICE]:='Service'
locstr[MENU_SETTINGS_OFFSET]:='Timeserver Offset'
locstr[MENU_SETTINGS_CHECK_ON_STARTUP]:='Check On Startup'
locstr[MENU_SETTINGS_ANINC]:='Use Aninc'
locstr[MENU_SETTINGS_COMPATIBILITY]:='Compatibility'
locstr[MENU_SETTINGS_COMPAT_MODE]:='2036 Mode'
locstr[MENU_SETTINGS_COMPAT_BASE]:='2015 Is 2 Digit Base'
locstr[MENU_SETTINGS_COMPAT_BATTCLOCK]:='BattClock 2078'
locstr[MENU_SETTINGS_APPMENU]:='AppMenu Item'
locstr[MENU_SETTINGS_EXTRAHALF]:='Extra Half Hour'
locstr[MENU_SETTINGS_SNAPSHOT]:='Snapshot Window'
locstr[MENU_SETTINGS_SAVE]:='Save Settings'
locstr[MENU_SPECIAL]:='Special'
locstr[MENU_SPECIAL_RESET]:='Reset BattClock'
locstr[MENU_SPECIAL_READ]:='Read BattClock Time'
locstr[MENU_SPECIAL_DOWNLOAD]:='Download Databases'
locstr[MENU_SPECIAL_BOREDOM]:='Boredom Relief...'
locstr[MSG_EH]:='Eh?'
locstr[MSG_DST_OUT]:='No longer in DST'
locstr[MSG_DST_IN]:='Now in DST'
locstr[MSG_INFO_IP]:='IP Address:'
locstr[MSG_INFO_STRATUM]:='Stratum:'
locstr[MSG_INFO_REFERENCE]:='Reference:'
locstr[MSG_INFO_POLL]:='Poll Interval:'
locstr[MSG_INFO_PRECISION]:='Precision:'
locstr[MSG_INFO_ROOT_DELAY]:='Root Delay:'
locstr[MSG_INFO_ROOT_DISP]:='Root Dispersion:'
locstr[MSG_INFO_LEAP_ADDED]:='Leap second will be added 00:00\n(last minute will have 61 seconds)'
locstr[MSG_INFO_LEAP_REMOVED]:='Leap second will be removed 00:00\n(last minute will have 59 seconds)'
locstr[MSG_INFO_ALARM]:='ALARM!'
locstr[MSG_INFO_USE_SNTP]:='Use SNTP service for more information'
locstr[MSG_OK]:='OK'
locstr[MSG_INFO_PUBLISHED]:='Published Information...'
locstr[MSG_ERR_STRATUMONE]:='Could not find Stratum 1 information file.'
locstr[MSG_ERR_STRATUMTWO]:='Could not find Stratum 2 information file.'
locstr[MSG_ASK_DOWNLOAD]:='Do you want to download this now?'
locstr[MSG_STRATUMONE_DATA]:='(Stratum 1 Database)'
locstr[MSG_STRATUMTWO_DATA]:='(Stratum 2 Database)'
locstr[MSG_INFO_NOINFO]:='No information available'
locstr[MSG_RECEIVING]:='Receiving'
locstr[MSG_ERR_STRATUM_LATEST]:='You already have the latest version\nof the Stratum \s database.'
locstr[MSG_ERR_STRATUM_OTHER]:='Stratum \s database\nError reported from server:'
locstr[MSG_RECEIVED]:='Document Received'
locstr[MSG_CANCEL]:='Cancel'
locstr[MSG_ASK_DOWNLOAD_NEWER]:='Newer data available:\nDownload?'
locstr[MSG_MOTD_NOT_AVAILABLE]:='MOTD: (not available)'
locstr[MSG_NEWER_NOT_AVAILABLE]:='A new version of FACTS is not yet available'
locstr[MSG_VERSION_CHECK]:='Checking Version...'
locstr[MSG_NEWER_AVAILABLE]:='A new version of FACTS is\navailable for download.'
locstr[MSG_ERR_CONN_DEFAULT]:='Error connecting'
locstr[MSG_ERR_DOWNLOADING_STRATUMONE]:='Error while downloading Stratum 1 information'
locstr[MSG_ERR_DOWNLOADING_STRATUMTWO]:='Error while downloading Stratum 2 information'
locstr[MSG_ERR_CONN_EBADF]:='Socket descriptor not valid'
locstr[MSG_ERR_CONN_EADDRNOTAVAIL]:='Address not available'
locstr[MSG_ERR_CONN_EAFNOSUPPORT]:='Address cannot be used with this socket'
locstr[MSG_ERR_CONN_EISCONN]:='Already connected'
locstr[MSG_ERR_CONN_ETIMEDOUT]:='Connection timed out'
locstr[MSG_ERR_CONN_ECONNREFUSED]:='Connection refused'
locstr[MSG_ERR_CONN_ENETUNREACH]:='Network unreachable'
locstr[MSG_ERR_CONN_EADDRINUSE]:='Address already in use'
locstr[MSG_ERR_CONN_EINPROGRESS]:='Socket non-blocking; connection cannot be immediately completed'
locstr[MSG_ERR_CONN_EALREADY]:='Socket non-blocking; previous connection not yet completed'
locstr[APP_MENU]:='Sync Clock'
locstr[MENU_SETTINGS_AUTODST_LONGTZ]:='Long TZ Format'
locstr[MSG_LEAPSEC]:='Leap second!'
locstr[MENU_SETTINGS_LEAPSEC]:='Obey Leap Seconds'
locstr[MENU_SETTINGS_COMPAT_CLKDAEMON]:='ClockDaemon'
locstr[MSG_ICON_NOTIFY]:='The Facts icon file has been modified.\nDo you want to update Facts with the new configuration?\nThis will lose any unsaved settings in Facts.'
locstr[MSG_ICON_NOTIFY_OPTS]:='Update|Ignore'
locstr[MENU_SETTINGS_TIMEENV]:='Save Time To Env-var'
locstr[MSG_GUESSED]:=' (guessed)'
StrCopy(lang,'en')

IF localebase:=OpenLibrary('locale.library',0)

    localeinfo:=OpenLocale(NIL)

    catalog:=OpenCatalogA(NIL,'facts.catalog',[OC_BUILTINLANGUAGE,'english',NIL])

StrCopy(lang,localeinfo.languagename,2)

-> WriteF('\s\n',localeinfo.calendartype)

FOR ls:=0 TO 134
    locstr[ls]:=GetCatalogStr(catalog,ls,locstr[ls])
ENDFOR

    StringF(daynames,'\s[3]\s[3]\s[3]\s[3]\s[3]\s[3]\s[3]',GetLocaleStr(localeinfo,ABDAY_1),
        GetLocaleStr(localeinfo,ABDAY_2),
        GetLocaleStr(localeinfo,ABDAY_3),
        GetLocaleStr(localeinfo,ABDAY_4),
        GetLocaleStr(localeinfo,ABDAY_5),
        GetLocaleStr(localeinfo,ABDAY_6),
        GetLocaleStr(localeinfo,ABDAY_7))
    StringF(monthnames,'   \s[3]\s[3]\s[3]\s[3]\s[3]\s[3]\s[3]\s[3]\s[3]\s[3]\s[3]\s[3]',GetLocaleStr(localeinfo,ABMON_1),
        GetLocaleStr(localeinfo,ABMON_2),
        GetLocaleStr(localeinfo,ABMON_3),
        GetLocaleStr(localeinfo,ABMON_4),
        GetLocaleStr(localeinfo,ABMON_5),
        GetLocaleStr(localeinfo,ABMON_6),
        GetLocaleStr(localeinfo,ABMON_7),
        GetLocaleStr(localeinfo,ABMON_8),
        GetLocaleStr(localeinfo,ABMON_9),
        GetLocaleStr(localeinfo,ABMON_10),
        GetLocaleStr(localeinfo,ABMON_11),
        GetLocaleStr(localeinfo,ABMON_12))
ELSE
    StrCopy(monthnames,'   JanFebMarAprMayJunJulAugSepOctNovDec')
    StrCopy(daynames,'SunMonTueWedThuFriSat')
ENDIF

StringF(yesno,'\s|\s',locstr[MSG_YES],locstr[MSG_NO])

IF (timemem:=AllocMem(SIZEOF timeval,MEMF_CLEAR))=0 THEN Raise(ERR_MEM)

-> readtz()
nullmsgport:=CreateMsgPort()
ioreq:=CreateIORequest(nullmsgport,SIZEOF mp)

IF OpenDevice('timer.device',0,ioreq,NIL)<>0 THEN Raise(ERR_DEV)
timerbase:=ioreq.io.device

loadserver()

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

IF wbstart=0
readtz()

    rargs:=[0,0,0,0,0,0,0,0,0,0,0,0,0]
    templ:='SERVER,PORT/K,SNTP/S,TIMETCP/S,TIMEUDP/S,DAYTIME/S,EXTRAHALF/S,AUTODST/K,GUI=STAYRESIDENT/S,INT=TIMEINTERVAL/K,OPENWIN=OPENGUI/S,CX_POPKEY/K,CLOCKDAEMON/S'
    IF args:=ReadArgs(templ,rargs,NIL)
        IF rargs[0] THEN StrCopy(server,rargs[0])
        IF rargs[6] THEN extrahalf:=1
        IF rargs[8] THEN wbst:=1
        IF rargs[7]
            autodst:=Val(rargs[7])
            IF wbst=0 THEN dstpresets(autodst)
        ENDIF
        IF rargs[9] THEN timeint:=Val(rargs[9])
        IF rargs[10]
            donotopenwin:=0
        ELSE
            donotopenwin:=1
        ENDIF
        IF rargs[11] THEN StrCopy(popkey,rargs[10])
        IF rargs[12] THEN clkdaemon:=1
        IF rargs[1]
            port:=Val(rargs[1])
            service:=Val(rargs[1])
            IF rargs[2] THEN service:=123
            IF rargs[3] THEN service:=37
            IF rargs[4] THEN service:=372
            IF rargs[5] THEN service:=13
        ELSE
            IF rargs[2]
                service:=123
                port:=123
            ENDIF
            IF rargs[3]
                service:=37
                port:=37
            ENDIF
            IF rargs[4]
                service:=372
                port:=37
            ENDIF
            IF rargs[5]
                service:=13
                port:=13
            ENDIF
        ENDIF
        
        FreeArgs(args)
    ENDIF


IF wbst
    wbstart:=2
ENDIF

rcode:=retrievetime()
IF rcode=0
    dodefop()
    displaytimes()
ELSE
    IF rcode=1
        rcode:=10
    ELSE
        rcode:=5
    ENDIF
ENDIF

ENDIF

IF wbstart THEN dowbstartup()

EXCEPT DO
    IF exception
        SELECT exception
            CASE ERR_MEM
                StrCopy(errmsg,locstr[MSG_ERR_MEM])
                rcode:=20
            CASE ERR_NOHOST
                StrCopy(errmsg,locstr[MSG_ERR_NOHOST])
                rcode:=5
            CASE ERR_NOTCP
                StrCopy(errmsg,locstr[MSG_ERR_NOTCP])
                rcode:=10
            CASE ERR_DEV
                StringF(errmsg,locstr[MSG_ERR_DEV],'timer.device')
                rcode:=20
            CASE ERR_UTIL
                StringF(errmsg,locstr[MSG_ERR_DEV],'utility.library')
                rcode:=20
            CASE ERR_WBLIB
                StringF(errmsg,locstr[MSG_ERR_DEV],'workbench.library')
                rcode:=20
            CASE ERR_GDTL
                StringF(errmsg,locstr[MSG_ERR_DEV],'gadtools.library')
                rcode:=20
            CASE ERR_CXLIB
                StringF(errmsg,locstr[MSG_ERR_DEV],'commodities.library')
                rcode:=20
            CASE ERR_ICON
                StringF(errmsg,locstr[MSG_ERR_DEV],'icon.library')
                rcode:=20
            CASE ERR_CMDY; StrCopy(errmsg,'') -> Already running!')
            CASE "DOUB"; StrCopy(errmsg,locstr[MSG_ERR_AREXX])
            CASE "SIG"; StrCopy(errmsg,locstr[MSG_ERR_AREXX])
            CASE "MEM"    ; StrCopy(errmsg,locstr[MSG_ERR_MEM])
            CASE ERR_NOBATT
                StrCopy(errmsg,locstr[MSG_ERR_NOBATT])
                rcode:=5
        ENDSELECT
        
        IF wbstart
            IF EstrLen(errmsg)>0 THEN EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS',errmsg,'OK'],0,NIL)
        ELSE
            WriteF('\s\n',errmsg)
        ENDIF
        
    ENDIF

IF wbstart

  IF appitem THEN RemoveAppMenuItem(appitem)
  IF appmenu
    -> Clear away any messages that arrived at the last moment
    -> and let Workbench know we're done with the messages
    WHILE appmsg:=GetMsg(appmenu) DO ReplyMsg(appmsg)
    DeleteMsgPort(appmenu)
  ENDIF

stopnotify()
stopttnotify()

IF arexxport THEN rx_ClosePort(arexxport)

    IF tioreq
        IF CheckIO(tioreq)=0
            AbortIO(tioreq)
            WaitIO(tioreq)
        ENDIF
        CloseDevice(tioreq)
        DeleteIORequest(tioreq)
        IF msgport THEN DeleteMsgPort(msgport)
    ENDIF
    
    IF clkioreq
        IF CheckIO(clkioreq)=0
            AbortIO(clkioreq)
            WaitIO(clkioreq)
        ENDIF
        CloseDevice(clkioreq)
        DeleteIORequest(clkioreq)
        IF clkmsgport THEN DeleteMsgPort(clkmsgport)
    ENDIF

    IF envioreq
        IF CheckIO(envioreq)=0
            AbortIO(envioreq)
            WaitIO(envioreq)
        ENDIF
        CloseDevice(envioreq)
        DeleteIORequest(envioreq)
        IF envmsgport THEN DeleteMsgPort(envmsgport)
    ENDIF

killautodsttimer()

-> IF inputevent THEN freeIEvents(inputevent)
IF broker THEN DeleteCxObjAll(broker)
IF broker_mp
    WHILE msg:=GetMsg(broker_mp) DO ReplyMsg(msg)
    DeleteMsgPort(broker_mp)
ENDIF

IF mysc THEN UnlockPubScreen(NIL,mysc)
IF win THEN CloseWindow(win)

ENDIF -> end wbmessage stuff

IF ioreq THEN CloseDevice(ioreq)

IF wbstart
-> **** bodges up CLI interface for some reason
    IF ioreq THEN DeleteIORequest(ioreq)
ENDIF

IF nullmsgport
    WHILE msg:=GetMsg(nullmsgport) DO ReplyMsg(msg)
    DeleteMsgPort(nullmsgport)
ENDIF

    IF buffer THEN FreeMem(buffer,101)

    IF timemem THEN FreeMem(timemem,SIZEOF timeval)

-> IF sock THEN CloseSocket(sock)
-> IF tcp THEN Close(tcp)

IF dirlock THEN UnLock(dirlock)

IF localeinfo THEN CloseLocale(localeinfo)

/*
IF amigaguidebase
    CloseAmigaGuide(context)
    CloseLibrary(amigaguidebase)
ENDIF
*/

    IF identifybase THEN CloseLibrary(identifybase)
    IF localebase THEN CloseLibrary(localebase)
    IF workbenchbase THEN CloseLibrary(workbenchbase)
    IF socketbase THEN CloseLibrary(socketbase)
    IF utilitybase THEN CloseLibrary(utilitybase)
    IF gadtoolsbase THEN CloseLibrary(gadtoolsbase)
    IF cxbase THEN CloseLibrary(cxbase)
    IF iconbase THEN CloseLibrary(iconbase)
    IF alsbase THEN CloseLibrary(alsbase)
ENDPROC rcode

PROC dowbstartup()
-> tint
msgport:=CreateMsgPort()
tioreq:=CreateIORequest(msgport,SIZEOF mp)
IF OpenDevice('timer.device',UNIT_VBLANK,tioreq,NIL)<>0 THEN Raise(ERR_DEV)

-> updating clock
clkmsgport:=CreateMsgPort()
clkioreq:=CreateIORequest(clkmsgport,SIZEOF mp)
IF OpenDevice('timer.device',UNIT_VBLANK,clkioreq,NIL)<>0 THEN Raise(ERR_DEV)
startclkint()

-> autodst
dstmsgport:=CreateMsgPort()
dstioreq:=CreateIORequest(dstmsgport,SIZEOF mp)
IF OpenDevice('timer.device',UNIT_WAITUNTIL,dstioreq,NIL)<>0 THEN Raise(ERR_DEV)

IF (gadtoolsbase:=OpenLibrary('gadtools.library',0))=NIL THEN Raise(ERR_GDTL)
IF (cxbase:=OpenLibrary('commodities.library',0))=NIL THEN Raise(ERR_CXLIB)
IF (iconbase:=OpenLibrary('icon.library',0))=NIL THEN Raise(ERR_ICON)
IF (workbenchbase:=OpenLibrary('workbench.library',0))=NIL THEN Raise(ERR_WBLIB)
alsbase:=OpenLibrary('als.library',6)

IF wbstart=1
    wbenchMsg:=wbmessage
    wbarg:=wbenchMsg.arglist

readtooltypes()
readtz()
loadserver()

-> env-var
envmsgport:=CreateMsgPort()
envioreq:=CreateIORequest(envmsgport,SIZEOF mp)
IF OpenDevice('timer.device',UNIT_VBLANK,envioreq,NIL)<>0 THEN Raise(ERR_DEV)
IF timeenv THEN startenvint()

ENDIF

setupgadgets()
IF mysc
    UnlockPubScreen(NIL,mysc)
    mysc:=NIL
ENDIF
addcommodity()
IF chkstart THEN syncclock()
IF batt2078 THEN getclock()
IF autodst THEN dstpresets(autodst)
checkoldleapsecs()

->chkautodst_generic()

IF ignoretz=0 THEN setupnotify()

actuallydosomething()

ENDPROC

PROC retrievetime()
DEF servertime:clockdata,sock,conn,size
DEF remote=NIL:PTR TO hostent
DEF url=NIL:PTR TO in_addr
DEF fhand

-> WriteF('\s\n',server)

IF service=13 THEN utc:=2 ELSE utc:=1

lastsntp:=0
lastsyncok:=0

IF winopen THEN SetWindowPointerA(win,[WA_BUSYPOINTER,TRUE,WA_POINTERDELAY,TRUE,NIL])

IF (socketbase:=OpenLibrary(socketlib,socketver))=0
    IF wbstart
        -> *** no TCP/IP stack! message ***
->        EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS','Please start your TCP/IP stack!','OK'],0,NIL)
        StrCopy(ntptime,locstr[MSG_GUI_NOTCP])
		IF winopen
		    Gt_SetGadgetAttrsA(gads[GADG_SERVER],win,NIL,[GTTX_TEXT,ntptime,NIL])
	    	SetWindowPointerA(win,[WA_POINTERDELAY,TRUE,NIL])
		ENDIF
        
    ELSE
        WriteF(locstr[MSG_GUI_NOTCP])
        WriteF('\n\n')
;WriteF('********')
;         Raise(ERR_NOTCP)
    ENDIF

	RETURN 1

ENDIF
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

	IF wbstart
    -> *** show attempting to connect in server time box ***
->    WriteF('attempting to connect...')
		StrCopy(ntptime,locstr[MSG_LOOKUP])
		IF winopen THEN Gt_SetGadgetAttrsA(gads[GADG_SERVER],win,NIL,[GTTX_TEXT,ntptime,NIL])

	ELSE
    	WriteF(locstr[MSG_LOOKUP])
    	WriteF(' \s...\n\n',server)
	ENDIF

IF addrs=NIL
	remote := Gethostbyname(server)

	IF remote=0
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
    		SetWindowPointerA(win,[WA_POINTERDELAY,TRUE,NIL])
		ELSE
    		IF wbstart=0 THEN WriteF('\s\n',ntptime)
		ENDIF

		closetcp()
		RETURN 3

-> addr.addr.addr:=Inet_addr(server)
->    Raise(ERR_MEM)

	ELSE

url:=addr.addr

url:=remote.addr_list[0] ->[0] in_addr   Inet_addr(remote. etc)

CopyMem(url,addr.addr,SIZEOF in_addr)

-> CopyMem(remote.addr_list,url,10)

	ENDIF

ENDIF

  addr.port := port -> Htons(port)
  addr.family := AF_INET
  addr.len:=SIZEOF sockaddr_in

IF service<100
    sock:=Socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)
ELSE
    IF mcast
        sock:=Socket(AF_INET,SOCK_DGRAM,3)
    ELSE
        sock:=Socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP)
    ENDIF
ENDIF

Setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,[timeout,0]:timeval,SIZEOF timeval)


addrs:=addr

-> IF service<100

StrCopy(ipaddr,Inet_NtoA(addr.addr.addr))

IF wbstart
    -> *** show attempting to connect in server time box ***
->    WriteF('attempting to connect...')
StrCopy(ntptime,locstr[MSG_GUI_CONNECT])
StrAdd(ntptime,' ')
StrAdd(ntptime,ipaddr)
IF winopen THEN Gt_SetGadgetAttrsA(gads[GADG_SERVER],win,NIL,[GTTX_TEXT,ntptime,NIL])

ELSE
    WriteF(locstr[MSG_CONNECT])
    WriteF(' \s...\n\n',ipaddr)
ENDIF

conn:=Connect(sock,addrs,SIZEOF sockaddr_in)
IF conn=-1
    connecterror()
    closetcp()
    RETURN 2
ENDIF

-> service<100 ENDIF

IF service<100
    StrCopy(ntptime,locstr[MSG_RECEIVING_DATA])
ELSE
    StrCopy(ntptime,locstr[MSG_SENDING_REQUEST])
ENDIF

    IF wbstart
        IF winopen THEN Gt_SetGadgetAttrsA(gads[GADG_SERVER],win,NIL,[GTTX_TEXT,ntptime,NIL])
    ELSE
        WriteF('\s\n',ntptime)
    ENDIF

-> check current time
-> GetSysTime(measure)

ioreq.io.command:=TR_GETSYSTIME

DoIO(ioreq)

CopyMem(ioreq.time,timemem,SIZEOF timeval)

measure:=timemem

curtime:=measure.secs
curmics:=measure.micro

-> end checktime

IF service<100
    size:=Recv(sock,buffer,50,NIL)
ENDIF
IF service=372
    Send(sock,' ',1,NIL)

    StrCopy(ntptime,locstr[MSG_RECEIVING_DATA])
    IF wbstart
        IF winopen THEN Gt_SetGadgetAttrsA(gads[GADG_SERVER],win,NIL,[GTTX_TEXT,ntptime,NIL])
    ELSE
        WriteF('\s\n',ntptime)
    ENDIF

    size:=Recv(sock,buffer,4,NIL)
ENDIF


/******* debug - possibly micro compensation code in future
    ioreq.io.command:=TR_GETSYSTIME
    DoIO(ioreq)
    WriteF('\n\d \d',ioreq.time.secs,ioreq.time.micro)
****/

IF service=123

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

    pkt.header:=%11001011
    pkt.stratum:=0
    pkt.poll:=0
    pkt.precision:=0
    pkt.rootdelay:=0
    pkt.rootdelayf:=0
    pkt.rootdisp:=0
    pkt.rootdispf:=0
    pkt.refid[0]:=0
    pkt.refid[1]:=0
    pkt.refid[2]:=0
    pkt.refid[3]:=0
    pkt.reftimes:=0
    pkt.reftimef:=0
    pkt.orgs:=0
    pkt.orgf:=0
    pkt.recs:=0
    pkt.recf:=0
    pkt.xmts:=measure.secs+2461449600-tzsecs
    pkt.xmtf:=Div(Shl(measure.micro,12),1000000)
  ->  pkt.xmtf:=measure.micro*4295

->WriteF('\d\n',pkt.xmtf)

    Send(sock,pkt,48,NIL)

    StrCopy(ntptime,locstr[MSG_RECEIVING_DATA])
    IF wbstart
        IF winopen THEN Gt_SetGadgetAttrsA(gads[GADG_SERVER],win,NIL,[GTTX_TEXT,ntptime,NIL])
    ELSE
        WriteF('\s\n',ntptime)
    ENDIF

    size:=Recv(sock,pkt,48,NIL)

ENDIF -> end SNTP

IF size<1
    StrCopy(ntptime,locstr[MSG_TIMED_OUT])
    IF wbstart
        IF winopen
            Gt_SetGadgetAttrsA(gads[GADG_SERVER],win,NIL,[GTTX_TEXT,ntptime,NIL])
            SetWindowPointerA(win,[WA_POINTERDELAY,TRUE,NIL])
        ENDIF
    ELSE
        WriteF('\s\n',ntptime)
    ENDIF
    closetcp()
    RETURN 4
ENDIF

-> IF sock THEN CloseSocket(sock)
-> IF sock2 THEN CloseSocket(sock2)

-> IF debug=0 THEN Raise(ERR_MEM)

IF service=37
    gettimeport()
    RETURN
ENDIF

IF service=372
    gettimeport()
    RETURN
ENDIF

IF service=123
    lastsntp:=1
    leapsec:=Shr(pkt.header,6)
-> WriteF('\d\n',leapsec)
    IF leapsec=3 -> Alarm Condition
        StrCopy(ntptime,locstr[MSG_ALARM])
        IF wbstart
            IF winopen
                Gt_SetGadgetAttrsA(gads[GADG_SERVER],win,NIL,[GTTX_TEXT,ntptime,NIL])
                SetWindowPointerA(win,[WA_POINTERDELAY,TRUE,NIL])
            ENDIF
        ELSE
            WriteF('\s: \s\n',ntptime,locstr[MSG_ALARM_INFO])
        ENDIF
        closetcp()
        RETURN 5
    ELSE
IF debug THEN leapsec:=1 /******* DEBUG *******/
           IF leapsec THEN startleapsec()
    ENDIF
    gettimeport()
    RETURN

	IF pkt.stratum=0 /* KoD */
		StrCopy(ntptime,pkt.refid,4)
	   IF wbstart
            IF winopen
                Gt_SetGadgetAttrsA(gads[GADG_SERVER],win,NIL,[GTTX_TEXT,ntptime,NIL])
                SetWindowPointerA(win,[WA_POINTERDELAY,TRUE,NIL])
            ENDIF
        ELSE
            WriteF('KoD: \s\n',ntptime)
        ENDIF
        closetcp()
        RETURN 10
	ENDIF


ENDIF

StrCopy(ntptime,buffer)

IF size<3
-> EstrLen(ntptime)<3
    typetwo()
    RETURN
ENDIF

closetcp()

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
-> *** get current system time ***
/*
ioreq.io.command:=TR_GETSYSTIME

DoIO(ioreq)

curtime:=ioreq.time.secs

-> SubTime(ioreq.time,measure)

response:=(ioreq.time.secs-measure.secs)/2
*/
response:=0
-> microtime:=ioreq.time.micro/2

-> WriteF('response \d\n',response)

-> curmicros:=ioreq.time.micro
-> *** got it! ***


MidStr(hour,ntptime,11,2)
MidStr(min,ntptime,14,2)
MidStr(sec,ntptime,17,2)
MidStr(day,ntptime,8,2)
MidStr(month,ntptime,4,3)
MidStr(year,ntptime,20,4)
MidStr(wday,ntptime,0,3)

-> WriteF('\s \s \s\n',sec,min,wday)


mnth:=(InStr(monthnames,month))/3

wd:=(InStr(daynames,wday))/3

servertime.sec:=Val(sec)
servertime.min:=Val(min)
servertime.hour:=Val(hour)
servertime.mday:=Val(day)
servertime.month:=mnth
servertime.year:=Val(year)
servertime.wday:=wd

-> WriteF('\d \d \d \d \d \d \d\n',servertime.sec,servertime.min,servertime.hour,servertime.mday,servertime.month,servertime.year,servertime.wday)

-> curtime.micro:=???

IF utc=2 THEN response:=response-utcoffset

servsecs:=CheckDate(servertime)-tzsecs+response

IF extrahalf
    IF tzsecs<0 THEN servsecs:=servsecs+1800 ELSE servsecs:=servsecs-1800
ENDIF

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
-> setdate()

IF winopen THEN SetWindowPointerA(win,[WA_POINTERDELAY,TRUE,NIL])

ENDPROC

PROC typetwo()
DEF servertime:clockdata

-> size:=
Recv(sock,buffer,50,NIL)

-> Fgets(tcp,buffer,50)

StrCopy(ntptime,buffer)

-> WriteF('\s\n',ntptime)

closetcp()

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

->deal with UTC0 later

-> *** get current system time ***
/*

ioreq.io.command:=TR_GETSYSTIME

DoIO(ioreq)

curtime:=ioreq.time.secs

response:=(ioreq.time.secs-measure.secs)/2
*/

response:=0

-> *** got it! ***

MidStr(hour,ntptime,15,2)
MidStr(min,ntptime,18,2)
MidStr(sec,ntptime,21,2)
MidStr(day,ntptime,12,2)
MidStr(month,ntptime,9,3)
MidStr(year,ntptime,6,2) -> TWO DIGITS FOR YEAR!
-> MidStr(wday,ntptime,0,3) -> none!

-> WriteF('\s \s \s\n',sec,min,wday)


-> mnth:=(InStr(monthnames,month))/3

-> wd:=(InStr(daynames,wday))/3

IF base2015
    IF Val(year)<15 THEN wd:=Val(year)+2100 ELSE wd:=Val(year)+2000
ELSE
    IF Val(year)<78 THEN wd:=Val(year)+2000 ELSE wd:=Val(year)+1900
ENDIF

servertime.sec:=Val(sec)
servertime.min:=Val(min)
servertime.hour:=Val(hour)
servertime.mday:=Val(day)
servertime.month:=Val(month)
servertime.year:=wd
servertime.wday:=0

-> WriteF('\d \d \d \d \d \d \d\n',servertime.sec,servertime.min,servertime.hour,servertime.mday,servertime.month,servertime.year,servertime.wday)

-> Show server time

/********* UTC=0 section! ******
IF utc=0
    servsecs:=CheckDate(servertime)+response
    StrCopy(ntptime,sectostr(servsecs))
    IF winopen THEN Gt_SetGadgetAttrsA(gads[GADG_SERVER],win,NIL,[GTTX_TEXT,ntptime,NIL])
ENDIF
****** end utc=0 *******/

IF utc=2 THEN response:=response-utcoffset

servsecs:=CheckDate(servertime)-tzsecs+response
IF extrahalf
    IF tzsecs<0 THEN servsecs:=servsecs+1800 ELSE servsecs:=servsecs-1800
ENDIF

IF winopen THEN SetWindowPointerA(win,[WA_POINTERDELAY,TRUE,NIL])



ENDPROC




PROC displaytimes(returnsystime=0,gmtadj=0)
DEF servtv:timeval

IF winopen THEN SetWindowPointerA(win,[WA_BUSYPOINTER,TRUE,WA_POINTERDELAY,TRUE,NIL])

-> IF utc ********* UTC IS ALWAYS SET ************

/*
IF clkdaemon
    IF dst THEN servsecs:=servsecs+dstoffset
ENDIF
*/

    StrCopy(ntptime,sectostr(servsecs,gmtadj)) ->localdate

IF guessed
	StrAdd(ntptime,locstr[MSG_GUESSED])
	guessed:=0
ENDIF

->    WriteF('\s (\d)\n',localdate,servsecs)

-> IF utc
    IF winopen THEN Gt_SetGadgetAttrsA(gads[GADG_SERVER],win,NIL,[GTTX_TEXT,ntptime,NIL])
-> ENDIF **** END UTC ***************

StrCopy(sysdate,sectostr(curtime))

-> WriteF('\s (\d)\n',sysdate,curtime)

IF winopen THEN Gt_SetGadgetAttrsA(gads[GADG_AMIGA],win,NIL,[GTTX_TEXT,sysdate,NIL])


-> IF service <> 123

	servtv.secs:=servsecs
	servtv.micro:=servmicros

	smeasure:=servtv
	-> CopyMem(servtv,smeasure,SIZEOF timeval)


	SubTime(measure,smeasure)
	difference:=measure.secs

-> difference:=curtime-servsecs
-> ENDIF

IF difference=0
    StrCopy(diffbox,locstr[MSG_DIFF_MATCH])
ELSE
        IF difference<0
            StringF(diffbox,locstr[MSG_DIFF_SLOW],-difference)
        ELSE
            StringF(diffbox,locstr[MSG_DIFF_FAST],difference)
        ENDIF
ENDIF

    IF wbstart=0
        WriteF('\s\n',diffbox)
    ENDIF

IF winopen
    Gt_SetGadgetAttrsA(gads[GADG_DIFF],win,NIL,[GTTX_TEXT,diffbox,NIL])

-> format = Mon Oct 26 22:53:57 1998
    SetWindowPointerA(win,[WA_POINTERDELAY,TRUE,NIL])

ENDIF

IF returnsystime THEN RETURN sysdate
ENDPROC ntptime

PROC sectostr(secssince,gmtadj=0)
DEF datestring:clockdata

IF dadj=0 THEN gmtadj:=0

IF gmtadj
    secssince:=secssince+tzsecs
    IF extrahalf
        IF tzsecs<0 THEN secssince:=secssince-1800 ELSE secssince:=secssince+1800
    ENDIF
ENDIF

Amiga2Date(secssince,datestring)

MidStr(tmp2,daynames,datestring.wday*3,3)


StrAdd(tmp2,' ')
StrAdd(tmp2,RightStr(temp,StringF(temp,'\d',datestring.mday+100),2))

StrAdd(tmp2,' ')
StrAdd(tmp2,MidStr(temp,monthnames,datestring.month*3,3))

StrAdd(tmp2,' ')
StrAdd(tmp2,StringF(temp,'\d',datestring.year))

StrAdd(tmp2,' ')
    StrAdd(tmp2,RightStr(temp,StringF(temp,'\d',datestring.hour+100),2))
StrAdd(tmp2,':')


    StrAdd(tmp2,RightStr(temp,StringF(temp,'\d',datestring.min+100),2))


StrAdd(tmp2,':')
StrAdd(tmp2,RightStr(temp,StringF(temp,'\d',datestring.sec+100),2))


-> WriteF('\s\n',tmp2)

ENDPROC tmp2

PROC setclock(seconds)
DEF micros=0,psdrift=0,pmdrift=0,psdrifts=0,pmdrifts=0

/*
IF clkdaemon
    IF dst THEN seconds:=seconds+dstoffset
ENDIF
*/

-> IF seconds=0 THEN seconds:=servsecs  ALREADY DONE!!!!
micros:=servmicros

-> WriteF('secs: \d\nmicros: \d\n\n',seconds,micros)
/************ 4.1 TEST *********/

IF ((psecs<>0) AND (dadj=0))
/* Get the time again to ensure accuracy of difference counter and drift
GetSysTime(measure)
curtime:=measure.secs
curmics:=measure.micro*/

	pmdrift:=mdrift
	pmdrifts:=mdrifts

	mdrift:=Mul((curtime-seconds),1000000)+(curmics-micros)
	mdrifts:=(seconds-psecs)

/* incremental adjustment, not sure how to do this without using fractions*/
	IF pdadj
		mdrift:=mdrift+pmdrift
	ELSE
		mdrift:=(mdrift+pmdrift)/2
	ENDIF

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

ENDIF

	pdadj:=dadj
	dadj:=0
	psecs:=seconds
	pmics:=micros

IF defuse
    ioreq.io.command:=TR_SETSYSTIME
    ioreq.time.secs:=seconds
    IF service=123 THEN ioreq.time.micro:=micros
    DoIO(ioreq)
ENDIF

IF defsave
    IF battclockbase:=OpenResource(BATTCLOCKNAME)
        writeBattClock(seconds)
    ELSE
        IF wbstart
            EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS',locstr[MSG_GUI_BATTCLOCK],'OK'],0,NIL)
        ELSE
            Raise(ERR_NOBATT)
        ENDIF
    ENDIF
ENDIF

ENDPROC

PROC getclock()
IF battclockbase:=OpenResource(BATTCLOCKNAME)
    ioreq.time.secs:=readBattClock()
    IF batt2078
        ioreq.time.secs:=ioreq.time.secs+3155760000
    ENDIF
    ioreq.io.command:=TR_SETSYSTIME
    DoIO(ioreq)
ELSE
    IF wbstart=0
        Raise(ERR_NOBATT)
    ELSE
        EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS',locstr[MSG_GUI_BATTCLOCK],'OK'],0,NIL)
    ENDIF
ENDIF
ENDPROC

PROC resetclock()
IF battclockbase:=OpenResource(BATTCLOCKNAME)
    resetBattClock()
ELSE
    IF wbstart=0
        Raise(ERR_NOBATT)
    ELSE
        EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS',locstr[MSG_GUI_BATTCLOCK],'OK'],0,NIL)
    ENDIF
ENDIF
ENDPROC

PROC startgui()
DEF wdth,zoompos[4]:ARRAY OF INT

noquiet:=0

IF mysc=NIL THEN mysc:=LockPubScreen(NIL)

wdth:=setupgadgets()  -> was 344
menu()

zoompos[0]:=-1
zoompos[1]:=-1
zoompos[2]:=txtxs*30    -> 290 - 355
zoompos[3]:=tbar+3
zoompos[4]:=NIL

IF winopen=0

IF winy=-1 THEN winy:=tbar+3

IF win:=OpenWindowTagList(NIL,[WA_LEFT,winx,
                            WA_TOP,winy,
                            WA_INNERWIDTH,wdth,
                            WA_INNERHEIGHT,(pixsep*5)+(mysc.font.ysize*5)+2,
                            WA_IDCMP,IDCMP_CLOSEWINDOW OR
                                     IDCMP_REFRESHWINDOW OR BUTTONIDCMP
                                     OR IDCMP_MENUPICK OR IDCMP_NEWSIZE,
                            WA_GADGETS,glist,
                            WA_TITLE,defwintit,
                            WA_DRAGBAR,TRUE,
                            WA_DEPTHGADGET,TRUE,
                            WA_CLOSEGADGET,TRUE,
                            WA_ACTIVATE,TRUE,
                            WA_NEWLOOKMENUS,TRUE,
                            WA_SCREENTITLE,'FACTS  © 1998-2007 Unsatisfactory Software',
                            WA_AUTOADJUST,TRUE,
                            WA_ZOOM,zoompos,
                            NIL])

  Gt_RefreshWindow(win, NIL)

IF zoomed THEN ZipWindow(win)

IF LayoutMenusA(menustrip,vi,[GTMN_NEWLOOKMENUS,TRUE,NIL])
  SetMenuStrip(win,menustrip) -> *IF*
ENDIF

IF mysc
    UnlockPubScreen(NIL,mysc)
    mysc:=NIL
ENDIF

winsig:=Shl(1,win.userport.sigbit)
winopen:=1
ENDIF
ENDIF

ENDPROC

PROC setupgadgets()
DEF gad,maxi,butsize,bottomlinesize,intsize,widthts

IF mysc=NIL THEN mysc:=LockPubScreen(NIL)
->  tbar:=mysc.font.ysize+mysc.wbortop+1 /* "real" tbar */
  tbar:=mysc.font.ysize+mysc.wbortop-2
txtf := OpenFont(mysc.font)
txtxs:=txtf.xsize+1

  vi:=GetVisualInfoA(mysc, [NIL])
  -> GadTools gadgets require this step to be taken
  gad:=CreateContext({glist})

maxi:=Max(StrLen(locstr[LAB_GADG_SERVER]),StrLen(locstr[LAB_GADG_AMIGA]))
maxi:=Max(maxi,StrLen(locstr[LAB_GADG_DIFF]))
maxi:=Max(maxi,StrLen(locstr[LAB_GADG_TIMESERVER]))
maxi:=((maxi+1)*txtxs)

butsize:=(Max(StrLen(locstr[LAB_GADG_CHECK]),StrLen(locstr[LAB_GADG_SAVE]))*txtxs)+10
-> was 70

intsize:=(Max(StrLen(locstr[LAB_GADG_TIMEINT]),StrLen(locstr[LAB_GADG_DEFPORT]))*txtxs)+maxi

bottomlinesize:=StrLen(locstr[LAB_GADG_TZNAME])+StrLen(locstr[LAB_GADG_TZ])+StrLen(locstr[LAB_GADG_DST])+206

-> x90

  gads[GADG_TZNAME]:=CreateGadgetA(STRING_KIND, gad,
                    [(StrLen(locstr[LAB_GADG_TZNAME])+1)*txtxs,
                     (pixsep*5)+tbar+(mysc.font.ysize*4),
                     (txtxs*5)+5, (mysc.font.ysize+4),
                     locstr[LAB_GADG_TZNAME], mysc.font,
                     GADG_TZNAME, 0,
                     vi,NIL]:newgadget,
                    [GTST_STRING,tzname1,GTST_MAXCHARS,3,NIL])
                   
-> above was 45 for width
 
->x185
  gads[GADG_TZ]:=CreateGadgetA(INTEGER_KIND, gads[GADG_TZNAME],
                    [((StrLen(locstr[LAB_GADG_TZNAME])+StrLen(locstr[LAB_GADG_TZ])+6)*txtxs)+15, (pixsep*5)+tbar+(mysc.font.ysize*4),
                     (txtxs*4)+5, (mysc.font.ysize+4),
                     locstr[LAB_GADG_TZ], mysc.font,
                     GADG_TZ, 0,
                     vi,NIL]:newgadget,
                    [GTIN_NUMBER,-tz,NIL])

-> 270

  gads[GADG_DST]:=CreateGadgetA(CHECKBOX_KIND, gads[GADG_TZ],
                    [((StrLen(locstr[LAB_GADG_TZNAME])+
StrLen(locstr[LAB_GADG_TZ])+
StrLen(locstr[LAB_GADG_DST])+10)*txtxs)+30, (pixsep*5)+tbar+(mysc.font.ysize*4),
                     (mysc.font.ysize+7), (mysc.font.ysize+4),   -> was 25
                     locstr[LAB_GADG_DST], mysc.font,
                     GADG_DST, 0,
                     vi,NIL]:newgadget,
                    [GTCB_CHECKED,dst,GTCB_SCALED,TRUE,NIL])

-> 300

  gads[GADG_DSTNAME]:=CreateGadgetA(STRING_KIND, gads[GADG_DST],
                    [((StrLen(locstr[LAB_GADG_TZNAME])+
StrLen(locstr[LAB_GADG_TZ])+
StrLen(locstr[LAB_GADG_DST])+10)*txtxs)+42+mysc.font.ysize,
(pixsep*5)+tbar+(mysc.font.ysize*4),
                     (txtxs*6)+5, (mysc.font.ysize+4), -> was 45
                     '', mysc.font,
                     GADG_DSTNAME, 0,
                     vi,NIL]:newgadget,
                    [GTST_STRING,tzname2,GTST_MAXCHARS,4,
                    GA_DISABLED,dst-1,NIL])

/*
bottomlinesize:=((StrLen(locstr[LAB_GADG_TZNAME])+StrLen(locstr[LAB_GADG_TZ])+StrLen(locstr[LAB_GADG_DST])+5)*txtxs)+192+mysc.font.ysize
*/

bottomlinesize:=((StrLen(locstr[LAB_GADG_TZNAME])+StrLen(locstr[LAB_GADG_TZ])+
StrLen(locstr[LAB_GADG_DST])+16)*txtxs)+47+mysc.font.ysize

bottomlinesize:=Max(bottomlinesize,intsize+
210+(txtxs*4))
bottomlinesize:=Max(bottomlinesize,maxi+
210+butsize)

widthts:=bottomlinesize-Max(intsize+
210+(txtxs*4),maxi+205+butsize)
  gads[GADG_TIMESERVER]:=CreateGadgetA(STRING_KIND, gads[GADG_DSTNAME],
                    [maxi, (pixsep*4)+tbar+(mysc.font.ysize*3),
                     (195+widthts-(txtxs*5)), (mysc.font.ysize+4),
                     locstr[LAB_GADG_TIMESERVER], mysc.font,
                     GADG_TIMESERVER, 0,
                     vi,NIL]:newgadget,
                    [GTST_STRING,server,NIL])

-> below xposn was 233

  gads[GADG_PORT]:=CreateGadgetA(INTEGER_KIND, gads[GADG_TIMESERVER],
                    [(195+maxi-(txtxs*4))+widthts, (pixsep*4)+tbar+(mysc.font.ysize*3),
                     (txtxs*4)+5, (mysc.font.ysize+4),
                     '', mysc.font,
                     GADG_PORT, 0,
                     vi,NIL]:newgadget,
                    [GTIN_NUMBER,port,GA_DISABLED,defport,NIL])

->****
  gads[GADG_SERVER]:=CreateGadgetA(TEXT_KIND, gads[GADG_PORT],
                    [maxi, (pixsep+tbar),
                     (200+widthts), (mysc.font.ysize+4),
                     locstr[LAB_GADG_SERVER], mysc.font,
                     GADG_SERVER, 0,
                     vi,NIL]:newgadget,
                    [GTTX_BORDER,TRUE,GTTX_TEXT,ntptime,
                    NIL])
-> 130

-> x posn was 275

  gads[GADG_CHECK]:=CreateGadgetA(BUTTON_KIND, gads[GADG_SERVER],
                    [(200+maxi+5+widthts), pixsep+tbar,
                     butsize, (mysc.font.ysize+4),
                     locstr[LAB_GADG_CHECK], mysc.font,
                     GADG_CHECK, 0,
                     vi,NIL]:newgadget,
                    [NIL])
->150
  gads[GADG_SAVE]:=CreateGadgetA(BUTTON_KIND, gads[GADG_CHECK],
                    [(200+maxi+5+widthts), (pixsep*2)+tbar+(mysc.font.ysize*1),
                     butsize, (mysc.font.ysize+4),
                     locstr[LAB_GADG_SAVE], mysc.font,
                     GADG_SAVE, 0,
                     vi,NIL]:newgadget,
                    [NIL])
                    
  gads[GADG_AMIGA]:=CreateGadgetA(TEXT_KIND, gads[GADG_SAVE],
                    [maxi, (pixsep*2)+tbar+(mysc.font.ysize),
                     (200+widthts), (mysc.font.ysize+4),
                     locstr[LAB_GADG_AMIGA], mysc.font,
                     GADG_AMIGA, 0,
                     vi,NIL]:newgadget,
                    [GTTX_BORDER,TRUE,GTTX_TEXT,sysdate,
                    NIL])
                    
  gads[GADG_DIFF]:=CreateGadgetA(TEXT_KIND, gads[GADG_AMIGA],
                    [maxi, (pixsep*3)+tbar+(mysc.font.ysize*2),
                     (200+widthts), (mysc.font.ysize+4),
                     locstr[LAB_GADG_DIFF], mysc.font,
                     GADG_DIFF, 0,
                     vi,NIL]:newgadget,
                    [GTTX_BORDER,TRUE,GTTX_TEXT,diffbox,
                    NIL])
                
-> was 308

  gads[GADG_TIMEINT]:=CreateGadgetA(INTEGER_KIND, gads[GADG_DIFF],
                    [(intsize+205+widthts), (pixsep*3)+tbar+(mysc.font.ysize*2),
                     (txtxs*4)+5, (mysc.font.ysize+4),
                     locstr[LAB_GADG_TIMEINT], mysc.font,
                     GADG_TIMEINT, 0,
                     vi,NIL]:newgadget,
                    [GTIN_NUMBER,timeint,NIL])

-> was 320   width was 37
  gads[GADG_DEFPORT]:=CreateGadgetA(CHECKBOX_KIND, gads[GADG_TIMEINT],
                    [intsize+205+12+widthts, (pixsep*4)+tbar+(mysc.font.ysize*3),
                     mysc.font.ysize+7, (mysc.font.ysize+4),
                     locstr[LAB_GADG_DEFPORT], mysc.font,
                     GADG_DEFPORT, 0,
                     vi,NIL]:newgadget,
                    [GTCB_CHECKED,defport,GTCB_SCALED,TRUE,NIL])

-> width was 25
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
CloseFont(txtf)

ENDPROC bottomlinesize

PROC actuallydosomething()
  DEF imsg:PTR TO intuimessage, gad:PTR TO gadget, terminated=FALSE, class
DEF signal,msgid,msgtype,msg,clgui=0,tisig,oldtz,clksig,tmpint,envsig,colon
  DEF menunumber,menunum,itemnum,subnum,item:PTR TO menuitem
  DEF dstsig,arexxsig,rexxcmd,arexxmsg,rc,result[100]:STRING,appmenusig

    arexxport,arexxsig:=rx_OpenPort('FACTS')

-> ******* IMPORTANT! IF timeinterval is set SendIO etc etc msgport

starttint()

tisig:=Shl(1,msgport.sigbit)
clksig:=Shl(1,clkmsgport.sigbit)
dstsig:=Shl(1,dstmsgport.sigbit)
notifysig:=Shl(1,notifyport.sigbit)
ttnotifysig:=Shl(1,ttnotifyport.sigbit)
appmenusig:=Shl(1,appmenu.sigbit)
envsig:=Shl(1,envmsgport.sigbit)

IF forcesync THEN syncclock()

  REPEAT

signal:=Wait(winsig OR cxsigflag OR tisig OR arexxsig OR notifysig OR ttnotifysig OR appmenusig OR dstsig OR clksig OR envsig)

IF signal AND winsig

    -> Use Gt_GetIMsg() and Gt_ReplyIMsg() for handling IntuiMessages
    -> with GadTools gadgets.
    WHILE (terminated=FALSE) AND (imsg:=Gt_GetIMsg(win.userport))
      -> Gt_ReplyIMsg() at end of loop
      class:=imsg.class
      SELECT class
    CASE IDCMP_NEWSIZE
        SetWindowTitles(win,defwintit,-1)
->        IF win.height < (tbar+10) THEN 
-> startclkint() -> ELSE stopclkint()

      CASE IDCMP_GADGETUP  -> Buttons only report GADGETUP
        gad:=imsg.iaddress
/*
            IF gad.gadgetid=GADG_USE
                IF retrievetime()=0
                    setdate()
                displaytimes()
            ENDIF
        ENDIF
*/
            IF gad.gadgetid=GADG_SAVE THEN syncclock()

        IF gad.gadgetid=GADG_CHECK THEN syncclock(0) -> do not "dodefop"

        IF gad.gadgetid=GADG_DST
              dst:=imsg.code
              oldtz:=tz
              IF dst THEN tz:=tz-nicedst ELSE tz:=tz+nicedst
              tzsecs:=tz*3600
            adjustclock((oldtz-tz)*3600)
                 Gt_SetGadgetAttrsA(gads[GADG_DSTNAME],win,NIL,[GA_DISABLED,(dst-1),NIL])
                 Gt_SetGadgetAttrsA(gads[GADG_TZ],win,NIL,[GTIN_NUMBER,-tz,NIL])
                 savetz(1)
        ENDIF

        IF gad.gadgetid=GADG_TZNAME
            StrCopy(tzname1,gad.specialinfo::stringinfo.buffer)
            savetz(1)
        ENDIF

        IF gad.gadgetid=GADG_DSTNAME
            StrCopy(tzname2,gad.specialinfo::stringinfo.buffer)
            savetz(1)
        ENDIF

        IF gad.gadgetid=GADG_TZ
            oldtz:=tz
            tz:=-Val(gad.specialinfo::stringinfo.buffer)
            tzsecs:=tz*3600
            adjustclock((oldtz-tz)*3600)
            savetz(1) -> save ENV:TZ
        ENDIF

        IF gad.gadgetid=GADG_TIMESERVER
            StrCopy(server,gad.specialinfo::stringinfo.buffer)
        ENDIF
        
        IF gad.gadgetid=GADG_PORT
            port:=Val(gad.specialinfo::stringinfo.buffer)
            IF service=13 THEN daytimeport:=port
            IF service=37 THEN timeport:=port
            IF service=123 THEN sntpport:=port
            IF service=372 THEN timeudpport:=port
        ENDIF

        IF gad.gadgetid=GADG_DEFPORT
            defport:=imsg.code
            IF defport
                IF service=13 THEN port:=defdaytimeport
                IF service=37 THEN port:=deftimeport
                IF service=123 THEN port:=defsntpport
                IF service=372 THEN port:=deftimeudpport
            ELSE
                IF service=13 THEN port:=daytimeport
                IF service=37 THEN port:=timeport
                IF service=123 THEN port:=sntpport
                IF service=372 THEN port:=timeudpport
            ENDIF
                 Gt_SetGadgetAttrsA(gads[GADG_PORT],win,NIL,[GA_DISABLED,defport,GTIN_NUMBER,port,NIL])
        ENDIF
/*
        IF gad.gadgetid=GADG_QUIT
            IF EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS',locstr[MSG_QUIT],yesno],0,NIL) THEN terminated:=TRUE
        ENDIF
        
        IF gad.gadgetid=GADG_CANCEL
            clgui:=1 -> closegui()
        ENDIF
*/        
        IF gad.gadgetid=GADG_TIMEINT
            timeint:=Val(gad.specialinfo::stringinfo.buffer)
            starttint()
        ENDIF

      CASE IDCMP_CLOSEWINDOW
        clgui:=1 -> closegui()
      CASE IDCMP_REFRESHWINDOW
        -> This handling is REQUIRED with GadTools.

        Gt_BeginRefresh(win)
        Gt_EndRefresh(win, TRUE)

      CASE IDCMP_MENUPICK
        SetWindowPointerA(win,[WA_BUSYPOINTER,TRUE,WA_POINTERDELAY,TRUE,NIL])
        menunumber:=UNSIGNED(imsg.code)
        WHILE(menunumber<>MENUNULL)
            item:=ItemAddress(menustrip,menunumber)
            menunum:=MENUNUM(menunumber)
            itemnum:=ITEMNUM(menunumber)
            subnum:=SUBNUM(menunumber)
            
            IF menunum=0 -> Project
              IF itemnum=0 THEN sntpinfo()
           ->   IF itemnum=2 THEN checkfactsver()
              IF itemnum=2 THEN about()
              IF itemnum=4 THEN clgui:=1
              IF itemnum=5 THEN terminated:=TRUE
/*
                IF EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS',locstr[MSG_QUIT],yesno],0,NIL) THEN terminated:=TRUE
              ENDIF
*/
            ENDIF

            IF menunum=1
                IF itemnum=0
                    IF subnum=0
                        IF (item.flags AND CHECKED)
                            defuse:=1
                        ELSE
                            defuse:=0
                        ENDIF
                    ENDIF
                    IF subnum=1
                        IF (item.flags AND CHECKED)
                            defsave:=1
                        ELSE
                            defsave:=0
                        ENDIF
                    ENDIF
                ENDIF
                
                IF itemnum=1
                    IF subnum>=22
                        IF subnum=22
                            IF (item.flags AND CHECKED)
                                autodquiet:=1
                            ELSE
                                autodquiet:=0
                            ENDIF
                        ENDIF
                        
                        IF subnum=23
                            IF (item.flags AND CHECKED)
                                stgemul:=1
                                savetz()
                            ELSE
                                stgemul:=0
                                DeleteFile('ENVARC:SUMMERTIME')
                            ENDIF
                        ENDIF
                        IF subnum=24
                            IF (item.flags AND CHECKED)
                                changelong(1)
                            ELSE
                                changelong(0)
                            ENDIF
                        ENDIF
                    ELSE
                        IF (item.flags AND CHECKED)
                            IF subnum=20 -> Custom mode
                                autodst:=-1
                                dstpresets(autodst)
                            ELSE
                                autodst:=subnum
                                IF autodst=0 THEN DeleteFile('ENVARC:SUMMERTIME') ELSE dstpresets(autodst)
                            ENDIF
                        ENDIF
                    ENDIF
                ENDIF
                
                IF itemnum=2
                    IF (item.flags AND CHECKED)
                        obsession:=1
                    ELSE
                        obsession:=0
                    ENDIF
                ENDIF

                IF itemnum=3
                    IF (item.flags AND CHECKED)
                        ignoretz:=1
                        stopnotify()
                    ELSE
                        ignoretz:=0
                        setupnotify()
                    ENDIF
                ENDIF
                
                IF itemnum=4
                    IF subnum=0
                        IF (item.flags AND CHECKED)
                            service:=123
                            IF defport THEN port:=defsntpport ELSE port:=sntpport
                                 Gt_SetGadgetAttrsA(gads[GADG_PORT],win,NIL,[GTIN_NUMBER,port,NIL])
                        ENDIF
                    ENDIF

                    IF subnum=1
                        IF (item.flags AND CHECKED)
                            service:=372
                            IF defport THEN port:=deftimeudpport ELSE port:=timeudpport
                                 Gt_SetGadgetAttrsA(gads[GADG_PORT],win,NIL,[GTIN_NUMBER,port,NIL])
                        ENDIF
                    ENDIF


                    IF subnum=2
                        IF (item.flags AND CHECKED)
                            service:=37
                            IF defport THEN port:=deftimeport ELSE port:=timeport
                                 Gt_SetGadgetAttrsA(gads[GADG_PORT],win,NIL,[GTIN_NUMBER,port,NIL])
                        ENDIF
                    ENDIF
                    
                    IF subnum=3
                        IF (item.flags AND CHECKED)
                            service:=13
                            IF defport THEN port:=defdaytimeport ELSE port:=daytimeport
                                 Gt_SetGadgetAttrsA(gads[GADG_PORT],win,NIL,[GTIN_NUMBER,port,NIL])
                        ENDIF
                    ENDIF

                ENDIF

                IF itemnum=5
                    IF subnum=0
                        IF (item.flags AND CHECKED)
                            utcneg:=-1
                            utcoffset:=utcnum*3600*utcneg
                        ENDIF
                    ENDIF
                    
                    IF subnum=1
                        IF (item.flags AND CHECKED)
                            utcneg:=0
                            utcoffset:=utcnum*3600*utcneg
                        ENDIF
                    ENDIF
                    
                    IF subnum>2
                        utcnum:=subnum-3
                        utcoffset:=utcnum*3600*utcneg
                    ENDIF
                    
                ENDIF

                IF itemnum=6
                    IF (item.flags AND CHECKED) THEN chkstart:=1 ELSE chkstart:=0
                ENDIF

                IF itemnum=7
                    IF (item.flags AND CHECKED)
			timeenv:=1
			startenvint()
		 ELSE
			 timeenv:=0
			stopenvint()
		ENDIF
                ENDIF

                IF itemnum=8
                    IF subnum=0
                        IF (item.flags AND CHECKED)
                            trynext:=1
                            aninc:=0
                            nextservice:=0
                        ELSE
                            trynext:=0
                        ENDIF
                    ENDIF

                    IF subnum=2
                        IF (item.flags AND CHECKED)
                            aninc:=1
                            trynext:=0
                        ELSE
                            aninc:=0
                        ENDIF
                    ENDIF

                    IF subnum=3
                        IF (item.flags AND CHECKED)
                            nextservice:=1
                            trynext:=0
                        ELSE
                            nextservice:=0
                        ENDIF
                    ENDIF

                ENDIF

                IF itemnum=9
                    IF subnum=0
                        IF (item.flags AND CHECKED) THEN mode2036:=1 ELSE mode2036:=0
                    ENDIF

                    IF subnum=1
                        IF (item.flags AND CHECKED) THEN base2015:=1 ELSE base2015:=0
                    ENDIF
                    
                    IF subnum=2
                        IF (item.flags AND CHECKED) THEN batt2078:=1 ELSE batt2078:=0
                    ENDIF

                    IF subnum=4
                        IF (item.flags AND CHECKED)
                            clkdaemon:=1
                        ELSE
                            clkdaemon:=0
                        ENDIF
                    ENDIF
                ENDIF

                IF itemnum=10
                    IF (item.flags AND CHECKED)
                        toolsmenu:=1
                        startappmenu()
                    ELSE
                        toolsmenu:=0
                        removeappitem()
                    ENDIF
                ENDIF
                
                IF itemnum=11
                    IF (item.flags AND CHECKED)
                        extrahalf:=1
                            IF tzsecs<0 THEN adjustclock(1800) ELSE adjustclock(-1800)
                    ELSE
                        extrahalf:=0
                            IF tzsecs<0 THEN adjustclock(-1800) ELSE adjustclock(1800)
                    ENDIF
                ENDIF

                IF itemnum=12
                    IF (item.flags AND CHECKED)
                        alwaysupdate:=1
                         -> startclkint()
                    ELSE
                        alwaysupdate:=0
			-> stopclkint()
                    ENDIF
                ENDIF

                IF itemnum=14
                    -> snapshot
                    winx:=win.leftedge
                    winy:=win.topedge
                    IF win.height < (tbar+10) THEN zoomed:=1 ELSE zoomed:=0
                ENDIF

                IF itemnum=15 THEN saveconfig()
            ENDIF


            IF menunum=2 -> Special
                IF itemnum=0 THEN resetclock()
                IF itemnum=1 THEN getclock()

->                IF itemnum=2 THEN showtime()

            ->    IF itemnum=3 THEN downloadfiles()

                IF itemnum=3 THEN Als(0)
            ENDIF

            menunumber:=UNSIGNED(item.nextselect)
        ENDWHILE
        SetWindowPointerA(win,[WA_BUSYPOINTER,FALSE,NIL])
    ENDSELECT
Gt_ReplyIMsg(imsg)
ENDWHILE
ENDIF

IF signal AND cxsigflag
    WHILE msg:=GetMsg(broker_mp)
        msgid:=CxMsgID(msg)
        msgtype:=CxMsgType(msg)
        ReplyMsg(msg)
        
        SELECT msgtype
        CASE CXM_IEVENT
            SELECT msgid
            CASE EVT_POPKEY
            IF win
                WindowToFront(win)
            ELSE
                startgui()
            ENDIF
            AddIEvents(inputevent)
            CASE EVT_SAVEKEY
            syncclock()
            AddIEvents(inputevent)
            ENDSELECT
        CASE CXM_COMMAND
            SELECT msgid
            CASE CXCMD_DISABLE
            ActivateCxObj(broker,FALSE)
            enabled:=FALSE
            CASE CXCMD_ENABLE
            ActivateCxObj(broker,TRUE)
            enabled:=TRUE
            CASE CXCMD_APPEAR
            IF win
                WindowToFront(win)
            ELSE
                startgui()
            ENDIF
            CASE CXCMD_DISAPPEAR
            clgui:=1 ->closegui()
            CASE CXCMD_KILL
            terminated:=TRUE
            CASE CXCMD_UNIQUE
            IF win
                WindowToFront(win)
            ELSE
                startgui()
            ENDIF
            ENDSELECT
        ENDSELECT
    ENDWHILE
ENDIF

IF appmenusig
  WHILE (appmsg:=GetMsg(appmenu))
/*
    SELECT appmsg.id
        CASE 0
*/
syncclock()
->    ENDSELECT
    ReplyMsg(appmsg)
  ENDWHILE
ENDIF

IF arexxsig<>NIL
    IF signal AND arexxsig
        REPEAT
            IF win THEN SetWindowPointerA(win,[WA_BUSYPOINTER,TRUE,WA_POINTERDELAY,TRUE,NIL])
            arexxmsg,rexxcmd:=rx_GetMsg(arexxport)
            rc:=0
            StrCopy(result,'')
            IF arexxmsg<>NIL
                
                UpperStr(rexxcmd)
                IF StrCmp(rexxcmd,'COPYRIGHT')
                    StrCopy(result,'© 1998-2007 Unsatisfactory Software')
                ENDIF
                
                IF StrCmp(rexxcmd,'VERSION')
                    StrCopy(result,fver)
                ENDIF
                
                IF StrCmp(rexxcmd,'REVISION')
                    StrCopy(result,frev)
                ENDIF
                
                IF StrCmp(rexxcmd,'HIDE')
                    clgui:=1
                ENDIF
                
                IF StrCmp(rexxcmd,'SHOW')
                    IF win
                        WindowToFront(win)
                    ELSE
                        startgui()
                    ENDIF
                ENDIF
                
                IF StrCmp(rexxcmd,'DISABLE')
                    ActivateCxObj(broker,FALSE)
                    enabled:=FALSE
                ENDIF
                
                IF StrCmp(rexxcmd,'ENABLE')
                    ActivateCxObj(broker,TRUE)
                    enabled:=TRUE
                ENDIF
                
                IF StrCmp(rexxcmd,'QUIT')
                    terminated:=TRUE
                ENDIF
                
                IF StrCmp(rexxcmd,'GETTIME')
                    IF retrievetime()=0
                        StrCopy(result,displaytimes())
                    ELSE
                        ioreq.io.command:=TR_GETSYSTIME
                        DoIO(ioreq)
                        curtime:=ioreq.time.secs
                        StrCopy(result,sectostr(curtime))
                    ENDIF
                ENDIF

                IF StrCmp(rexxcmd,'GETGMT')
                    IF retrievetime()=0
                        StrCopy(result,displaytimes())
                    ELSE
                        ioreq.io.command:=TR_GETSYSTIME
                        DoIO(ioreq)
                        curtime:=ioreq.time.secs
                        StrCopy(result,sectostr(curtime,1))
                    ENDIF
                ENDIF
                
                IF StrCmp(rexxcmd,'SETTIME')
                    IF (rc:=retrievetime())=0
                        dodefop()
                        displaytimes()
                    ELSE
                        tryalternative()
                    ENDIF
                ENDIF

                IF StrCmp(rexxcmd,'DSTMODE',7)
                    IF StrLen(rexxcmd)>7
                        dstswitch(Val(MidStr(temp,rexxcmd,8)),1)
                    ELSE
                        StringF(result,'\d',dst)
                    ENDIF
                ENDIF

                IF StrCmp(rexxcmd,'SERVER',6)
                    IF StrLen(rexxcmd)>6
					    MidStr(temp,rexxcmd,7)
					    colon:=InStr(temp,':')
    					IF colon<>-1
        					StrCopy(server,temp,colon)
        					MidStr(temp,temp,colon+1)
        					port:=Val(temp)
					    ELSE
    	    				StrCopy(server,temp)
    					ENDIF

				        IF winopen THEN updategui()
                    ELSE
                        StringF(result,'\s',server)
                    ENDIF
                ENDIF

                IF StrCmp(rexxcmd,'SERVICE',7)
                    IF StrLen(rexxcmd)>7
					    MidStr(temp,rexxcmd,8)
		                IF StrCmp(temp,'SNTP')
							service:=123
						ENDIF
	            	    IF StrCmp(temp,'TIMEUDP')
							service:=372
						ENDIF
		                IF StrCmp(temp,'TIMETCP')
							service:=37
						ENDIF
	            	    IF StrCmp(temp,'DAYTIME')
							service:=13
						ENDIF

			            IF defport
    	    		        IF service=13 THEN port:=defdaytimeport
        	        		IF service=37 THEN port:=deftimeport
		    	            IF service=123 THEN port:=defsntpport
        			        IF service=372 THEN port:=deftimeudpport
            			ENDIF

				        IF winopen THEN updategui()
					ELSE
   	    		        IF service=13 THEN StrCopy(temp,'DAYTIME')
   	    		        IF service=37 THEN StrCopy(temp,'TIMETCP')
   	    		        IF service=372 THEN StrCopy(temp,'TIMEUDP')
   	    		        IF service=123 THEN StrCopy(temp,'SNTP')

                        StringF(result,'\s',temp)
					ENDIF
                ENDIF

                IF StrCmp(rexxcmd,'TIMEOUT',7)
                    IF StrLen(rexxcmd)>7
					    MidStr(temp,rexxcmd,8)
						timeout:=Val(temp)
					ELSE
                        StringF(result,'\d',timeout)
					ENDIF
				ENDIF

                IF StrCmp(rexxcmd,'TIMEZONE')
                    tmpint := (tz*-100)+10000
                    IF extrahalf
                        IF tzsecs<0 THEN tmpint := tmpint+30 ELSE tmpint := tmpint - 30
                    ENDIF
                    IF tmpint<0 THEN StrCopy(result,'-') ELSE StrCopy(result,'+')
                    StrAdd(result,RightStr(temp,StringF(temp,'\d',tmpint),4))
                ENDIF

                IF StrCmp(rexxcmd,'SAVECFG') THEN saveconfig()

            rx_ReplyMsg(arexxmsg,rc,result)
        ENDIF
        IF win THEN SetWindowPointerA(win,[NIL])
        UNTIL arexxmsg=NIL
    ENDIF
ENDIF

IF signal AND notifysig
    WHILE msg:=GetMsg(notifyport)
->        WriteF('notified\n')
    IF ignoretz=0
        oldtz:=tz
        readtz()
        adjustclock((oldtz-tz)*3600)
        IF winopen
                 Gt_SetGadgetAttrsA(gads[GADG_DST],win,NIL,[GTCB_CHECKED,dst,NIL])
                 Gt_SetGadgetAttrsA(gads[GADG_TZNAME],win,NIL,[GTST_STRING,tzname1,NIL])
                 Gt_SetGadgetAttrsA(gads[GADG_DSTNAME],win,NIL,[GA_DISABLED,(dst-1),GTST_STRING,tzname2,NIL])
                 Gt_SetGadgetAttrsA(gads[GADG_TZ],win,NIL,[GTIN_NUMBER,-tz,NIL])
        ENDIF

    ENDIF
        ReplyMsg(msg)
    ENDWHILE
ENDIF

IF signal AND ttnotifysig
    WHILE msg:=GetMsg(ttnotifyport)
        tmpint:=EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS',locstr[MSG_ICON_NOTIFY],locstr[MSG_ICON_NOTIFY_OPTS]],0,NIL)
        IF tmpint=1 /* update */
            readtooltypes() /* load tooltype settings */
            IF winopen THEN updategui()
        ENDIF
        ReplyMsg(msg)
    ENDWHILE
ENDIF

IF signal AND tisig
    WHILE msg:=GetMsg(msgport)
        IF enabled
            syncclock()
        ENDIF

->        ReplyMsg(msg) ->  *** eh? need this, don't we?
        starttint()
    ENDWHILE
ENDIF

IF signal AND envsig
    WHILE msg:=GetMsg(envmsgport)
   ->     IF timeenv THEN writetimeenv()

->        ReplyMsg(msg) ->  *** eh? need this, don't we?
        startenvint()
    ENDWHILE
ENDIF


IF signal AND clksig
    WHILE msg:=GetMsg(clkmsgport)
        IF enabled
            IF win THEN showtime()
        ENDIF

->        ReplyMsg(msg) ->  *** eh? need this, don't we?
        startclkint()
    ENDWHILE
ENDIF


IF signal AND dstsig
    WHILE msg:=GetMsg(dstmsgport)
->        ReplyMsg(msg) ->  *** eh? need this, don't we?
->        WriteF('test\n')
        IF leapsec THEN leapsecchg()
        IF autodst THEN startdst(chkautodst_generic())
    ENDWHILE
ENDIF

IF clgui
    closegui()
    clgui:=0
ENDIF

UNTIL terminated

saveleapsecs()

ENDPROC

PROC addcommodity()
broker_mp:=CreateMsgPort()

IF (broker:=CxBroker(
        [NB_VERSION,
        0,
        'FACTS',
        'Finest Amiga Clock Time Synchroniser',
        locstr[CX_DESCRIPTION],
        NBU_UNIQUE OR NBU_NOTIFY,
        COF_SHOW_HIDE,
        cxpri,
        0,
        broker_mp,
        0]:newbroker,NIL))=NIL THEN Raise(ERR_CMDY)

cxsigflag:=Shl(1,broker_mp.sigbit)

filter:=hotKey(popkey,broker_mp,EVT_POPKEY)
AttachCxObj(broker,filter)
inputevent:=invertStringRev(NIL,NIL)

filter2:=hotKey(savekey,broker_mp,EVT_SAVEKEY)
AttachCxObj(broker,filter2)
inputevent:=invertStringRev(NIL,NIL)

ActivateCxObj(broker,TRUE)

appmenu:=CreateMsgPort()

IF toolsmenu THEN startappmenu()

IF donotopenwin=0 THEN startgui()
ENDPROC

PROC readtooltypes()
DEF dobj:PTR TO diskobject,tt,toolarray,olddir=-1

    IF (wbarg.lock) AND (wbarg.name<>0) THEN olddir:=CurrentDir(wbarg.lock)
    dobj:=GetDiskObject(wbarg.name)

IF dobj

toolarray:=dobj.tooltypes

IF tt:=FindToolType(toolarray,'CXPRIORITY') THEN cxpri:=Val(tt)
IF tt:=FindToolType(toolarray,'CXPOPUP')
    IF MatchToolValue(tt,'YES') THEN winopen:=0
    IF MatchToolValue(tt,'NO') THEN donotopenwin:=1
ENDIF

IF tt:=FindToolType(toolarray,'CXPOPKEY') THEN StrCopy(popkey,tt)
/*
ELSE
    StrCopy(popkey,'ctrl alt f')
ENDIF
*/

IF tt:=FindToolType(toolarray,'CHECKKEY')
    StrCopy(savekey,tt)
ELSE
    StrCopy(savekey,'ctrl shift alt f')
ENDIF


IF EstrLen(tzname2)<3
    IF tt:=FindToolType(toolarray,'DSTNAME')
        StrCopy(tzname2,tt,4)
        IF EstrLen(tzname2)<4 THEN StrAdd(tzname2,'    ')
    ELSE
        StrCopy(tzname2,'DST ')
    ENDIF
ENDIF

    IF tt:=FindToolType(toolarray,'COMMAND')
        StrCopy(cmd,tt,512)
    ELSE
        nocmd:=1
    ENDIF
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

IF tt:=FindToolType(toolarray,'TINT') THEN timeint:=Val(tt)

-> IF tt:=FindToolType(toolarray,'TIMEOUT') THEN timeout:=Val(tt)

IF tt:=FindToolType(toolarray,'UTCOFFSET')
    utcoffset:=Val(tt)
    utcnum:=(utcoffset/3600)

    IF utcnum<0
        utcneg:=-1
    ELSE
        utcneg:=0
    ENDIF

    utcnum:=Abs(utcnum)
ENDIF

IF tt:=FindToolType(toolarray,'OPMODE')
    defsave:=1
    defuse:=1
ENDIF

IF tt:=FindToolType(toolarray,'WRITETO')
    defsave:=0
    defuse:=0
    IF MatchToolValue(tt,'BATTCLOCK') THEN defsave:=1
    IF MatchToolValue(tt,'SYSTEM') THEN defuse:=1
ENDIF

IF FindToolType(toolarray,'DEBUG') THEN debug:=1

IF tt:=FindToolType(toolarray,'COMPATIBILITY')

    IF MatchToolValue(tt,'MODE2036')
        mode2036:=1
    ENDIF

    IF MatchToolValue(tt,'BASE2015')
        base2015:=1
    ENDIF

    IF MatchToolValue(tt,'BATT2078')
        batt2078:=1
    ENDIF

ENDIF

IF tt:=FindToolType(toolarray,'AUTODST') THEN autodst:=Val(tt)

IF longtz=0
    IF tt:=FindToolType(toolarray,'DSTSTARTMONTH') THEN ostartmonth:=Val(tt)
    IF tt:=FindToolType(toolarray,'DSTSTARTWDAY') THEN ostartwday:=Val(tt)
    IF tt:=FindToolType(toolarray,'DSTSTARTHOUR') THEN ostarthour:=Val(tt)
    IF tt:=FindToolType(toolarray,'DSTSTARTWEEK') THEN ostartweek:=Val(tt)

    IF tt:=FindToolType(toolarray,'DSTENDMONTH') THEN oendmonth:=Val(tt)
    IF tt:=FindToolType(toolarray,'DSTENDWDAY') THEN oendwday:=Val(tt)
    IF tt:=FindToolType(toolarray,'DSTENDHOUR') THEN oendhour:=Val(tt)
    IF tt:=FindToolType(toolarray,'DSTENDWEEK') THEN oendweek:=Val(tt)

IF ostartweek=5 THEN ostartweek:=0
IF oendweek=5 THEN oendweek:=0

ENDIF

    IF tt:=FindToolType(toolarray,'DSTHEMISPHERE')

        IF MatchToolValue(tt,'NORTH')
            ohemisphere:=0
        ENDIF

        IF MatchToolValue(tt,'SOUTH')
            ohemisphere:=1
        ENDIF

    ENDIF

IF FindToolType(toolarray,'IGNORETZ') THEN ignoretz:=1 ELSE ignoretz:=0
IF FindToolType(toolarray,'OBSESSION') THEN obsession:=1 ELSE obsession:=0
IF FindToolType(toolarray,'CLOCKDAEMON') THEN clkdaemon:=1 ELSE clkdaemon:=0

IF FindToolType(toolarray,'MULTICAST') THEN mcast:=1 ELSE mcast:=0
-> or anycast
IF tt:=FindToolType(toolarray,'DSTADJUST')
    dstoffset:=Val(tt)
    nicedst:=dstoffset/3600
ENDIF

-> IF tt:=FindToolType(toolarray,'SERVICE') THEN service:=Val(tt)
IF tt:=FindToolType(toolarray,'DEFSERVER') THEN servnum:=Val(tt)
IF tt:=FindToolType(toolarray,'TIMEPORT') THEN timeport:=Val(tt)
IF tt:=FindToolType(toolarray,'DAYTIMEPORT') THEN daytimeport:=Val(tt)
IF tt:=FindToolType(toolarray,'SNTPPORT') THEN sntpport:=Val(tt)
IF tt:=FindToolType(toolarray,'UDPTIMEPORT') THEN timeudpport:=Val(tt)
IF FindToolType(toolarray,'APPMENU') THEN toolsmenu:=1 ELSE toolsmenu:=0
IF FindToolType(toolarray,'EXTRAHALF') THEN extrahalf:=1 ELSE extrahalf:=0
IF FindToolType(toolarray,'TIMEENV') THEN timeenv:=1 ELSE timeenv:=0
IF FindToolType(toolarray,'QUIETDST') THEN autodquiet:=1 ELSE autodquiet:=0
IF longtz THEN autodquiet:=1
IF FindToolType(toolarray,'STGEMUL') THEN stgemul:=1 ELSE stgemul:=0
IF FindToolType(toolarray,'CHECKONSTARTUP') THEN chkstart:=1 ELSE chkstart:=0
IF tt:=FindToolType(toolarray,'ONERROR')
        IF MatchToolValue(tt,'ANINC') THEN aninc:=1 ELSE aninc:=0
        IF MatchToolValue(tt,'NEXTSERVICE') THEN nextservice:=1 ELSE nextservice:=0
        IF MatchToolValue(tt,'USELIST') THEN trynext:=1 ELSE trynext:=0
ENDIF
IF FindToolType(toolarray,'ALWAYSUPDATE') THEN alwaysupdate:=1 ELSE alwaysupdate:=0

IF tt:=FindToolType(toolarray,'WINX') THEN winx:=Val(tt)
IF tt:=FindToolType(toolarray,'WINY') THEN winy:=Val(tt)
IF FindToolType(toolarray,'ZOOMED') THEN zoomed:=1 ELSE zoomed:=0

-> IF FindToolType(toolarray,'DEFPORT')
    defport:=0 -> was 1
IF service=13 AND port=defdaytimeport THEN defport:=1
IF service=37 AND port=deftimeport THEN defport:=1
IF service=123 AND port=defsntpport THEN defport:=1
IF service=372 AND port=deftimeudpport THEN defport:=1
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
-> ENDIF

FreeDiskObject(dobj)
dobj:=NIL

startttnotify()

ENDIF

IF olddir<>-1 THEN CurrentDir(olddir)

ENDPROC

PROC closegui()
IF win THEN CloseWindow(win)

IF mysc THEN UnlockPubScreen(NIL,mysc)
mysc:=NIL
winopen:=0
win:=NIL
winsig:=NIL
ENDPROC

PROC savetz(useonly=0)
DEF tzstring[50]:STRING,stgstr[50]:STRING,stgtz,wdltzs,wdltze,fhand=0

IF EstrLen(tzname1)<3 THEN StrAdd(tzname1,'xxx')

IF stgemul
    stgtz:=-tz
    IF dst THEN stgtz:=stgtz-1
    stgtz:=stgtz*60
   
 StringF(stgstr,'NT:\d:\d:\d:\d:\d:\d:\d:\d:\d:\d:',stgtz,startweek,startwday,startmonth,(starthour*60),endweek,endwday,endmonth,(endhour*60),stgtz+60)
ENDIF

StrCopy(tzstring,tzname1,3)

IF longtz
/*
    IF dst
        StrAdd(tzstring,RealF(temp,(tz-1)!,0))
    ELSE
*/
        StrAdd(tzstring,RealF(temp,tz!,0))
->    ENDIF
ELSE
    StrAdd(tzstring,RealF(temp,tz!,0))
ENDIF

IF dst
    IF EstrLen(tzname2)<3 THEN StrAdd(tzname2,' xxx')
    StrAdd(tzstring,tzname2)
    StrAdd(stgstr,'YES')
ELSE
    StrAdd(stgstr,'NO')
ENDIF

-> WriteF('\s-\d-\s\n',tzname1,tz,tzstring)

IF stgemul
    IF useonly=0
        IF tzfile:=Open('ENVARC:SUMMERTIME',MODE_NEWFILE)
            Fputs(tzfile,stgstr)
            Close(tzfile)
        ELSE
            EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS',locstr[MSG_ERR_WRITING],'OK','ENVARC:SUMMERTIME'],0,NIL)
        ENDIF
    ENDIF

    IF tzfile:=Open('ENV:SUMMERTIME',MODE_NEWFILE)
        Fputs(tzfile,stgstr)
        Close(tzfile)
    ELSE
        EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS',locstr[MSG_ERR_WRITING],'OK','ENV:SUMMERTIME'],0,NIL)
    ENDIF
ENDIF

IF longtz
    IF dst=0
        IF EstrLen(tzname2)<3 THEN StrAdd(tzname2,' xxx')
        StrAdd(tzstring,tzname2)
   ENDIF

IF ostartweek=0 THEN wdltzs:=5 ELSE wdltzs:=ostartweek
IF oendweek=0 THEN wdltze:=5 ELSE wdltze:=oendweek

 StringF(temp,',M\d.\d.\d/\d,M\d.\d.\d/\d',ostartmonth,wdltzs,ostartwday,ostarthour,oendmonth,wdltze,oendwday,oendhour)
    StrAdd(tzstring,temp)
ENDIF

    IF (fhand:=Lock('ENV:FACTS',ACCESS_WRITE))=0
        fhand:=CreateDir('ENV:FACTS')
    ENDIF
    UnLock(fhand)

IF tzfile:=Open('ENV:TZ',MODE_NEWFILE)
Fputs(tzfile,tzstring)
Close(tzfile)
ELSE
    EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS',locstr[MSG_ERR_WRITING],'OK','ENV:TZ'],0,NIL)
ENDIF
    IF tzfile:=Open('ENV:FACTS/DST',MODE_NEWFILE)
        FputC(tzfile,dst)
        Close(tzfile)
    ELSE
        EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS',locstr[MSG_ERR_WRITING],'OK','ENV:FACTS/DST'],0,NIL)
    ENDIF
    IF (fhand:=Lock('ENVARC:FACTS',ACCESS_WRITE))=0
        fhand:=CreateDir('ENVARC:FACTS')
    ENDIF
    UnLock(fhand)
    IF tzfile:=Open('ENVARC:FACTS/DST',MODE_NEWFILE)
        FputC(tzfile,dst)
        Close(tzfile)
    ELSE
        EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS',locstr[MSG_ERR_WRITING],'OK','ENVARC:FACTS/DST'],0,NIL)
    ENDIF

IF useonly=0
    IF tzfile:=Open('ENVARC:TZ',MODE_NEWFILE)
        Fputs(tzfile,tzstring)
        Close(tzfile)
    ELSE
EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS',locstr[MSG_ERR_WRITING],'OK','ENVARC:TZ'],0,NIL)
    ENDIF
ENDIF

ENDPROC

PROC saveconfig()
DEF olddir=-1,oldtooltypes,newtooltypes[50]:ARRAY OF LONG,tmptt1[50]:STRING
DEF tmptt2[20]:STRING,tmptt3[50]:STRING,tmptt4[100]:STRING,tmptt5[20]:STRING,tmptt6[20]:STRING
DEF tmptt7[20]:STRING,tmptt8[20]:STRING,tmptt10[20]:STRING
DEF tmptt11[20]:STRING,tmptt12[20]:STRING,tmptt13[20]:STRING, tmptt15[50]:STRING
DEF tmptt22[20]:STRING,tmptt23[20]:STRING,tmptt24[20]:STRING,tmptt25[20]:STRING
DEF tmptt26[20]:STRING,tmptt27[20]:STRING,tmptt28[20]:STRING,tmptt29[20]:STRING
DEF tmptt30[20]:STRING,tmptt31[20]:STRING,tmptt32[40]:STRING,tmptt33[20]:STRING
DEF tmptt34[20]:STRING,tmptt35[512]:STRING,tmptt36[512]:STRING

-> no tmptt9 or tmptt14!

DEF dobj:PTR TO diskobject /* ,wbarg:PTR TO wbarg,wbenchMsg:PTR TO wbstartup */
DEF tmp[10]:STRING
DEF servfile[100]:STRING

savetz()

IF wbstart=2 THEN RETURN
/*
    wbenchMsg:=wbmessage
    wbarg:=wbenchMsg.arglist
*/
    StringF(temp,'\s:\d:\d:\d',server,port,service,timeout)

StringF(servfile,'ENV:FACTS/TIMESERVER.\d',servnum)

IF ntphost:=Open(servfile,MODE_NEWFILE)
    Fputs(ntphost,temp)
    Close(ntphost)
ELSE
    EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS',locstr[MSG_ERR_WRITING],'OK',servfile],0,NIL)
ENDIF

StringF(servfile,'ENVARC:FACTS/TIMESERVER.\d',servnum)


IF ntphost:=Open(servfile,MODE_NEWFILE)
    Fputs(ntphost,temp)
    Close(ntphost)
ELSE
    EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS',locstr[MSG_ERR_WRITING],'OK',servfile],0,NIL)
ENDIF

stopttnotify()

IF (wbarg.lock) AND (wbarg.name<>0) THEN olddir:=CurrentDir(wbarg.lock)

IF dobj:=GetDiskObject(wbarg.name)

oldtooltypes:=dobj.tooltypes

StrCopy(tmptt1,'CXPOPKEY=')
StrAdd(tmptt1,popkey)
newtooltypes[1]:=tmptt1

StringF(tmptt2,'CXPRIORITY=\d',cxpri)
newtooltypes[2]:=tmptt2

IF donotopenwin
    newtooltypes[3]:='CXPOPUP=NO'
ELSE
    newtooltypes[3]:='CXPOPUP=YES'
ENDIF

newtooltypes[0]:='DONOTWAIT'

StrCopy(tmptt3,'CHECKKEY=')
StrAdd(tmptt3,savekey)
newtooltypes[4]:=tmptt3

StrCopy(tmptt4,'WRITETO=')
IF defsave THEN StrAdd(tmptt4,'BATTCLOCK')
IF defuse THEN StrAdd(tmptt4,'|SYSTEM')
newtooltypes[5]:=tmptt4

StrCopy(tmptt5,'TINT=')
StrAdd(tmptt5,RealF(temp,timeint!,0))
newtooltypes[6]:=tmptt5

StrCopy(tmptt6,'DSTNAME=')
StrAdd(tmptt6,tzname2)
newtooltypes[7]:=tmptt6

IF zoomed THEN newtooltypes[8]:='ZOOMED' ELSE newtooltypes[8]:='(ZOOMED)'

-> IF defport THEN newtooltypes[8]:='DEFPORT' ELSE newtooltypes[8]:='(DEFPORT)'

StringF(tmptt11,'AUTODST=\d',autodst)
newtooltypes[9]:=tmptt11

IF ignoretz THEN newtooltypes[10]:='IGNORETZ' ELSE newtooltypes[10]:='(IGNORETZ)'

StringF(tmptt7,'DAYTIMEPORT=\d',daytimeport)
newtooltypes[11]:=tmptt7

StringF(tmptt8,'TIMEPORT=\d',timeport)
newtooltypes[12]:=tmptt8

/*
StringF(tmptt9,'SERVICE=\d',service)
newtooltypes[13]:=tmptt9
*/

IF alwaysupdate THEN newtooltypes[13]:='ALWAYSUPDATE' ELSE newtooltypes[13]:='(ALWAYSUPDATE)'

StringF(tmptt10,'UTCOFFSET=\d',utcoffset)
newtooltypes[14]:=tmptt10

StringF(tmptt12,'SNTPPORT=\d',sntpport)
newtooltypes[15]:=tmptt12

StringF(tmptt13,'UDPTIMEPORT=\d',timeudpport)
newtooltypes[16]:=tmptt13

/*
StringF(tmptt14,'TIMEOUT=\d',timeout)
newtooltypes[17]:=tmptt14
*/

IF trynext
    newtooltypes[17]:='ONERROR=USELIST'
ELSE
    StrCopy(tmptt32,'ONERROR=')
    IF aninc THEN StrAdd(tmptt32,'ANINC|')
    IF nextservice THEN StrAdd(tmptt32,'NEXTSERVICE')
    newtooltypes[17]:=tmptt32
ENDIF


StrCopy(tmptt15,'COMPATIBILITY=')
    IF mode2036
        StrAdd(tmptt15,'MODE2036|')
    ENDIF
    IF base2015
        StrAdd(tmptt15,'BASE2015|')
    ENDIF
    IF batt2078
        StrAdd(tmptt15,'BATT2078')
    ENDIF

newtooltypes[18]:=tmptt15

IF toolsmenu THEN newtooltypes[19]:='APPMENU' ELSE newtooltypes[19]:='(APPMENU)'
IF extrahalf THEN newtooltypes[20]:='EXTRAHALF' ELSE newtooltypes[20]:='(EXTRAHALF)'

IF autodquiet THEN newtooltypes[21]:='QUIETDST' ELSE newtooltypes[21]:='(QUIETDST)'

StringF(tmptt22,'DSTSTARTMONTH=\d',ostartmonth)
newtooltypes[22]:=tmptt22
StringF(tmptt23,'DSTSTARTWDAY=\d',ostartwday)
newtooltypes[23]:=tmptt23
StringF(tmptt24,'DSTSTARTHOUR=\d',ostarthour)
newtooltypes[24]:=tmptt24
StringF(tmptt25,'DSTSTARTWEEK=\d',ostartweek)
newtooltypes[25]:=tmptt25

StringF(tmptt26,'DSTENDMONTH=\d',oendmonth)
newtooltypes[26]:=tmptt26
StringF(tmptt27,'DSTENDWDAY=\d',oendwday)
newtooltypes[27]:=tmptt27
StringF(tmptt28,'DSTENDHOUR=\d',oendhour)
newtooltypes[28]:=tmptt28
StringF(tmptt29,'DSTENDWEEK=\d',oendweek)
newtooltypes[29]:=tmptt29

IF ohemisphere=0 THEN newtooltypes[30]:='DSTHEMISPHERE=NORTH' ELSE newtooltypes[31]:='DSTHEMISPHERE=SOUTH'

IF stgemul THEN newtooltypes[31]:='STGEMUL' ELSE newtooltypes[31]:='(STGEMUL)'

IF chkstart THEN newtooltypes[32]:='CHECKONSTARTUP' ELSE newtooltypes[32]:='(CHECKONSTARTUP)'

StringF(tmptt30,'WINX=\d',winx)
newtooltypes[33]:=tmptt30

StringF(tmptt31,'WINY=\d',winy)
newtooltypes[34]:=tmptt31

IF obsession THEN newtooltypes[35]:='OBSESSION' ELSE newtooltypes[35]:='(OBSESSION)'

StringF(temp,'COMMAND=\s',cmd)
newtooltypes[36]:=temp

IF clkdaemon THEN newtooltypes[37]:='CLOCKDAEMON' ELSE newtooltypes[37]:='(CLOCKDAEMON)'

StringF(tmptt33,'DEFSERVER=\d',servnum)
newtooltypes[38]:=tmptt33

StringF(tmptt34,'DSTADJUST=\d',dstoffset)
newtooltypes[39]:=tmptt34
/*
StrCopy(tmptt35,'STRATUM1DB=')
StrAdd(tmptt35,st1db)
newtooltypes[40]:=tmptt35
StrCopy(tmptt36,'STRATUM2DB=')
StrAdd(tmptt36,st2db)
newtooltypes[41]:=tmptt36
*/

IF timeenv THEN newtooltypes[40]:='TIMEENV' ELSE newtooltypes[40]:='(TIMEENV)'

newtooltypes[41]:=NIL

dobj.tooltypes:=newtooltypes

StringF(tmp,locstr[MSG_ERR_WRITING],'facts.info')
IF PutDiskObject(wbarg.name,dobj)=0
        EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS',tmp,locstr[MSG_OK]],0,NIL)
ENDIF

dobj.tooltypes:=oldtooltypes
FreeDiskObject(dobj)

startttnotify()

ENDIF
IF olddir<>-1 THEN CurrentDir(olddir)

ENDPROC


PROC menu()
DEF defopmen[3]:ARRAY OF LONG,i,alsmen=0,autodstmen[20]:ARRAY OF LONG,ignoretzmen=0
DEF servmen[5]:ARRAY OF LONG,utcomen[15]:ARRAY OF LONG,utcplus,utcminus,mode2036men
DEF batt2078men,base2015men,appmenumen,halfmen,autodquietmen,custommen,stgemulmen
DEF chkstartmen,savesetmen=NM_ITEMDISABLED,anincmen,longtzmen
DEF trynextmen,nextservmen,showtimemen,obsessmen,clkdaemonmen,ddbmen=0
DEF timeenvmen

IF wbstart=1 THEN savesetmen:=0

FOR i:=0 TO 19
autodstmen[i]:=(CHECKIT)
ENDFOR
custommen:=(CHECKIT)

IF autodst<>-1
    autodstmen[autodst]:=(CHECKIT OR CHECKED)
ELSE
    custommen:=(CHECKIT OR CHECKED)
ENDIF

IF aninc=0 THEN anincmen:=(MENUTOGGLE OR CHECKIT) ELSE anincmen:=(CHECKIT OR MENUTOGGLE OR CHECKED)

IF obsession=0 THEN obsessmen:=(MENUTOGGLE OR CHECKIT) ELSE obsessmen:=(CHECKIT OR MENUTOGGLE OR CHECKED)
IF timeenv=0 THEN timeenvmen:=(MENUTOGGLE OR CHECKIT) ELSE timeenvmen:=(CHECKIT OR MENUTOGGLE OR CHECKED)

IF nextservice=0 THEN nextservmen:=(MENUTOGGLE OR CHECKIT) ELSE nextservmen:=(CHECKIT OR MENUTOGGLE OR CHECKED)

IF trynext=0 THEN trynextmen:=(MENUTOGGLE OR CHECKIT) ELSE trynextmen:=(CHECKIT OR MENUTOGGLE OR CHECKED)
-> trynextmen:=(trynextmen OR NM_ITEMDISABLED)

IF ignoretz THEN ignoretzmen:=(CHECKIT OR MENUTOGGLE OR CHECKED) ELSE ignoretzmen:=(CHECKIT OR MENUTOGGLE)

IF alsbase=NIL THEN alsmen:=NM_ITEMDISABLED

IF defuse THEN defopmen[1]:=CHECKIT OR MENUTOGGLE OR CHECKED ELSE defopmen[1]:=CHECKIT OR MENUTOGGLE
IF defsave THEN defopmen[2]:=CHECKIT OR MENUTOGGLE OR CHECKED ELSE defopmen[2]:=CHECKIT OR MENUTOGGLE

servmen[1]:=CHECKIT
servmen[2]:=CHECKIT
servmen[3]:=CHECKIT
servmen[4]:=CHECKIT

IF service=13 THEN servmen[2]:=CHECKIT OR CHECKED
IF service=37 THEN servmen[1]:=CHECKIT OR CHECKED
IF service=123 THEN servmen[3]:=CHECKIT OR CHECKED
IF service=372 THEN servmen[4]:=CHECKIT OR CHECKED

IF mode2036 THEN mode2036men:=(CHECKIT OR MENUTOGGLE OR CHECKED) ELSE mode2036men:=(CHECKIT OR MENUTOGGLE)
IF base2015 THEN base2015men:=(CHECKIT OR MENUTOGGLE OR CHECKED) ELSE base2015men:=(CHECKIT OR MENUTOGGLE)
IF batt2078 THEN batt2078men:=(CHECKIT OR MENUTOGGLE OR CHECKED) ELSE batt2078men:=(CHECKIT OR MENUTOGGLE)
IF toolsmenu THEN appmenumen:=(CHECKIT OR MENUTOGGLE OR CHECKED) ELSE appmenumen:=(CHECKIT OR MENUTOGGLE)
IF extrahalf THEN halfmen:=(CHECKIT OR MENUTOGGLE OR CHECKED) ELSE halfmen:=(CHECKIT OR MENUTOGGLE)

FOR i:=0 TO 12
utcomen[i]:=CHECKIT
ENDFOR
utcplus:=CHECKIT
utcminus:=CHECKIT

IF autodquiet THEN autodquietmen:=(CHECKIT OR MENUTOGGLE OR CHECKED) ELSE autodquietmen:=(CHECKIT OR MENUTOGGLE)
IF clkdaemon THEN clkdaemonmen:=(CHECKIT OR MENUTOGGLE OR CHECKED) ELSE clkdaemonmen:=(CHECKIT OR MENUTOGGLE)
IF alwaysupdate THEN showtimemen:=(CHECKIT OR MENUTOGGLE OR CHECKED) ELSE showtimemen:=(CHECKIT OR MENUTOGGLE)
IF stgemul THEN stgemulmen:=(CHECKIT OR MENUTOGGLE OR CHECKED) ELSE stgemulmen:=(CHECKIT OR MENUTOGGLE)
IF chkstart THEN chkstartmen:=(CHECKIT OR MENUTOGGLE OR CHECKED) ELSE chkstartmen:=(CHECKIT OR MENUTOGGLE)

utcomen[utcnum]:=CHECKIT OR CHECKED

IF utcneg
    utcminus:=CHECKIT OR CHECKED
ELSE
    utcplus:=CHECKIT OR CHECKED
ENDIF

IF longtz
    longtzmen:=(CHECKIT OR MENUTOGGLE OR CHECKED)
    autodquietmen:=(CHECKIT OR MENUTOGGLE OR CHECKED OR NM_ITEMDISABLED)
    ignoretzmen:=(CHECKIT OR MENUTOGGLE OR CHECKED OR NM_ITEMDISABLED)
ELSE
    longtzmen:=(CHECKIT OR MENUTOGGLE)
ENDIF

/*
IF StrCmp(st1db,'http://',7) AND StrCmp(st2db,'http://',7)
    ddbmen:=NM_ITEMDISABLED
ENDIF
*/

menustrip:=CreateMenusA([NM_TITLE,0,locstr[MENU_PROJECT],0,0,0,0,
                         NM_ITEM,0,locstr[MENU_PROJECT_SERVER_INFO],'I',0,0,0,
                         NM_ITEM,0,NM_BARLABEL,0,0,0,0,
                ->         NM_ITEM,0,locstr[MENU_PROJECT_VERSION],'V',0,0,0,
                         NM_ITEM,0,locstr[MENU_PROJECT_ABOUT],'A',0,0,0,
                         NM_ITEM,0,NM_BARLABEL,0,0,0,0,
                         NM_ITEM,0,locstr[MENU_PROJECT_HIDE],'H',0,0,0,
                         NM_ITEM,0,locstr[MENU_PROJECT_QUIT],'Q',0,0,0,
                         NM_TITLE,0,locstr[MENU_SETTINGS],0,0,0,0,
                         NM_ITEM,0,locstr[MENU_SETTINGS_WRITE_TIME_TO],0,0,0,0,
                         NM_SUB,0,locstr[MENU_SETTINGS_WRITE_SYSTEM],0,defopmen[1],0,0,
                         NM_SUB,0,locstr[MENU_SETTINGS_WRITE_BATTCLOCK],0,defopmen[2],0,0,
                         NM_ITEM,0,locstr[MENU_SETTINGS_AUTODST],0,0,0,0,
                         NM_SUB,0,locstr[MENU_SETTINGS_AUTODST_OFF],0,autodstmen[0],2097150,0,
                         NM_SUB,0,'Australia (Sydney/Melbourne)',0,autodstmen[1],2097149,0,
                         NM_SUB,0,'Australia (Tasmania/NSW)',0,autodstmen[2],2097147,0,
                         NM_SUB,0,'Azerbaijan',0,autodstmen[3],2097143,0,
                         NM_SUB,0,'Brazil',0,autodstmen[4],2097135,0,
                         NM_SUB,0,'Newfoundland/Mexico',0,autodstmen[5],2097119,0,
                         NM_SUB,0,'Chile',0,autodstmen[6],2097087,0,
                         NM_SUB,0,'Cuba/Turks/Caicos',0,autodstmen[7],2097023,0,
                         NM_SUB,0,'Egypt',0,autodstmen[8],2096895,0,
                         NM_SUB,0,'Europe',0,autodstmen[9],2096639,0,
                         NM_SUB,0,'Falklands',0,autodstmen[10],2096127,0,
                         NM_SUB,0,'Fiji',0,autodstmen[11],2095103,0,
                         NM_SUB,0,'Jordan',0,autodstmen[12],2093055,0,
                         NM_SUB,0,'Lebanon/Mongol',0,autodstmen[13],2088959,0,
                         NM_SUB,0,'New Zealand',0,autodstmen[14],2080767,0,
                         NM_SUB,0,'Paraguay',0,autodstmen[15],2064383,0,
                         NM_SUB,0,'Russia/Asia/Kirgizstan',0,autodstmen[16],2031615,0,
                         NM_SUB,0,'United Kingdom',0,autodstmen[17],1966079,0,
                         NM_SUB,0,'USA and Canada',0,autodstmen[18],1835007,0,
                         NM_SUB,0,'Zion',0,autodstmen[19],1572863,0,
                         NM_SUB,0,locstr[MENU_SETTINGS_AUTODST_CUSTOM],0,custommen,1048575,0, -> 1048576
                         NM_SUB,0,NM_BARLABEL,0,0,0,0,
                         NM_SUB,0,locstr[MENU_SETTINGS_AUTODST_QUIET],0,autodquietmen,0,0,
                         NM_SUB,0,locstr[MENU_SETTINGS_AUTODST_EMULATE],0,stgemulmen,0,0,
                         NM_SUB,0,locstr[MENU_SETTINGS_AUTODST_LONGTZ],0,longtzmen,0,0,
                         NM_ITEM,0,locstr[MENU_SETTINGS_LEAPSEC],0,obsessmen,0,0,
                         NM_ITEM,0,locstr[MENU_SETTINGS_IGNORETZ],'C',ignoretzmen,0,0,
                         NM_ITEM,0,locstr[MENU_SETTINGS_SERVICE],0,0,0,0,
                         NM_SUB,0,'sntp','N',servmen[3],14,0,
                         NM_SUB,0,'time/udp','U',servmen[4],13,0,
                         NM_SUB,0,'time/tcp','T',servmen[1],11,0,
                         NM_SUB,0,'daytime','D',servmen[2],7,0,
                    NM_ITEM,0,locstr[MENU_SETTINGS_OFFSET],0,0,0,0,
                    NM_SUB,0,'+',0,utcplus,2,0,
                    NM_SUB,0,'-',0,utcminus,1,0,
                    NM_SUB,0,NM_BARLABEL,0,0,0,0,
                    NM_SUB,0,'0',0,utcomen[0],(65528-8),0,
                    NM_SUB,0,'1',0,utcomen[1],(65528-16),0,
                    NM_SUB,0,'2',0,utcomen[2],(65528-32),0,
                    NM_SUB,0,'3',0,utcomen[3],(65528-64),0,
                    NM_SUB,0,'4',0,utcomen[4],(65528-128),0,
                    NM_SUB,0,'5',0,utcomen[5],(65528-256),0,
                    NM_SUB,0,'6',0,utcomen[6],(65528-512),0,
                    NM_SUB,0,'7',0,utcomen[7],(65528-1024),0,
                    NM_SUB,0,'8',0,utcomen[8],(65528-2048),0,
                    NM_SUB,0,'9',0,utcomen[9],(65528-4096),0,
                    NM_SUB,0,'10',0,utcomen[10],(65528-8192),0,
                    NM_SUB,0,'11',0,utcomen[11],(65528-16384),0,
                    NM_SUB,0,'12',0,utcomen[12],(65528-32768),0,
                    NM_ITEM,0,locstr[MENU_SETTINGS_CHECK_ON_STARTUP],0,chkstartmen,0,0,
                    NM_ITEM,0,locstr[MENU_SETTINGS_TIMEENV],0,timeenvmen,0,0,
                    NM_ITEM,0,locstr[MENU_SETTINGS_ONERROR],0,0,0,0,
                    NM_SUB,0,locstr[MENU_SETTINGS_TRYNEXT],0,trynextmen,12,0,
                    NM_SUB,0,NM_BARLABEL,0,0,0,0,
                    NM_SUB,0,locstr[MENU_SETTINGS_ANINC],0,anincmen,1,0,
                    NM_SUB,0,locstr[MENU_SETTINGS_NEXTSERVICE],0,nextservmen,1,0,
                    NM_ITEM,0,locstr[MENU_SETTINGS_COMPATIBILITY],0,0,0,0,
                    NM_SUB,0,locstr[MENU_SETTINGS_COMPAT_MODE],0,mode2036men,0,0,
                    NM_SUB,0,locstr[MENU_SETTINGS_COMPAT_BASE],0,base2015men,0,0,
                    NM_SUB,0,locstr[MENU_SETTINGS_COMPAT_BATTCLOCK],0,batt2078men,0,0,
                    NM_SUB,0,NM_BARLABEL,0,0,0,0,
                    NM_SUB,0,locstr[MENU_SETTINGS_COMPAT_CLKDAEMON],0,clkdaemonmen,0,0,
                    NM_ITEM,0,locstr[MENU_SETTINGS_APPMENU],0,appmenumen,0,0,
                    NM_ITEM,0,locstr[MENU_SETTINGS_EXTRAHALF],0,halfmen,0,0,
                         NM_ITEM,0,locstr[MENU_SPECIAL_SHOWTIME],'.',showtimemen,0,0,
                         NM_ITEM,0,NM_BARLABEL,0,0,0,0,
                         NM_ITEM,0,locstr[MENU_SETTINGS_SNAPSHOT],0,0,0,0,
                         NM_ITEM,0,locstr[MENU_SETTINGS_SAVE],'S',savesetmen,0,0,
                         NM_TITLE,0,locstr[MENU_SPECIAL],0,0,0,0,
                         NM_ITEM,0,locstr[MENU_SPECIAL_RESET],0,0,0,0,
                         NM_ITEM,0,locstr[MENU_SPECIAL_READ],'L',0,0,0,
                         NM_ITEM,0,NM_BARLABEL,0,0,0,0,
                  ->       NM_ITEM,0,locstr[MENU_SPECIAL_DOWNLOAD],'D',ddbmen,0,0,
                  ->       NM_ITEM,0,NM_BARLABEL,0,0,0,0,
                         NM_ITEM,0,locstr[MENU_SPECIAL_BOREDOM],0,alsmen,0,0,
                         NM_END,0,NIL,0,0,0,0]:newmenu,
                         [GTMN_FULLMENU,TRUE,NIL])

-> old tag ... ,GTMN_FRONTPEN,1

ENDPROC

PROC about()
DEF abstr[2048]:STRING,bsdver[50]:STRING

StringF(abstr,'FACTS by Chris Young\n© 2007 Unsatisfactory Software\n\nThe Finest Amiga Clock Time Synchroniser\n\nhttp://www.unsatisfactorysoftware.co.uk\nfacts@unsatisfactorysoftware.co.uk\n\n')
StrAdd(abstr,'Release: ')
StrAdd(abstr,frel)
StrAdd(abstr,' (v')
StrAdd(abstr,fver)
StrAdd(abstr,'.')
StrAdd(abstr,frev)
StrAdd(abstr,')')
StrAdd(abstr,'\nTranslation by: ')
StrAdd(abstr,locstr[MSG_TRANS_BY])

StrAdd(abstr,'\nARexx Port: FACTS')
StrAdd(abstr,'\nTCP/IP: ')

IF identifybase:=OpenLibrary('identify.library',9)
    StrAdd(abstr,IdHardware(IDHW_TCPIP,NIL))
    StrAdd(abstr,'\nBattClock: ')
    StrAdd(abstr,IdHardware(IDHW_BATTCLOCK,NIL))
    CloseLibrary(identifybase)
    identifybase:=NIL
ELSE
    IF socketbase:=OpenLibrary(socketlib,socketver)
      StrCopy(bsdver,socketbase.idstring,StrLen(socketbase.idstring)-2)
      CloseLibrary(socketbase)
      socketbase:=NIL
      StrAdd(abstr,bsdver)
    ENDIF
ENDIF

	-> StrAdd(abstr,'\nm: ')
->        StrAdd(abstr,StringF(temp,'\nClock drift: \dµs/s',mdrift))

-> StrAdd(abstr,'\n\n')

-> (A poor quality attempt at "AtomTime",\nbut on the Amiga)\n\n

    EasyRequestArgs(0,[SIZEOF easystruct,0,'About FACTS...',abstr,locstr[MSG_EH]],0,NIL)
ENDPROC

PROC starttint()
IF CheckIO(tioreq)=0
    AbortIO(tioreq)
    WaitIO(tioreq)
ENDIF

IF timeint>0
            tioreq.io.command:=TR_ADDREQUEST
            tioreq.time.secs:=timeint
            tioreq.time.micro:=0
            SendIO(tioreq)
ENDIF
ENDPROC

PROC startclkint()
            clkioreq.io.command:=TR_ADDREQUEST
            clkioreq.time.secs:=1
            clkioreq.time.micro:=0
            SendIO(clkioreq)
ENDPROC

PROC stopclkint()
IF CheckIO(clkioreq)=0
    AbortIO(clkioreq)
    WaitIO(clkioreq)
ENDIF


ENDPROC

PROC startenvint()
DEF envint

envint := writetimeenv()
            envioreq.io.command:=TR_ADDREQUEST
            envioreq.time.secs:=envint
            envioreq.time.micro:=0
            SendIO(envioreq)
ENDPROC

PROC stopenvint()
IF CheckIO(envioreq)=0
    AbortIO(envioreq)
    WaitIO(envioreq)
ENDIF
ENDPROC

PROC writetimeenv()
DEF datestring:clockdata,fhand
                    ioreq.io.command:=TR_GETSYSTIME
                    DoIO(ioreq)
                    curtime:=ioreq.time.secs

Amiga2Date(curtime,datestring)

    IF fhand:=Open('ENV:Facts/time',MODE_NEWFILE)
    Fputs(fhand,StringF(temp,'\z\d[2]:\z\d[2]',datestring.hour,datestring.min))
    Close(fhand)
    ENDIF

RETURN (60-datestring.sec)
ENDPROC

PROC dodefop(seconds=0)
IF clkdaemon
    IF dst THEN servsecs:=servsecs+dstoffset
ENDIF

IF seconds=0 THEN seconds:=servsecs

setclock(seconds)

IF nocmd=0 THEN SystemTagList(cmd,[SYS_ASYNCH,TRUE,NIL])

/*
IF defsave THEN setclock(seconds)
IF defuse THEN setdate(seconds)
*/

ENDPROC

PROC dstswitch(on,quiet=2)
DEF res=0,rest[100]:STRING,ttimeint=0

IF quiet=2 THEN quiet:=autodquiet

IF enabled=0 THEN RETURN

StringF(rest,'\s|\s',locstr[MSG_DST_SYNCNOW],locstr[MSG_OK])

IF dst
    IF on=0
        dst:=0
        IF clkdaemon=0
          tz:=tz+nicedst
/*
           ioreq.io.command:=TR_GETSYSTIME
            DoIO(ioreq)
            dodefop(ioreq.time.secs-3600)
*/
			adjustclock(-3600)
        IF quiet=0
            res:=EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS',locstr[MSG_DST_OUT],rest],0,NIL)
            IF res=1
				forcesync:=1
			ENDIF
        ENDIF

        ENDIF
            savetz()

    ENDIF
ELSE
    IF on
        dst:=1
        IF clkdaemon=0
          tz:=tz-nicedst
        
/*
            ioreq.io.command:=TR_GETSYSTIME
            DoIO(ioreq)
        
            dodefop(ioreq.time.secs+3600)
*/
			adjustclock(3600)
        IF quiet=0
            res:=EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS',locstr[MSG_DST_IN],rest],0,NIL)
            IF res=1
				forcesync:=1
			ENDIF

        ENDIF

        ENDIF
            savetz()

    ENDIF
ENDIF

             tzsecs:=tz*3600
             IF winopen
                 Gt_SetGadgetAttrsA(gads[GADG_DST],win,NIL,[GTCB_CHECKED,dst,NIL])
                 Gt_SetGadgetAttrsA(gads[GADG_DSTNAME],win,NIL,[GA_DISABLED,(dst-1),NIL])
                    Gt_SetGadgetAttrsA(gads[GADG_TZ],win,NIL,[GTIN_NUMBER,-tz,NIL])
        ENDIF

ENDPROC

PROC setupnotify()
notifyport:=CreateMsgPort()
tznotify.name:='ENV:TZ'
tznotify.flags:=NRF_SEND_MESSAGE
tznotify.port:=notifyport
snot:=StartNotify(tznotify)
ENDPROC

PROC startttnotify()
StrCopy(tmp,wbarg.name)
StrAdd(tmp,'.info')
stopttnotify()
ttnotifyport:=CreateMsgPort()
ttnotify.name:=tmp
ttnotify.flags:=NRF_SEND_MESSAGE
ttnotify.port:=ttnotifyport
ttsnot:=StartNotify(ttnotify)
-> WriteF('\d \s \s\n',ttsnot,wbarg.name,tmp)
ENDPROC


PROC adjustclock(addon)
IF ignoretz=0
    ioreq.io.command:=TR_GETSYSTIME
    DoIO(ioreq)
    dodefop(ioreq.time.secs+addon)
ENDIF
ENDPROC

PROC closetcp()

IF sock<>-1
    CloseSocket(sock)
    sock:=-1
ENDIF

/*
IF tcp
    Close(tcp)
    tcp:=NIL
ENDIF
*/

IF socketbase
    CloseLibrary(socketbase)
    socketbase:=NIL
ENDIF

ENDPROC

PROC gettimeport()
DEF dess,desf,ntps,ntpf,rtrips,rtripf,ntpfrac
DEF tmp[50]:STRING
closetcp()
-> WriteF('\d: ',Long(buffer))

IF service=123
	GetSysTime(measure)
	curtime:=measure.secs
	curmics:=measure.micro
    /* ioreq.io.command:=TR_GETSYSTIME
    DoIO(ioreq) */
    dess:=measure.secs+2461449600-tzsecs
    desf:=Div(Shl(measure.micro,12),1000000)

    -> local clock offset 
    difference := ((pkt.recs-pkt.orgs)+(pkt.xmts-dess))/2
    -> roundtrip delay
    rtrips := (dess-pkt.orgs) - (pkt.recs-pkt.xmts)
    rtripf := (desf-pkt.orgf) - (pkt.recf-pkt.xmtf)

    servsecs:=pkt.xmts-2461449600+rtrips
servmicros:=Shr(Mul(pkt.xmtf,1000000),12)+rtripf+500000

-> WriteF('\d\nrtripf\d\n',servmicros,rtripf)
IF servmicros>999999
    servmicros:=servmicros-1000000
    servsecs:=servsecs+1
ENDIF

IF servmicros<0
    servmicros:=0
ENDIF

-> servmicros:=Mul(Shr(pkt.xmtf,16),1000000)

-> servmicros:=!Mul(Div(1.0,Fpow(2.0,32.0)),ntpfrac)!

-> servmicros:=!Mul(
/*
servmicros:=!Fpow(31.0,2.0)
servmicros:=!Div(1.0,servmicros)
*/

-> ,ntpfrac)

/*
RealF(tmp,pkt.xmtf!,8)
StrAdd(temp,tmp)
ntpfrac:=RealVal(temp)
servmicros:=Mul(ntpfrac,1000000.0)
servmicros:=Shr(servmicros,32)
*/
-> servmicros:=!Div(1.0,Mul(ntpfrac,1000000.0))!
-> servmicros:=Mul(Div(1,2^32),pkt.xmtf)

->    servmicros:=((pkt.xmtf+rtripf)/4295)+500000 /****** WAS: (pkt.xmtf+rtripf)/4295 *******/


/*
RealF(temp,servmicros,4)

WriteF('\s\n\d\n',temp,servmicros)
*/

ELSE
servsecs:=Long(buffer)-2461449600 /* num secs from  1 Jan 1900 00:00:00
                                  **            to 31 Dec 1977 23:59:59
                                  */
ENDIF

IF mode2036 THEN servsecs:=servsecs+1833517695

/*
ioreq.io.command:=TR_GETSYSTIME
DoIO(ioreq)
curtime:=ioreq.time.secs
response:=(ioreq.time.secs-measure.secs)/2
*/

response:=0

/********* UTC=0 section! ******
IF utc=0
    servsecs:=servsecs
    StrCopy(ntptime,sectostr(servsecs))
    IF winopen THEN Gt_SetGadgetAttrsA(gads[GADG_SERVER],win,NIL,[GTTX_TEXT,ntptime,NIL])
ENDIF
********** END UTC=0 ********/

IF utc=2 THEN response:=response-utcoffset

servsecs:=servsecs-tzsecs+response
IF extrahalf
    IF tzsecs<0 THEN servsecs:=servsecs+1800 ELSE servsecs:=servsecs-1800
ENDIF

IF winopen THEN SetWindowPointerA(win,[WA_POINTERDELAY,TRUE,NIL])

lastsyncok:=1

ENDPROC

PROC sntpinfo()
DEF info[2048]:STRING,x=0,fhand=NIL,reqtmp1[200]:STRING

IF lastsyncok ->EstrLen(ipaddr)>2
    StringF(info,'\s \s',locstr[MSG_INFO_IP],ipaddr)

	IF lastsntp=1

		StringF(temp,'\n\s \d',locstr[MSG_INFO_STRATUM],pkt.stratum)
		StrAdd(info,temp)

		IF pkt.stratum>1
    		StringF(temp,'\n\s \d.\d.\d.\d',locstr[MSG_INFO_REFERENCE],pkt.refid[0],pkt.refid[1],pkt.refid[2],pkt.refid[3])
    		StrAdd(info,temp)
		ELSE
    		StringF(temp,'\n\s \s',locstr[MSG_INFO_REFERENCE],StrCopy(temp,pkt.refid,4))
	    	StrAdd(info,temp)
		ENDIF

		StringF(temp,'\n\s \d',locstr[MSG_INFO_POLL],pkt.poll)
		StrAdd(info,temp)

		StringF(temp,'\n\s \d',locstr[MSG_INFO_PRECISION],pkt.precision-256)
		StrAdd(info,temp)

		StringF(temp,'\n\s \d.\d',locstr[MSG_INFO_ROOT_DELAY],pkt.rootdelay,pkt.rootdelayf)
		StrAdd(info,temp)

		StringF(temp,'\n\s \d.\d',locstr[MSG_INFO_ROOT_DISP],pkt.rootdisp,pkt.rootdispf)
		StrAdd(info,temp)

		IF leapsec=1 THEN StrAdd(info,'\n\n\s',locstr[MSG_INFO_LEAP_ADDED]) -> Leap second will be added 00:00')
		IF leapsec=2 THEN StrAdd(info,'\n\n\s',locstr[MSG_INFO_LEAP_REMOVED]) -> Leap second will be removed 00:00')
		IF leapsec=3 THEN StrAdd(info,'\n\n\s',locstr[MSG_INFO_ALARM]) -> ALARM!')

	ELSE
    	StrAdd(info,'\n\n\s',locstr[MSG_INFO_USE_SNTP]) ->Use SNTP service for more information')
	ENDIF

    StringF(temp,'\s',locstr[MSG_OK])
   	x:=EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS',info,temp],0,NIL)

ELSE
	StrCopy(temp,locstr[MSG_INFO_NOINFO])
	EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS',temp,'OK'],0,NIL)
ENDIF


/****** info request two ******/

-> IF x<>0 THEN RETURN

->**** downloadfiles()

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
ENDPROC


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
DEF fhand=NIL,x,tmp2[50]:STRING

/*
RightStr(tmp2,tmp,10)

StrCopy(tmp,'PROGDIR:')
StrAdd(tmp,tmp2)
*/

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
ENDPROC 0

PROC checkdataver(strat)
DEF sock,fhand,mod=1,tmp2[50]:STRING,file[512]:STRING

IF strat=1 THEN StrCopy(file,st1db) ELSE StrCopy(file,st2db)

IF StrCmp(file,'http://',7) THEN RETURN 0

StrCopy(tmp,'http://www.eecis.udel.edu/~mills/ntp/')
IF strat=1 THEN StrAdd(tmp,'clock1.htm') ELSE StrAdd(tmp,'clock2.htm')
-> StrAdd(tmp,file)

IF (sock:=httpconnect('www.eecis.udel.edu',1,1))>-1
-> ****** ,1,1  ^^^

/*
    StrCopy(ntptime,'Checking Header...')
    IF winopen THEN Gt_SetGadgetAttrsA(gads[GADG_SERVER],win,NIL,[GTTX_TEXT,ntptime,NIL])
*/

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

/*
    StrCopy(ntptime,' ')
    IF winopen THEN Gt_SetGadgetAttrsA(gads[GADG_SERVER],win,NIL,[GTTX_TEXT,ntptime,NIL])
*/

IF StrCmp(MidStr(temp,buffer,9,3),'200',3)
    IF EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS',locstr[MSG_ASK_DOWNLOAD_NEWER],yesno],0,NIL)
        RETURN 1
    ENDIF
ENDIF
ENDIF

-> WriteF('[data]buff: \s\nchkstr: \s\n',buffer,temp)

ENDPROC 0
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
/*
            ELSE
                StrAdd(motd,buffer,StrLen(buffer)-2)
*/
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
/*
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
*/
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

PROC startappmenu()
  appitem:=AddAppMenuItemA(0,                   -> Our ID# for item
                          0, -> Our UserData
                          locstr[APP_MENU],       -> MenuItem Text
                           appmenu, NIL)         -> MsgPort, no tags

ENDPROC

PROC removeappitem()
  IF appitem THEN RemoveAppMenuItem(appitem)
/*
  IF appmenu
    -> Clear away any messages that arrived at the last moment
    -> and let Workbench know we're done with the messages
    WHILE appmsg:=GetMsg(appmenu) DO ReplyMsg(appmsg)
    DeleteMsgPort(appmenu)
  ENDIF
appmenu:=NIL
*/

appitem:=NIL
ENDPROC

PROC dstpresets(autodst)
IF autodst=0 THEN RETURN

SELECT autodst
    CASE -1 -> Custom
    startmonth:=ostartmonth
    startwday:=ostartwday
    starthour:=ostarthour
    startweek:=ostartweek
    endmonth:=oendmonth
    endwday:=oendwday
    endhour:=oendhour
    endweek:=oendweek
    hemisphere:=ohemisphere

    CASE 1 -> Australia (Sydney/Melbourne)
    startmonth:=10
    startwday:=0
    starthour:=2
    startweek:=0
    endmonth:=3
    endwday:=0
    endhour:=2
    endweek:=0
    hemisphere:=1

    CASE 2 -> Australia (Tasmania/NSW)
    startmonth:=10
    startwday:=0
    starthour:=2
    startweek:=1
    endmonth:=3
    endwday:=0
    endhour:=2
    endweek:=0
    hemisphere:=1

    CASE 3 -> Azerbaijan
    startmonth:=3
    startwday:=0
    starthour:=1
    startweek:=0
    endmonth:=10
    endwday:=0
    endhour:=1
    endweek:=0
    hemisphere:=0

    CASE 4 -> Brazil
    startmonth:=10
    startwday:=0
    starthour:=0
    startweek:=2
    endmonth:=2
    endwday:=0
    endhour:=0
    endweek:=3
    hemisphere:=1

    CASE 5 -> Thule/Newfoundland/Mexico/Bahamas
    startmonth:=4
    startwday:=0
    starthour:=2
    startweek:=1
    endmonth:=10
    endwday:=0
    endhour:=2
    endweek:=0
    hemisphere:=0

    CASE 6 -> Chile
    startmonth:=10
    startwday:=0
    starthour:=0
    startweek:=2
    endmonth:=3
    endwday:=0
    endhour:=0
    endweek:=2
    hemisphere:=1

    CASE 7 -> Cuba/Turks/Caicos
    startmonth:=4
    startwday:=0
    starthour:=0
    startweek:=1
    endmonth:=10
    endwday:=0
    endhour:=0
    endweek:=0
    hemisphere:=0

    CASE 8 -> Egypt
    startmonth:=4
    startwday:=6
    starthour:=0
    startweek:=4
    endmonth:=9
    endwday:=5
    endhour:=23
    endweek:=0
    hemisphere:=0

    CASE 9 -> Europe (Central)
    startmonth:=3
    startwday:=0
    starthour:=2
    startweek:=0
    endmonth:=10
    endwday:=0
    endhour:=3
    endweek:=0
    hemisphere:=0

    CASE 10 -> Falklands
    startmonth:=9
    startwday:=0
    starthour:=0
    startweek:=2
    endmonth:=4
    endwday:=0
    endhour:=0
    endweek:=3
    hemisphere:=1

    CASE 11 -> Fiji
    startmonth:=11
    startwday:=0
    starthour:=2
    startweek:=1
    endmonth:=2
    endwday:=0
    endhour:=3
    endweek:=0
    hemisphere:=1

    CASE 12 -> Jordan
    startmonth:=4
    startwday:=6
    starthour:=0
    startweek:=1
    endmonth:=9
    endwday:=6
    endhour:=0
    endweek:=3
    hemisphere:=0

    CASE 13 -> Lebanon/Mongol
    startmonth:=3
    startwday:=0
    starthour:=0
    startweek:=0
    endmonth:=9
    endwday:=0
    endhour:=0
    endweek:=0
    hemisphere:=0

    CASE 14 -> NZ
    startmonth:=9
    startwday:=0
    starthour:=2
    startweek:=0
    endmonth:=4
    endwday:=0
    endhour:=3
    endweek:=1
    hemisphere:=1

    CASE 15 -> Paraguay
    startmonth:=10
    startwday:=0
    starthour:=0
    startweek:=1
    endmonth:=2
    endwday:=0
    endhour:=0
    endweek:=0
    hemisphere:=1

    CASE 16 -> RussiaAsia  Kirgizstan
    startmonth:=3
    startwday:=0
    starthour:=2
    startweek:=0
    endmonth:=10
    endwday:=0
    endhour:=2
    endweek:=0
    hemisphere:=0

    CASE 17 -> UK   Europe (West)
    startmonth:=3
    startwday:=0
    starthour:=1
    startweek:=0
    endmonth:=10
    endwday:=0
    endhour:=2
    endweek:=0
    hemisphere:=0

    CASE 18 -> United States & Canada
    startmonth:=3
    startwday:=0
    starthour:=2
    startweek:=2
    endmonth:=11
    endwday:=0
    endhour:=2
    endweek:=1
    hemisphere:=0

    CASE 19 -> Zion
    startmonth:=4
    startwday:=6
    starthour:=2
    startweek:=1
    endmonth:=9
    endwday:=6
    endhour:=2
    endweek:=1
    hemisphere:=0

ENDSELECT

startdst(chkautodst_generic())
ENDPROC

PROC startdst(startval)

IF CheckIO(dstioreq)=0
    AbortIO(dstioreq)
    WaitIO(dstioreq)
ENDIF

-> WriteF('\d\n',autodst)

-> IF longtz=0

IF autodst<>0

-> WriteF('\d\n',startval)

            dstioreq.io.command:=TR_ADDREQUEST
            dstioreq.time.secs:=startval
            dstioreq.time.micro:=0
            SendIO(dstioreq)
ENDIF
-> ENDIF

IF (leapsec=1 OR leapsec=2) THEN startleapsec()

ENDPROC

PROC verifyportnumbers()
DEF servie:PTR TO servent -> ,protoe:PTR TO protoent

IF (socketbase:=OpenLibrary(socketlib,socketver))=NIL THEN RETURN

servie:=Getservbyname('daytime','tcp')
IF servie<>NIL THEN defdaytimeport:=servie.port

servie:=Getservbyname('time','tcp')
IF servie<>NIL THEN deftimeport:=servie.port

servie:=Getservbyname('time','udp')
IF servie<>NIL THEN deftimeudpport:=servie.port

servie:=Getservbyname('ntp','udp')
IF servie<>NIL THEN defsntpport:=servie.port

/*
protoe:=Getprotobyname('tcp')
IF protoe<>NIL THEN tcpproto:=protoe.proto

protoe:=Getprotobyname('udp')
IF protoe<>NIL THEN udpproto:=protoe.proto
*/

CloseLibrary(socketbase)
socketbase:=NIL
ENDPROC

PROC doaninc()
DEF dot

dot:=InStr(server,'.')-1
server[dot]:=server[dot]+aninc

IF aninc=1 THEN aninc:=-1 ELSE aninc:=1

ENDPROC

PROC connecterror()
DEF conn

    conn:=Errno()
    SELECT conn
        CASE EBADF
        StrCopy(ntptime,locstr[MSG_ERR_CONN_EBADF])
        CASE EADDRNOTAVAIL
        StrCopy(ntptime,locstr[MSG_ERR_CONN_EADDRNOTAVAIL])
        CASE EAFNOSUPPORT
        StrCopy(ntptime,locstr[MSG_ERR_CONN_EAFNOSUPPORT])
        CASE EISCONN
        StrCopy(ntptime,locstr[MSG_ERR_CONN_EISCONN])
        CASE ETIMEDOUT
        StrCopy(ntptime,locstr[MSG_ERR_CONN_ETIMEDOUT])
        CASE ECONNREFUSED
        StrCopy(ntptime,locstr[MSG_ERR_CONN_ECONNREFUSED])
        CASE ENETUNREACH
        StrCopy(ntptime,locstr[MSG_ERR_CONN_ENETUNREACH])
        CASE EADDRINUSE
        StrCopy(ntptime,locstr[MSG_ERR_CONN_EADDRINUSE])
        CASE EINPROGRESS
        StrCopy(ntptime,locstr[MSG_ERR_CONN_EINPROGRESS])
        CASE EALREADY
        StrCopy(ntptime,locstr[MSG_ERR_CONN_EALREADY])
        DEFAULT
        StrCopy(ntptime,locstr[MSG_ERR_CONN_DEFAULT])
    ENDSELECT
->    Raise(ERR_MEM)
IF winopen
    Gt_SetGadgetAttrsA(gads[GADG_SERVER],win,NIL,[GTTX_TEXT,ntptime,NIL])
ENDIF
ENDPROC

PROC chkautodst_generic(nextyear=0)
DEF datestr:clockdata,dststr:clockdata,utcstr:clockdata
DEF dstsecs,utcsecs,dstoff=0,dston=1,stday
DEF sm,sw,swd,sh,em,ew,ewd,eh

IF hemisphere=1
-> Switch everything around!
    dstoff:=1
    dston:=0
    em:=startmonth
    sm:=endmonth
    ewd:=startwday
    swd:=endwday
    eh:=starthour
    sh:=endhour
    ew:=startweek
    sw:=endweek
ELSE
    sm:=startmonth
    em:=endmonth
    swd:=startwday
    ewd:=endwday
    sh:=starthour
    eh:=endhour
    sw:=startweek
    ew:=endweek
ENDIF

ioreq.io.command:=TR_GETSYSTIME

DoIO(ioreq)

Amiga2Date(ioreq.time.secs,datestr)

CopyMem(ioreq.time,timemem,SIZEOF timeval)

IF nextyear THEN datestr.year:=datestr.year+1

dststr.mday:=datestr.mday-datestr.wday+swd

IF sw=0
    IF sm=2 THEN stday:=21 ELSE stday:=25
    WHILE dststr.mday<stday DO dststr.mday:=dststr.mday+7
ELSE
    stday:=(sw-1)*7
    WHILE dststr.mday<=stday DO dststr.mday:=dststr.mday+7
    IF dststr.mday>(stday+7) THEN dststr.mday:=dststr.mday-7
ENDIF

    dststr.month:=sm
    dststr.wday:=swd
    dststr.hour:=sh
    dststr.year:=datestr.year
    dststr.min:=0
    dststr.sec:=0

    dstsecs:=Date2Amiga(dststr)

    utcstr.mday:=datestr.mday-datestr.wday+ewd
    
IF ew=0
    IF em=2 THEN stday:=21 ELSE stday:=25
    WHILE utcstr.mday<stday DO utcstr.mday:=utcstr.mday+7
ELSE
    stday:=(ew-1)*7
    WHILE utcstr.mday<stday DO utcstr.mday:=utcstr.mday+7
    IF utcstr.mday>(stday+7) THEN utcstr.mday:=utcstr.mday-7
ENDIF

->    WriteF('sunday \d\n',utcstr.mday)
    
    utcstr.month:=em
    utcstr.wday:=ewd
    utcstr.hour:=eh
    utcstr.year:=datestr.year
    utcstr.min:=0
    utcstr.sec:=0

    utcsecs:=Date2Amiga(utcstr)

IF timemem.secs<dstsecs THEN dstswitch(dstoff)

IF timemem.secs>=dstsecs
    IF (timemem.secs>utcsecs-3600)
        IF timemem.secs>=utcsecs
            dstswitch(dstoff)
        ELSEIF (timemem.secs<(utcsecs-3600))
            dstswitch(dston)
        ENDIF
    ENDIF
ENDIF

-> WriteF('utc: \d\ndst: \d\ncurrent: \d\n',utcsecs,dstsecs,timemem.secs)
IF timemem.secs>=utcsecs
    IF timemem.secs>=dstsecs
        utcsecs:=chkautodst_generic(1)
        RETURN utcsecs
    ENDIF
ENDIF

IF dstsecs<utcsecs
  IF dstsecs>timemem.secs THEN RETURN dstsecs ELSE RETURN utcsecs
ELSE
  IF utcsecs>timemem.secs THEN RETURN utcsecs ELSE RETURN dstsecs
ENDIF

ENDPROC

PROC readtz()
DEF comma,comma2,dot,dot2,tmp2[30]:STRING

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

    IF tzfile:=Open('ENV:FACTS/DST',MODE_OLDFILE)
        dst:=FgetC(tzfile)
        Close(tzfile)
ENDIF

IF tzfile:=Open('ENV:TZ',MODE_OLDFILE)
    Fgets(tzfile,buffer,50)
    Close(tzfile)
    /**** old routine below ****/
    MidStr(temp,buffer,3)
    tz:=Val(temp)
    tzsecs:=tz*3600
    MidStr(tzname1,buffer,0,3)
    IF StrLen(buffer)>6
        IF (comma:=InStr(buffer,','))<>-1
            MidStr(tzname2,buffer,comma-4,4)
            IF Val(MidStr(temp,tzname2,0,1))>0
                MidStr(tzname2,buffer,comma-3,3)
                StrAdd(tzname2,' ')
            ENDIF
            comma2:=InStr(buffer,',',comma+1)
            MidStr(temp,buffer,comma+2,comma2-comma-2)
            dot:=InStr(temp,'.')
            MidStr(tmp2,temp,0,dot)
            ostartmonth:=Val(tmp2)
            dot2:=InStr(temp,'.',dot+1)
            MidStr(tmp2,temp,dot+1,dot2-dot-1)
            ostartweek:=Val(tmp2)
            IF ostartweek=5 THEN ostartweek:=0
            MidStr(tmp2,temp,dot2+1,1)
            ostartwday:=Val(tmp2)
IF (dot:=InStr(temp,'/',dot2+1))<>-1
    MidStr(tmp2,temp,dot2+3,dot-dot2-2)
    ostarthour:=Val(tmp2)
ELSE
    ostarthour:=2
ENDIF

            comma:=InStr(buffer,',',comma2+1)
            MidStr(temp,buffer,comma2+2,comma-comma2-2)
            dot:=InStr(temp,'.')
            MidStr(tmp2,temp,0,dot)
            oendmonth:=Val(tmp2)
            dot2:=InStr(temp,'.',dot+1)
            MidStr(tmp2,temp,dot+1,dot2-dot-1)
            oendweek:=Val(tmp2)
            IF oendweek=5 THEN ostartweek:=0
            MidStr(tmp2,temp,dot2+1,1)
            oendwday:=Val(tmp2)
IF (dot:=InStr(temp,'/',dot2+1))<>-1
    MidStr(tmp2,temp,dot2+3,dot-dot2-2)
    oendhour:=Val(tmp2)
ELSE
    oendhour:=2
ENDIF

IF ostartweek=5 THEN ostartweek:=0
IF oendweek=5 THEN oendweek:=0

-> IF autodst<>0 THEN autodst:=-1
changelong(1)
dstpresets(autodst)

        ELSE
/**** old routine ****/
            dst:=1
            MidStr(tzname2,buffer,StrLen(buffer)-4,4)
            IF Val(MidStr(temp,tzname2,0,1))>0
                MidStr(tzname2,buffer,StrLen(buffer)-3,3)
                StrAdd(tzname2,' ')
            ENDIF
            IF wbstart=0 THEN WriteF('\s (\s)\n',locstr[MSG_IN_DST],tzname2)
        ENDIF
    ENDIF
    IF wbstart=0
        WriteF(locstr[MSG_TZ],tzname1,-tz)
        WriteF('\n\n')
    ENDIF
ELSE
    IF localebase
        StrCopy(tzname1,'SYS')
        tz:=(localeinfo.gmtoffset)/60
    ELSE
        StrCopy(tzname1,'UTC')
    ENDIF
ENDIF

ENDPROC

PROC tryalternative(dodef=1)
DEF s

s:=servnum

IF trynext
REPEAT
    servnum:=servnum+1
    loadserver()
    IF servnum=s
        IF winopen THEN updategui()
        RETURN
    ENDIF
    IF retrievetime()=0
        IF dodef THEN dodefop()
        displaytimes()
        IF winopen THEN updategui()
        RETURN
    ENDIF
->    servnum:=servnum+1
UNTIL servnum=s
-> servnum:=s
IF winopen THEN updategui()
RETURN
ENDIF

s:=service

IF nextservice
    selectservice()
    WHILE s<>service
    IF retrievetime()=0
        IF dodef THEN dodefop()
        displaytimes()
        IF winopen THEN updategui()
        RETURN
->    ELSE
-> >>>>>>>>
    ENDIF
    selectservice()
    ENDWHILE
ENDIF

IF aninc
    doaninc()
IF nextservice
    selectservice()
    WHILE s<>service
    IF retrievetime()=0
        IF dodef THEN dodefop()
        displaytimes()
        IF winopen THEN updategui()
        RETURN
->    ELSE
-> >>>>>>>>
    ENDIF
    selectservice()
    ENDWHILE
ENDIF
ENDIF

IF winopen THEN updategui()
ENDPROC

PROC selectservice()
    SELECT service
        CASE 123
            service:=372
            port:=timeudpport
        CASE 372
            service:=37
            port:=timeport
        CASE 37
            service:=13
            port:=daytimeport
        CASE 13
            service:=123
            port:=sntpport
    ENDSELECT
ENDPROC

PROC updategui()
        menu()
        IF LayoutMenusA(menustrip,vi,[GTMN_NEWLOOKMENUS,TRUE,NIL])
            SetMenuStrip(win,menustrip) -> *IF*
        ENDIF

Gt_SetGadgetAttrsA(gads[GADG_TIMESERVER],win,NIL,[GTST_STRING,server,NIL])
Gt_SetGadgetAttrsA(gads[GADG_PORT],win,NIL,[GTIN_NUMBER,port,NIL])
Gt_SetGadgetAttrsA(gads[GADG_TIMEINT],win,NIL,[GTIN_NUMBER,timeint,NIL])
ENDPROC

PROC syncclock(dodef=1)
                IF retrievetime()=0
                    IF dodef THEN dodefop()
                    displaytimes()
                ELSE
                    tryalternative(dodef)
                ENDIF

-> IF nocmd=0 THEN SystemTagList(cmd,[SYS_ASYNCH,TRUE,NIL])
-> moved above to dodefop routine so it only executes on success 03/06/02
ENDPROC

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

PROC showtime()
                    ioreq.io.command:=TR_GETSYSTIME
                    DoIO(ioreq)
                    curtime:=ioreq.time.secs
                    StrCopy(sd,sectostr(curtime))
IF win
    IF win.height < (tbar+10)
        SetWindowTitles(win,sd,-1)
    ELSE
        StrCopy(sysdate,sd)
        IF alwaysupdate THEN Gt_SetGadgetAttrsA(gads[GADG_AMIGA],win,NIL,[GTTX_TEXT,sysdate,NIL])
    ENDIF
ENDIF
ENDPROC

PROC startleapsec(quick=0)
DEF secs,datestr:clockdata

IF obsession=0 THEN RETURN

IF CheckIO(dstioreq)=0
    AbortIO(dstioreq)
    WaitIO(dstioreq)
ENDIF

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

IF quick=0
curtime:=curtime+86400

Amiga2Date(curtime,datestr)

    datestr.hour:=0
    datestr.min:=0
    datestr.sec:=0

    lssecs:=Date2Amiga(datestr)
ENDIF

-> WriteF('\d\n\d\n',curtime,secs)

            dstioreq.io.command:=TR_ADDREQUEST
            dstioreq.time.secs:=lssecs
            dstioreq.time.micro:=0
            SendIO(dstioreq)

saveleapsecs()

ENDPROC

PROC leapsecchg()
GetSysTime(measure)
/*
        ioreq.io.command:=TR_GETSYSTIME
        DoIO(ioreq)

CopyMem(ioreq.time,timemem,SIZEOF timeval)

measure:=timemem
*/
curtime:=measure.secs
        
IF leapsec=1 THEN setclock(curtime+1)
IF leapsec=2 THEN setclock(curtime-1)

leapsec:=0

IF autodquiet=0 THEN EasyRequestArgs(0,[SIZEOF easystruct,0,'FACTS',locstr[MSG_LEAPSEC],'OK'],0,NIL)

-> WriteF('leap second\n')

saveleapsecs()

ENDPROC

PROC saveleapsecs()
DEF fhand=0

IF obsession=0 THEN RETURN

    IF (fhand:=Lock('ENVARC:FACTS',ACCESS_WRITE))=0
        fhand:=CreateDir('ENVARC:FACTS')
    ENDIF
    UnLock(fhand)
    IF fhand:=Open('ENVARC:FACTS/LEAPSEC',MODE_NEWFILE)
        StringF(temp,'\d,\d',leapsec,lssecs)
        Fputs(fhand,temp)
        Close(fhand)
    ENDIF
ENDPROC

PROC checkoldleapsecs()
DEF fhand=0,comma=0

IF obsession=0 THEN RETURN

    IF fhand:=Open('ENVARC:FACTS/LEAPSEC',MODE_OLDFILE)
        Fgets(fhand,buffer,100)
        Close(fhand)
        StrCopy(temp,buffer)
        comma:=InStr(temp,',')
        leapsec:=Val(StrCopy(tmp,temp,comma))
        lssecs:=Val(MidStr(temp,temp,comma+1))
        IF leapsec>0 AND leapsec<3 THEN startleapsec(1)
    ENDIF
ENDPROC

PROC changelong(state)
IF state
    longtz:=1
    autodquiet:=1
    ignoretz:=1
    IF autodst<>0 THEN autodst:=-1
    stopnotify()
ELSE
    longtz:=0
ENDIF

IF win
    menu()
    IF LayoutMenusA(menustrip,vi,[GTMN_NEWLOOKMENUS,TRUE,NIL])
        SetMenuStrip(win,menustrip) -> *IF*
    ENDIF
ENDIF
ENDPROC

PROC stopnotify()
DEF msg=0
    IF snot
        EndNotify(tznotify) -> prob!
        IF notifyport
            WHILE msg:=GetMsg(notifyport) DO ReplyMsg(msg)
            DeleteMsgPort(notifyport)
        ENDIF
    snot:=0
    ENDIF
ENDPROC

PROC stopttnotify()
DEF msg=0
    IF ttsnot
        EndNotify(ttnotify) -> prob!
        IF ttnotifyport
            WHILE msg:=GetMsg(ttnotifyport) DO ReplyMsg(msg)
            DeleteMsgPort(ttnotifyport)
        ENDIF
    ttsnot:=0
    ENDIF
ENDPROC

PROC killautodsttimer()
    IF dstioreq
        IF CheckIO(dstioreq)=0
            AbortIO(dstioreq)
            WaitIO(dstioreq)
        ENDIF
        CloseDevice(dstioreq)
        DeleteIORequest(dstioreq)
        IF dstmsgport THEN DeleteMsgPort(dstmsgport)
    ENDIF
ENDPROC

PROC loadserver()
DEF servfile[100]:STRING,colon

StringF(servfile,'ENV:FACTS/TIMESERVER.\d',servnum)

IF lock:=Lock(servfile,ACCESS_READ)
    UnLock(lock)
ELSE
    IF servnum>0
        servnum:=0
        StringF(servfile,'ENV:FACTS/TIMESERVER.\d',servnum)
        IF lock:=Lock(servfile,ACCESS_READ)
            UnLock(lock)
        ELSE
            StrCopy(server,'pool.ntp.org')
            RETURN
        ENDIF
    ELSE
        StrCopy(server,'pool.ntp.org')
        RETURN
    ENDIF
ENDIF

    ntphost:=Open(servfile,MODE_OLDFILE)
    Fgets(ntphost,buffer,100)
    Close(ntphost)
    StrCopy(temp,buffer)
    colon:=InStr(temp,':')
    IF colon<>-1
        StrCopy(server,temp,colon)
        MidStr(temp,temp,colon+1)
        port:=Val(temp)
->        service:=Val(temp)
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


ENDPROC

PROC autocompat()
                        ioreq.io.command:=TR_GETSYSTIME
                        DoIO(ioreq)
                        curtime:=ioreq.time.secs
                        curmics:=ioreq.time.micro


ENDPROC

PROC clockdrift()
DEF sdrifted=0,mdrifted=0

-> check current time
GetSysTime(measure)
/*
ioreq.io.command:=TR_GETSYSTIME

DoIO(ioreq)

CopyMem(ioreq.time,timemem,SIZEOF timeval)

measure:=timemem
*/
curtime:=measure.secs
curmics:=measure.micro

-> end checktime

IF mdrift
	mdrifted:=((curtime-psecs)/mdrifts)*mdrift

	sdrifted:=Div(mdrifted,1000000)

	servsecs:=curtime+sdrifted
	servmicros:=curmics+(mdrifted-Mul(sdrifted,1000000))

/*
	IF servmicros>99999
		servmicros:=servmicros-1000000
		servsecs:=servsecs+1
	ENDIF
	*/

	dadj:=1

	RETURN 0 -> ok

ENDIF

RETURN 1 -> failed
ENDPROC
