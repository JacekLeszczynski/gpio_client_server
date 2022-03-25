unit main;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, StdCtrls, Spin,
  Buttons, ExtCtrls, XMLPropStorage, ueled, uETilePanel, NetSocket, ExtMessage, lNet;

type

  { TForm1 }

  TForm1 = class(TForm)
    cConnecting: TuELED;
    cStan: TuELED;
    Label1: TLabel;
    Label2: TLabel;
    mess: TExtMessage;
    ImageList1: TImageList;
    net: TNetSocket;
    autoconnect: TTimer;
    autorun: TTimer;
    SpeedButton1: TSpeedButton;
    SpeedButton2: TSpeedButton;
    uETilePanel1: TuETilePanel;
    propstorage: TXMLPropStorage;
    procedure autoconnectTimer(Sender: TObject);
    procedure autorunTimer(Sender: TObject);
    procedure FormClose(Sender: TObject; var CloseAction: TCloseAction);
    procedure FormCreate(Sender: TObject);
    procedure netConnect(aSocket: TLSocket);
    procedure netProcessMessage;
    procedure netReceiveString(aMsg: string; aSocket: TLSocket;
      aBinSize: integer; var aReadBin: boolean);
    procedure netStatus(aActive, aCrypt: boolean);
    procedure SpeedButton1Click(Sender: TObject);
    procedure SpeedButton2Click(Sender: TObject);
    procedure _CHANGE(Sender: TObject);
  private
    host: string;
    port: word;
    con_wyjscie: boolean;
    function test(aHost: string): boolean;
    procedure wczytaj_default;
  public

  end;

var
  Form1: TForm1;

implementation

uses
  ecode;

{$R *.lfm}

{ TForm1 }

procedure TForm1._CHANGE(Sender: TObject);
begin
  net.Disconnect;
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

procedure TForm1.wczytaj_default;
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

procedure TForm1.autorunTimer(Sender: TObject);
begin
  autorun.Enabled:=false;
  autoconnect.Enabled:=true;
end;

procedure TForm1.autoconnectTimer(Sender: TObject);
begin
  if not test(host) then exit;
  net.Host:=host;
  net.Port:=port;
  net.Connect;
end;

procedure TForm1.FormClose(Sender: TObject; var CloseAction: TCloseAction);
begin
  con_wyjscie:=true;
  autoconnect.Enabled:=false;
  if net.Active then net.Disconnect;
end;

procedure TForm1.FormCreate(Sender: TObject);
begin
  con_wyjscie:=false;
  wczytaj_default;
  SetConfDir('PiStudio');
  //propstorage.FileName:=MyConfDir('GPioGUI.xml');
  //propstorage.Active:=true;
  autorun.Enabled:=true;
end;

procedure TForm1.netConnect(aSocket: TLSocket);
begin
  net.SendStringEx('STATUS'+#0);
end;

procedure TForm1.netProcessMessage;
begin
  application.ProcessMessages;
end;

procedure TForm1.netReceiveString(aMsg: string; aSocket: TLSocket;
  aBinSize: integer; var aReadBin: boolean);
begin
  cStan.Active:=aMsg='1';
end;

procedure TForm1.netStatus(aActive, aCrypt: boolean);
begin
  autoconnect.Enabled:=(not aActive) and (not con_wyjscie);
  cConnecting.Active:=aActive;
end;

procedure TForm1.SpeedButton1Click(Sender: TObject);
begin
  net.SendStringEx('ON'+#0);
end;

procedure TForm1.SpeedButton2Click(Sender: TObject);
begin
  net.SendStringEx('OFF'+#0);
end;

end.

