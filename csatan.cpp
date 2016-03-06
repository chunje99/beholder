#include "csatan.h"
#include "gen/strUtil.h"
#include "gen/textEncoder.h"
#include "mon/netSatan.h"


CSatan::CSatan()
{
    m_checkIdxGuru = 0;
    m_checkIdxFront = 0;
}

CSatan::~CSatan()
{
}

int CSatan::Init()
{
   int retVal = m_sm.Setup("./satan.ini");
   if( retVal != 0 )
       return -1;
   if( (retVal = _Parse()) != 0 )
       return -1;
   return 0;
}

int CSatan::getHostIPList( CSsArray& list )
{
    for( int i = 0 ; i < m_hostArray.Count() ; i++ )
        list.Add( m_hostArray.getStr0(i), m_hostArray.getStr1(i));
    return 0;
}

int CSatan::getGuruList( CSsArray& list )
{
    for( int i = 0 ; i < m_guruArray.Count() ; i++ )
        list.Add( m_guruArray.getStr0(i), m_guruArray.getStr1(i));
    return 0;
}

int CSatan::getFrontList( CSsArray& list )
{
    for( int i = 0 ; i < m_frontArray.Count() ; i++ )
        list.Add( m_frontArray.getStr0(i), m_frontArray.getStr1(i));
    return 0;
}


int CSatan::_Parse()
{
    CStrArray parseList = *m_sm.getStrArray();

    int retVal = 0;
   // m_cmdArray;
    int act = 0;
    GString cmd;
    CTextEncoder te;
    for( int i = 0 ; i < parseList.Count() ; i++ )
    {
        char* bufStr = parseList.Get(i);
        CStrUtil::TrimBoth(bufStr);
        GString buf = bufStr;
        if( act == 1 ) act++;

        if( buf.GetAt(0) == ';'  ) continue;
        if( buf.GetAt(0) == '[' && buf.GetAt( buf.GetLength()-1) == ']' ) ///Cmd Name
        {
            act = 1;
            if( cmd != "" )
            {
                m_cmdArray.Add( (const char*)cmd, te.get() );
                te.Clear();
            }
            cmd = buf.Mid( 1, buf.GetLength() - 2 );
            continue;
        }
        else
        {

            if( buf.Find( "=" ) < 0 ) continue;

            buf.Replace( "=", "," );
            CTextProto tp( (const char*)buf );
            retVal = tp.Parse();
            if( retVal == -1 || tp.FieldCount() < 2 )
                continue;
            m_hostArray.Add( tp.getToken(0), tp.getToken(1));
            m_lineArray.Add( buf );

            if( cmd == "GURUGURU" )
            {
                m_guruArray.Add( tp.getToken(0), tp.getToken(1));
                m_guruLine.Add( bufStr );
            }
            else if( cmd == "FRONTEND" )
            {
                m_frontArray.Add( tp.getToken(0), tp.getToken(1));
                m_frontLine.Add( bufStr );
            }
            else
                te.Append( "S", bufStr );
        }

    }
    return 0;
}

int CSatan::CheckServer( int type )
{
    GString line;
    if( type == ESERVER_TYPE_GURU )
    {
        line = m_guruLine.Get( m_checkIdxGuru );
        m_checkIdxGuru++;
        if( m_checkIdxGuru >= m_guruLine.Count() )
            m_checkIdxGuru = 0;
    }
    else if( type == ESERVER_TYPE_FRONT )
    {
        line = m_frontLine.Get( m_checkIdxFront );
        m_checkIdxFront++;
        if( m_checkIdxFront >= m_frontLine.Count() )
            m_checkIdxFront = 0;
    }

    return _CheckServer( type, (const char*)line );
}

int CSatan::_CheckServer( int type, const char* line  )
{
    GString buf = line;
    buf.Replace( "=", "," );
    CTextProto tp( (const char*)buf );
    if( tp.Parse() == -1 )
    {
        fprintf( stderr, "Server Parse Error[%s]\n", line);
        return -1;
    }
    if( tp.FieldCount() <= 3 )
    {
        fprintf( stderr, "Server Field Error[%s]\n", line);
        return -1;
    }

    GString host = tp.getToken();
    GString ip 	= tp.getToken();
    GString loc  	= tp.getToken();

    //fprintf( stderr, "connect %s , %s\n",  (const char*)host, (const char*)ip );
    CSatanClient satanClient( (const char*)ip, 10, 10 );
    if( satanClient.ConnectionBad() )
    {
        fprintf( stderr, "Satan Con Error %s %s\n", (const char*)host, (const char*)ip );
        return -1;
    }
    ///get cmd
    int idx = -1;
    while(1) ///tp loop
    {
        if( tp.RemainCount() <= 0 )
            break;
        GString serverCmd = tp.getToken();
        idx = m_cmdArray.GetIndex0( (const char*)serverCmd );
        if( idx >= 0 )
        {
            CTextProto tpServer( m_cmdArray.getStr1(idx) );
            if( tpServer.Parse() == -1 )
            {
                fprintf( stderr, "tpServer Parse Error[%s]\n", m_cmdArray.getStr1(idx) );
                return -1;
            }
            GString checkCmd;
            while(1) ///tpServer loop
            {
                if( tpServer.RemainCount() <= 0 )
                    break;
                checkCmd = tpServer.getToken();
                if( checkCmd.Find( "send=") >= 0)
                {
                    checkCmd.Replace( "send=", "" );
                    //fprintf( stderr, "CMD: %s\n",  (const char*)checkCmd );
                    _SendSatan( satanClient, (const char*)checkCmd, (const char*)serverCmd );
                }
            } ///tpServer loop

        }
        else
        {
            fprintf( stderr, "CMD: NOT FOUND\n" );
        }
    } /// tp loop

    return 0;
}

int CSatan::_SendSatan( CSatanClient& satanClient, const char* send, const char* serverCmd )
{
    CTextProto tp( send );
    if( tp.Parse() == -1 || tp.FieldCount() < 1 )
    {
        fprintf( stderr, "SendSata Parse Error[%s]\n", send );
        return -1;
    }
    GString cmd = tp.getToken();
    int code = CNetSatan::MatchCode( (const char*)cmd );
    if( code < 0 ) return -1;

    switch( code )
    {
         case eNSatanCode_Time:      return _Time( satanClient);
         case eNSatanCode_Df:        return _Df( satanClient, tp, serverCmd );
         case eNSatanCode_DfAll:     return _DfAll(satanClient);
         case eNSatanCode_Memory:    return _Memory(satanClient);
         case eNSatanCode_Ps:        return _Ps(satanClient, tp );
         case eNSatanCode_Load:      return _Load(satanClient);
         case eNSatanCode_Postgres:  return _Postgres(satanClient);
         case eNSatanCode_Server:    return _Server(satanClient, tp);
         case eNSatanCode_IpInfo:    return _IpInfo(satanClient);
         case eNSatanCode_Sysctl:    return _Sysctl(satanClient);
         case eNSatanCode_NfsCount:  return _NfsCount(satanClient);
         case eNSatanCode_Relay:     return _Relay(satanClient);
    }

    return 0;
}

int CSatan::_Time( CSatanClient& satanClient )
{
    return 0;
}

int CSatan::_Df( CSatanClient& satanClient,  CTextProto& tp, const char* serverCmd )
{
    if( tp.RemainCount() < 1 )
        return -1;
    GString sendCmd = tp.getToken();
    if( sendCmd == "%s" ) sendCmd = serverCmd;

    CMountRec mountRec;
    int retVal = satanClient.Df( (const char*)sendCmd, mountRec );
    if( retVal != 0) return retVal;

    fprintf( stderr, "DF : %s %d %d %d\n", mountRec.getName(), mountRec.getAvail(), mountRec.getTotal(), mountRec.getUsed()  );
    return 0;
}

int CSatan::_DfAll( CSatanClient& satanClient )
{
    CMountArray mountArray;
    int retVal = satanClient.DfAll( mountArray );
    if( retVal != 0 ) return retVal;
    for( int i = 0 ; i < mountArray.Count() ; i++ )
    {
        CMountRec mountRec = *mountArray.Get(i);
        fprintf( stderr, "DFALL : %s %d %d %d\n",
             mountRec.getName(), mountRec.getAvail(), mountRec.getTotal(), mountRec.getUsed()  );
    }
    return 0;
}

int CSatan::_Memory( CSatanClient& satanClient )
{
    CMemoryRec memoryRec;
    int retVal = satanClient.Memory( memoryRec );
    if( retVal != 0 ) return retVal;

    fprintf( stderr, "Memory : %d %d %d %d %d %d %d\n",
                    memoryRec.getBuf(),
                    memoryRec.getCache(),
                    memoryRec.getFree(),
                    memoryRec.getInact(),
                    memoryRec.getTotal(),
                    memoryRec.getWired(),
                    memoryRec.getActive()
             );
    return 0;
}

int CSatan::_Ps( CSatanClient& satanClient,  CTextProto& tp )
{
    if( tp.RemainCount() < 1 )
        return -1;
    GString sendCmd = tp.getToken();

    int count;
    int retVal = satanClient.Ps( sendCmd, count );
    if( retVal != 0 ) return retVal;

    fprintf( stderr, "PS : %s %d\n", (const char*)sendCmd, count );

    return 0;
}

int CSatan::_Load( CSatanClient& satanClient )
{
    CLoadRec loadRec;
    int retVal = satanClient.Load( loadRec );
    if( retVal != 0 ) return retVal;

    fprintf( stderr, "LOAD : %d %d %d\n", loadRec.getLoad(), loadRec.getLoad5(), loadRec.getLoad15() );
    return 0;
}

int CSatan::_Postgres( CSatanClient& satanClient )
{
    return 0;
}

int CSatan::_Server( CSatanClient& satanClient,  CTextProto& tp )
{
    if( tp.RemainCount() < 2 )
        return -1;
    int port = tp.getTokenI();
    GString cmd = tp.getToken();
    CStrArray result;
    int retVal = satanClient.Server( port, (const char*)cmd, result );
    if( retVal != 0 ) return retVal;
    fprintf( stderr, "Server : ");
    for( int i = 0 ; i < result.Count() ; i++ )
        fprintf( stderr, " %s ", result.Get(i) );

    fprintf( stderr, "\n" );

    return 0;
}

int CSatan::_IpInfo( CSatanClient& satanClient )
{
    CIfArray ifArray;
    int retVal = satanClient.IpInfo( ifArray );
    if( retVal != 0 ) return retVal;

    for( int i = 0 ; i < ifArray.Count() ; i++ )
    {
        CIfRec ifRec = *ifArray.Get(i);
        fprintf( stderr, "IPINFO : %s %s %s\n", ifRec.getEther(), ifRec.getIp(), ifRec.getName() );
    }

    return 0;
}

int CSatan::_Sysctl( CSatanClient& satanClient )
{
    CStrArray nameList, valueList;
    int retVal = satanClient.Sysctl( nameList, valueList );
    if( retVal != 0 ) return retVal;
    fprintf( stderr, "SYSCTL : ");
    for( int i = 0 ; i < nameList.Count() ; i++ )
    {
        fprintf( stderr, "%s=%s ", nameList.Get(i), valueList.Get(i) );
    }
    fprintf( stderr, "\n" );

    return 0;
}

int CSatan::_NfsCount( CSatanClient& satanClient )
{
    int count;
    int retVal = satanClient.NfsCount( count );
    if( retVal != 0 ) return retVal;
    fprintf( stderr, "NFS : %d\n", count );
    return 0;
}

int CSatan::_Relay( CSatanClient& satanClient )
{
    return 0;
}
