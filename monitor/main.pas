unit main;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, StdCtrls,
  XMLPropStorage, ExtCtrls, Buttons, Process, AsyncProcess, NetSocket, uETilePanel,
  switches, lNet;

type

  { TcMain }

  TcMain = class(TForm)
    AsyncProcess1: TAsyncProcess;
    Bevel1: TBevel;
    Bevel2: TBevel;
    BitBtn1: TBitBtn;
    cSrcName1: TEdit;
    cSrcName2: TEdit;
    cSrcName3: TEdit;
    cSrcName4: TEdit;
    cSrcName5: TEdit;
    cSrcName6: TEdit;
    cSrcName7: TEdit;
    cSrcName8: TEdit;
    Image1: TImage;
    Label1: TLabel;
    Label10: TLabel;
    Label11: TLabel;
    Label12: TLabel;
    Label13: TLabel;
    Label14: TLabel;
    Label15: TLabel;
    Label16: TLabel;
    Label17: TLabel;
    Label2: TLabel;
    Label3: TLabel;
    Label4: TLabel;
    Label5: TLabel;
    Label6: TLabel;
    Label7: TLabel;
    Label8: TLabel;
    Label9: TLabel;
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
    Panel1: TPanel;
    SpeedButton1: TSpeedButton;
    uETilePanel1: TuETilePanel;
    propstorage: TXMLPropStorage;
    procedure BitBtn1Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure monConnect(aSocket: TLSocket);
    procedure monProcessMessage;
    procedure monReceiveString(aMsg: string; aSocket: TLSocket;
      aBinSize: integer; var aReadBin: boolean);
    procedure SpeedButton1Click(Sender: TObject);
    procedure _ZMIANA_PINU(Sender: TObject);
  private
    FDragging: Boolean;
    FMouseStartPoint: TPoint;
    vData: TDate;
    vBlokowanie: boolean;
    vHost: string;
    vPort: word;
    procedure wczytaj_default;
    procedure SetConfig(aStr: string);
    procedure SetDzien(aCode: integer);
    procedure SetDzien(aCode: string);
    procedure WykonajSkrypt(aScript: string; aParametry: string = ''; aResult: TStrings = nil);
  public
    procedure MemMessage(Sender: TObject; AMessage: string);
  end;

var
  cMain: TcMain;

implementation

uses
  ecode, LazUTF8, Dateutils;

{$R *.lfm}

function GetMiesiac(aMiesiac: integer): string;
begin
  case aMiesiac of
    1: result:='Styczeń';
    2: result:='Luty';
    3: result:='Marzec';
    4: result:='Kwiecień';
    5: result:='Maj';
    6: result:='Czerwiec';
    7: result:='Lipiec';
    8: result:='Sierpień';
    9: result:='Wrzesień';
    10: result:='Październik';
    11: result:='Listopad';
    12: result:='Grudzień';
  end;
end;

function DzienTygodnia(aCode: integer): string;
begin
  case aCode of
    0: result:='Sobota';
    1: result:='Niedziela';
    2: result:='Poniedziałek';
    3: result:='Wtorek';
    4: result:='Środa';
    5: result:='Czwartek';
    6: result:='Piątek';
    7: result:='Sobota';
  end;
end;

function DataToZnakZodiaku(aData: TDate): string;
var
  i: integer;
  r,m,d: word;
  z: array [0..12] of record
    d1,d2: TDate;
    s: string;
  end;
begin
  DecodeDate(aData,r,m,d);
  (* init *)
  z[0].d1:=EncodeDate(r,1,1);
  z[0].d2:=EncodeDate(r,1,19);
  z[0].s:='Koziorożec';
  z[1].d1:=EncodeDate(r,1,20);
  z[1].d2:=EncodeDate(r,2,18);
  z[1].s:='Wodnik';
  z[2].d1:=EncodeDate(r,2,19);
  z[2].d2:=EncodeDate(r,3,20);
  z[2].s:='Ryby';
  z[3].d1:=EncodeDate(r,3,21);
  z[3].d2:=EncodeDate(r,4,19);
  z[3].s:='Baran';
  z[4].d1:=EncodeDate(r,4,20);
  z[4].d2:=EncodeDate(r,5,20);
  z[4].s:='Byk';
  z[5].d1:=EncodeDate(r,5,21);
  z[5].d2:=EncodeDate(r,6,20);
  z[5].s:='Bliźnięta';
  z[6].d1:=EncodeDate(r,6,21);
  z[6].d2:=EncodeDate(r,7,22);
  z[6].s:='Rak';
  z[7].d1:=EncodeDate(r,7,23);
  z[7].d2:=EncodeDate(r,8,22);
  z[7].s:='Lew';
  z[8].d1:=EncodeDate(r,8,23);
  z[8].d2:=EncodeDate(r,9,22);
  z[8].s:='Panna';
  z[9].d1:=EncodeDate(r,9,23);
  z[9].d2:=EncodeDate(r,10,22);
  z[9].s:='Waga';
  z[10].d1:=EncodeDate(r,10,23);
  z[10].d2:=EncodeDate(r,11,21);
  z[10].s:='Skorpion';
  z[11].d1:=EncodeDate(r,11,22);
  z[11].d2:=EncodeDate(r,12,21);
  z[11].s:='Strzelec';
  z[12].d1:=EncodeDate(r,12,22);
  z[12].d2:=EncodeDate(r,12,31);
  z[12].s:='Koziorożec';
  (* reszta *)
  result:='?';
  for i:=0 to 12 do
  begin
    if (aData>=z[i].d1) and (aData<=z[i].d2) then
    begin
      result:=z[i].s;
      break;
    end;
  end;
end;

procedure GetDayAndWeekOfYear(const ADate: TDateTime; out DayOfYear, WeekOfYear: Integer);
begin
  // Oblicz dzień w roku
  DayOfYear := DayOfTheYear(ADate);
  // Oblicz tydzień w roku
  WeekOfYear := WeekOfTheYear(ADate);
end;

{ TcMain }

procedure TcMain.FormCreate(Sender: TObject);
begin
  wczytaj_default;
  SetConfDir('PiStudio');
  propstorage.FileName:=MyConfDir('GpioMonitor.xml');
  propstorage.Active:=true;
  vData:=0;
  mon.Host:=vHost;
  mon.Port:=vPort;
  mon.Connect;
end;

procedure TcMain.BitBtn1Click(Sender: TObject);
begin
  close;
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
  if s1='gpio_status' then
  begin
    SetConfig(s2);
    if vData=0 then
    begin
      vData:=date;
      mon.SendString('getcalendar');
    end;
  end else
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
  end else
  if s1='workday' then SetDzien(StrToInt(s2)) else
  if s1='calendar' then SetDzien(s2);
end;

procedure TcMain.SpeedButton1Click(Sender: TObject);
var
  ss: TStrings;
  b: boolean;
begin
  (* sprawdzam stan *)
  ss:=TStringList.Create;
  try
    WykonajSkrypt('ddcutil','getvcp D6',ss);
    b:=pos('DPM: On',ss.Text)>0;
  finally
    ss.Free;
  end;
  (* zmieniam stan na przeciwny *)
  if b then WykonajSkrypt('ddcutil','setvcp D6 04') else WykonajSkrypt('ddcutil','setvcp D6 01');
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

procedure TcMain.SetDzien(aCode: integer);
begin
  case aCode of
    0: Caption:='Monitor GPio (data: '+FormatDateTime('yyyy-mm-dd',date)+' - dzień wolny)';
    1: Caption:='Monitor GPio (data: '+FormatDateTime('yyyy-mm-dd',date)+' - dzień roboczy)';
    else Caption:='Monitor GPio (data: '+FormatDateTime('yyyy-mm-dd',date)+')';
  end;
end;

procedure TcMain.SetDzien(aCode: string);
var
  data: TDate;
  data_dzien,data_miesiac,data_rok: word;
  dr: string[1];
  kolor: TColor;
  fs: TFormatSettings;
  st1,st2,znak: string;
  res: TResourceStream;
  i1,i2: integer;
begin
  fs.ShortDateFormat:='y/m/d';
  fs.DateSeparator:='-';
  data:=StrToDate(GetLineToStr(aCode,1,';'),fs);
  DecodeDate(data,data_rok,data_miesiac,data_dzien);
  Label4.Caption:=Utf8UpperCase(GetMiesiac(data_miesiac));
  Label6.Caption:=IntToStr(data_rok);
  Label15.Caption:=IntToStr(data_dzien);
  Label16.Caption:=Utf8UpperCase(DzienTygodnia(DayOfWeek(data)));
  dr:=GetLineToStr(aCode,5,';');
  if dr='1' then kolor:=clBlack else kolor:=clRed;
  Label15.Font.Color:=kolor;
  Label16.Font.Color:=kolor;
  if dr='0' then Caption:='Monitor GPio (data: '+FormatDateTime('yyyy-mm-dd',date)+' - dzień wolny)' else
  if dr='1' then Caption:='Monitor GPio (data: '+FormatDateTime('yyyy-mm-dd',date)+' - dzień roboczy)' else
  Caption:='Monitor GPio (data: '+FormatDateTime('yyyy-mm-dd',date)+')';
  st1:=GetLineToStr(aCode,3,';');
  st2:=GetLineToStr(aCode,4,';');
  Label9.Caption:=st1;
  Label10.Caption:=st2;
  Label14.Caption:=DataToZnakZodiaku(data);
  znak:=Utf8UpperCase(Label14.Caption);
  try
    res:=TResourceStream.Create(hInstance,znak,RT_RCDATA);
    Image1.Picture.LoadFromStream(res);
  finally
    res.Free;
  end;
  Label17.Caption:=trim(GetLineToStr(aCode,7,';'));
  GetDayAndWeekOfYear(data,i1,i2);
  Label3.Caption:=IntToStr(i1);
  Label7.Caption:=IntToStr(i2);
end;

procedure TcMain.WykonajSkrypt(aScript: string; aParametry: string; aResult: TStrings);
var
  a: TAsyncProcess;
  i,ii: integer;
  ss: TStringList;
begin
  a:=TAsyncProcess.Create(self);
  try
    a.Executable:=aScript;
    if aParametry<>'' then
    begin
      ii:=GetLineCount(aParametry,' ');
      for i:=1 to ii do a.Parameters.Add(GetLineToStr(aParametry,i,' '));
    end;
    a.ShowWindow:=swoHIDE;
    if aResult=nil then a.Options:=[poWaitOnExit] else a.Options:=[poWaitOnExit,poUsePipes];
    a.Execute;
    if aResult<>nil then
    begin
      ss:=TStringList.Create;
      try
        ss.LoadFromStream(a.Output);
        aResult.Assign(ss);
      finally
        ss.Free;
      end;
    end;
  finally
    a.Terminate(0);
    a.Free;
  end;
end;

procedure TcMain.MemMessage(Sender: TObject; AMessage: string);
begin
  if AMessage='HALT' then close;
end;

end.

