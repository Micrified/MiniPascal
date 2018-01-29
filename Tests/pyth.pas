PROGRAM pyth(input, output);
{ Program that reads three numbers and tells bou
  - 0 if they're not a triangle
  - 1 if it's a right-angled triangle
  - 2 if it's an obtuse triangle
  - 3 if it's a sharp triangle
}
VAR a, b, c, t: integer;

BEGIN
	readln(a, b, c);
	while a > b do
	begin
		t := a;
		a := b;
		b := t
	end; {a <= b}
	while a > c do
	begin
		t := a;
		a := c;
		c := t
	end; {a <= c && a <= b}	
	while b > c do
	begin
		t := b;
		b := c;
		c := t
	end; {a <= b <= c}	
	if a + b <= c then
		writeln(0)
	else if a * a + b * b < c * c then
		writeln(2)
	else if a * a + b * b > c * c then
		writeln(3)
	else
		writeln(1)
END
.
