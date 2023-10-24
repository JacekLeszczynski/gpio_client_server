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
begin
  FClientSocket:=TTCPBlockSocket.Create;
  try
    FClientSocket.Connect('192.168.10.2','2122');
    if FClientSocket.LastError = 0 then
    begin
      // Połączono z serwerem
      FClientSocket.SendString('tryb=laptop');
      while not Terminated do
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
              //if s2='active' then SetStatus(1) else
              if s2='shutdown' then
              begin
                //cTest.Tag:=StrToInt(s3);
                s:='laptop=shutdowning='+s3;
                FClientSocket.SendString(s);
                x:=TExtShutdown.Create(nil);
                try
                  x.Mode:=smWindows;
                  x.execute;
                finally
                  x.Free;
                end;
              end;
            end;
          end;
        end;
        Sleep(100);
      end;
      // Zakończ połączenie z serwerem
      FClientSocket.CloseSocket;
    end;
  finally
    FClientSocket.Free;
  end;
end;

end.

