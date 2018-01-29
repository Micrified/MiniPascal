PROGRAM prime(input, output);
VAR p : integer;

procedure isPrime(p : integer);
var c : integer;
begin
	c := 2;
	while c * c <= p do
	begin
		if p mod c <> 0 then
		begin
			c := c + 1;
			if c * c > p then
				writeln(1)
			else
			begin
			end
		end
		else
		begin
			writeln(0);
			c := p
		end
	end
end;

BEGIN
	readln(p);
	isPrime(p)
END
.
