unit main;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, process, Forms, Controls, Graphics, Dialogs, StdCtrls,
  Spin, Buttons, ExtCtrls, XMLPropStorage, Menus, AsyncProcess, ueled,
  uETilePanel, NetSocket, ExtMessage, ExtShutdown, lNet, Types;

type

  { TgPioGui }

  TgPioGui = class(TForm)
    AsyncProcess1: TAsyncProcess;
    MenuItem2: TMenuItem;
    shutdown: TExtShutdown;
    mess: TExtMessage;
    ImageList1: TImageList;
    net: TNetSocket;
    autoconnect: TTimer;
    autorun: TTimer;
    PopupMenu1: TPopupMenu;
    poprawka: TTimer;
    auto_hide: TTimer;
    autotimer: TTimer;
    TrayIcon1: TTrayIcon;
    propstorage: TXMLPropStorage;
    procedure autoconnectTimer(Sender: TObject);
    procedure autorunTimer(Sender: TObject);
    procedure autotimerTimer(Sender: TObject);
    procedure FormClose(Sender: TObject; var CloseAction: TCloseAction);
    procedure FormCreate(Sender: TObject);
    procedure MenuItem2Click(Sender: TObject);
    procedure netConnect(aSocket: TLSocket);
    procedure netProcessMessage;
    procedure netReceiveString(aMsg: string; aSocket: TLSocket;
      aBinSize: integer; var aReadBin: boolean);
    procedure netStatus(aActive, aCrypt: boolean);
    procedure poprawkaTimer(Sender: TObject);
    procedure auto_hideTimer(Sender: TObject);
    procedure _CHANGE(Sender: TObject);
  private
    host: string;
    port: word;
    con_wyjscie: boolean;
    AUTO_TIMER: boolean;
    EKRAN: string;
    vvAmixerIsExist: integer;
    function test(aHost: string): boolean;
    procedure wczytaj_default;
    procedure auto_hide_go(aMiliSeconds: integer = 5000);
    procedure SetVolume(aSetVol: integer);
  public
    procedure SetStatus(aValue: integer);
  end;

var
  gPioGui: TgPioGui;

implementation

uses
  ecode, fileutil;

{$R *.lfm}

{ TgPioGui }

procedure TgPioGui._CHANGE(Sender: TObject);
begin
  net.Disconnect;
end;

function TgPioGui.test(aHost: string): boolean;
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

procedure TgPioGui.wczytaj_default;
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
      if s1='TIMER' then AUTO_TIMER:=s2='1';
    end;
  finally
    c.Free;
  end;
end;

procedure TgPioGui.auto_hide_go(aMiliSeconds: integer);
begin
  auto_hide.Enabled:=false;
  auto_hide.Interval:=aMiliSeconds;
  auto_hide.Enabled:=true;
end;

procedure TgPioGui.SetVolume(aSetVol: integer);
var
  a: TAsyncProcess;
  b: integer;
  s: string;
  ss: TStringList;
begin
  if aSetVol<0 then s:='-' else s:='+';
  b:=abs(aSetVol);
  //writeln('vol = '+IntToStr(aSetVol),' b = '+IntToStr(b));
  //exit;
  a:=TAsyncProcess.Create(self);
  ss:=TStringList.Create;
  try
    if vvAmixerIsExist=-1 then
    begin
      //sprawdzam czy program do uruchomienia istnieje
      a.Executable:='which';
      a.Parameters.Add('amixer');
      a.Options:=[poWaitOnExit,poUsePipes];
      a.ShowWindow:=swoHIDE;
      a.Execute;
      if a.Output.NumBytesAvailable>0 then ss.LoadFromStream(a.Output);
      vvAmixerIsExist:=ss.Count;
    end;
    if vvAmixerIsExist>0 then
    begin
      //wykonuję program
      a.Parameters.Clear;
      a.Executable:='amixer';
      a.Parameters.Add('sset');
      a.Parameters.Add('Master');
      a.Parameters.Add('playback');
      a.Parameters.Add(IntToStr(b)+'%'+s);
      a.Options:=[poWaitOnExit];
      a.ShowWindow:=swoHIDE;
      a.Execute;
    end;
  finally
    a.Terminate(0);
    a.Free;
    ss.Free;
  end;
end;

procedure TgPioGui.SetStatus(aValue: integer);
begin
  case aValue of
    00: TrayIcon1.Icon.LoadFromResourceName(Hinstance,'LED_OFF');
    01: TrayIcon1.Icon.LoadFromResourceName(Hinstance,'LED_ON');
    10: TrayIcon1.Icon.LoadFromResourceName(Hinstance,'LED_OFF_LAP');
    11: TrayIcon1.Icon.LoadFromResourceName(Hinstance,'LED_ON_LAP');
  end;
end;

procedure TgPioGui.autorunTimer(Sender: TObject);
begin
  autorun.Enabled:=false;
  autoconnect.Enabled:=true;
end;

procedure TgPioGui.autotimerTimer(Sender: TObject);
begin
  net.SendString('gpio=refresh');
end;

procedure TgPioGui.autoconnectTimer(Sender: TObject);
begin
  if not test(host) then exit;
  net.Host:=host;
  net.Port:=port;
  net.Connect;
end;

procedure TgPioGui.FormClose(Sender: TObject; var CloseAction: TCloseAction);
begin
  if con_wyjscie then
  begin
    autoconnect.Enabled:=false;
    if net.Active then
    begin
      net.SendString('exit');
      net.Disconnect;
    end;
  end else begin
    CloseAction:=caNone;
    WindowState:=wsMinimized;
    hide;
  end;
end;

procedure TgPioGui.FormCreate(Sender: TObject);
begin
  vvAmixerIsExist:=-1;
  AUTO_TIMER:=false;
  Application.ShowMainForm:=false;
  WindowState:=wsMinimized;
  hide;
  con_wyjscie:=false;
  wczytaj_default;
  SetConfDir('PiStudio');
  propstorage.FileName:=MyConfDir('GPioGUI.xml');
  propstorage.Active:=true;
  autorun.Enabled:=true;
end;

procedure TgPioGui.MenuItem2Click(Sender: TObject);
begin
  con_wyjscie:=true;
  close;
end;

procedure TgPioGui.netConnect(aSocket: TLSocket);
begin
  net.SendString('tryb=gpio');
  sleep(500);
  net.SendString('gpio=status');
end;

procedure TgPioGui.netProcessMessage;
begin
  application.ProcessMessages;
end;

procedure TgPioGui.netReceiveString(aMsg: string; aSocket: TLSocket;
  aBinSize: integer; var aReadBin: boolean);
var
  s1,s2: string;
  a: integer;
begin
  s1:=GetLineToStr(aMsg,1,'=');
  s2:=GetLineToStr(aMsg,2,'=');
  //writeln('MSG: "',aMsg,'" s1="'+s1+'" s2="'+s2+'"');
  //exit;
  if s1='gpio' then
  begin
    a:=StrToInt(s2);
    SetStatus(a);
  end else
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
    if s2='key_power' then shutdown.execute;
  end;
end;

procedure TgPioGui.netStatus(aActive, aCrypt: boolean);
begin
  autoconnect.Enabled:=(not aActive) and (not con_wyjscie);
  if AUTO_TIMER then autotimer.Enabled:=aActive else autotimer.Enabled:=false;
  if not aActive then SetStatus(0);
end;

procedure TgPioGui.poprawkaTimer(Sender: TObject);
begin
  poprawka.Enabled:=false;
end;

procedure TgPioGui.auto_hideTimer(Sender: TObject);
begin
  auto_hide.Enabled:=false;
  if WindowState=wsNormal then
  begin
    WindowState:=wsMinimized;
    hide;
  end;
end;

end.

