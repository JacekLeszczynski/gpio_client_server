unit urlop;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, StdCtrls, EditBtn,
  Buttons;

type

  { TFUrlop }

  TFUrlop = class(TForm)
    BitBtn1: TBitBtn;
    BitBtn2: TBitBtn;
    DateEdit1: TDateEdit;
    DateEdit2: TDateEdit;
    Label1: TLabel;
    Label2: TLabel;
    procedure BitBtn1Click(Sender: TObject);
    procedure BitBtn2Click(Sender: TObject);
    procedure DateEdit1Change(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure FormShow(Sender: TObject);
  private
  public
    io_set: boolean;
    io_ok: boolean;
    io_data_od,io_data_do: TDate;
  end;

implementation

{$R *.lfm}

{ TFUrlop }

procedure TFUrlop.BitBtn1Click(Sender: TObject);
begin
  io_data_od:=DateEdit1.Date;
  io_data_do:=DateEdit2.Date;
  io_ok:=true;
  close;
end;

procedure TFUrlop.BitBtn2Click(Sender: TObject);
begin
  io_ok:=false;
  close;
end;

procedure TFUrlop.DateEdit1Change(Sender: TObject);
begin
  DateEdit2.Date:=DateEdit1.Date;
end;

procedure TFUrlop.FormCreate(Sender: TObject);
begin
  io_set:=false;
end;

procedure TFUrlop.FormShow(Sender: TObject);
begin
  if io_set then
  begin
    DateEdit1.Date:=io_data_od;
    DateEdit2.Date:=io_data_do;
  end;
end;

end.

