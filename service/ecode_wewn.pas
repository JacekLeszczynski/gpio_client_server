unit ecode_wewn;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils;

function IsDigit(c:char):boolean;
function MyDir(Filename: string): string;
function GetLineToStr(s:string;l:integer;separator:char;wynik:string=''):string;

implementation

const
  textseparator = '"';

var
  MyDirectory: string = '';

function IsDigit(c:char):boolean;
begin
  if (c>='0') and (c<='9') then result:=true else result:=false;
end;

function MyDir(Filename: string): string;
var
  s,s2: string;
begin
  if MyDirectory='' then
  begin
    s:=ExtractFilePath(ParamStr(0));
    delete(s,length(s),1);
  end else s:=MyDirectory;
  {$IFDEF WINDOWS}
  if AddingExeExtension then s2:='.exe' else s2:='';
  if Filename<>'' then s:=StringReplace(s+'\'+Filename+s2,'/','\',[rfReplaceAll]);
  {$ELSE}
  if Filename<>'' then s:=StringReplace(s+'/'+Filename,'\','/',[rfReplaceAll]);
  {$ENDIF}
  result:=s;
end;

function GetLineToStr(s:string;l:integer;separator:char;wynik:string=''):string;
var
  i,ll,dl: integer;
  b: boolean;
begin
  b:=false;
  dl:=length(s);
  ll:=1;
  s:=s+separator;
  for i:=1 to length(s) do
  begin
    if s[i]=textseparator then b:=not b;
    if (not b) and (s[i]=separator) then inc(ll);
    if ll=l then break;
  end;
  if ll=1 then dec(i);
  delete(s,1,i);
  b:=false;
  for i:=1 to length(s) do
  begin
    if s[i]=textseparator then b:=not b;
    if (not b) and (s[i]=separator) then break;
  end;
  delete(s,i,dl);
  if (s<>'') and (s[1]=textseparator) then
  begin
    delete(s,1,1);
    delete(s,length(s),1);
  end;
  if s='' then s:=wynik;
  result:=s;
end;

end.

