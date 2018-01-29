program fibonacci(input, output);

var n,i : integer;
var fibs : array [1 .. 40] of integer;

function fib(n : integer) : integer;
begin
    if n <= 2 then
        fib := 1
    else
        fib := fib(n-1) + fib(n-2)
end;

function fibMem(n : integer) : integer;
var fibCache : array [0 .. 100] of integer;
var i : integer;
begin
    if n >= 100 then
        fibMem := 0
    else
    begin
        fibCache[1] := 1;
        fibCache[2] := 1;
        i := 3;
        while i <= n do
        begin
            fibCache[i] := fibCache[i - 1] + fibCache[i - 2];
            i := i + 1
        end;
        fibMem := fibCache[i-1]
    end
end;

begin
    readln(n);
    i := 1;
    while i <= n do
    begin
        writeln(fib(i));
        i := i + 1
    end;
    { the dynamic }
    writeln(fibMem(39));
    { now for the array }
    i := 1;
    while i <= 39 do
    begin
        fibs[i] := fib(i);
        i := i + 1
    end;
    writeln(fibs[39])
end.
