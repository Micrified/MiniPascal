{ This is the Pasal implementation 
  of Euclid's algorithm for computing 
  the greatest common divisor of two integers.
}

{ a recursive version }
PROGRAM euclid (input, output);

VAR a,b : integer;

FUNCTION gcd_recursive(u, v : integer) : integer;
BEGIN
  IF u mod v <> 0 THEN
    gcd_recursive := gcd_recursive(v, u mod v)
  ELSE
    gcd_recursive := v
END;

{ an iterative version }
function gcd_iterative(u, v : integer) : integer;
var t : integer;
begin
  while v <> 0 do
  begin
    t := u;
    u := v;
    v := t mod v
  end;
  gcd_iterative := u
end;

{ main program starts here }
BEGIN
  readln(a,b);
  writeln(gcd_recursive(a,b));
  writeln(gcd_iterative(a,b))
END.
