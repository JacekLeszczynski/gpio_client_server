unit DaemonUnit1;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, DaemonApp, DaemonMain;

type

  { TDaemon1 }

  TDaemon1 = class(TDaemon)
    procedure DataModuleContinue(Sender: TCustomDaemon; var OK: Boolean);
    procedure DataModulePause(Sender: TCustomDaemon; var OK: Boolean);
    procedure DataModuleShutDown(Sender: TCustomDaemon);
    procedure DataModuleStart(Sender: TCustomDaemon; var OK: Boolean);
    procedure DataModuleStop(Sender: TCustomDaemon; var OK: Boolean);
  private
    FDaemonWorkerThread: TDaemonWorkerThread;
  public
  end;

var
  Daemon1: TDaemon1;

implementation

procedure RegisterDaemon;
begin
  RegisterDaemonClass(TDaemon1)
end;

{$R *.lfm}

{ TDaemon1 }

procedure TDaemon1.DataModuleContinue(Sender: TCustomDaemon; var OK: Boolean);
begin
  FDaemonWorkerThread.Resume;
  OK:=true;
end;

procedure TDaemon1.DataModulePause(Sender: TCustomDaemon; var OK: Boolean);
begin
  FDaemonWorkerThread.Suspend;
  OK:=true;
end;

procedure TDaemon1.DataModuleShutDown(Sender: TCustomDaemon);
begin
  self.Stop;
end;

procedure TDaemon1.DataModuleStart(Sender: TCustomDaemon; var OK: Boolean);
begin
  FDaemonWorkerThread:=TDaemonWorkerThread.Create;
  FDaemonWorkerThread.FreeOnTerminate:=false;
  FDaemonWorkerThread.Start;
  OK:=true;
end;

procedure TDaemon1.DataModuleStop(Sender: TCustomDaemon; var OK: Boolean);
begin
  if assigned(FDaemonWorkerThread) then
  begin
    FDaemonWorkerThread.Terminate;
    FDaemonWorkerThread.WaitFor;
    FreeAndNil(FDaemonWorkerThread);
  end;
  OK:=true;
end;

initialization
  RegisterDaemon;
end.

