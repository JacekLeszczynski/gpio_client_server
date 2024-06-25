unit main;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, ExtCtrls, Spin,
  StdCtrls, Buttons, DBGrids, XMLPropStorage, Menus, DBDateTimePicker,
  ZConnection, ZDataset, ZStoredProcedure, ZSqlUpdate, YearCalendar,
  ZTransaction, DSMaster, ExtMessage, DB;

type

  { TFMain }

  TFMain = class(TForm)
    BitBtn1: TBitBtn;
    BitBtn2: TBitBtn;
    BitBtn3: TBitBtn;
    BitBtn4: TBitBtn;
    BitBtn5: TBitBtn;
    BitBtn6: TBitBtn;
    cBezUrlopow: TCheckBox;
    DBGrid1: TDBGrid;
    dsUrl: TDataSource;
    dni_wolnedata: TDateField;
    dni_wolneniedziela: TSmallintField;
    dni_wolnesobota: TSmallintField;
    dni_wolneswieto: TSmallintField;
    mess: TExtMessage;
    Label1: TLabel;
    Panel1: TPanel;
    Panel2: TPanel;
    Panel3: TPanel;
    Panel4: TPanel;
    Panel5: TPanel;
    cal: TYearCalendar;
    db: TZConnection;
    cRok: TSpinEdit;
    testdata: TDateField;
    testniedziela: TSmallintField;
    testsobota: TSmallintField;
    testswieto: TSmallintField;
    trans: TZTransaction;
    dni_wolne: TZQuery;
    urldata: TStringField;
    urldata_do: TDateField;
    urldata_od: TDateField;
    urlid: TLargeintField;
    urlopy: TZQuery;
    urlopydata_do: TDateField;
    urlopydata_od: TDateField;
    urlopyid: TLargeintField;
    aRok: TZStoredProc;
    dRok: TZStoredProc;
    aSwieto: TZStoredProc;
    dSwieto: TZStoredProc;
    test: TZQuery;
    url: TZQuery;
    upUrl: TZUpdateSQL;
    propstorage: TXMLPropStorage;
    procedure BitBtn1Click(Sender: TObject);
    procedure BitBtn2Click(Sender: TObject);
    procedure BitBtn3Click(Sender: TObject);
    procedure BitBtn4Click(Sender: TObject);
    procedure BitBtn5Click(Sender: TObject);
    procedure BitBtn6Click(Sender: TObject);
    procedure calDrawColor(aSender: TObject; aX, aY: integer; aDate: TDate;
      var aColor: TColor);
    procedure calSelectDate(aSender: TObject; aDate: TDate);
    procedure cBezUrlopowChange(Sender: TObject);
    procedure cRokChange(Sender: TObject);
    procedure dbAfterConnect(Sender: TObject);
    procedure dbBeforeDisconnect(Sender: TObject);
    procedure dni_wolneBeforeOpen(DataSet: TDataSet);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure FormKeyDown(Sender: TObject; var Key: Word; Shift: TShiftState);
    procedure urlBeforeOpen(DataSet: TDataSet);
    procedure urlopyBeforeOpen(DataSet: TDataSet);
  private
    calData: TDate;
    ssDniWolne,ssUrlopy: TStringList;
    procedure wczytaj;
  public
  end;

var
  FMain: TFMain;

implementation

uses
  ecode, urlop;

{$R *.lfm}

function DateToRok(aData: TDate): Word;
var
  r,m,d: Word;
begin
  DecodeDate(date,r,m,d);
  result:=r;
end;

{ TFMain }

procedure TFMain.FormCreate(Sender: TObject);
begin
  ssDniWolne:=TStringList.Create;
  ssUrlopy:=TStringList.Create;
  SetConfDir('PiStudio');
  propstorage.FileName:=MyConfDir('kalendarz.xml');
  propstorage.Active:=true;
  calData:=0;
  cRok.Value:=DateToRok(date);
  try
    db.Connect;
    wczytaj;
    cal.Init;
  except
    halt(1);
  end;
end;

procedure TFMain.FormDestroy(Sender: TObject);
begin
  db.Disconnect;
  ssDniWolne.Free;
  ssUrlopy.Free;
end;

procedure TFMain.FormKeyDown(Sender: TObject; var Key: Word; Shift: TShiftState
  );
begin
  if Key=27 then close;
end;

procedure TFMain.urlBeforeOpen(DataSet: TDataSet);
begin
  url.ParamByName('rok').AsInteger:=cRok.Value;
end;

procedure TFMain.urlopyBeforeOpen(DataSet: TDataSet);
begin
  urlopy.ParamByName('rok').AsInteger:=cRok.Value;
end;

procedure TFMain.wczytaj;
var
  rok: Word;
  s: string;
  dt: TDate;
begin
  rok:=cRok.Value;
  ssDniWolne.Clear;
  (* dni wolne i święta *)
  dni_wolne.Open;
  while not dni_wolne.EOF do
  begin
    ssDniWolne.Add(FormatDateTime('yyyy-mm-dd',dni_wolnedata.AsDateTime));
    dni_wolne.Next;
  end;
  dni_wolne.Close;
  (* urlopy *)
  ssUrlopy.Clear;
  urlopy.Open;
  while not urlopy.EOF do
  begin
    dt:=urlopydata_od.AsDateTime;
    while dt<=urlopydata_do.AsDateTime do
    begin
      if DateToRok(dt)=rok then ssUrlopy.Add(FormatDateTime('yyyy-mm-dd',dt));
      dt:=dt+1;
    end;
    urlopy.Next;
  end;
  urlopy.Close;
end;

procedure TFMain.dni_wolneBeforeOpen(DataSet: TDataSet);
begin
  dni_wolne.ParamByName('rok').AsInteger:=cRok.Value;
end;

procedure TFMain.cRokChange(Sender: TObject);
begin
  wczytaj;
  cal.SetYear(cRok.Value);
  url.Close;
  url.Open;
end;

procedure TFMain.dbAfterConnect(Sender: TObject);
begin
  url.Open;
end;

procedure TFMain.dbBeforeDisconnect(Sender: TObject);
begin
  url.Close;
end;

procedure TFMain.calDrawColor(aSender: TObject; aX, aY: integer; aDate: TDate;
  var aColor: TColor);
begin
  if StringToItemIndex(ssDniWolne,FormatDateTime('yyyy-mm-dd',aDate))>-1 then aColor:=clRed else
  begin
    if cBezUrlopow.Checked then exit;
    if StringToItemIndex(ssUrlopy,FormatDateTime('yyyy-mm-dd',aDate))>-1 then aColor:=clYellow;
  end;
end;

procedure TFMain.calSelectDate(aSender: TObject; aDate: TDate);
begin
  calData:=aDate;
  Caption:='Kalendarz - Święta i Dni wolne [dzień: '+FormatDateTime('dd.mm.yyyy',calData)+']';
end;

procedure TFMain.cBezUrlopowChange(Sender: TObject);
begin
  wczytaj;
  cal.Refresh;
end;

procedure TFMain.BitBtn1Click(Sender: TObject);
begin
  aRok.ParamByName('a_rok').AsInteger:=cRok.Value;
  aRok.ExecProc;
  wczytaj;
  cal.Refresh;
end;

procedure TFMain.BitBtn2Click(Sender: TObject);
begin
  if cRok.Value<DateToRok(date) then exit;
  if mess.ShowConfirmationYesNo('Czy usunąć rok "'+IntToStr(cRok.Value)+'"?') then
  begin
    dRok.ParamByName('a_rok').AsInteger:=cRok.Value;
    dRok.ExecProc;
    wczytaj;
    cal.Refresh;
  end;
end;

procedure TFMain.BitBtn3Click(Sender: TObject);
var
  swieto,wolne: boolean;
begin
  if calData=0 then exit;
  test.ParamByName('data').AsDate:=calData;
  test.Open;
  wolne:=(testsobota.AsInteger=1) or (testniedziela.AsInteger=1);
  swieto:=testswieto.AsInteger=1;
  test.Close;
  if wolne then exit;
  if swieto then
  begin
    dSwieto.ParamByName('a_data').AsDate:=calData;
    dSwieto.ExecProc;
  end else begin
    aSwieto.ParamByName('a_data').AsDate:=calData;
    aSwieto.ExecProc;
  end;
  wczytaj;
  cal.Refresh;
end;

procedure TFMain.BitBtn4Click(Sender: TObject);
var
  a: TFUrlop;
begin
  a:=TFUrlop.Create(self);
  try
    a.ShowModal;
    if a.io_ok then
    begin
      (* dodanie nowego urlopu *)
      url.Append;
      urldata_od.AsDateTime:=a.io_data_od;
      urldata_do.AsDateTime:=a.io_data_do;
      url.Post;
      url.Refresh;
      url.Last;
      wczytaj;
      cal.Refresh;
    end;
  finally
    a.Free;
  end;
end;

procedure TFMain.BitBtn5Click(Sender: TObject);
var
  id: integer;
  a: TFUrlop;
  a1,a2,a3,a4: TDate;
begin
  if url.IsEmpty then exit;
  a:=TFUrlop.Create(self);
  a1:=urldata_od.AsDateTime;
  a2:=urldata_do.AsDateTime;
  a.io_data_od:=a1;
  a.io_data_do:=a2;
  a.io_set:=true;
  try
    a.ShowModal;
    if a.io_ok then
    begin
      a3:=a.io_data_od;
      a4:=a.io_data_do;
      if (a1<>a3) or (a2<>a4) then
      begin
        (* dodanie nowego urlopu *)
        id:=urlid.AsInteger;
        url.Edit;
        urldata_od.AsDateTime:=a3;
        urldata_do.AsDateTime:=a4;
        url.Post;
        url.Refresh;
        url.Locate('id',id,[]);
        wczytaj;
        cal.Refresh;
      end;
    end;
  finally
    a.Free;
  end;
end;

procedure TFMain.BitBtn6Click(Sender: TObject);
begin
  if url.IsEmpty then exit;
  if mess.ShowConfirmationYesNo('Czy usunąć ten urlop?') then
  begin
    url.Delete;
    wczytaj;
    cal.Refresh;
  end;
end;

end.

