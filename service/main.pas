unit Main;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, Sockets;

type

  { TDaemonWorkerThread }

  TDaemonWorkerThread = class(TThread)
  private
    host: string;
    port: word;
    procedure log(aMsg: string);
    procedure ReadDefault;
    function Test(aHost: string): boolean;
    procedure SetVolume(aSetVol: integer);
  public
    constructor Create;
    destructor Destroy; override;
    procedure Execute; override;
  end;

implementation

uses
  ecode_wewn, Unix;

{ TDaemonWorkerThread }

procedure TDaemonWorkerThread.log(aMsg: string);
var
  d: TDateTime;
  ss: TStringList;
begin
  d:=now;
  ss:=TStringList.Create;
  try
    if FileExists('/home/tao/serwis_log.txt') then ss.LoadFromFile('/home/tao/serwis_log.txt');
    ss.Add(FormatDateTime('yyyy-mm-dd hh:nn:ss',d)+': '+aMsg);
    ss.SaveToFile('/home/tao/serwis_log.txt');
  finally
    ss.Free;
  end;
end;

procedure TDaemonWorkerThread.ReadDefault;
var
  c: TStringList;
  i,a: integer;
  s,s1,s2: string;
begin
  c:=TStringList.Create;
  try
    c.LoadFromFile('/etc/default/gpio.client');
    for i:=0 to c.Count-1 do
    begin
      s:=trim(c[i]);
      if s='' then continue;
      a:=pos('#',s);
      if a>0 then delete(s,a,maxint);
      s:=trim(s);
      if s='' then continue;
      s1:=trim(GetLineToStr(s,1,'='));
      s2:=trim(GetLineToStr(s,2,'='));
      if s1='HOST' then host:=s2 else
      if s1='PORT' then port:=StrToInt(s2);
    end;
  finally
    c.Free;
  end;
end;

function TDaemonWorkerThread.Test(aHost: string): boolean;
var
  s,s1,s2,s3,s4: string;
  i: integer;
begin
  s:=trim(aHost);
  (* jeśli puste nie ma nic do roboty *)
  if s='' then
  begin
    result:=false;
    exit;
  end;
  (* sprawdzam czy w adresie występuje litera *)
  for i:=1 to length(s) do if (not IsDigit(s[i])) and (s[i]<>'.') then
  begin
    result:=true;
    exit;
  end;
  (* sprawdzam czy adres IP4 jest wpisany prawidłowo *)
  if pos(' ',s)>0 then
  begin
    result:=false;
    exit;
  end;
  s1:=GetLineToStr(s,1,'.');
  s2:=GetLineToStr(s,2,'.');
  s3:=GetLineToStr(s,3,'.');
  s4:=GetLineToStr(s,4,'.');
  if (s1<>'') and (s2<>'') and (s3<>'') and (s4<>'') then result:=true else result:=false;
end;

procedure TDaemonWorkerThread.SetVolume(aSetVol: integer);
var
  b: integer;
  s: string;
begin
  if aSetVol<0 then s:='-' else s:='+';
  b:=abs(aSetVol);
  fpsystem('amixer sset Master playback '+IntToStr(b)+'%'+s);
end;

constructor TDaemonWorkerThread.Create;
begin
  inherited Create(true);
  ReadDefault;
  self.Start;
end;

destructor TDaemonWorkerThread.Destroy;
begin
  inherited Destroy;
end;

procedure TDaemonWorkerThread.Execute;
var
  adres: TInetSockAddr;
  bufer: string[255];
  sock,odebrane: longint;
  s,s1,s2: string;
begin
  log('execute watek');
  sock:=fpSocket(AF_INET,SOCK_STREAM,0);
  if sock=-1 then halt(100);
  adres.sin_family:=AF_INET;
  adres.sin_port:=htons(port);
  adres.sin_addr:=StrToNetAddr(host);

  if fpConnect(sock,@adres,sizeof(adres))=-1 then halt(101);
  bufer:='tryb=gpio'+#0;
  fpsend(sock,@bufer[1],length(bufer)+1,1);

  while not Terminated do
  begin
    odebrane:=fprecv(sock,@bufer[1],255,0);
    if odebrane<1 then
    begin
      sleep(100);
      continue;
    end;
    log('- odebrano: '+IntToStr(odebrane));
    SetLength(bufer,odebrane);
    s:=bufer;
    log('- text: '+s);
    s1:=GetLineToStr(s,1,'=');
    s2:=GetLineToStr(s,2,'=');
    if s1='pilot' then
    begin
      if s2='key_volume_down' then
      begin
        SetVolume(-5);
      end else
      if s2='key_volume_up' then
      begin
        SetVolume(5);
      end else
      if s2='key_power' then
      begin
        //shutdown.execute;
      end;
    end;
  end;

  if fpShutdown(sock,2)=-1 then halt(102);
  if CloseSocket(sock)=-1 then halt(103);
  log('execute end watek');
end;

end.

