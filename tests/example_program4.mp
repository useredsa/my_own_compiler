(********************************
 * Fichero de funcionalidad n 1 *
 ********************************)
program test1 ();

function foo(const bar : int) : int
begin
    write(bar)
end;

var a,i : int;
var b : str;

begin
    read(a);
    if (a) then
    begin
        for i := 1+0 to 2+3 do
        begin
            write(a, "," + " ");
            a := a + 1
        end;
        write(a, "\n")
    end
    else
        a := foo(a)
end.


