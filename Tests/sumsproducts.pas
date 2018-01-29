program sumsproducts(input, output);

var n : integer;

function prodSumHelper(current : integer; n : integer; sum : integer; prod : integer) : integer;
begin
    prodSumHelper := 0;
    if current = 10 then
    begin
        if prod = 1 then
        begin
            if sum = n then
                prodSumHelper := 1
            else begin end
        end else begin end
    end else begin end;
    
    if(prodSumHelper = 0) then
    begin
        if sum > n then
            prodSumHelper := 0
        else if prod > n then
            prodSumHelper := 0
        else begin
            prod := prod * current;
            prodSumHelper := prodSumHelper(current + 1, n, sum + prod, 1) + prodSumHelper(current + 1, n, sum, prod)
        end
    end else begin end
end;

function prodSum(n : integer): integer;
begin
    prodSum := prodSumHelper(1, n, 0, 1)
end;

begin
    readln(n);
    writeln(prodSum(n))
end.
