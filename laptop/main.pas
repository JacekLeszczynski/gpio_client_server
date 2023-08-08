unit main;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, XMLPropStorage,
  ExtCtrls, Menus, NetSocket, ExtShutdown, lNet;

type

  { TForm1 }

  TForm1 = class(TForm)
    ExtShutdown1: TExtShutdown;
    MenuItem1: TMenuItem;
    net: TNetSocket;
    PopupMenu1: TPopupMenu;
    propstorage: TXMLPropStorage;
    autorun: TTimer;
    autoconnect: TTimer;
    cTest: TTimer;
    TrayIcon1: TTrayIcon;
    procedure autoconnectTimer(Sender: TObject);
    procedure autorunTimer(Sender: TObject);
    procedure cTestStartTimer(Sender: TObject);
    procedure cTestTimer(Sender: TObject);
    procedure FormClose(Sender: TObject; var CloseAction: TCloseAction);
    procedure FormCreate(Sender: TObject);
    procedure MenuItem1Click(Sender: TObject);
    procedure netConnect(aSocket: TLSocket);
    procedure netProcessMessage;
    procedure netReceiveString(aMsg: string; aSocket: TLSocket;
      aBinSize: integer; var aReadBin: boolean);
    procedure netStatus(aActive, aCrypt: boolean);
    procedure TrayIcon1Click(Sender: TObject);
  private
    AUTO_TIMER: boolean;
    con_wyjscie: boolean;
    function test(aHost: string): boolean;
  public
    procedure SetStatus(aValue: integer);
  end;

var
  Form1: TForm1;

implementation

uses
  ecode, fileutil;

{$R *.lfm}

{ TForm1 }

procedure TForm1.FormCreate(Sender: TObject);
begin
  AUTO_TIMER:=false;
  Application.ShowMainForm:=false;
  WindowState:=wsMinimized;
  hide;
  con_wyjscie:=false;
  //wczytaj_default;
  //init;
  SetConfDir('PiStudio');
  propstorage.FileName:=MyConfDir('GPioClient.xml');
  propstorage.Active:=true;
  autorun.Enabled:=true;
end;

procedure TForm1.MenuItem1Click(Sender: TObject);
begin
  con_wyjscie:=true;
  close;
end;

procedure TForm1.netConnect(aSocket: TLSocket);
begin
  net.SendString('tryb=laptop');
  //sleep(500);
  //net.SendString('gpio=status');
end;

procedure TForm1.netProcessMessage;
begin
  application.ProcessMessages;
end;

procedure TForm1.netReceiveString(aMsg: string; aSocket: TLSocket;
  aBinSize: integer; var aReadBin: boolean);
var
  s,s1,s2,s3: string;
begin
  s1:=GetLineToStr(aMsg,1,'=');
  s2:=GetLineToStr(aMsg,2,'=');
  s3:=GetLineToStr(aMsg,3,'=');
  if s1='laptop' then
  begin
    if s2='active' then SetStatus(1) else
    if s2='shutdown' then
    begin
      cTest.Tag:=StrToInt(s3);
      s:='laptop=shutdowning='+s3;
      net.SendString(s);
      cTest.Enabled:=true;
    end;
  end;
end;

procedure TForm1.netStatus(aActive, aCrypt: boolean);
begin
  autoconnect.Enabled:=(not aActive) and (not con_wyjscie);
  if not aActive then SetStatus(0);
end;

procedure TForm1.TrayIcon1Click(Sender: TObject);
begin
  PopupMenu1.PopUp;
end;

function TForm1.test(aHost: string): boolean;
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

procedure TForm1.SetStatus(aValue: integer);
begin
  case aValue of
    1: TrayIcon1.Icon.LoadFromResourceName(Hinstance,'LED_ON');
    else TrayIcon1.Icon.LoadFromResourceName(Hinstance,'LED_OFF');
  end;
end;

procedure TForm1.autorunTimer(Sender: TObject);
begin
  autorun.Enabled:=false;
  autoconnect.Enabled:=true;
end;

procedure TForm1.cTestStartTimer(Sender: TObject);
begin
  ExtShutdown1.execute;
end;

procedure TForm1.cTestTimer(Sender: TObject);
var
  s: string;
begin
  cTest.Enabled:=false;
  s:='laptop=not_shutdown='+IntToStr(cTest.Tag);
  net.SendString(s);
end;

procedure TForm1.FormClose(Sender: TObject; var CloseAction: TCloseAction);
begin
  autoconnect.Enabled:=false;
  if net.Active then
  begin
    net.SendString('exit');
    net.Disconnect;
  end;
end;

procedure TForm1.autoconnectTimer(Sender: TObject);
begin
  if not test(net.Host) then exit;
  net.Connect;
end;

end.

