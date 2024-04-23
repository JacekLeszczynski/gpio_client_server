unit main;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, StdCtrls,
  XMLPropStorage, ExtCtrls, Buttons, NetSocket, uETilePanel, ueled, switches,
  lNet;

type

  { TcMain }

  TcMain = class(TForm)
    Bevel1: TBevel;
    Bevel2: TBevel;
    cSrcName1: TEdit;
    cSrcName2: TEdit;
    cSrcName3: TEdit;
    cSrcName4: TEdit;
    cSrcName5: TEdit;
    cSrcName6: TEdit;
    cSrcName7: TEdit;
    cSrcName8: TEdit;
    Label1: TLabel;
    mon: TNetSocket;
    OnOffSwitch1: TOnOffSwitch;
    OnOffSwitch2: TOnOffSwitch;
    OnOffSwitch3: TOnOffSwitch;
    OnOffSwitch4: TOnOffSwitch;
    OnOffSwitch5: TOnOffSwitch;
    OnOffSwitch6: TOnOffSwitch;
    OnOffSwitch7: TOnOffSwitch;
    OnOffSwitch8: TOnOffSwitch;
    OnOffSwitch9: TOnOffSwitch;
    uETilePanel1: TuETilePanel;
    propstorage: TXMLPropStorage;
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure monConnect(aSocket: TLSocket);
    procedure monProcessMessage;
    procedure monReceiveString(aMsg: string; aSocket: TLSocket;
      aBinSize: integer; var aReadBin: boolean);
    procedure _ZMIANA_PINU(Sender: TObject);
  private
    vBlokowanie: boolean;
    vHost: string;
    vPort: word;
    procedure wczytaj_default;
    procedure SetConfig(aStr: string);
  public
  end;

var
  cMain: TcMain;

implementation

uses
  ecode;

{$R *.lfm}

{ TcMain }

procedure TcMain.FormCreate(Sender: TObject);
begin
  wczytaj_default;
  SetConfDir('PiStudio');
  propstorage.FileName:=MyConfDir('GpioMonitor.xml');
  propstorage.Active:=true;
  mon.Host:=vHost;
  mon.Port:=vPort;
  mon.Connect;
end;

procedure TcMain.FormDestroy(Sender: TObject);
begin
  if mon.Active then mon.Disconnect;
end;

procedure TcMain.monConnect(aSocket: TLSocket);
begin
  mon.SendString('tryb=gpio_monitor');
end;

procedure TcMain.monProcessMessage;
begin
  application.ProcessMessages;
end;

procedure TcMain.monReceiveString(aMsg: string; aSocket: TLSocket;
  aBinSize: integer; var aReadBin: boolean);
var
  s,s1,s2,s3: string;
begin
  s:=aMsg;
  if s='' then exit;
  s1:=GetLineToStr(s,1,'=');
  s2:=GetLineToStr(s,2,'=');
  s3:=GetLineToStr(s,3,'=');
  if s='monitor_gpio=active' then mon.SendString('status') else
  if s1='gpio_status' then SetConfig(s2) else
  if s1='gpio' then
  begin
    vBlokowanie:=true;
    OnOffSwitch1.Checked:=s2='1';
    vBlokowanie:=false;
  end else
  if s1='gpio2' then
  begin
    vBlokowanie:=true;
    OnOffSwitch2.Checked:=s2='1';
    vBlokowanie:=false;
  end else
  if s1='gpio3' then
  begin
    vBlokowanie:=true;
    OnOffSwitch3.Checked:=s2='1';
    vBlokowanie:=false;
  end else
  if s1='gpio4' then
  begin
    vBlokowanie:=true;
    OnOffSwitch4.Checked:=s2='1';
    vBlokowanie:=false;
  end else
  if s1='gpio5' then
  begin
    vBlokowanie:=true;
    OnOffSwitch5.Checked:=s2='1';
    vBlokowanie:=false;
  end else
  if s1='gpio6' then
  begin
    vBlokowanie:=true;
    OnOffSwitch6.Checked:=s2='1';
    vBlokowanie:=false;
  end else
  if s1='gpio7' then
  begin
    vBlokowanie:=true;
    OnOffSwitch7.Checked:=s2='1';
    vBlokowanie:=false;
  end else
  if s1='gpio8' then
  begin
    vBlokowanie:=true;
    OnOffSwitch8.Checked:=s2='1';
    vBlokowanie:=false;
  end else
  if s1='laptop' then
  begin
    if s2='login' then
    begin
      vBlokowanie:=true;
      OnOffSwitch9.Checked:=true;
      OnOffSwitch9.ColorON:=clYellow;
      OnOffSwitch9.ColorOFF:=clYellow;
      vBlokowanie:=false;
    end else
    if s2='logout' then
    begin
      vBlokowanie:=true;
      OnOffSwitch9.Checked:=false;
      OnOffSwitch9.ColorON:=clMaroon;
      OnOffSwitch9.ColorOFF:=clMaroon;
      vBlokowanie:=false;
    end else
    if s2='starting' then
    begin
      OnOffSwitch9.ColorON:=clRed;
      OnOffSwitch9.ColorOFF:=clRed;
    end;
  end;
end;

procedure TcMain._ZMIANA_PINU(Sender: TObject);
var
  a: TOnOffSwitch;
  s: string;
begin
  if vBlokowanie then exit;
  a:=TOnOffSwitch(Sender);
  if a.Tag<9 then
  begin
    if a.Checked then s:='1' else s:='0';
    mon.SendString('set='+IntToStr(a.Tag)+'='+s);
  end else
  if a.Tag=9 then
  begin
    if a.Checked then mon.SendString('laptop=start') else mon.SendString('laptop=shutdown');
  end;
end;

procedure TcMain.wczytaj_default;
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
      if s1='HOST' then vHost:=s2 else
      if s1='PORT' then vPort:=StrToInt(s2);
    end;
  finally
    c.Free;
  end;
end;

procedure TcMain.SetConfig(aStr: string);
var
  l: integer;
begin
  l:=length(aStr);
  if l<9 then exit;
  vBlokowanie:=true;
  OnOffSwitch1.Checked:=aStr[1]='1';
  OnOffSwitch2.Checked:=aStr[2]='1';
  OnOffSwitch3.Checked:=aStr[3]='1';
  OnOffSwitch4.Checked:=aStr[4]='1';
  OnOffSwitch5.Checked:=aStr[5]='1';
  OnOffSwitch6.Checked:=aStr[6]='1';
  OnOffSwitch7.Checked:=aStr[7]='1';
  OnOffSwitch8.Checked:=aStr[8]='1';
  OnOffSwitch9.Checked:=aStr[9]='1';
  if OnOffSwitch9.Checked then
  begin
    OnOffSwitch9.ColorON:=clYellow;
    OnOffSwitch9.ColorOFF:=clYellow;
  end else begin
    OnOffSwitch9.ColorON:=clMaroon;
    OnOffSwitch9.ColorOFF:=clMaroon;
  end;
  vBlokowanie:=false;
end;

end.

