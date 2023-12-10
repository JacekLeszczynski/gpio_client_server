unit DaemonMain;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, blcksock, synsock;

type

  { TDaemonWorkerThread }

  TDaemonWorkerThread = class(TThread)
  private
    FClientSocket: TTCPBlockSocket;
  public
    constructor Create;
    destructor Destroy; override;
    procedure Execute; override;
  end;


implementation

uses
  ecode, ExtShutdown;

{ TDaemonWorkerThread }

constructor TDaemonWorkerThread.Create;
begin
  inherited Create(True);
end;

destructor TDaemonWorkerThread.Destroy;
begin
  inherited Destroy;
end;

procedure TDaemonWorkerThread.Execute;
var
  Buffer: string;
  s,s1,s2,s3: string;
  x: TExtShutdown;
  b_shutdown: boolean;
  i: integer;
begin
  while not Terminated do //while A
  begin
    b_shutdown:=false;
    FClientSocket:=TTCPBlockSocket.Create;
    try
      FClientSocket.Connect('192.168.10.2','2122');
      if FClientSocket.LastError = 0 then
      begin
        // Połączono z serwerem
        FClientSocket.SendString('tryb=laptop');
        while not Terminated do //while B
        begin
          if Terminated then break;
          if FClientSocket.CanRead(1000) then
          begin
            Buffer:=FClientSocket.RecvPacket(0);
            if Buffer<>'' then
            begin
              // Odebrano dane, tutaj możesz przetwarzać komunikaty
              // na przykład Buffer zawiera odebrane dane
              // Możesz również wysyłać dane do serwera używając FClientSocket.SendString
              s1:=GetLineToStr(Buffer,1,'=');
              s2:=GetLineToStr(Buffer,2,'=');
              s3:=GetLineToStr(Buffer,3,'=');
              if s1='laptop' then
              begin
                if s2='shutdown' then
                begin
                  b_shutdown:=true;
                  s:='laptop=shutdowning='+s3;
                  FClientSocket.SendString(s);
                  break;
                end;
              end;
            end;
          end;
          Sleep(100);
        end; //while B
        // Zakończ połączenie z serwerem
        FClientSocket.CloseSocket;
      end;
    finally
      FClientSocket.Free;
    end;
    if b_shutdown then
    begin
      x:=TExtShutdown.Create(nil);
      try
        x.Mode:=smWindows;
        x.execute;
      finally
        x.Free;
      end;
    end;
    for i:=1 to 1200 do if Terminated then break else sleep(100);
    if Terminated then break;
  end; //while A
end;

end.

