PROGRAM calc(input, output);
{small calculator with 4 registers
<register> : integer [1..4]
<value> : real
Options:
 - 0 exit
 - 1 <register> <value> : assign value to register
 - 2 <ra> <rb> <rc> store (rb + rc) in ra (registers)
 - 3 <ra> <rb> <rc> store (rb * rc) in ra (registers)
}

var r1, r2, r3, r4, rt: real;
var option, rid1, rid2, rid3 : integer;

function readRegister(regId : integer) : real;
begin
	if regId <= 1 then
		readRegister := r1
	else if regId <= 2 then
		readRegister := r2
	else if regId <= 3 then
		readRegister := r3
	else if regId <= 4 then
		readRegister := r4
	else
		readRegister := 0.0
end;

procedure writeRegister(regId : integer; number : real);
begin
	if regId < 1 then
	begin
	end
	else if regId <= 1 then
		r1 := number
	else if regId <= 2 then
		r2 := number
	else if regId <= 3 then
		r3 := number
	else if regId <= 4 then
		r4 := number
    else begin end
end;

begin
	r1 := 0.0;
	r2 := 0.0; 
	r3 := 0.0;
	r4 := 0.0;
	writeln(r1, r2, r3, r4);
	readln(option);
	while option <> 0 do
	begin
		if option <= 1 then
		begin
			readln(rid1, rt);
			writeRegister(rid1, rt)
		end
		else if option <= 2 then
		begin
			readln(rid1, rid2, rid3);
			writeRegister(rid1, readRegister(rid2) + readRegister(rid3))
		end
		else if option <= 3 then
		begin
			readln(rid1, rid2, rid3);
			writeRegister(rid1, readRegister(rid2) * readRegister(rid3))
		end else begin end;
		writeln(r1, r2, r3, r4);
		readln(option)
	end
end
.
