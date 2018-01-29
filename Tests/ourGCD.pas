PROGRAM euclid (input, output);
VAR a, b : integer;

BEGIN
    readln(a, b);
    WHILE a <> b DO
    BEGIN
        WHILE a > b DO a := a - b;
        WHILE b > a DO b := b - a
    END;
    writeln(a)
END.