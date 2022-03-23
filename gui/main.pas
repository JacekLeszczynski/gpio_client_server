unit main;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, StdCtrls, Spin,
  Buttons, ExtCtrls, XMLPropStorage, ueled, uETilePanel, NetSocket, ExtMessage;

type

  { TForm1 }

  TForm1 = class(TForm)
    BitBtn1: TBitBtn;
    BitBtn2: TBitBtn;
    cConnecting: TuELED;
    cHost: TEdit;
    cPort: TSpinEdit;
    mess: TExtMessage;
    ImageList1: TImageList;
    Label1: TLabel;
    Label2: TLabel;
    Label3: TLabel;
    net: TNetSocket;
    SpeedButton1: TSpeedButton;
    autoconnect: TTimer;
    autorun: TTimer;
    uETilePanel1: TuETilePanel;
    propstorage: TXMLPropStorage;
    procedure autoconnectTimer(Sender: TObject);
    procedure autorunTimer(Sender: TObject);
    procedure BitBtn1Click(Sender: TObject);
    procedure BitBtn2Click(Sender: TObject);
    procedure FormClose(Sender: TObject; var CloseAction: TCloseAction);
    procedure FormCreate(Sender: TObject);
    procedure netProcessMessage;
    procedure netStatus(aActive, aCrypt: boolean);
    procedure SpeedButton1Click(Sender: TObject);
    procedure _CHANGE(Sender: TObject);
  private
    con_wyjscie: boolean;
    function test(aHost: string): boolean;
  public

  end;

var
  Form1: TForm1;

implementation

uses
  ecode;

{$R *.lfm}

{ TForm1 }

procedure TForm1.SpeedButton1Click(Sender: TObject);
begin
  cPort.Value:=2122;
end;

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

procedure TForm1.autorunTimer(Sender: TObject);
begin
  autorun.Enabled:=false;
  autoconnect.Enabled:=true;
end;

procedure TForm1.BitBtn1Click(Sender: TObject);
begin
  writeln(net.SendStringEx('OFF'+#0)); (* reverse *)
end;

procedure TForm1.BitBtn2Click(Sender: TObject);
begin
  writeln(net.SendStringEx('ON'+#0)); (* reverse *)
end;

procedure TForm1.autoconnectTimer(Sender: TObject);
begin
  if not test(cHost.Text) then exit;
  net.Host:=cHost.Text;
  net.Port:=cPort.Value;
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
  SetConfDir('PiStudio');
  propstorage.FileName:=MyConfDir('GPioGUI.xml');
  propstorage.Active:=true;
  autorun.Enabled:=true;
end;

procedure TForm1.netProcessMessage;
begin
  application.ProcessMessages;
end;

procedure TForm1.netStatus(aActive, aCrypt: boolean);
begin
  autoconnect.Enabled:=(not aActive) and (not con_wyjscie);
  cConnecting.Active:=aActive;
end;

end.

