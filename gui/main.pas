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
    cConnecting: TuELED;
    cStan: TuELED;
    MenuItem2: TMenuItem;
    MenuItem3: TMenuItem;
    MenuItem4: TMenuItem;
    MenuItem5: TMenuItem;
    MenuItem6: TMenuItem;
    MenuItem7: TMenuItem;
    shutdown: TExtShutdown;
    Label1: TLabel;
    Label2: TLabel;
    MenuItem1: TMenuItem;
    mess: TExtMessage;
    ImageList1: TImageList;
    net: TNetSocket;
    autoconnect: TTimer;
    autorun: TTimer;
    PopupMenu1: TPopupMenu;
    SpeedButton1: TSpeedButton;
    SpeedButton2: TSpeedButton;
    SpeedButton3: TSpeedButton;
    poprawka: TTimer;
    auto_hide: TTimer;
    autotimer: TTimer;
    TrayIcon1: TTrayIcon;
    uETilePanel1: TuETilePanel;
    propstorage: TXMLPropStorage;
    procedure autoconnectTimer(Sender: TObject);
    procedure autorunTimer(Sender: TObject);
    procedure autotimerTimer(Sender: TObject);
    procedure FormClose(Sender: TObject; var CloseAction: TCloseAction);
    procedure FormCreate(Sender: TObject);
    procedure MenuItem1Click(Sender: TObject);
    procedure MenuItem2Click(Sender: TObject);
    procedure MenuItem3Click(Sender: TObject);
    procedure MenuItem4Click(Sender: TObject);
    procedure MenuItem6Click(Sender: TObject);
    procedure netConnect(aSocket: TLSocket);
    procedure netProcessMessage;
    procedure netReceiveString(aMsg: string; aSocket: TLSocket;
      aBinSize: integer; var aReadBin: boolean);
    procedure netStatus(aActive, aCrypt: boolean);
    procedure poprawkaTimer(Sender: TObject);
    procedure SpeedButton1Click(Sender: TObject);
    procedure SpeedButton2Click(Sender: TObject);
    procedure SpeedButton3Click(Sender: TObject);
    procedure auto_hideTimer(Sender: TObject);
    procedure TrayIcon1Click(Sender: TObject);
    procedure _CHANGE(Sender: TObject);
  private
    host: string;
    port: word;
    con_wyjscie,cmenu: boolean;
    cmem: integer;
    AUTO_TIMER,SERWIS_TV,LAPTOP: boolean;
    procedure init;
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

procedure TgPioGui.init;
begin
  SpeedButton3.Visible:=cmenu;
  if cmenu then Height:=148 else Height:=100;
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
  cmenu:=false;
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
      if s1='CUSTOM_MENU' then cmenu:=true;
      if s1='TIMER' then AUTO_TIMER:=s2='1';
      if s1='SERWIS_TV' then SERWIS_TV:=s2='1';
      if s1='LAPTOP' then LAPTOP:=s2='1';
    end;
  finally
    c.Free;
  end;
  MenuItem4.Enabled:=SERWIS_TV;
  MenuItem6.Enabled:=SERWIS_TV;
  //MenuItem2.Enabled:=LAPTOP;
  //MenuItem3.Enabled:=LAPTOP;
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

procedure TgPioGui.SetStatus(aValue: integer);
begin
  case aValue of
    1: TrayIcon1.Icon.LoadFromResourceName(Hinstance,'LED_ON');
    else TrayIcon1.Icon.LoadFromResourceName(Hinstance,'LED_OFF');
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
  AUTO_TIMER:=false;
  Application.ShowMainForm:=false;
  WindowState:=wsMinimized;
  hide;
  cmem:=-1;
  con_wyjscie:=false;
  wczytaj_default;
  init;
  SetConfDir('PiStudio');
  propstorage.FileName:=MyConfDir('GPioGUI.xml');
  propstorage.Active:=true;
  autorun.Enabled:=true;
end;

procedure TgPioGui.MenuItem1Click(Sender: TObject);
begin
  con_wyjscie:=true;
  close;
end;

procedure TgPioGui.MenuItem2Click(Sender: TObject);
begin
  net.SendString('laptop=start');
end;

procedure TgPioGui.MenuItem3Click(Sender: TObject);
begin
  net.SendString('laptop=shutdown');
end;

procedure TgPioGui.MenuItem4Click(Sender: TObject);
begin
  net.SendString('tv=on');
end;

procedure TgPioGui.MenuItem6Click(Sender: TObject);
begin
  net.SendString('tv=off');
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
begin
  //writeln('MSG: ',aMsg);
  s1:=GetLineToStr(aMsg,1,'=');
  s2:=GetLineToStr(aMsg,2,'=');
  if s1='gpio' then
  begin
    //writeln('s1=',s1,' s2=',s2);
    cStan.Active:=s2='1';
    SetStatus(StrToInt(s2));
    if cmem=-1 then
    begin
      if s2='1' then cmem:=1 else cmem:=0;
    end else begin
      if cmem<>StrToInt(s2) then poprawka.Enabled:=true;
    end;
    net.SendString('laptop=status');
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
    if s2='key_power' then
    begin
      shutdown.execute;
    end;
  end else
  if s1='laptop' then
  begin
    if s2='login' then
    begin
      MenuItem2.Enabled:=false;
      MenuItem3.Enabled:=LAPTOP and true;
    end else
    if s2='logout' then
    begin
      MenuItem2.Enabled:=LAPTOP and true;
      MenuItem3.Enabled:=false;
    end else
    //if s2='starting' then mess.ShowInformation('Zdalny host otrzymał polecenie startu i procedura uruchomienia jest w trakcie.') else
    //if s2='shutdowning' then mess.ShowInformation('Zdalny host potwierdził rozpoczęcie procedury zamykania.') else
    //if s2='shutdown_ok' then mess.ShowInformation('Zdalny host zamyka się poprawnie.') else
    if s2='not_shutdown' then mess.ShowWarning('Zdalny host nie może się zamknąć!^Dalej działa!');
  end else
  if s1='echo' then
  begin
    mess.ShowInformation('Odebrano ramkę s2 = "'+s2+'"');
  end;
end;

procedure TgPioGui.netStatus(aActive, aCrypt: boolean);
begin
  autoconnect.Enabled:=(not aActive) and (not con_wyjscie);
  cConnecting.Active:=aActive;
  if AUTO_TIMER then autotimer.Enabled:=aActive else autotimer.Enabled:=false;
  if not aActive then
  begin
    cStan.Active:=(not aActive);
    SetStatus(0);
  end;
  SpeedButton1.Enabled:=aActive;
  SpeedButton2.Enabled:=aActive;
end;

procedure TgPioGui.poprawkaTimer(Sender: TObject);
begin
  poprawka.Enabled:=false;
  if cmem=1 then net.SendString('gpio=on') else net.SendString('gpio=off');
end;

procedure TgPioGui.SpeedButton1Click(Sender: TObject);
begin
  cmem:=0;
  net.SendString('gpio=off');
  auto_hide_go(200);
end;

procedure TgPioGui.SpeedButton2Click(Sender: TObject);
begin
  cmem:=1;
  net.SendString('gpio=on');
  auto_hide_go(200);
end;

procedure TgPioGui.SpeedButton3Click(Sender: TObject);
var
  s1,s2: string;
begin
  s1:='/home/tao/.config/budgie-app-launcher/Locale State.wzor';
  s2:='/home/tao/.config/budgie-app-launcher/Locale State';
  CopyFile(s1,s2);
  auto_hide_go(200);
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

procedure TgPioGui.TrayIcon1Click(Sender: TObject);
begin
  if WindowState=wsNormal then
  begin
    WindowState:=wsMinimized;
    hide;
  end else begin
    WindowState:=wsNormal;
    show;
  end;
end;

end.

