unit main;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, StdCtrls, Spin,
  Buttons, ExtCtrls, XMLPropStorage, Menus, ueled, uETilePanel, NetSocket,
  ExtMessage, lNet;

type

  { TgPioGui }

  TgPioGui = class(TForm)
    cConnecting: TuELED;
    cStan: TuELED;
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
    TrayIcon1: TTrayIcon;
    uETilePanel1: TuETilePanel;
    propstorage: TXMLPropStorage;
    procedure autoconnectTimer(Sender: TObject);
    procedure autorunTimer(Sender: TObject);
    procedure FormClose(Sender: TObject; var CloseAction: TCloseAction);
    procedure FormCreate(Sender: TObject);
    procedure MenuItem1Click(Sender: TObject);
    procedure netConnect(aSocket: TLSocket);
    procedure netProcessMessage;
    procedure netReceiveString(aMsg: string; aSocket: TLSocket;
      aBinSize: integer; var aReadBin: boolean);
    procedure netStatus(aActive, aCrypt: boolean);
    procedure SpeedButton1Click(Sender: TObject);
    procedure SpeedButton2Click(Sender: TObject);
    procedure SpeedButton3Click(Sender: TObject);
    procedure TrayIcon1Click(Sender: TObject);
    procedure _CHANGE(Sender: TObject);
  private
    host: string;
    port: word;
    con_wyjscie,cmenu: boolean;
    procedure init;
    function test(aHost: string): boolean;
    procedure wczytaj_default;
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
  if cmenu then Height:=154 else Height:=116;
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
    end;
  finally
    c.Free;
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

procedure TgPioGui.netConnect(aSocket: TLSocket);
begin
  net.SendStringEx('STATUS'+#0);
end;

procedure TgPioGui.netProcessMessage;
begin
  application.ProcessMessages;
end;

procedure TgPioGui.netReceiveString(aMsg: string; aSocket: TLSocket;
  aBinSize: integer; var aReadBin: boolean);
begin
  cStan.Active:=aMsg='1';
  SetStatus(StrToInt(aMsg));
end;

procedure TgPioGui.netStatus(aActive, aCrypt: boolean);
begin
  autoconnect.Enabled:=(not aActive) and (not con_wyjscie);
  cConnecting.Active:=aActive;
end;

procedure TgPioGui.SpeedButton1Click(Sender: TObject);
begin
  net.SendStringEx('ON'+#0);
end;

procedure TgPioGui.SpeedButton2Click(Sender: TObject);
begin
  net.SendStringEx('OFF'+#0);
end;

procedure TgPioGui.SpeedButton3Click(Sender: TObject);
var
  s1,s2: string;
begin
  s1:='/home/tao/.config/budgie-app-launcher/Locale State.wzor';
  s2:='/home/tao/.config/budgie-app-launcher/Locale State';
  CopyFile(s1,s2);
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

