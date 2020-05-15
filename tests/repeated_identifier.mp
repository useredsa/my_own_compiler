program repeated_ids ();

function overloaded(const bar : int) : int
begin
    write(bar)
end;

function overloaded(const bar : str) : int
begin
    write(bar)
end;

function repeated(const name1 : int) : int
begin
    write(name1)
end;

function repeated(const name2 : int) : int
begin
    write(name2)
end;

function double_argument(const a: int, const a : int) : int
begin
    write(a)
end;

function redeclared(const baz : int) : int
begin
    write (baz)
end;

var redeclared : int;

begin
     write("done\n")
end.


