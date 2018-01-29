PROGRAM euclid (input, output);
VAR a, b : integer;

BEGIN
    a := 0.0;
    b := a;
    readln(a, b);
    WHILE a <> b DO
    BEGIN
        WHILE a > b DO a := a - b;
        WHILE b > a DO b := b - a
    END;
    writeln(a)
END.