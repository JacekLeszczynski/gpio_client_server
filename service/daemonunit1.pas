unit DaemonUnit1;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, DaemonApp, Main;

type

  { TDaemon1 }

  TDaemon1 = class(TDaemon)
    procedure DataModuleAfterInstall(Sender: TCustomDaemon);
    procedure DataModuleBeforeUnInstall(Sender: TCustomDaemon);
    procedure DataModuleContinue(Sender: TCustomDaemon; var OK: Boolean);
    procedure DataModulePause(Sender: TCustomDaemon; var OK: Boolean);
    procedure DataModuleShutDown(Sender: TCustomDaemon);
    procedure DataModuleStart(Sender: TCustomDaemon; var OK: Boolean);
    procedure DataModuleStop(Sender: TCustomDaemon; var OK: Boolean);
  private
    FDaemonWorkerThread: TDaemonWorkerThread;
    procedure reload_systemd;
  public
  end;

var
  Daemon1: TDaemon1;

implementation

uses
  ecode, Process, AsyncProcess;

const
  SERVICE_FILENAME = '/usr/lib/systemd/system/gpio-service.service';

procedure RegisterDaemon;
begin
  RegisterDaemonClass(TDaemon1)
end;

{$R *.lfm}

{ TDaemon1 }

procedure TDaemon1.DataModuleAfterInstall(Sender: TCustomDaemon);
var
  ss: TStringList;
begin
  {$IFDEF UNIX}
  ss:=TStringList.Create;
  try
    ss.Add('[Unit]');
    ss.Add('Requires=network-online.target');
    ss.Add('After=network-online.target');
    ss.Add('');
    ss.Add('[Service]');
    ss.Add('ExecStart='+MyDir('gpio-service')+' -r');
    //ss.Add('ExecStop=/usr/local/bin/client.gpio AUTO-OFF');
    ss.Add('Type=simple');
    ss.Add('RemainAfterExit=yes');
    ss.Add('');
    ss.Add('[Install]');
    ss.Add('WantedBy=default.target');
    ss.SaveToFile(SERVICE_FILENAME);
  finally
    ss.Free;
  end;
  reload_systemd;
  {$ENDIF}
end;

procedure TDaemon1.DataModuleBeforeUnInstall(Sender: TCustomDaemon);
begin
  {$IFDEF UNIX}
  if FileExists(SERVICE_FILENAME) then
  begin
    DeleteFile(SERVICE_FILENAME);
    reload_systemd;
  end;
  {$ENDIF}
end;

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

procedure TDaemon1.reload_systemd;
var
  a: TAsyncProcess;
  b: integer;
  s: string;
begin
  a:=TAsyncProcess.Create(self);
  try
    a.Executable:='systemctl';
    a.Parameters.Add('daemon-reload');
    a.Options:=[poWaitOnExit];
    a.ShowWindow:=swoHIDE;
    a.Execute;
  finally
    a.Terminate(0);
    a.Free;
  end;
end;


initialization
  RegisterDaemon;
end.

