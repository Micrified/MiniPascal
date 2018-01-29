program puzzle(input, output);

var len, temp : integer;
var series : array [0 .. 20] of integer;

function canReachLast(i : integer; pos : integer; len : integer; series : array [0 .. 20] of integer) : integer;
begin
    if pos < 0 then
        canReachLast := 0
    else if pos >= len then
        canReachLast := 0
    else if i >= len then
        canReachLast := 0
    else if len - 1 = pos + series[pos] then
        canReachLast := 1
    else if canReachLast(i + 1, pos + series[pos], len, series) = 1 then
        canReachLast := 1
    else
        canReachLast := canReachLast(i + 1, pos - series[pos], len, series)
end;

function isSolvable(len : integer; series : array [0 .. 20] of integer) : integer;
begin
    isSolvable := canReachLast(0, 0, len, series)
end;

begin
    len := 0;
    readln(temp);
    series[len] := temp;
    while series[len] <> 0 do
    begin
        len := len + 1;
        readln(temp);
        series[len] := temp
    end;
    len := len + 1;
    writeln(isSolvable(len, series))
end.
