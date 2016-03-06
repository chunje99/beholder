#ifndef CSATAN_H
#define CSATAN_H

#include <stdio.h>
#include "gen/ssArray.h"
#include "gen/strArray.h"
#include "gen/scriptMan.h"
#include "GString.h"
#include "gen/textProto.h"
#include "mon/satanClient.h"

enum{
    ESERVER_TYPE_GURU 	= 0,
    ESERVER_TYPE_FRONT 	= 1,
};

class CSatan
{
public:
    CSatan();
    ~CSatan();
    int Init(); ///file open
    int getHostIPList( CSsArray& list );
    int getGuruList( CSsArray& list );
    int getFrontList( CSsArray& list );

    int CheckServer( int type );
    int GetCheckIdx( int type );

private:
    int m_checkIdxGuru;
    int m_checkIdxFront;
    CScriptMan m_sm;

    CStrArray m_lineArray;
    CSsArray m_hostArray;

    CSsArray m_frontArray;
    CSsArray m_guruArray;
    CStrArray m_frontLine;
    CStrArray m_guruLine;

    CSsArray m_cmdArray;

    int _Parse();
    int _CheckServer( int type, const char* line );
    int _SendSatan( CSatanClient& satanClient, const char* send, const char* serverCmd );

    int _Time( CSatanClient& satanClient );
    int _Df(  CSatanClient& satanClient, CTextProto& tp , const char* serverCmd );
    int _DfAll(CSatanClient& satanClient );
    int _Memory(CSatanClient& satanClient );
    int _Ps(CSatanClient& satanClient,  CTextProto& tp );
    int _Load(CSatanClient& satanClient );
    int _Postgres(CSatanClient& satanClient );
    int _Server(CSatanClient& satanClient, CTextProto& tp );
    int _IpInfo(CSatanClient& satanClient );
    int _Sysctl(CSatanClient& satanClient );
    int _NfsCount(CSatanClient& satanClient );
    int _Relay(CSatanClient& satanClient );
};

#endif // CSATAN_H
