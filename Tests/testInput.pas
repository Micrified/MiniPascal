PROGRAM emirps (n);

VAR a, b : INTEGER;
VAR z : array [0..9] of INTEGER;

BEGIN
a := 1;

while a < 10 do
BEGIN
    z[a - 1] := a;
    a := a + 1
END;

a := z[0];
b := z[8];
writeln(a,b)
END.
