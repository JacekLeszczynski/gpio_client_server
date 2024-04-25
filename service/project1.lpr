Program project1;

Uses
{$IFDEF UNIX}{$IFDEF UseCThreads}
  CThreads,
{$ENDIF}{$ENDIF}
  DaemonApp, lazdaemonapp, DaemonMapperUnit1, DaemonUnit1, Interfaces, Main;

begin
  Application.Title:='Daemon application';
  Application.Initialize;
  Application.Run;
end.
