unit Main;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, NetSocket;

type

  { TFMain }

  TFMain = class(TForm)
    mon: TNetSocket;
    procedure monProcessMessage;
  private
  public
  end;

  { TDaemonWorkerThread }

  TDaemonWorkerThread = class(TThread)
  private
    FMain: TFMain;
  public
    constructor Create;
    destructor Destroy; override;
    procedure Execute; override;
  end;

implementation

{$R *.lfm}

{ TDaemonWorkerThread }

constructor TDaemonWorkerThread.Create;
begin
  inherited Create(True);
  FMain:=TFMain.Create(nil);
end;

destructor TDaemonWorkerThread.Destroy;
begin
  FMain.Free;
  inherited Destroy;
end;

procedure TDaemonWorkerThread.Execute;
begin

end;

{ TFMain }

procedure TFMain.monProcessMessage;
begin
  application.ProcessMessages;
end;

end.

