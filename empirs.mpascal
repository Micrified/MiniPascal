PROGRAM emirps (n);

VAR a, b : INTEGER;

{ Reverses an integer number }
FUNCTION reverse (n : INTEGER) : INTEGER;
BEGIN
    reverse := 0;
    WHILE n > 0 DO
    BEGIN
        reverse := reverse + (n MOD 10);
        reverse := reverse * 10;
        n := n DIV 10
    END
END;

{ Returns 1 if the number is prime, else 0 }
FUNCTION isPrime (n : INTEGER) : INTEGER;
VAR i : INTEGER;
BEGIN
    i := 2;
    isPrime := 1;

    WHILE (i * i < n) DO
    BEGIN
        IF ((n MOD i) = 0) THEN
            BEGIN
                isPrime := 0;
                i := n
            END
        ELSE
            i := i + 1
    END
END;

{ Main Program Body: Counting emirps: Exam 2015 }
BEGIN
    a := 0;
    b := 0;
    readln(a,b);
    WHILE (a < b) DO
    BEGIN
        IF (isPrime(a) + isPrime(reverse(a)) = 2) THEN
            writeln(a)
        ELSE
            a := a; { Redundant statement for matching grammar }
        a := a + 1
    END
END.
