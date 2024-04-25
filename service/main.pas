unit Main;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, process, Forms, Controls, Graphics, Dialogs, ExtCtrls,
  AsyncProcess, NetSocket, ExtShutdown, lNet;

type

  { TFMain }

  TFMain = class(TForm)
    shutdown: TExtShutdown;
    net: TNetSocket;
    autorun: TTimer;
    autoconnect: TTimer;
    procedure autoconnectTimer(Sender: TObject);
    procedure autorunTimer(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure netConnect(aSocket: TLSocket);
    procedure netProcessMessage;
    procedure netReceiveString(aMsg: string; aSocket: TLSocket;
      aBinSize: integer; var aReadBin: boolean);
    procedure netStatus(aActive, aCrypt: boolean);
  private
    host: string;
    port: word;
    con_wyjscie: boolean;
    procedure wczytaj_default;
    function test(aHost: string): boolean;
    procedure SetVolume(aSetVol: integer);
  public
  end;

  { TDaemonWorkerThread }

  TDaemonWorkerThread = class(TThread)
  private
    FMain: TFMain;
  public
    constructor Create;
    destructor Destroy; override;
    procedure Execute; override;
  end;

implementation

uses
  ecode;

{$R *.lfm}

{ TDaemonWorkerThread }

constructor TDaemonWorkerThread.Create;
begin
  inherited Create(True);
  FMain:=TFMain.Create(nil);
end;

destructor TDaemonWorkerThread.Destroy;
begin
  FMain.Free;
  inherited Destroy;
end;

procedure TDaemonWorkerThread.Execute;
begin
  while not Terminated do sleep(200);
end;

{ TFMain }

procedure TFMain.netProcessMessage;
begin
  application.ProcessMessages;
end;

procedure TFMain.netReceiveString(aMsg: string; aSocket: TLSocket;
  aBinSize: integer; var aReadBin: boolean);
var
  s1,s2: string;
begin
  s1:=GetLineToStr(aMsg,1,'=');
  s2:=GetLineToStr(aMsg,2,'=');
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
      shutdown.execute;
    end;
  end;
end;

procedure TFMain.netStatus(aActive, aCrypt: boolean);
begin
  autoconnect.Enabled:=(not aActive) and (not con_wyjscie);
end;

procedure TFMain.wczytaj_default;
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

function TFMain.test(aHost: string): boolean;
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

procedure TFMain.SetVolume(aSetVol: integer);
var
  a: TAsyncProcess;
  b: integer;
  s: string;
begin
  if aSetVol<0 then s:='-' else s:='+';
  b:=abs(aSetVol);
  a:=TAsyncProcess.Create(self);
  try
    a.Executable:='amixer';
    a.Parameters.Add('sset');
    a.Parameters.Add('Master');
    a.Parameters.Add('playback');
    a.Parameters.Add(IntToStr(b)+'%'+s);
    a.Options:=[poWaitOnExit];
    a.ShowWindow:=swoHIDE;
    a.Execute;
  finally
    a.Terminate(0);
    a.Free;
  end;
end;

procedure TFMain.FormCreate(Sender: TObject);
begin
  //Application.ShowMainForm:=false;
  //WindowState:=wsMinimized;
  //hide;
  con_wyjscie:=false;
  wczytaj_default;
  autorun.Enabled:=true;
end;

procedure TFMain.FormDestroy(Sender: TObject);
begin
  con_wyjscie:=true;
  autoconnect.Enabled:=false;
  if net.Active then
  begin
    net.SendString('exit');
    net.Disconnect;
  end;
end;

procedure TFMain.netConnect(aSocket: TLSocket);
begin
  net.SendString('tryb=gpio');
end;

procedure TFMain.autorunTimer(Sender: TObject);
begin
  autorun.Enabled:=false;
  autoconnect.Enabled:=true;
end;

procedure TFMain.autoconnectTimer(Sender: TObject);
begin
  if not test(host) then exit;
  net.Host:=host;
  net.Port:=port;
  net.Connect;
end;

end.

