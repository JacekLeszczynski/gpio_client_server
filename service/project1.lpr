Program project1;

Uses
{$IFDEF UNIX}
  CThreads,
{$ENDIF}
  DaemonApp, lazdaemonapp, DaemonMapperUnit1, DaemonUnit1, Interfaces;

begin
  Application.Title:='Daemon application';
  Application.Initialize;
  Application.Run;
end.
