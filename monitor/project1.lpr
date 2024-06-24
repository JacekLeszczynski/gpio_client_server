program project1;

{$mode objfpc}{$H+}

uses
  {$IFDEF UNIX}{$IFDEF UseCThreads}
  cthreads,
  {$ENDIF}{$ENDIF}
  Interfaces, Forms, uecontrols, main, ExtSharedMemory;

{$R *.res}

var
  x: TExtSharedMemory;

begin
  x:=TExtSharedMemory.Create(nil);
  x.ApplicationKey:='{MONITOR_GPIO_469900E4-FD19-4A6E-8D62-0A12668D0AD3}';
  if not x.Execute then
  begin
    x.SendMessage('HALT');
    exit;
  end;
  try
    RequireDerivedFormResource:=True;
    Application.Scaled:=True;
    Application.Initialize;
    Application.CreateForm(TcMain, cMain);
    x.OnMessage:=@main.cMain.MemMessage;
    Application.Run;
  finally
    x.Uninstall;
    x.Free;
  end;
end.

