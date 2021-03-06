unit main;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, StdCtrls, Spin,
  Buttons, ExtCtrls, XMLPropStorage, Menus, ueled, uETilePanel, NetSocket,
  ExtMessage, ExtShutdown, lNet;

type

  { TgPioGui }

  TgPioGui = class(TForm)
    cConnecting: TuELED;
    cStan: TuELED;
    MenuItem4: TMenuItem;
    MenuItem5: TMenuItem;
    MenuItem6: TMenuItem;
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
    TrayIcon1: TTrayIcon;
    uETilePanel1: TuETilePanel;
    propstorage: TXMLPropStorage;
    procedure autoconnectTimer(Sender: TObject);
    procedure autorunTimer(Sender: TObject);
    procedure FormClose(Sender: TObject; var CloseAction: TCloseAction);
    procedure FormCreate(Sender: TObject);
    procedure MenuItem1Click(Sender: TObject);
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
    procedure init;
    function test(aHost: string): boolean;
    procedure wczytaj_default;
    procedure auto_hide_go(aMiliSeconds: integer = 5000);
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
  if cmenu then Height:=148 else Height:=116;
end;

function TgPioGui.test(aHost: string): boolean;
var
  s,s1,s2,s3,s4: string;
  i: integer;
begin
  s:=trim(aHost);
  (* je??li puste nie ma nic do roboty *)
  if s='' then
  begin
    result:=false;
    exit;
  end;
  (* sprawdzam czy w adresie wyst??puje litera *)
  for i:=1 to length(s) do if (not IsDigit(s[i])) and (s[i]<>'.') then
  begin
    result:=true;
    exit;
  end;
  (* sprawdzam czy adres IP4 jest wpisany prawid??owo *)
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
    if net.Active then net.Disconnect;
  end else begin
    CloseAction:=caNone;
    WindowState:=wsMinimized;
    hide;
  end;
end;

procedure TgPioGui.FormCreate(Sender: TObject);
begin
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
    cStan.Active:=s2='1';
    SetStatus(StrToInt(s2));
    if cmem=-1 then
    begin
      if s2='1' then cmem:=1 else cmem:=0;
    end else begin
      if cmem<>StrToInt(s2) then poprawka.Enabled:=true;
    end;
  end;
end;

procedure TgPioGui.netStatus(aActive, aCrypt: boolean);
begin
  autoconnect.Enabled:=(not aActive) and (not con_wyjscie);
  cConnecting.Active:=aActive;
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
  auto_hide_go;
end;

procedure TgPioGui.SpeedButton2Click(Sender: TObject);
begin
  cmem:=1;
  net.SendString('gpio=on');
  auto_hide_go;
end;

procedure TgPioGui.SpeedButton3Click(Sender: TObject);
var
  s1,s2: string;
begin
  s1:='/home/tao/.config/budgie-app-launcher/Locale State.wzor';
  s2:='/home/tao/.config/budgie-app-launcher/Locale State';
  CopyFile(s1,s2);
  auto_hide_go(500);
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

