program missing(input, output);

var n, total, a, i : integer;

begin
    readln(n);
    total := n;
    i := 0;

    while i < n do
    begin
        readln(a);
        total := total + i;
        total := total - a;
        i := i + 1
    end;

    writeln(total)
end.
