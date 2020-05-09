program FibonacciDrawing ();

function fib_drawing (const upto : int) : int
var i, last, new : int;
begin
    last := 0;
	for i := 0 to upto do
	begin
		new := fibonacci(i);
        write("_" * (new-last));
		write("F");
		last := new
	end;
	fib_drawing := 0
end;

function fibonacci (const n : int) : int
var a,b, target_a, target_b, tmp1, tmp2 : int;
begin
	target_a := 0;
	target_b := 0;
	if (n) then
	begin
		n := n - 1;
		a := 0;
		b := 1;
		target_a := 1;
		while (n) do
		begin
			if (n-n/2*2) then
			begin
				tmp1 := target_a*a + target_b*b;
				tmp2 := target_a*b + target_b*(a+b);
				target_a := tmp1;
				target_b := tmp2
			end;
			tmp1 := a*a + b*b;
			tmp2 := a*b + b*(a+b);
			a := tmp1;
			b := tmp2;
			n := n / 2
		end
	end;
	fibonacci := target_a+target_b
end;

function * operator (const lhs : str, const rhs : int) : str
var i : int;
begin
	op := "";
	for i := 0 to rhs do
	begin
		op := op + lhs
	end
end;

function max(const lhs : int, const rhs : int) : int
var dif : int;
begin
	max := lhs;
	dif := rhs-lhs;
	while (dif) do
	begin
		if (dif-1) then
		begin
		end
		else
			max := rhs;
		if (dif+1) then
		begin
		end
		else
			max := lhs;
		dif := dif/2
	end
end;

		

var upTo : int;

begin
	read(upTo);
	upTo := fib_drawing(upTo)
end.



