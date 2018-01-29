PROGRAM alt(ctrl);
var shift : integer;

begin
    readln(shift);
	if shift < 0 then
		shift := shift + shift
	else
		shift := shift - shift;
	writeln(shift)
end
.
