Program lap_service;

Uses
{$IFDEF UNIX}
  CThreads,
{$ENDIF}
  Interfaces, DaemonApp, lazdaemonapp, DaemonMapperUnit1, DaemonUnit1, DaemonMain;

begin
  Application.Initialize;
  Application.Run;
end.
